#ifndef PLUGINSTRUCTS_P094_FILTER_H
#define PLUGINSTRUCTS_P094_FILTER_H

#include "../../_Plugin_Helper.h"
#ifdef USES_P094

#include "../DataStructs/mBusPacket.h"

// Is stored, so do not change the int values.
enum class P094_Filter_Window : uint8_t {
  All             = 1, // Realtime, every message passes the filter
  Five_minutes    = 2, // a message passes the filter every 5 minutes, aligned to time (00:00:00, 00:05:00, ...)
  Fifteen_minutes = 3, // a message passes the filter every 15 minutes, aligned to time
  One_hour        = 4, // a message passes the filter every hour, aligned to time
  Day             = 5, // a message passes the filter once every day
                       // - between 00:00 and 12:00,
                       // - between 12:00 and 23:00 and
                       // - between 23:00 and 00:00
  Month = 6,           // a message passes the filter
                       // - between 1st of month 00:00:00 and 15th of month 00:00:00
                       // - between 15th of month 00:00:00 and last of month 00:00:00
                       // - between last of month 00:00:00 and 1st of next month 00:00:00
  Once = 7,            // only one message passes the filter until next reboot
  None = 0             // no messages pass the filter
};


// Examples for a filter definition list
//   EBZ.02.12345678;all
//   *.02.*;15m
//   TCH.44.*;Once
//   !TCH.*.*;None
//   *.*.*;5m

struct P094_filter {
  void           fromString(const String& str);
  String         toString() const;

  const uint8_t* toBinary(size_t& size) const;
  size_t         fromBinary(const uint8_t *data);

  // Is valid when it doesn't match: *.*.*;none
  bool           isValid() const;

  static size_t  getBinarySize();


  // Check to see if the manufacturer, metertype and serial matches.
  bool matches(const mBusPacket_header_t& other) const;

  // Check against the filter window.
  bool shouldPass();

  void WebformLoad(uint8_t filterIndex) const;
  bool WebformSave(uint8_t filterIndex);

  bool isWildcardManufacturer() const {
    return _filter._manufacturer == 0;
  }

  bool isWildcardMeterType() const {
    return _filter._meterType == 0;
  }

  bool isWildcardSerial() const {
    return _filter._serialNr == 0;
  }

  String getManufacturer() const;
  String getMeterType(bool includeHexPrefix) const;
  String getSerial(bool includeHexPrefix) const;

  // Keep this order of members as this is how it will be stored.
  union {
    // Defaults to no matching filter:
    //   *.*.*;none
    uint64_t _encodedValue{};
    struct {
      uint64_t _serialNr     : 32;
      uint64_t _manufacturer : 16;
      uint64_t _meterType    : 8;

      // Use for filtering
      uint64_t _filterWindow : 8;
    };
  } _filter;

  unsigned long _lastSeenUnixTime{};
};

#endif

#endif