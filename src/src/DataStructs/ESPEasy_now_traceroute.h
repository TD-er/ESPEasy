#ifndef DATASTRUCTS_ESPEASY_NOW_TRACEROUTE_H
#define DATASTRUCTS_ESPEASY_NOW_TRACEROUTE_H

#include "../../ESPEasy_common.h"

#include <vector>
#include <map>

/*********************************************************************************************\
* ESPEasy-NOW Trace route
\*********************************************************************************************/
struct ESPEasy_now_traceroute_struct
{
  ESPEasy_now_traceroute_struct();

  ESPEasy_now_traceroute_struct(uint8_t size);

  // Return the unit and RSSI given a distance.
  // @retval 0 when giving invalid distance.
  uint8_t getUnit(uint8_t distance,
                  int8_t& rssi) const;

  // Append unit at the end (thus furthest distance)
  void           addUnit(uint8_t unit,
                         int8_t  rssi = 0);

  uint8_t        getDistance() const;

  const uint8_t* getData(uint8_t& size) const;

  // Get pointer to the raw data.
  // Make sure the array is large enough to store the data.
  uint8_t* get();

  void     setRSSI_last_node(int8_t rssi) const;

  // Return true when this traceroute has lower penalty (thus more favorable)
  bool     operator<(const ESPEasy_now_traceroute_struct& other) const;

  // Remove duplicate entries (e.g. loops)
  void     sanetize();

  // For debugging purposes
  String toString() const;

  // Compute penalty. Higher value means less preferred.
  int compute_penalty() const;

private:

  // Node with distance 0 at front, so index/2 equals distance.
  // index%2 == 0 is unit
  // index%2 == 1 is RSSI
  // Made mutable so setRSSI_last_node can remain const.
  mutable std::vector<uint8_t>unit_vector;
};

typedef std::map<byte, ESPEasy_now_traceroute_struct> TraceRouteMap;

#endif // ifndef DATASTRUCTS_ESPEASY_NOW_TRACEROUTE_H
