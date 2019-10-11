#ifndef LORA_LORACOMMON_H
#define LORA_LORACOMMON_H

#include <stdint.h>

#define LORA_PAYLOAD_BUFFER_SIZE        51      // maximum size of payload block per transmit
#define SEND_QUEUE_SIZE                 10      // maximum number of messages in payload send queue [1 = no queue]
#define OTA_MIN_BATT                   3600 


// FIXME TD-er: Is this still needed for ESPEasy?
// Ports on which the device sends and listenes on LoRaWAN and SPI
#define COUNTERPORT                     1       // counts
#define MACPORT                         0       // network commands
#define RCMDPORT                        2       // remote commands
#define STATUSPORT                      2       // remote command results
#define CONFIGPORT                      3       // config query results
#define GPSPORT                         4       // gps
#define BUTTONPORT                      5       // button pressed signal
#define BEACONPORT                      6       // beacon alarms
#define BMEPORT                         7       // BME680 sensor
#define BATTPORT                        8       // battery voltage
#define TIMEPORT                        9       // time query and response
#define TIMEDIFFPORT                    13      // time adjust diff
#define SENSOR1PORT                     10      // user sensor #1
#define SENSOR2PORT                     11      // user sensor #2
#define SENSOR3PORT                     12      // user sensor #3


enum sendprio_t { prio_low, prio_normal, prio_high };

// Struct holding payload for data send queue
typedef struct {
  uint8_t MessageSize = 0;
  uint8_t MessagePort = 0;
  sendprio_t MessagePrio = prio_normal;
  uint8_t Message[LORA_PAYLOAD_BUFFER_SIZE] = {0};
} MessageBuffer_t;


// Struct holding devices's runtime configuration
typedef struct {
  uint8_t loradr;      // 0-15, lora datarate
  uint8_t txpower;     // 2-15, lora tx power
  uint8_t adrmode;     // 0=disabled, 1=enabled
  uint8_t screensaver; // 0=disabled, 1=enabled
  uint8_t screenon;    // 0=disabled, 1=enabled
  uint8_t countermode; // 0=cyclic unconfirmed, 1=cumulative, 2=cyclic confirmed
  int16_t rssilimit;   // threshold for rssilimiter, negative value!
  uint8_t sendcycle;   // payload send cycle [seconds/2]
  uint8_t wifichancycle; // wifi channel switch cycle [seconds/100]
  uint8_t blescantime;   // BLE scan cycle duration [seconds]
  uint8_t blescan;       // 0=disabled, 1=enabled
  uint8_t wifiant;       // 0=internal, 1=external (for LoPy/LoPy4)
  uint8_t vendorfilter;  // 0=disabled, 1=enabled
  uint8_t rgblum;        // RGB Led luminosity (0..100%)
  uint8_t monitormode;   // 0=disabled, 1=enabled
  uint8_t runmode;       // 0=normal, 1=update
  uint8_t payloadmask;   // bitswitches for payload data
  char version[10];      // Firmware version
} configData_t;


#endif // LORA_LORACOMMON_H