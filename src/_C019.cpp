#include "src/Helpers/_CPlugin_Helper.h"

#ifdef USES_C019

// #######################################################################################################
// ################### Controller Plugin 019: ESPEasy-NOW ################################################
// #######################################################################################################

# include "ESPEasy_common.h"
# include "src/ControllerQueue/C019_queue_element.h"
# include "src/Controller_config/C019_config.h"
# include "src/Globals/ESPEasy_now_handler.h"
# include "src/Globals/SendData_DuplicateChecker.h"
# include "src/Helpers/C019_ESPEasyNow_helper.h"
# include "src/ControllerQueue/C019_queue_element.h"


# define CPLUGIN_019
# define CPLUGIN_ID_019         19
# define CPLUGIN_NAME_019       ESPEASY_NOW_NAME

bool CPlugin_019(CPlugin::Function function, struct EventStruct *event, String& string)
{
  bool success = false;

  switch (function)
  {
    case CPlugin::Function::CPLUGIN_PROTOCOL_ADD:
    {
      Protocol[++protocolCount].Number       = CPLUGIN_ID_019;
      Protocol[protocolCount].usesMQTT       = false;
      Protocol[protocolCount].usesTemplate   = false;
      Protocol[protocolCount].usesAccount    = false;
      Protocol[protocolCount].usesPassword   = false;
      Protocol[protocolCount].usesExtCreds   = false;
      Protocol[protocolCount].defaultPort    = 0;
      Protocol[protocolCount].usesID         = false;
      Protocol[protocolCount].Custom         = false;
      Protocol[protocolCount].usesHost       = false;
      Protocol[protocolCount].usesPort       = false;
      Protocol[protocolCount].usesQueue      = false;
      Protocol[protocolCount].usesCheckReply = false;
      Protocol[protocolCount].usesTimeout    = false;
      Protocol[protocolCount].usesSampleSets = false;
      Protocol[protocolCount].needsNetwork   = false;
      break;
    }

    case CPlugin::Function::CPLUGIN_GET_DEVICENAME:
    {
      string = F(CPLUGIN_NAME_019);
      break;
    }

    case CPlugin::Function::CPLUGIN_INIT:
    {
      //      success = init_c019_delay_queue(event->ControllerIndex);
      if (use_EspEasy_now) { ESPEasy_now_handler.end(); }

      ESPEasy_now_handler.loadConfig(event);
      ESPEasy_now_handler.do_begin();
      success = true;
      break;
    }

    case CPlugin::Function::CPLUGIN_WEBFORM_LOAD:
    {
      C019_ConfigStruct_ptr customConfig(new (std::nothrow) C019_ConfigStruct);

      if (customConfig) {
        customConfig->webform_load(event);
      }

      break;
    }

    case CPlugin::Function::CPLUGIN_WEBFORM_SAVE:
    {
      C019_ConfigStruct_ptr customConfig(new (std::nothrow) C019_ConfigStruct);

      if (customConfig) {
        customConfig->webform_save(event);
      }

      break;
    }

    case CPlugin::Function::CPLUGIN_PROTOCOL_TEMPLATE:
    {
      C019_ConfigStruct_ptr customConfig(new (std::nothrow) C019_ConfigStruct);

      if (customConfig) {
        //        customConfig->webform_save(event);
      }
      break;
    }

    case CPlugin::Function::CPLUGIN_PROTOCOL_SEND:
    {
      std::unique_ptr<C019_queue_element> element(new C019_queue_element(event));

      success = C019_DelayHandler->addToQueue(std::move(element));

      Scheduler.scheduleNextDelayQueue(ESPEasy_Scheduler::IntervalTimer_e::TIMER_C019_DELAY_QUEUE, C019_DelayHandler->getNextScheduleTime());
      break;
    }

    case CPlugin::Function::CPLUGIN_PROTOCOL_RECV:
    {
      C019_ESPEasyNow_helper::process_receive(event);

      break;
    }

    case CPlugin::Function::CPLUGIN_FIFTY_PER_SECOND:
    {
      // FIXME: This must be removed from the run50TimesPerSecond() function
      if (ESPEasy_now_handler.loop()) {
        // FIXME TD-er: Must check if enabled, etc.
      }
      START_TIMER;
      SendData_DuplicateChecker.loop();
      STOP_TIMER(ESPEASY_NOW_DEDUP_LOOP);
      break;
    }

    case CPlugin::Function::CPLUGIN_FLUSH:
    {
      process_c019_delay_queue();
      delay(0);
      break;
    }

    default:
      break;
  }

  return success;
}

bool do_process_c019_delay_queue(int controller_number, const Queue_element_base& element_base,
                                 ControllerSettingsStruct& ControllerSettings) {
  const C019_queue_element& element = static_cast<const C019_queue_element&>(element_base);

  ESPEasy_Now_p2p_data data;

  const taskIndex_t taskIndex = element.event.TaskIndex;

  data.dataType        = ESPEasy_Now_p2p_data_type::PluginData;
  data.sourceTaskIndex = taskIndex;
  data.plugin_id       = getPluginID_from_TaskIndex(taskIndex);
  data.sourceUnit      = Settings.Unit;
  data.idx             = Settings.TaskDeviceID[element._controller_idx][taskIndex];
  data.sensorType      = element.event.sensorType;
  data.valueCount      = getValueCountForTask(taskIndex);

  if (Sensor_VType::SENSOR_TYPE_ULONG == data.sensorType) {
    data.addString(element.event.String2);
  } else {
    const TaskValues_Data_t * taskValues = UserVar.getTaskValues_Data(taskIndex);
    if (taskValues != nullptr) {
      data.addBinaryData((uint8_t *)(taskValues), sizeof(TaskValues_Data_t));
    }
  }

  if (element.packed.length() > 0) {
    data.addString(element.packed);
  }

  MAC_address broadcast;

  // FIXME TD-er: Must add a way to reach further than WiFi reach
  for (int i = 0; i < 6; ++i) {
    broadcast.mac[i] = 0xFF;
  }

  return ESPEasy_now_handler.sendESPEasyNow_p2p(controller_number, broadcast, data);
}

#endif // ifdef USES_C019
