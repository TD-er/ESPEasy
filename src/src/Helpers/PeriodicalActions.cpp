#include "../Helpers/PeriodicalActions.h"


#include "../../ESPEasy-Globals.h"

#include "../ControllerQueue/DelayQueueElements.h"
#include "../ControllerQueue/MQTT_queue_element.h"
#include "../DataStructs/TimingStats.h"
#include "../DataTypes/ESPEasy_plugin_functions.h"
#include "../ESPEasyCore/Controller.h"
#include "../ESPEasyCore/ESPEasyEth.h"
#include "../ESPEasyCore/ESPEasyGPIO.h"
#include "../ESPEasyCore/ESPEasy_Log.h"
#include "../ESPEasyCore/ESPEasyNetwork.h"
#include "../ESPEasyCore/ESPEasyWifi.h"
#include "../ESPEasyCore/ESPEasyRules.h"
#include "../ESPEasyCore/Serial.h"
#include "../Globals/ESPEasyWiFiEvent.h"
#if FEATURE_ETHERNET
#include "../Globals/ESPEasyEthEvent.h"
#endif
#include "../Globals/ESPEasy_Scheduler.h"
#include "../Globals/ESPEasy_time.h"
#include "../Globals/EventQueue.h"
#include "../Globals/MainLoopCommand.h"
#include "../Globals/MQTT.h"
#include "../Globals/NetworkState.h"
#include "../Globals/RTC.h"
#include "../Globals/Services.h"
#include "../Globals/Settings.h"
#include "../Globals/Statistics.h"
#ifdef USES_ESPEASY_NOW
#include "../Globals/ESPEasy_now_handler.h"
#include "../Globals/SendData_DuplicateChecker.h"
#endif
#include "../Globals/WiFi_AP_Candidates.h"
#include "../Helpers/ESPEasyRTC.h"
#include "../Helpers/FS_Helper.h"
#include "../Helpers/Hardware_temperature_sensor.h"
#include "../Helpers/Memory.h"
#include "../Helpers/Misc.h"
#include "../Helpers/Network.h"
#include "../Helpers/Networking.h"
#include "../Helpers/StringGenerator_System.h"
#include "../Helpers/StringGenerator_WiFi.h"
#include "../Helpers/StringProvider.h"

#ifdef USES_C015
#include "../../ESPEasy_fdwdecl.h"
#endif



#define PLUGIN_ID_MQTT_IMPORT         37


/*********************************************************************************************\
 * Tasks that run 50 times per second
\*********************************************************************************************/

void run50TimesPerSecond() {
  String dummy;
  {
    START_TIMER;
    PluginCall(PLUGIN_FIFTY_PER_SECOND, 0, dummy);
    STOP_TIMER(PLUGIN_CALL_50PS);
  }
  {
    START_TIMER;
    CPluginCall(CPlugin::Function::CPLUGIN_FIFTY_PER_SECOND, 0, dummy);
    STOP_TIMER(CPLUGIN_CALL_50PS);
  }
  #ifdef USES_ESPEASY_NOW
  {
    if (ESPEasy_now_handler.loop()) {
      // FIXME TD-er: Must check if enabled, etc.
    }
    START_TIMER;
    SendData_DuplicateChecker.loop();
    STOP_TIMER(ESPEASY_NOW_DEDUP_LOOP);
  }
  #endif

  processNextEvent();
}

/*********************************************************************************************\
 * Tasks that run 10 times per second
\*********************************************************************************************/
void run10TimesPerSecond() {
  String dummy;
  //@giig19767g: WARNING: Monitor10xSec must run before PLUGIN_TEN_PER_SECOND
  {
    START_TIMER;
    GPIO_Monitor10xSec();
    STOP_TIMER(PLUGIN_CALL_10PSU);
  }
  {
    START_TIMER;
    PluginCall(PLUGIN_TEN_PER_SECOND, 0, dummy);
    STOP_TIMER(PLUGIN_CALL_10PS);
  }
  {
    START_TIMER;
//    PluginCall(PLUGIN_UNCONDITIONAL_POLL, 0, dummyString);
    PluginCall(PLUGIN_MONITOR, 0, dummy);
    STOP_TIMER(PLUGIN_CALL_10PSU);
  }
  {
    START_TIMER;
    CPluginCall(CPlugin::Function::CPLUGIN_TEN_PER_SECOND, 0, dummy);
    STOP_TIMER(CPLUGIN_CALL_10PS);
  }
  
  #ifdef USES_C015
  if (NetworkConnected())
      Blynk_Run_c015();
  #endif
  #ifndef USE_RTOS_MULTITASKING
    web_server.handleClient();
  #endif
}


