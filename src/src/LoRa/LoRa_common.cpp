#include "../LoRa/LoRa_common.h"


configData_t cfg; // struct holds current device configuration
char lmic_event_msg[LMIC_EVENTMSG_LEN]; // display buffer for LMIC event message