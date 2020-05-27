#include "NodesHandler.h"

void NodesHandler::addNode(const NodeStruct& node)
{
  _nodes[node.unit]             = node;
  _nodes[node.unit].lastUpdated = millis();
}

bool NodesHandler::hasNode(uint8_t unit_nr) const
{
  return _nodes.find(unit_nr) != _nodes.end();
}

bool NodesHandler::hasNode(const uint8_t *mac) const
{
  return getNodeByMac(mac) != nullptr;
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
      it          = _nodes.erase(it);
      nodeRemoved = true;
    } else {
      ++it;

      if (age > max_age) {
        max_age = age;
      }
    }
  }
  return nodeRemoved;
}
