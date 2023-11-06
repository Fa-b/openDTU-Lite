
#if defined(FUSION_V2)
// Mapping for NRF24L01+
    #define NRF_CLK     36
    #define NRF_MISO    48
    #define NRF_MOSI    35
    #define NRF_CS      37
    #define NRF_CE      38
    #define NRF_IRQ     47
// Mapping for CMT2300A
    #define CMT_CLK     6
    #define CMT_CS      4
    #define CMT_FCS     21
    #define CMT_SDIO    5
    #define CMT_GPIO2   3
    #define CMT_GPIO3   8
#elif defined(ESP32)
// Mapping for NRF24L01+
    #define NRF_CLK     18
    #define NRF_MISO    19
    #define NRF_MOSI    23
    #define NRF_CS      5
    #define NRF_CE      17
    #define NRF_IRQ     16
// Mapping for CMT2300A
    #define CMT_CLK     12
    #define CMT_CS      27
    #define CMT_FCS     26
    #define CMT_SDIO    14
    #define CMT_GPIO2   -1
    #define CMT_GPIO3   -1
#else
    #define NRF_CLK     14
    #define NRF_MISO    12
    #define NRF_MOSI    13
    #define NRF_CS      15
    #define NRF_CE      0
    #define NRF_IRQ     2
#endif