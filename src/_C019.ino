#ifdef USES_C019

// #######################################################################################################
// ########################### Controller Plugin 019: LoRa TTN - SX127x/LMIC   ###########################
// ########################### !!! ESP32 only controller !!!                   ###########################
// #######################################################################################################

#define CPLUGIN_019
#define CPLUGIN_ID_019         19
#define CPLUGIN_NAME_019       "LoRa TTN - SX127x/LMIC [TESTING]"


#include "src/LoRa/lorawan.h"
#include "src/LoRa/LoRa_common.h"
#include "ESPEasy_common.h"

struct C019_data_struct {
  C019_data_struct() {}

  ~C019_data_struct() {
    reset();
  }

  void reset() {
    if (lmic != nullptr) {
      delete lmic;
      lmic = nullptr;
    }
  }

  bool init() {
    // FIXME TD-er: Call function to init LMIC pin map
    if (lmic == nullptr) {
      lmic = new LMIC_Handler_t();
    }
    if (lmic->lora_stack_init() != ESP_OK) return false;
    return isInitialized();
  }

  bool isInitialized() {
    if (lmic == nullptr) { return false; }
    return lmic->isInitialized();
  }

private:

  LMIC_Handler_t *lmic               = nullptr;
  uint8_t              sampleSetCounter   = 0;
  uint8_t              sampleSetInitiator = 0;
} C019_data;


#define C019_DEVICE_EUI_LEN          17
#define C019_DEVICE_ADDR_LEN         33
#define C019_NETWORK_SESSION_KEY_LEN 33
#define C019_APP_SESSION_KEY_LEN     33
#define C019_USE_OTAA                0
#define C019_USE_ABP                 1
struct C019_ConfigStruct
{
  C019_ConfigStruct() {
    reset();
  }

  void validate() {
    ZERO_TERMINATE(DeviceEUI);
    ZERO_TERMINATE(DeviceAddr);
    ZERO_TERMINATE(NetworkSessionKey);
    ZERO_TERMINATE(AppSessionKey);
  }

  void reset() {
    ZERO_FILL(DeviceEUI);
    ZERO_FILL(DeviceAddr);
    ZERO_FILL(NetworkSessionKey);
    ZERO_FILL(AppSessionKey);
    sf            = 7;
    frequencyplan = 0; //TTN_EU;
    joinmethod    = C019_USE_OTAA;
  }

  char          DeviceEUI[C019_DEVICE_EUI_LEN]                  = { 0 };
  char          DeviceAddr[C019_DEVICE_ADDR_LEN]                = { 0 };
  char          NetworkSessionKey[C019_NETWORK_SESSION_KEY_LEN] = { 0 };
  char          AppSessionKey[C019_APP_SESSION_KEY_LEN]         = { 0 };
  uint8_t       sf                                              = 7;
  uint8_t       frequencyplan                                   = 0; //TTN_EU;
  uint8_t       joinmethod                                      = C019_USE_OTAA;
};


bool CPlugin_019(byte function, struct EventStruct *event, String& string)
{
  bool success = false;

  switch (function)
  {
    case CPLUGIN_PROTOCOL_ADD:
    {
      Protocol[++protocolCount].Number       = CPLUGIN_ID_019;
      Protocol[protocolCount].usesMQTT       = false;
      Protocol[protocolCount].usesAccount    = true;
      Protocol[protocolCount].usesPassword   = true;
      Protocol[protocolCount].defaultPort    = 1;
      Protocol[protocolCount].usesID         = true;
      Protocol[protocolCount].usesHost       = false;
      Protocol[protocolCount].usesSampleSets = true;
      break;
    }

    case CPLUGIN_GET_DEVICENAME:
    {
      string = F(CPLUGIN_NAME_019);
      break;
    }

    case CPLUGIN_WEBFORM_SHOW_HOST_CONFIG:
    {
      break;
    }

    case CPLUGIN_INIT:
    {
      MakeControllerSettings(ControllerSettings);
      LoadControllerSettings(event->ControllerIndex, ControllerSettings);
      C019_DelayHandler.configureControllerSettings(ControllerSettings);

      C019_ConfigStruct customConfig;
      LoadCustomControllerSettings(event->ControllerIndex, (byte *)&customConfig, sizeof(customConfig));
      customConfig.validate();


      break;
    }

    case CPLUGIN_WEBFORM_LOAD:
    {
      C019_ConfigStruct customConfig;

      LoadCustomControllerSettings(event->ControllerIndex, (byte *)&customConfig, sizeof(customConfig));
      customConfig.validate();


      break;
    }
    case CPLUGIN_WEBFORM_SAVE:
    {
      break;
    }

    case CPLUGIN_GET_PROTOCOL_DISPLAY_NAME:
    {
      success = true;

      switch (event->idx) {
        case CONTROLLER_USER:
          string = F("AppEUI");
          break;
        case CONTROLLER_PASS:
          string = F("AppKey");
          break;
        case CONTROLLER_TIMEOUT:
          string = F("Gateway Timeout");
          break;
        case CONTROLLER_PORT:
          string = F("Port");
        default:
          success = false;
          break;
      }
      break;
    }

    case CPLUGIN_PROTOCOL_SEND:
    {
      /*
      success = C019_DelayHandler.addToQueue(
          C019_queue_element(event, C019_data.getSampleSetCount(event->TaskIndex)));
      scheduleNextDelayQueue(TIMER_C019_DELAY_QUEUE, C019_DelayHandler.getNextScheduleTime());
      */
      break;
    }

    case CPLUGIN_FLUSH:
    {
      process_c019_delay_queue();
      delay(0);
      break;
    }
  }
  return success;
}

bool do_process_c019_delay_queue(int controller_number, const C019_queue_element& element, ControllerSettingsStruct& ControllerSettings);

bool do_process_c019_delay_queue(int controller_number, const C019_queue_element& element, ControllerSettingsStruct& ControllerSettings) {
  bool success = false; // C019_data.txHexBytes(element.packed, ControllerSettings.Port);

  return success;
}

String c019_add_joinChanged_script_element_line(const String& id, bool forOTAA) {
  String result = F("document.getElementById('tr_");

  result += id;
  result += F("').style.display = style");
  result += forOTAA ? F("OTAA") : F("ABP");
  result += ';';
  return result;
}

#endif // ifdef USES_C019
