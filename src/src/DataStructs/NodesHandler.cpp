#include "NodesHandler.h"

#include "../../ESPEasy_common.h"
#include "../../ESPEasy-Globals.h"
#include "../../ESPEasy_fdwdecl.h"
#include "../ESPEasyCore/ESPEasy_Log.h"
#include "../ESPEasyCore/ESPEasyNetwork.h"
#include "../ESPEasyCore/ESPEasyWifi.h"
#include "../Helpers/ESPEasy_time_calc.h"
#include "../Helpers/PeriodicalActions.h"
#include "../Globals/ESPEasy_time.h"
#include "../Globals/ESPEasy_now_peermanager.h"
#include "../Globals/ESPEasy_now_state.h"
#include "../Globals/ESPEasyWiFiEvent.h"
#include "../Globals/MQTT.h"
#include "../Globals/RTC.h"
#include "../Globals/Settings.h"
#include "../Globals/WiFi_AP_Candidates.h"

bool NodesHandler::addNode(const NodeStruct& node)
{
  int8_t rssi = 0;
  MAC_address match_sta;
  MAC_address match_ap;
  MAC_address ESPEasy_NOW_MAC;

  bool isNewNode = true;

  // Erase any existing node with matching MAC address
  for (auto it = _nodes.begin(); it != _nodes.end(); )
  {
    const MAC_address sta = it->second.sta_mac;
    const MAC_address ap  = it->second.ap_mac;
    if ((!sta.all_zero() && node.match(sta)) || (!ap.all_zero() && node.match(ap))) {
      rssi = it->second.getRSSI();
      if (!sta.all_zero())
        match_sta = sta;
      if (!ap.all_zero())
        match_ap = ap;
      ESPEasy_NOW_MAC = it->second.ESPEasy_Now_MAC();

      isNewNode = false;
      it = _nodes.erase(it);
    } else {
      ++it;
    }
  }
  _nodes[node.unit]             = node;
  _nodes[node.unit].lastUpdated = millis();
  if (node.getRSSI() >= 0 && rssi < 0) {
    _nodes[node.unit].setRSSI(rssi);
  }
  const MAC_address node_ap(node.ap_mac);
  if (node_ap.all_zero()) {
    _nodes[node.unit].setAP_MAC(node_ap);
  }
  if (node.ESPEasy_Now_MAC().all_zero()) {
    _nodes[node.unit].setESPEasyNow_mac(ESPEasy_NOW_MAC);
  }
  return isNewNode;
}

#ifdef USES_ESPEASY_NOW
bool NodesHandler::addNode(const NodeStruct& node, const ESPEasy_now_traceroute_struct& traceRoute)
{
  const bool isNewNode = addNode(node);
  _nodeStats[node.unit].setDiscoveryRoute(node.unit, traceRoute);

  ESPEasy_now_peermanager.addPeer(node.ESPEasy_Now_MAC(), node.channel);  

  if (!node.isThisNode()) {
    if (traceRoute.getDistance() != 255) {
      if (loglevelActiveFor(LOG_LEVEL_INFO)) {
        String log;
        if (log.reserve(80)) {
          log  = F(ESPEASY_NOW_NAME);
          log += F(": Node: ");
          log += String(node.unit);
          log += F(" Traceroute received: ");
          log += _nodeStats[node.unit].latestRoute().toString();
          addLog(LOG_LEVEL_INFO, log);
        }
      }
    } else {}
  }
  return isNewNode;
}
#endif

bool NodesHandler::hasNode(uint8_t unit_nr) const
{
  return _nodes.find(unit_nr) != _nodes.end();
}

bool NodesHandler::hasNode(const uint8_t *mac) const
{
  return getNodeByMac(mac) != nullptr;
}

NodeStruct * NodesHandler::getNode(uint8_t unit_nr)
{
  auto it = _nodes.find(unit_nr);

  if (it == _nodes.end()) {
    return nullptr;
  }
  return &(it->second);
}

const NodeStruct * NodesHandler::getNode(uint8_t unit_nr) const
{
  auto it = _nodes.find(unit_nr);

  if (it == _nodes.end()) {
    return nullptr;
  }
  return &(it->second);
}

NodeStruct * NodesHandler::getNodeByMac(const MAC_address& mac)
{
  if (mac.all_zero()) {
    return nullptr;
  }
  delay(0);

  for (auto it = _nodes.begin(); it != _nodes.end(); ++it)
  {
    if (mac == it->second.sta_mac) {
      return &(it->second);
    }

    if (mac == it->second.ap_mac) {
      return &(it->second);
    }
  }
  return nullptr;
}

