#ifndef HELPER_CPLUGIN_SENSORTYPEHELPER_H
#define HELPER_CPLUGIN_SENSORTYPEHELPER_H

#include "../../ESPEasy_common.h"

#include "../DataStructs/DeviceStruct.h"

void sensorTypeHelper_webformLoad_allTypes(struct EventStruct *event, int pconfigIndex);

void sensorTypeHelper_webformLoad_simple(struct EventStruct *event, int pconfigIndex);

void sensorTypeHelper_webformLoad(struct EventStruct *event, int pconfigIndex, int optionCount, const uint8_t options[]);

void sensorTypeHelper_saveOutputSelector(struct EventStruct *event, int pconfigIndex, uint8_t valueIndex, const String& defaultValueName);

void pconfig_webformSave(struct EventStruct *event, int pconfigIndex);

void sensorTypeHelper_loadOutputSelector(
  struct EventStruct *event, int pconfigIndex, uint8_t valuenr,
  int optionCount, const __FlashStringHelper * options[], const int indices[] = nullptr);

void sensorTypeHelper_loadOutputSelector(
  struct EventStruct *event, int pconfigIndex, uint8_t valuenr,
  int optionCount, const String options[], const int indices[] = nullptr);


#endif // HELPER_CPLUGIN_SENSORTYPEHELPER_H