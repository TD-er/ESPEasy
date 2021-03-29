#ifndef DATASTRUCTS_ESPEASY_NOW_NODE_STATISTICS_H
#define DATASTRUCTS_ESPEASY_NOW_NODE_STATISTICS_H

#include "../../ESPEasy_common.h"

#include "../DataStructs/ESPEasy_now_traceroute.h"

#include <map>

#define ESPEASY_NOW_NODE_STATISTICS_NR_ROUTES  2

struct ESPEasy_now_Node_statistics_t {

  unsigned long getAge() const;

  void addRoute(byte unit, const ESPEasy_now_traceroute_struct& route);

  void updateSuccessRate(byte unit, bool success);

  ESPEasy_now_traceroute_struct& latestRoute();

  const ESPEasy_now_traceroute_struct& latestRoute() const;

  const ESPEasy_now_traceroute_struct* bestRoute() const;

  ESPEasy_now_traceroute_struct routes[ESPEASY_NOW_NODE_STATISTICS_NR_ROUTES];

  unsigned long last_update = 0;

  unsigned long last_update_route = 0;

  int bestRouteSuccessRate = 0;

  uint8_t last_route_index = 0;

  // Increase on success, decrease on fail, with limits of 255 and 0.
  uint8_t success_rate = 127;
};

typedef std::map<byte, ESPEasy_now_Node_statistics_t> ESPEasy_now_Node_statisticsMap;


#endif // ifndef DATASTRUCTS_ESPEASY_NOW_NODE_STATISTICS_H