const NodeStruct * NodesHandler::getNodeByMac(const MAC_address& mac) const
{
  bool match_STA;

  return getNodeByMac(mac, match_STA);
}

const NodeStruct * NodesHandler::getNodeByMac(const MAC_address& mac, bool& match_STA) const
{
  if (mac.all_zero()) {
    return nullptr;
  }
  delay(0);

  for (auto it = _nodes.begin(); it != _nodes.end(); ++it)
  {
    if (mac == it->second.sta_mac) {
      match_STA = true;
      return &(it->second);
    }

    if (mac == it->second.ap_mac) {
      match_STA = false;
      return &(it->second);
    }
  }
  return nullptr;
}

const NodeStruct * NodesHandler::getPreferredNode() const {
  MAC_address dummy;

  return getPreferredNode_notMatching(dummy);
}

const NodeStruct * NodesHandler::getPreferredNode_notMatching(const MAC_address& not_matching) const {
  MAC_address this_mac;

  WiFi.macAddress(this_mac.mac);
  const NodeStruct *thisNode = getNodeByMac(this_mac);
  const NodeStruct *reject   = getNodeByMac(not_matching);

  const NodeStruct *res = nullptr;

  for (auto it = _nodes.begin(); it != _nodes.end(); ++it)
  {
    if ((&(it->second) != reject) && (&(it->second) != thisNode)) {
      bool mustSet = false;
      if (res == nullptr) {
        mustSet = true;
      } else {
        #ifdef USES_ESPEASY_NOW

        uint8_t distance_new, distance_res = 0;

        const int successRate_new = getRouteSuccessRate(it->second.unit, distance_new);
        const int successRate_res = getRouteSuccessRate(res->unit, distance_res);

        if (distance_new == distance_res) {
          if (successRate_new > successRate_res) {
            mustSet = true;
          }
        } else if (distance_new < distance_res) {
          mustSet = true;
        }
        #else
        if (it->second < *res) {
            mustSet = true;
        }
        #endif
      }
      if (mustSet) {
        #ifdef USES_ESPEASY_NOW
        if (it->second.ESPEasyNowPeer && hasTraceRoute(it->second.unit)) {
          res = &(it->second);
        }
        #else
        res = &(it->second);
        #endif
      }
    }
  }

/*
  #ifdef USES_ESPEASY_NOW
  if (res != nullptr)
  {
    uint8_t distance_res = 255;
    const int successRate_res = getRouteSuccessRate(res->unit, distance_res);
    if (distance_res == 255) {
      return nullptr;
    }
  }
  #endif
*/

  return res;
}

#ifdef USES_ESPEASY_NOW
const ESPEasy_now_traceroute_struct* NodesHandler::getTraceRoute(uint8_t unit) const
{
  auto trace_it = _nodeStats.find(unit);
  if (trace_it == _nodeStats.end()) {
    return nullptr;
  }
  return trace_it->second.bestRoute();
}

const ESPEasy_now_traceroute_struct* NodesHandler::getDiscoveryRoute(uint8_t unit) const
{
  auto trace_it = _nodeStats.find(unit);
  if (trace_it == _nodeStats.end()) {
    return nullptr;
  }
  return &(trace_it->second.discoveryRoute());
}

void NodesHandler::setTraceRoute(const MAC_address& mac, const ESPEasy_now_traceroute_struct& traceRoute)
{
  if (traceRoute.computeSuccessRate() == 0) {
    // No need to store traceroute with low success rate.
    return;
  }
  NodeStruct* node = getNodeByMac(mac);
  if (node != nullptr) {
    auto trace_it = _nodeStats.find(node->unit);
    if (trace_it != _nodeStats.end()) {
      _lastTimeValidDistance = millis();
      trace_it->second.addRoute(node->unit, traceRoute);
    }
  }
}

#endif