/*********************************************************************************************\
 * Tasks each second
\*********************************************************************************************/
void runOncePerSecond()
{
  START_TIMER;
  updateLogLevelCache();
  dailyResetCounter++;
  if (dailyResetCounter > 86400) // 1 day elapsed... //86400
  {
    RTC.flashDayCounter=0;
    saveToRTC();
    dailyResetCounter=0;
    addLog(LOG_LEVEL_INFO, F("SYS  : Reset 24h counters"));
  }

  if (Settings.ConnectionFailuresThreshold)
    if (WiFiEventData.connectionFailures > Settings.ConnectionFailuresThreshold)
      delayedReboot(60, IntendedRebootReason_e::DelayedReboot);

  if (cmd_within_mainloop != 0)
  {
    switch (cmd_within_mainloop)
    {
      case CMD_WIFI_DISCONNECT:
        {
          WifiDisconnect();
          break;
        }
      case CMD_REBOOT:
        {
          reboot(IntendedRebootReason_e::CommandReboot);
          break;
        }
    }
    cmd_within_mainloop = 0;
  }
  // clock events
  if (node_time.reportNewMinute()) {
    String dummy;
    PluginCall(PLUGIN_CLOCK_IN, 0, dummy);
    if (Settings.UseRules)
    {
      // FIXME TD-er: What to do when the system time is not (yet) present?
      if (node_time.systemTimePresent()) {
        // TD-er: Do not add to the eventQueue, but execute right now.
        const String event = strformat(
          F("Clock#Time=%s,%s"), 
          node_time.weekday_str().c_str(),
          node_time.getTimeString(':', false).c_str());
        rulesProcessing(event);
      }
    }
  }

//  unsigned long start = micros();
  String dummy;
  PluginCall(PLUGIN_ONCE_A_SECOND, 0, dummy);
//  unsigned long elapsed = micros() - start;


  // I2C Watchdog feed
  if (Settings.WDI2CAddress != 0)
  {
    I2C_write8(Settings.WDI2CAddress, 0xA5);
  }

  #if FEATURE_MDNS
  #ifdef ESP8266
  // Allow MDNS processing
  if (NetworkConnected()) {
    MDNS.announce();
  }
  #endif
  #endif // if FEATURE_MDNS

  #if FEATURE_INTERNAL_TEMPERATURE && defined(ESP32_CLASSIC)
  getInternalTemperature(); // Just read the value every second to hopefully get a valid next reading on original ESP32
  #endif // if FEATURE_INTERNAL_TEMPERATURE && defined(ESP32_CLASSIC)

  checkResetFactoryPin();
  STOP_TIMER(PLUGIN_CALL_1PS);
}

/*********************************************************************************************\
 * Tasks each 30 seconds
\*********************************************************************************************/
void runEach30Seconds()
{
  #ifndef BUILD_NO_RAM_TRACKER
  checkRAMtoLog();
  #endif
  wdcounter++;
  if (loglevelActiveFor(LOG_LEVEL_INFO)) {
    String log = strformat(
      F("WD   : Uptime %d  ConnectFailures %u FreeMem %u"),
      getUptimeMinutes(),
      WiFiEventData.connectionFailures,
      FreeMem());
    bool logWiFiStatus = true;
    #if FEATURE_ETHERNET
    if(active_network_medium == NetworkMedium_t::Ethernet) {
      logWiFiStatus = false;
      log += F( " EthSpeedState ");
      log += getValue(LabelType::ETH_SPEED_STATE);
      log += F(" ETH status: ");
      log += EthEventData.ESPEasyEthStatusToString();
    }
    #endif // if FEATURE_ETHERNET
    if (logWiFiStatus) {
      log += strformat(
        F(" WiFiStatus: %s ESPeasy internal wifi status: %s"),
        ArduinoWifiStatusToString(WiFi.status()).c_str(),
        WiFiEventData.ESPeasyWifiStatusToString().c_str());
    }
//    log += F(" ListenInterval ");
//    log += WiFi.getListenInterval();
    addLogMove(LOG_LEVEL_INFO, log);
#if FEATURE_DEFINE_SERIAL_CONSOLE_PORT
//    addLogMove(LOG_LEVEL_INFO,  ESPEASY_SERIAL_CONSOLE_PORT.getLogString());
#endif
  }
  WiFi_AP_Candidates.purge_expired();
  #if FEATURE_ESPEASY_P2P
  sendSysInfoUDP(1);
  refreshNodeList();
  #endif

  // sending $stats to homie controller
  CPluginCall(CPlugin::Function::CPLUGIN_INTERVAL, 0);

  #if defined(ESP8266)
  #if FEATURE_SSDP
  if (Settings.UseSSDP)
    SSDP_update();

  #endif // if FEATURE_SSDP
  #endif
#if FEATURE_ADC_VCC
  if (!WiFiEventData.wifiConnectInProgress) {
    vcc = ESP.getVcc() / 1000.0f;
  }
#endif

  #if FEATURE_REPORTING
  ReportStatus();
  #endif // if FEATURE_REPORTING

}

