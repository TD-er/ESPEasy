// File copied from ESP32-PaxCounter
// See: https://github.com/cyberman54/ESP32-Paxcounter


// clang-format off
// upload_speed 921600
// board ttgo-t-beam

#ifndef _TTGOBEAM_H
#define _TTGOBEAM_H

#include <stdint.h>

// Hardware related definitions for TTGO T-Beam board
// (only) for older T-Beam version T22_V05 eternal wiring LORA_IO1 to GPIO33 is needed!
//
// pinouts taken from http://tinymicros.com/wiki/TTGO_T-Beam

#define HAS_LED GPIO_NUM_14                                   // on board green LED, only new version TTGO-BEAM V07
// #define HAS_LED GPIO_NUM_21                                // on board green LED, only old verison TTGO-BEAM V05

#define USES_LMIC_LORA 1                                            // comment out if device shall not send data via LoRa
#define CFG_sx1276_radio 1                                    // HPD13A LoRa SoC
// #define BOARD_HAS_PSRAM                                    // use extra 4MB external RAM
#define HAS_BUTTON GPIO_NUM_39                                // on board button (next to reset)
#define BAT_MEASURE_ADC ADC1_GPIO35_CHANNEL                   // battery probe GPIO pin -> ADC1_CHANNEL_7
#define BAT_VOLTAGE_DIVIDER 2                                 // voltage divider 100k/100k on board

// GPS settings
#define HAS_GPS 1                                             // use on board GPS
#define GPS_SERIAL 9600, SERIAL_8N1, GPIO_NUM_12, GPIO_NUM_15 // UBlox NEO 6M
// #define GPS_INT GPIO_NUM_34                                // 30ns accurary timepulse, 
                                                              // to be external wired on pcb: NEO 6M Pin#3 -> GPIO34


// #define DISABLE_BROWNOUT 1                                 // comment out if you want to keep brownout feature


#define ESP32_SER0_TX 1
#define ESP32_SER0_RX 3
#define ESP32_SER1_TX 15
#define ESP32_SER1_RX 12
#define ESP32_SER2_TX 23
#define ESP32_SER2_RX 4


#endif // ifndef _TTGOBEAM_H