void NodesHandler::updateThisNode() {
  NodeStruct thisNode;

  // Set local data
  WiFi.macAddress(thisNode.sta_mac);
  WiFi.softAPmacAddress(thisNode.ap_mac);
  {
    bool addIP = NetworkConnected();
    #ifdef USES_ESPEASY_NOW
    if (use_EspEasy_now) {
      thisNode.useAP_ESPEasyNow = 1;
    }
    #endif
    if (addIP) {
      IPAddress localIP = NetworkLocalIP();

      for (byte i = 0; i < 4; ++i) {
        thisNode.ip[i] = localIP[i];
      }
    }
  }
  thisNode.channel = WiFiEventData.usedChannel;
  if (thisNode.channel == 0) {
    thisNode.channel = WiFi.channel();
  }

  thisNode.unit  = Settings.Unit;
  thisNode.build = Settings.Build;
  memcpy(thisNode.nodeName, Settings.Name, 25);
  thisNode.nodeType = NODE_TYPE_ID;

  thisNode.webgui_portnumber = Settings.WebserverPort;
  int load_int = getCPUload() * 2.55;

  if (load_int > 255) {
    thisNode.load = 255;
  } else {
    thisNode.load = load_int;
  }
  thisNode.timeSource = static_cast<uint8_t>(node_time.timeSource);

  switch (node_time.timeSource) {
    case timeSource_t::No_time_source:
      thisNode.lastUpdated = (1 << 30);
      break;
    default:
    {
      thisNode.lastUpdated = timePassedSince(node_time.lastSyncTime);
      break;
    }
  }
  #ifdef USES_ESPEASY_NOW
  if (Settings.UseESPEasyNow()) {
    thisNode.ESPEasyNowPeer = 1;
  }
  #endif

  const uint8_t lastDistance = _distance;
  #ifdef USES_ESPEASY_NOW
  ESPEasy_now_traceroute_struct thisTraceRoute;
  #endif
  if (isEndpoint()) {
    _distance = 0;
    _lastTimeValidDistance = millis();
    if (lastDistance != _distance) {
      _recentlyBecameDistanceZero = true;
    }
    #ifdef USES_ESPEASY_NOW
    thisNode.distance = _distance;
    thisNode.setRSSI(WiFi.RSSI());
    thisTraceRoute.addUnit(thisNode.unit);
    #endif
  } else {
    _distance = 255;
    #ifdef USES_ESPEASY_NOW
    const NodeStruct *preferred = getPreferredNode_notMatching(thisNode.sta_mac);

    if (preferred != nullptr) {
      if (!preferred->isExpired()) {
        const ESPEasy_now_traceroute_struct* tracert_ptr = getTraceRoute(preferred->unit);
        if (tracert_ptr != nullptr && tracert_ptr->getDistance() < 255) {
          // Make a copy of the traceroute
          thisTraceRoute = *tracert_ptr;
          thisTraceRoute.addUnit(thisNode.unit);

          _distance = thisTraceRoute.getDistance();  // This node is already included in the traceroute.
        }
/*
        if (_distance != lastDistance) {
          if (isESPEasy_now_only() && WiFiConnected()) {
            // We are connected to a 'fake AP' for ESPEasy-NOW, but found a known AP
            // Try to reconnect to it.
            RTC.clearLastWiFi(); // Force a WiFi scan
            WifiDisconnect();
          }
        }
        */
      }
    }
    #endif
  }
  thisNode.distance = _distance;

  #ifdef USES_ESPEASY_NOW
  addNode(thisNode, thisTraceRoute);
  if (thisNode.distance == 0) {
    // Since we're the end node, claim highest success rate
    updateSuccessRate(thisNode.unit, 255);
  }
  #else
  addNode(thisNode);
  #endif
}

const NodeStruct * NodesHandler::getThisNode() {
  updateThisNode();
  MAC_address this_mac;
  WiFi.macAddress(this_mac.mac);
  return getNodeByMac(this_mac.mac);
}

uint8_t NodesHandler::getDistance() const {
  // Perform extra check since _distance is only updated once every 30 seconds.
  // And we don't want to tell other nodes we have distance 0 when we haven't.
  if (isEndpoint()) return 0;
  if (_distance == 0) {
    // Outdated info, so return "we don't know"
    return 255;
  }
  return _distance;
}


NodesMap::const_iterator NodesHandler::begin() const {
  return _nodes.begin();
}

NodesMap::const_iterator NodesHandler::end() const {
  return _nodes.end();
}

NodesMap::const_iterator NodesHandler::find(uint8_t unit_nr) const
{
  return _nodes.find(unit_nr);
}

bool NodesHandler::refreshNodeList(unsigned long max_age_allowed, unsigned long& max_age)
{
  max_age = 0;
  bool nodeRemoved = false;

  for (auto it = _nodes.begin(); it != _nodes.end();) {
    unsigned long age = it->second.getAge();
    if (age > max_age_allowed) {
      bool mustErase = true;
      #ifdef USES_ESPEASY_NOW
      auto route_it = _nodeStats.find(it->second.unit);
      if (route_it != _nodeStats.end()) {
        if (route_it->second.getAge() > max_age_allowed) {
          _nodeStats.erase(route_it);
        } else {
          mustErase = false;
        }
      }
      #endif
      if (mustErase) {
        it          = _nodes.erase(it);
        nodeRemoved = true;
      }
    } else {
      ++it;

      if (age > max_age) {
        max_age = age;
      }
    }
  }
  return nodeRemoved;
}

