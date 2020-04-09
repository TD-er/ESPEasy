#ifndef HELPERS_WEBSERVER_MENU_H
#define HELPERS_WEBSERVER_MENU_H

#define MENU_INDEX_MAIN          0
#define MENU_INDEX_CONFIG        1
#define MENU_INDEX_CONTROLLERS   2
#define MENU_INDEX_HARDWARE      3
#define MENU_INDEX_DEVICES       4
#define MENU_INDEX_RULES         5
#define MENU_INDEX_NOTIFICATIONS 6
#define MENU_INDEX_TOOLS         7

#include <Arduino.h>

// See https://github.com/letscontrolit/ESPEasy/issues/1650
String getGpMenuIcon(byte index);

String getGpMenuLabel(byte index);

String getGpMenuURL(byte index);

bool GpMenuVisible(byte index);

#endif