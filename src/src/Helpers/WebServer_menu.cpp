#include "WebServer_menu.h"

#include "ESPEasy_common.h"
#include "../../WebServer_fwddecl.h"


#ifndef MENU_INDEX_MAIN_VISIBLE
  # define MENU_INDEX_MAIN_VISIBLE true
#endif // ifndef MENU_INDEX_MAIN_VISIBLE

#ifndef MENU_INDEX_CONFIG_VISIBLE
  # define MENU_INDEX_CONFIG_VISIBLE true
#endif // ifndef MENU_INDEX_CONFIG_VISIBLE

#ifndef MENU_INDEX_CONTROLLERS_VISIBLE
  # define MENU_INDEX_CONTROLLERS_VISIBLE true
#endif // ifndef MENU_INDEX_CONTROLLERS_VISIBLE

#ifndef MENU_INDEX_HARDWARE_VISIBLE
  # define MENU_INDEX_HARDWARE_VISIBLE true
#endif // ifndef MENU_INDEX_HARDWARE_VISIBLE

#ifndef MENU_INDEX_DEVICES_VISIBLE
  # define MENU_INDEX_DEVICES_VISIBLE true
#endif // ifndef MENU_INDEX_DEVICES_VISIBLE

#ifndef MENU_INDEX_RULES_VISIBLE
  # define MENU_INDEX_RULES_VISIBLE true
#endif // ifndef MENU_INDEX_RULES_VISIBLE

#ifndef MENU_INDEX_NOTIFICATIONS_VISIBLE
  # define MENU_INDEX_NOTIFICATIONS_VISIBLE true
#endif // ifndef MENU_INDEX_NOTIFICATIONS_VISIBLE

#ifndef MENU_INDEX_TOOLS_VISIBLE
  # define MENU_INDEX_TOOLS_VISIBLE true
#endif // ifndef MENU_INDEX_TOOLS_VISIBLE


#if defined(NOTIFIER_SET_NONE) && defined(MENU_INDEX_NOTIFICATIONS_VISIBLE)
  #undef MENU_INDEX_NOTIFICATIONS_VISIBLE
  #define MENU_INDEX_NOTIFICATIONS_VISIBLE false
#endif


// See https://github.com/letscontrolit/ESPEasy/issues/1650
String getGpMenuIcon(byte index) {
  switch (index) {
    case MENU_INDEX_MAIN: return F("&#8962;");
    case MENU_INDEX_CONFIG: return F("&#9881;");
    case MENU_INDEX_CONTROLLERS: return F("&#128172;");
    case MENU_INDEX_HARDWARE: return F("&#128204;");
    case MENU_INDEX_DEVICES: return F("&#128268;");
    case MENU_INDEX_RULES: return F("&#10740;");
    case MENU_INDEX_NOTIFICATIONS: return F("&#9993;");
    case MENU_INDEX_TOOLS: return F("&#128295;");
  }
  return "";
}

String getGpMenuLabel(byte index) {
  switch (index) {
    case MENU_INDEX_MAIN: return F("Main");
    case MENU_INDEX_CONFIG: return F("Config");
    case MENU_INDEX_CONTROLLERS: return F("Controllers");
    case MENU_INDEX_HARDWARE: return F("Hardware");
    case MENU_INDEX_DEVICES: return F("Devices");
    case MENU_INDEX_RULES: return F("Rules");
    case MENU_INDEX_NOTIFICATIONS: return F("Notifications");
    case MENU_INDEX_TOOLS: return F("Tools");
  }
  return "";
}

String getGpMenuURL(byte index) {
  switch (index) {
    case MENU_INDEX_MAIN: return F("/");
    case MENU_INDEX_CONFIG: return F("/config");
    case MENU_INDEX_CONTROLLERS: return F("/controllers");
    case MENU_INDEX_HARDWARE: return F("/hardware");
    case MENU_INDEX_DEVICES: return F("/devices");
    case MENU_INDEX_RULES: return F("/rules");
    case MENU_INDEX_NOTIFICATIONS: return F("/notifications");
    case MENU_INDEX_TOOLS: return F("/tools");
  }
  return "";
}

bool GpMenuVisible(byte index) {
  /*
  if (isLoggedIn()) {
    switch (index) {
      case MENU_INDEX_NOTIFICATIONS: return MENU_INDEX_NOTIFICATIONS_VISIBLE;
      default: 
        break;
    }
    return true;
  } else {
    */
    switch (index) {
      case MENU_INDEX_MAIN: return MENU_INDEX_MAIN_VISIBLE;
      case MENU_INDEX_CONFIG: return MENU_INDEX_CONFIG_VISIBLE;
      case MENU_INDEX_CONTROLLERS: return MENU_INDEX_CONTROLLERS_VISIBLE;
      case MENU_INDEX_HARDWARE: return MENU_INDEX_HARDWARE_VISIBLE;
      case MENU_INDEX_DEVICES: return MENU_INDEX_DEVICES_VISIBLE;
      case MENU_INDEX_RULES: return MENU_INDEX_RULES_VISIBLE;
      case MENU_INDEX_NOTIFICATIONS: return MENU_INDEX_NOTIFICATIONS_VISIBLE;
      case MENU_INDEX_TOOLS: return MENU_INDEX_TOOLS_VISIBLE;
    }
//  }
  return false;
}
