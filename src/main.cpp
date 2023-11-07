// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Modified by Fa-b, 07-11-2023
 */
#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <Ticker.h>
#include <Hoymiles.h>

#include "Datastore.h"
#include "privates.h"
#include "pin_mapping.h"
#include "settings.h"
#include "utils.h"

// lib instances
Ticker updater;

// extern in privates.h
char name[] = NAME;
char ssid[] = SSID;
char pass[] = WIFI_PASS;
uint64_t dtu_serial = RADIO_DTU_SN;

SPIClass* spiClass =
#ifdef ESP32
#if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
    new SPIClass(HSPI);
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
    new SPIClass(FSPI);
#else
    new SPIClass(VSPI);
#endif
#else
new SPIClass();
#endif

// Global vars
char Date[16];
char Time[16];

// Function prototypes
static void update_Hoymiles(void);

// Setup
void setup() {
    Serial.begin(115200);
    while (!Serial) {
        ;  // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("Serial started");

    // Problematic since there may be multiple devices with the same name
    // Add serial number to name
    WiFi.hostname(String(name) + String("_") + String(Utils::getChipId()));

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

#ifdef ESP32
    configTzTime(NTP_TIMEZONE, NTP_SERVER);
    
    spiClass->begin(NRF_CLK, NRF_MISO, NRF_MOSI, NRF_CS);
#else
    if (esp8266::coreVersionNumeric() >= 20700000) {
        configTime(NTP_TIMEZONE, NTP_SERVER);
    } else {
        setenv("TZ", NTP_TIMEZONE, 1);
        configTime(0, 0, NTP_SERVER);
    }

    if(spiClass->pins(NRF_CLK, NRF_MISO, NRF_MOSI, NRF_CS)) {
        spiClass->begin();
    } else {
        Serial.println("SPI pins not set correctly!");
    }
#endif

    Hoymiles.setMessageOutput(&Serial);
    //Serial.println("Initializing Hoymiles with 5 seconds poll interval.");
    Hoymiles.init(5);

    //Serial.println("Setting DTU serial...");
    uint64_t dtuId = Utils::generateDtuSerial(); // RADIO_DTU_SN
    //Serial.printf("%0x%08x... ", ((uint32_t)((dtuId >> 32) & 0xFFFFFFFF)), ((uint32_t)(dtuId & 0xFFFFFFFF)));

#ifdef USE_NRF
    Hoymiles.initNRF(spiClass, NRF_CE, NRF_CS, NRF_IRQ);
    //Serial.println("Setting NRF radio PA level...");
    Hoymiles.getRadioNrf()->setPALevel(NRF_PA_LEVEL);
    Hoymiles.getRadioNrf()->setDtuSerial(dtuId);
    
    if(Hoymiles.getRadioNrf()->isConnected()) {
        Serial.println("openDTU-Lite NRF Connected!");
    } else {
        Serial.println("openDTU-Lite NRF Failed to connect!");
    }
#endif

#ifdef USE_CMT
    Hoymiles.initCMT(CMT_SDIO, CMT_CLK, CMT_CS, CMT_FCS, CMT_GPIO2, CMT_GPIO3);
    Serial.printf("Setting CMT radio frequency to %.1f MHz.\n", CMT_FREQUENCY / 1000);
    Hoymiles.getRadioCmt()->setInverterTargetFrequency(CMT_FREQUENCY);
    //Serial.println("Setting CMT radio PA level...");
    Hoymiles.getRadioCmt()->setPALevel(CMT_PA_LEVEL);
    Hoymiles.getRadioCmt()->setDtuSerial(dtuId);

    if(Hoymiles.getRadioCmt()->isConnected()) {
        Serial.println("openDTU-Lite CMT Connected!");
    } else {
        Serial.println("openDTU-Lite CMT Failed to connect!");
    }
#endif



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
