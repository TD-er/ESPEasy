#ifndef CONTROLLERQUEUE_C019_QUEUE_ELEMENT_H
#define CONTROLLERQUEUE_C019_QUEUE_ELEMENT_H

#include "../../ESPEasy_common.h"
#include "../DataStructs/ESPEasyLimits.h"
#include "../LoRa/LoRa_common.h"

struct EventStruct;

// #ifdef USES_C019

/*********************************************************************************************\
* C019_queue_element for queueing requests for C019: LoRa TTN - SX127x/LMIC
\*********************************************************************************************/


class C019_queue_element {
public:

  C019_queue_element();

  C019_queue_element(struct EventStruct *event, uint8_t sampleSetCount);

  size_t getSize() const;

  int controller_idx = 0;
  MessageBuffer_t packed;
};

// #endif //USES_C019


#endif // CONTROLLERQUEUE_C019_QUEUE_ELEMENT_H
