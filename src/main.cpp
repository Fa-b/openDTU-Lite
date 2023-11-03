#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <Hoymiles.h>

#include "Datastore.h"
#include "privates.h"
#include "utils.h"


#define UPDATE_INTERVAL 5000  // 5 second

// lib instances
Ticker updater;

// extern in privates.h
char name[] = NAME;
char ssid[] = SSID;
char pass[] = WIFI_PASS;
uint64_t dtu_serial = RADIO_DTU_SN;

SPIClass* spiClass = new SPIClass();

// Global vars
char Date[16];
char Time[16];

// Function prototypes
static void update_Hoymiles(void);
static void pre_setup(void);

// Setup
void setup() {
    Serial.begin(115200);
    while (!Serial) {
        ;  // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("Serial started");

    // Problematic since there may be multiple devices with the same name
    // Add serial number to name
    WiFi.hostname(String(name) + String("_") + String(ESP.getChipId()));

    // Connect to WiFi
    WiFi.begin(ssid, pass);
    Serial.printf("Connecting to %s\n", ssid);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.printf("Connected to %s\n", ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    pre_setup();
}

static void pre_setup() {
    if (esp8266::coreVersionNumeric() >= 20700000) {
        configTime(NTP_TIMEZONE, NTP_SERVER);
    } else {
        setenv("TZ", NTP_TIMEZONE, 1);
        configTime(0, 0, NTP_SERVER);
    }

    // NRF24L01+ radio pins:
    // 1. VCC -> 3.3V
    // 2. GND -> GND
    // 3. CE -> D3 -> GPIO0
    // 4. CSN -> D8 -> GPIO15
    // 5. SCK -> D5 -> GPIO14
    // 6. MOSI -> D7 -> GPIO13
    // 7. MISO -> D6 -> GPIO12
    // 8. IRQ -> D4 -> GPIO2
    if(spiClass->pins(D5, D6, D7, D8)) {
        spiClass->begin();
    } else {
        Serial.println("SPI pins not set correctly!");
    }

    Hoymiles.setMessageOutput(&Serial);
    Hoymiles.init();

    Hoymiles.initNRF(spiClass, D3, D8, D4);

    //Serial.println("Setting radio PA level...");
    Hoymiles.getRadioNrf()->setPALevel(RF24_PA_LOW);
    //Serial.println("Setting DTU serial...");
    uint64_t dtuId = Utils::generateDtuSerial(); // RADIO_DTU_SN
    //Serial.printf("%0x%08x... ", ((uint32_t)((dtuId >> 32) & 0xFFFFFFFF)), ((uint32_t)(dtuId & 0xFFFFFFFF)));

    Hoymiles.getRadioNrf()->setDtuSerial(dtuId);
    //Serial.println("Setting poll interval...");
    Hoymiles.setPollInterval(5);
    if(Hoymiles.getRadioNrf()->isConnected()) {
        Serial.println("openDTU-Lite Connected!");
    } else {
        Serial.println("openDTU-Lite Failed to connect!");
    }

    // TODO: For now only one hardcoded inverter is supported..
    //       This should be changed to support multiple inverters
    auto inv = Hoymiles.addInverter(HOYMILES_INVERTER_NAME, HOYMILES_INVERTER_SERIAL);
    if(inv == nullptr) {
        Serial.println("Failed to add inverter!");
    } else {
        Serial.printf("Inverter added! (type: %s)\n", inv->typeName().c_str());
        inv->setReachableThreshold(2);
        inv->setZeroValuesIfUnreachable(true);
        inv->setZeroYieldDayOnMidnight(true);
        for(uint8_t i = 0; i < 4; i++) {
            inv->Statistics()->setStringMaxPower(i, 0.0);
            inv->Statistics()->setChannelFieldOffset(TYPE_DC, static_cast<ChannelNum_t>(i), FLD_YT, 0.0f);
        }
        // Deactivate any Hoymiles attempts to communicate with the inverter:
        inv->setEnablePolling(true);
        inv->setEnableCommands(true);
    }
    
    Datastore.init();

    // Fetch current time
    time_t now = time(nullptr);
    while(now < 100000) {
        now = time(nullptr);
        delay(500);
        Serial.print("*");
    }

    struct tm* timeinfo = localtime(&now);
    strftime(Date, sizeof(Date), "%Y/%m/%d", timeinfo);
    strftime(Time, sizeof(Time), "%H:%M:%S", timeinfo);
    Serial.printf("\n\nCurrent time: %s %s\n", Date, Time);
    
    updater.attach_ms(UPDATE_INTERVAL, update_Hoymiles);
}

static void update_Hoymiles() {
    static float last_power_ac = 0.0;
    static float last_power_dc = 0.0;
    static float last_yield_day = 0.0;
    static float last_yield_total = 0.0;

    float power_ac = Datastore.getTotalAcPowerEnabled();
    float power_dc = Datastore.getTotalDcPowerEnabled();
    float yield_day = Datastore.getTotalAcYieldDayEnabled();
    float yield_total = Datastore.getTotalAcYieldTotalEnabled();

    if(last_power_dc != power_dc || last_power_ac != power_ac) {
        Serial.printf("Power Total DC: %0.2f W\n", power_dc);
        last_power_dc = power_dc;
    }

    if(last_power_ac != power_ac) {
        Serial.printf("Power Total AC: %0.2f W\n", power_ac);
        last_power_ac = power_ac;
    }

    if(last_yield_day != yield_day) {
        Serial.printf("Yield Day: %0.2f kWh\n", yield_day/1000);
        last_yield_day = yield_day;
    }

    if(last_yield_total != yield_total) {
        Serial.printf("Yield Total: %0.2f kWh\n", yield_total);
        last_yield_total = yield_total;
    }
}

void loop() {
    Hoymiles.loop();
    yield();
    Datastore.loop();
}