#if FEATURE_MQTT


void scheduleNextMQTTdelayQueue() {
  if (MQTTDelayHandler != nullptr) {
    unsigned long nextScheduled(MQTTDelayHandler->getNextScheduleTime());
    #ifdef USES_ESPEASY_NOW
    if (!MQTTclient_connected) {
      // Sending via the mesh may be retried at shorter intervals
      if (timePassedSince(nextScheduled) < -5) {
        nextScheduled = millis() + 5;
      }
    }
    #endif
    Scheduler.scheduleNextDelayQueue(SchedulerIntervalTimer_e::TIMER_MQTT_DELAY_QUEUE, nextScheduled);
  }
}

void schedule_all_MQTTimport_tasks() {
  controllerIndex_t ControllerIndex = firstEnabledMQTT_ControllerIndex();

  if (!validControllerIndex(ControllerIndex)) { return; }

  constexpr pluginID_t PLUGIN_MQTT_IMPORT(PLUGIN_ID_MQTT_IMPORT);

  deviceIndex_t DeviceIndex = getDeviceIndex(PLUGIN_MQTT_IMPORT); // Check if P037_MQTTimport is present in the build
  if (validDeviceIndex(DeviceIndex)) {
    for (taskIndex_t task = 0; task < TASKS_MAX; task++) {
      if ((Settings.getPluginID_for_task(task) == PLUGIN_MQTT_IMPORT) &&
          (Settings.TaskDeviceEnabled[task])) {
        // Schedule a call to each enabled MQTT import plugin to notify the broker connection state
        EventStruct event(task);
        event.Par1 = MQTTclient_connected ? 1 : 0;
        Scheduler.schedule_plugin_task_event_timer(DeviceIndex, PLUGIN_MQTT_CONNECTION_STATE, std::move(event));
      }
    }
  }
}


void processMQTTdelayQueue() {
  if (MQTTDelayHandler == nullptr) {
    return;
  }
  runPeriodicalMQTT(); // Update MQTT connected state.
  #ifndef USES_ESPEASY_NOW
  // When using ESPEasy_NOW we may still send MQTT messages even when we're not connected.
  // For all other situations no need to continue.
  if (!MQTTclient_connected) {
    scheduleNextMQTTdelayQueue();
    return;
  }
  #endif

  START_TIMER;
  MQTT_queue_element *element(static_cast<MQTT_queue_element *>(MQTTDelayHandler->getNext()));

  if (element == nullptr) { return; }

  bool processed = false;

  if (element->_call_PLUGIN_PROCESS_CONTROLLER_DATA) {
    struct EventStruct TempEvent(element->_taskIndex);
    String dummy;

    if (PluginCall(PLUGIN_PROCESS_CONTROLLER_DATA, &TempEvent, dummy)) {
      processed = true;
    }
  }
  if (!processed) {
#ifdef USES_ESPEASY_NOW
    MessageRouteInfo_t messageRouteInfo;
    if (element->getMessageRouteInfo() != nullptr) {
      messageRouteInfo = *(element->getMessageRouteInfo());
    }
    messageRouteInfo.appendUnit(Settings.Unit);

    if (element->_topic.startsWith(F("traceroute/")) || element->_topic.indexOf(F("/traceroute/")) != -1) {
      // Special debug feature for ESPEasy-NOW to perform a traceroute of packets.
      // The message is prepended by each unit number handling the message.
      const String replacement = getValue(LabelType::UNIT_NR);
      String message;
      message.reserve(replacement.length() + 1 + element->_payload.length());
      message  = replacement;
      message += ';';
      message += element->_payload;

      processed = processMQTT_message(element->_controller_idx, element->_topic, message, element->_retained, &messageRouteInfo);
    } else {
      processed = processMQTT_message(element->_controller_idx, element->_topic, element->_payload, element->_retained, &messageRouteInfo);
    }
#else
    processed = processMQTT_message(element->_controller_idx, element->_topic, element->_payload, element->_retained);
#endif
  }
  MQTTDelayHandler->markProcessed(processed);
  if (processed) {
    statusLED(true);
  } else {
#ifndef BUILD_NO_DEBUG
    if (loglevelActiveFor(LOG_LEVEL_DEBUG)) {
      String log = F("MQTT : process MQTT queue not published, ");
      log += MQTTDelayHandler->sendQueue.size();
      log += F(" items left in queue");
      addLogMove(LOG_LEVEL_DEBUG, log);
    }
#endif // ifndef BUILD_NO_DEBUG
  }


  Scheduler.setIntervalTimerOverride(SchedulerIntervalTimer_e::TIMER_MQTT, 1); // Make sure the MQTT is being processed as soon as possible.
  scheduleNextMQTTdelayQueue();
  STOP_TIMER(MQTT_DELAY_QUEUE);
}


