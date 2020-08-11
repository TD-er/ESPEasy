#ifndef DATASTRUCTS_ESPEASY_NOW_MESSAGE_HEADER_STRUCT_H
#define DATASTRUCTS_ESPEASY_NOW_MESSAGE_HEADER_STRUCT_H

/*********************************************************************************************\
* ESPEasy_now_message_struct
\*********************************************************************************************/

#include "../Globals/ESPEasy_now_state.h"
#ifdef USES_ESPEASY_NOW

# include <Arduino.h>

# define ESPEASY_NOW_HEADER_VERSION  1

struct ESPEasy_now_hdr {
  // Do not change the order of this enum as the value will be sent to other nodes.
  enum class message_t : uint8_t {
    NotSet = 0,
    Acknowledgement,
    Announcement,
    MQTTControllerMessage,

    ChecksumError = 255
  };

  ESPEasy_now_hdr();

  ESPEasy_now_hdr(message_t messageType);

  ESPEasy_now_hdr& operator=(const ESPEasy_now_hdr& other);

  void setChecksum();

  bool checksumValid() const;

private:

  uint8_t computeChecksum() const;

public:

  uint8_t header_version = ESPEASY_NOW_HEADER_VERSION; // To be used later to detect newer versions
  message_t     message_type   = message_t::NotSet;
  uint8_t       packet_nr      = 0;                          // Current message number (start at 0)
  uint8_t       nr_packets     = 1;                          // The highest message number of this sequence
  uint8_t       message_count  = 1;                          // A set of messages all have the same message_count
  uint8_t       notUsed1       = 0;                          // reserved
  uint8_t       checksum       = 0;                          // checksum
};

#endif // ifdef USES_ESPEASY_NOW

#endif // DATASTRUCTS_ESPEASY_NOW_MESSAGE_HEADER_STRUCT_H
