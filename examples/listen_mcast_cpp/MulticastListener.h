#ifndef MULTICAST_LISTENER_H
#define MULTICAST_LISTENER_H

#include "kmps_socket.h"
#include <cstdint>

class MulticastListener {
public:
  MulticastListener(uint32_t group_address, uint16_t port, uint32_t interface_address);
  ~MulticastListener();
  int init();
  int receive_data(char *buffer, size_t buffer_len);
private:
  SOCKET m_sock;
  // Ip address (network byte-order) of interface to join multicast group on.
  uint32_t m_interface_address;
  // Multicast group address (network byte-order).
  uint32_t m_group_address;
  // Multicast port (network byte-order).
  uint16_t m_port;
  // True if could successfully join group
  bool m_is_in_group;
};

#endif // MULTICAST_LISTENER_H