bool processMQTT_message(controllerIndex_t controllerIndex,
                        const String    & topic,
                        const String    & payload,
                        bool retained
#ifdef USES_ESPEASY_NOW
                        , const MessageRouteInfo_t* messageRouteInfo
#endif
                        ) 
{
  START_TIMER;
  bool processed = false;

  #ifdef USES_ESPEASY_NOW
  if (!MQTTclient_connected) {
    processed = ESPEasy_now_handler.sendToMQTT(controllerIndex, topic, payload, messageRouteInfo);
  }
  #endif

  if (!processed) {
    if (MQTTclient.publish(topic.c_str(), payload.c_str(), retained)) {
      // FIXME TD-er: Must check if connected via WiFi or Ethernet
      if (WiFiEventData.connectionFailures > 0) {
        --WiFiEventData.connectionFailures;
      }
#ifndef BUILD_NO_DEBUG
#ifdef USES_ESPEASY_NOW
      if (loglevelActiveFor(LOG_LEVEL_DEBUG) && messageRouteInfo != nullptr) {
        addLogMove(LOG_LEVEL_DEBUG, concat(F("MQTT : published from mesh: "), messageRouteInfo->toString()));
      }
#endif
#endif // ifndef BUILD_NO_DEBUG
      processed = true;
    }
  }
  Scheduler.setIntervalTimerOverride(SchedulerIntervalTimer_e::TIMER_MQTT, 10); // Make sure the MQTT is being processed as soon as possible.
  scheduleNextMQTTdelayQueue();
  STOP_TIMER(MQTT_DELAY_QUEUE);
  return processed;
}

void updateMQTTclient_connected() {
  if (MQTTclient_connected != MQTTclient.connected()) {
    MQTTclient_connected = !MQTTclient_connected;
    if (!MQTTclient_connected) {
      if (loglevelActiveFor(LOG_LEVEL_ERROR)) {
        String connectionError = F("MQTT : Connection lost, state: ");
        connectionError += getMQTT_state();
        addLogMove(LOG_LEVEL_ERROR, connectionError);
      }
      MQTTclient_must_send_LWT_connected = false;
    } else {
      // Now schedule all tasks using the MQTT controller.
      schedule_all_MQTTimport_tasks();
    }
    if (Settings.UseRules) {
      if (MQTTclient_connected) {
        eventQueue.add(F("MQTT#Connected"));
      } else {
        eventQueue.add(F("MQTT#Disconnected"));
      }
    }
  }
  if (!MQTTclient_connected) {
    // As suggested here: https://github.com/letscontrolit/ESPEasy/issues/1356
    if (timermqtt_interval < 30000) {
      timermqtt_interval += 5000;
    }
  } else {
    timermqtt_interval = 250;
  }
  Scheduler.setIntervalTimer(SchedulerIntervalTimer_e::TIMER_MQTT);
  scheduleNextMQTTdelayQueue();
}