// FIXME TD-er: should be a check per controller to see if it will accept messages
bool NodesHandler::isEndpoint() const
{
  // FIXME TD-er: Must check controller to see if it needs wifi (e.g. LoRa or cache controller do not need it)
  #ifdef USES_MQTT
  controllerIndex_t enabledMqttController = firstEnabledMQTT_ControllerIndex();
  if (validControllerIndex(enabledMqttController)) {
    // FIXME TD-er: Must call updateMQTTclient_connected() and see what effect
    // the MQTTclient_connected state has when using ESPEasy-NOW.
    return MQTTclient.connected();
  }
  #endif

  if (!WiFiConnected()) return false;

  return false;
}

#ifdef USES_ESPEASY_NOW
uint8_t NodesHandler::getESPEasyNOW_channel() const
{
  if (isEndpoint()) {
    return WiFi.channel();
  }
  const NodeStruct *preferred = getPreferredNode();
  if (preferred != nullptr) {
    uint8_t distance = 255;
    getRouteSuccessRate(preferred->unit, distance);

    if (distance < 255) {
      return preferred->channel;
    }
  }
  return 0;
}
#endif

bool NodesHandler::recentlyBecameDistanceZero() {
  if (!_recentlyBecameDistanceZero) {
    return false;
  }
  _recentlyBecameDistanceZero = false;
  return true;
}

void NodesHandler::setRSSI(const MAC_address& mac, int rssi)
{
  setRSSI(getNodeByMac(mac), rssi);
}

void NodesHandler::setRSSI(uint8_t unit, int rssi)
{
  setRSSI(getNode(unit), rssi);
}

void NodesHandler::setRSSI(NodeStruct * node, int rssi)
{
  if (node != nullptr) {
    node->setRSSI(rssi);
  }
}

bool NodesHandler::lastTimeValidDistanceExpired() const
{
//  if (_lastTimeValidDistance == 0) return false;
  return timePassedSince(_lastTimeValidDistance) > 120000; // 2 minutes
}

#ifdef USES_ESPEASY_NOW
bool NodesHandler::hasTraceRoute(uint8_t unit) const
{
  return _nodeStats.find(unit) != _nodeStats.end();  
}

void NodesHandler::updateSuccessRate(byte unit, bool success)
{
  auto it = _nodeStats.find(unit);
  if (it != _nodeStats.end()) {
    it->second.updateSuccessRate(unit, success);
  }
}

void NodesHandler::updateSuccessRate(const MAC_address& mac, bool success)
{
  const NodeStruct * node = getNodeByMac(mac);
  if (node == nullptr) {
    return;
  }
  updateSuccessRate(node->unit, success);
}

int NodesHandler::getRouteSuccessRate(byte unit, uint8_t& distance) const
{
  distance = 255;
  auto it = _nodeStats.find(unit);
  if (it != _nodeStats.end()) {
    const ESPEasy_now_traceroute_struct* route = it->second.bestRoute();
    if (route != nullptr) {
      distance = route->getDistance();
      return route->computeSuccessRate();
    }
  }
  return 0;
}

uint8_t NodesHandler::getSuccessRate(byte unit) const
{
  auto it = _nodeStats.find(unit);
  if (it != _nodeStats.end()) {
    return it->second.getNodeSuccessRate();
  }
  return 127;
}

ESPEasy_Now_MQTT_queue_check_packet::QueueState NodesHandler::getMQTTQueueState(byte unit) const
{
  auto it = _nodeStats.find(unit);
  if (it != _nodeStats.end()) {
    return it->second.getMQTTQueueState();
  }
  return ESPEasy_Now_MQTT_queue_check_packet::QueueState::Unset;

}

void NodesHandler::setMQTTQueueState(byte unit, ESPEasy_Now_MQTT_queue_check_packet::QueueState state)
{
  auto it = _nodeStats.find(unit);
  if (it != _nodeStats.end()) {
    it->second.setMQTTQueueState(state);
  }
}

void NodesHandler::setMQTTQueueState(const MAC_address& mac, ESPEasy_Now_MQTT_queue_check_packet::QueueState state)
{
  const NodeStruct * node = getNodeByMac(mac);
  if (node != nullptr) {
    setMQTTQueueState(node->unit, state);
  }
}


#endif
