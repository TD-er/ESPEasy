#include "ESPEasy_now_merger.h"

#ifdef USES_ESPEASY_NOW

# include "../Helpers/ESPEasy_time_calc.h"
# include "../../ESPEasy_fdwdecl.h"

# define ESPEASY_NOW_MESSAGE_TIMEOUT  5000

ESPEasy_now_merger::ESPEasy_now_merger() {
  _firstPacketTimestamp = millis();
}

void ESPEasy_now_merger::addPacket(
  uint8_t            packet_nr,
  const MAC_address& mac,
  const uint8_t     *buf,
  size_t             packetSize)
{
  const uint16_t maxFreeBlock = ESP.getMaxFreeBlockSize();

  if (2 * packetSize > maxFreeBlock) {
    // Not enough free memory to process the block.
    // Since this message will never be complete, set the timer to an expired value.
    _firstPacketTimestamp -= ESPEASY_NOW_MESSAGE_TIMEOUT;
    return;
  }

  _queue.emplace(std::make_pair(packet_nr, ESPEasy_Now_packet(mac, buf, packetSize)));
  _firstPacketTimestamp = millis();
}

bool ESPEasy_now_merger::messageComplete() const
{
  return _queue.size() >= getFirstHeader().nr_packets;
}

bool ESPEasy_now_merger::expired() const
{
  return timePassedSince(_firstPacketTimestamp) > ESPEASY_NOW_MESSAGE_TIMEOUT;
}

uint8_t ESPEasy_now_merger::receivedCount(uint8_t& nr_packets) const
{
  nr_packets = getFirstHeader().nr_packets;
  return _queue.size();
}

ESPEasy_Now_packet_map::const_iterator ESPEasy_now_merger::find(uint8_t packet_nr) const
{
  return _queue.find(packet_nr);
}

ESPEasy_now_hdr ESPEasy_now_merger::getFirstHeader() const
{
  ESPEasy_now_hdr header;
  auto it = _queue.find(0);

  if (it != _queue.end()) {
    header = it->second.getHeader();
  }
  return header;
}

unsigned long ESPEasy_now_merger::getFirstPacketTimestamp() const
{
  return _firstPacketTimestamp;
}

bool ESPEasy_now_merger::getMac(uint8_t *mac) const
{
  auto it = _queue.find(0);

  if (it == _queue.end()) {
    return false;
  }
  memcpy(mac, it->second._mac, 6);
  return true;
}

bool ESPEasy_now_merger::getMac(MAC_address& mac) const
{
  return getMac(mac.mac);
}

String ESPEasy_now_merger::getLogString() const
{
  MAC_address mac;

  getMac(mac);
  String log;
  log += mac.toString();
  log += F(" payload: ");
  log += getPayloadSize();
  log += F(" (");
  log += _queue.size();
  log += '/';
  log += getFirstHeader().nr_packets;
  log += ')';
  return log;
}

size_t ESPEasy_now_merger::getPayloadSize() const
{
  if (!messageComplete()) { return 0; }
  size_t payloadSize = 0;

  for (auto it = _queue.begin(); it != _queue.end(); ++it) {
    payloadSize += it->second.getPayloadSize();
  }
  return payloadSize;
}

String ESPEasy_now_merger::getString(size_t& payload_pos) const
{
  String res;

  getString(res, payload_pos);
  return res;
}

bool ESPEasy_now_merger::getString(String& string, size_t& payload_pos) const
{
  size_t stringLength = 0;
  {
    // Compute the expected string size, so we don't have to perform re-allocations
    size_t tmp_payload_pos = payload_pos;
    stringLength = str_len(tmp_payload_pos);

    if (stringLength == 0) {
      return false;
    }
    string.reserve(stringLength);
  }

  size_t bufsize = 128;

  if (stringLength < bufsize) {
    bufsize = stringLength;
  }
  std::vector<uint8_t> buf;
  buf.resize(bufsize);

  bool done = false;

  // We do fetch more data from the message than the string size, so copy payload_pos first
  size_t tmp_payload_pos = payload_pos;

  while (!done) {
    size_t received = getBinaryData(&buf[0], bufsize, tmp_payload_pos);

    for (size_t buf_pos = 0; buf_pos < received && !done; ++buf_pos) {
      char c = static_cast<char>(buf[buf_pos]);

      if (c == 0) {
        done = true;
      } else {
        string += c;
      }
    }

    if (received < bufsize) { done = true; }
  }
  payload_pos += string.length() + 1; // Store the position of the null termination
  return true;
}

size_t ESPEasy_now_merger::str_len(size_t& payload_pos) const
{
  const size_t bufsize = 128;
  std::vector<uint8_t> buf;

  buf.resize(bufsize);

  bool done = false;

  // We do fetch more data from the message than the string size, so copy payload_pos first
  size_t tmp_payload_pos = payload_pos;
  size_t res             = 0;

  while (!done) {
    size_t received = getBinaryData(&buf[0], bufsize, tmp_payload_pos);

    for (size_t buf_pos = 0; buf_pos < received && !done; ++buf_pos) {
      char c = static_cast<char>(buf[buf_pos]);

      if (c == 0) {
        done = true;
      } else {
        ++res;
      }
    }

    if (received < bufsize) { done = true; }
  }
  payload_pos += res + 1; // Store the position of the null termination
  return res;
}

size_t ESPEasy_now_merger::getBinaryData(uint8_t *data, size_t length, size_t& payload_pos) const
{
  size_t  payload_pos_in_packet;
  uint8_t packet_nr = findPacketWithPayloadPos(payload_pos, payload_pos_in_packet);

  if (packet_nr >= getFirstHeader().nr_packets) {
    return 0;
  }

  auto   it       = _queue.find(packet_nr);
  size_t data_pos = 0;

  while (it != _queue.end() && data_pos < length) {
    size_t added_length = it->second.getBinaryData(data, length - data_pos, payload_pos_in_packet);
    data     += added_length;
    data_pos += added_length;

    // Continue in next packet
    payload_pos_in_packet = 0;
    ++packet_nr;
    it = _queue.find(packet_nr);
  }
  payload_pos += data_pos;
  return data_pos;
}

uint8_t ESPEasy_now_merger::findPacketWithPayloadPos(size_t payload_pos, size_t& payload_pos_in_packet) const
{
  // First find the place in the queue to continue based on the payload_pos
  uint8_t packet_nr = 0;

  payload_pos_in_packet = 0;
  auto it = _queue.find(packet_nr);

  // Position in message payload at the start of a packet
  size_t packet_start_payload_pos = 0;

  while (it != _queue.end() && packet_start_payload_pos <= payload_pos) {
    if ((packet_start_payload_pos + it->second.getPayloadSize()) >= payload_pos) {
      // Message payload position is in current packet.
      payload_pos_in_packet = payload_pos - packet_start_payload_pos;
      return packet_nr;
    }
    packet_start_payload_pos += it->second.getPayloadSize();

    ++packet_nr;
    it = _queue.find(packet_nr);
  }
  return 255; // Error value
}

#endif // ifdef USES_ESPEASY_NOW