void runPeriodicalMQTT() {
  // MQTT_KEEPALIVE = 15 seconds.
  if (!NetworkConnected(10)) {
    updateMQTTclient_connected();
    return;
  }
  //dont do this in backgroundtasks(), otherwise causes crashes. (https://github.com/letscontrolit/ESPEasy/issues/683)
  controllerIndex_t enabledMqttController = firstEnabledMQTT_ControllerIndex();
  if (validControllerIndex(enabledMqttController)) {
    if (!MQTTclient.loop()) {
      updateMQTTclient_connected();
      if (MQTTCheck(enabledMqttController)) {
        updateMQTTclient_connected();
      }
    }
  } else {
    if (MQTTclient.connected()) {
      MQTTclient.disconnect();
      updateMQTTclient_connected();
    }
  }
}


#endif //if FEATURE_MQTT



void logTimerStatistics() {
  static bool firstRun = true;
  if (firstRun) {
    Scheduler.setIntervalTimerOverride(SchedulerIntervalTimer_e::TIMER_STATISTICS, 1000);
    firstRun = false;
  }

# ifndef BUILD_NO_DEBUG
  const uint8_t loglevel = LOG_LEVEL_DEBUG;
#else
  const uint8_t loglevel = LOG_LEVEL_NONE;
#endif
  updateLoopStats_30sec(loglevel);
#ifndef BUILD_NO_DEBUG
//  logStatistics(loglevel, true);
  if (loglevelActiveFor(loglevel)) {
    String queueLog = F("Scheduler stats: (called/tasks/max_length/idle%) ");
    queueLog += Scheduler.getQueueStats();
    addLogMove(loglevel, queueLog);
  }
#endif
}

void updateLoopStats_30sec(uint8_t loglevel) {
  loopCounterLast = loopCounter;
  loopCounter = 0;
  if (loopCounterLast > loopCounterMax)
    loopCounterMax = loopCounterLast;

  Scheduler.updateIdleTimeStats();

#ifndef BUILD_NO_DEBUG
  if (loglevelActiveFor(loglevel)) {
    String log = F("LoopStats: shortestLoop: ");
    log += shortestLoop;
    log += F(" longestLoop: ");
    log += longestLoop;
    log += F(" avgLoopDuration: ");
    log += loop_usec_duration_total / loopCounter_full;
    log += F(" loopCounterMax: ");
    log += loopCounterMax;
    log += F(" loopCounterLast: ");
    log += loopCounterLast;
    addLogMove(loglevel, log);
  }
#endif
  loop_usec_duration_total = 0;
  loopCounter_full = 1;
}


/********************************************************************************************\
   Clean up all before going to sleep or reboot.
 \*********************************************************************************************/
void flushAndDisconnectAllClients() {
  if (anyControllerEnabled()) {
#if FEATURE_MQTT
    bool mqttControllerEnabled = validControllerIndex(firstEnabledMQTT_ControllerIndex());
#endif //if FEATURE_MQTT
    unsigned long timer = millis() + 1000;
    while (!timeOutReached(timer)) {
      // call to all controllers (delay queue) to flush all data.
      CPluginCall(CPlugin::Function::CPLUGIN_FLUSH, 0);
#if FEATURE_MQTT      
      if (mqttControllerEnabled && MQTTclient.connected()) {
        MQTTclient.loop();
      }
#endif //if FEATURE_MQTT
    }
#if FEATURE_MQTT
    if (mqttControllerEnabled && MQTTclient.connected()) {
      MQTTclient.disconnect();
      updateMQTTclient_connected();
    }
#endif //if FEATURE_MQTT
    saveToRTC();
    delay(100); // Flush anything in the network buffers.
  }
  process_serialWriteBuffer();
}


void prepareShutdown(IntendedRebootReason_e reason)
{
  WiFiEventData.intent_to_reboot = true;
#if FEATURE_MQTT
  runPeriodicalMQTT(); // Flush outstanding MQTT messages
#endif // if FEATURE_MQTT
  process_serialWriteBuffer();
  flushAndDisconnectAllClients();
  saveUserVarToRTC();
  setWifiMode(WIFI_OFF);
  #if FEATURE_ETHERNET
  ethPower(false);
  #endif
  ESPEASY_FS.end();
  process_serialWriteBuffer();
  delay(100); // give the node time to flush all before reboot or sleep
  node_time.now_();
  Scheduler.markIntendedReboot(reason);
  saveToRTC();
}


