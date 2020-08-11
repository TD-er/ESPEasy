#ifndef DATASTRUCTS_NODESTRUCT_H
#define DATASTRUCTS_NODESTRUCT_H

#include "../../ESPEasy_common.h"
#include "../Helpers/ESPEasy_time.h"
#include <map>
#include <IPAddress.h>


#define NODE_TYPE_ID_ESP_EASY_STD           1
#define NODE_TYPE_ID_RPI_EASY_STD           5 // https://github.com/enesbcs/rpieasy
#define NODE_TYPE_ID_ESP_EASYM_STD         17
#define NODE_TYPE_ID_ESP_EASY32_STD        33
#define NODE_TYPE_ID_ARDUINO_EASY_STD      65
#define NODE_TYPE_ID_NANO_EASY_STD         81


/*********************************************************************************************\
* NodeStruct
\*********************************************************************************************/
struct __attribute__((__packed__)) NodeStruct
{
  bool          validate();

  void          setLocalData();
  String        getNodeTypeDisplayString() const;

  String        getNodeName() const;

  IPAddress     IP() const;

  unsigned long getAge() const;

  float         getLoad() const;

  String        getSummary() const;


  // Do not change the order of this data, as it is being sent via P2P UDP.
  // 6 byte mac  (STA interface)
  // 4 byte ip
  // 1 byte unit
  // 2 byte build
  // 25 char name
  // 1 byte node type id

  // Added starting build '20107':
  // 2 bytes webserver port
  // 6 bytes AP MAC
  // 1 byte system load
  // 1 byte administrative distance


  uint8_t  mac[6]        = { 0 }; // STA mode MAC
  uint8_t  ip[4]         = { 0 };
  byte     unit          = 0;
  uint16_t build         = 0;
  byte     nodeName[25]  = { 0 };
  byte     nodeType      = 0;
  uint16_t webserverPort = 80;
  uint8_t  ap_mac[6]     = { 0 }; // AP mode MAC
  uint8_t  load          = 127;   // Default to average load
  uint8_t  distance      = 255;   // Administrative distance for routing
  uint8_t  timeSource    = static_cast<uint8_t>(timeSource_t::No_time_source);

  // When sending system info, this value contains the time since last time sync.
  // When kept as node info, this is the last time stamp the node info was updated.
  unsigned long lastUpdated = (1 << 30);
};
typedef std::map<byte, NodeStruct> NodesMap;


#endif // DATASTRUCTS_NODESTRUCT_H
