#ifndef MULTICAST_SENDER_H
#define MULTICAST_SENDER_H

#include "kmps_socket.h"
#include <cstdint>

class MulticastSender {
public:
  MulticastSender(uint32_t group_address, uint16_t port, uint32_t interface_address);
  ~MulticastSender();
  int init();
  int send_data(char *buffer, size_t buffer_len);
private:
  SOCKET m_sock;
  // Ip address (network byte-order) of interface to join multicast group on.
  uint32_t m_interface_address;
  // Multicast group address (network byte-order).
  uint32_t m_group_address;
  // Multicast port (network byte-order).
  uint16_t m_port;
  // Destination IP and Port for multicast messages.
  struct sockaddr_in m_target_sock;
};

#endif
