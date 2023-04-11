#include <iostream>
#include "MulticastListener.h"

MulticastListener::MulticastListener(uint32_t group_address, uint16_t port, uint32_t interface_address) :
  m_sock(-1),
  m_group_address(group_address),
  m_port(port),
  m_interface_address(interface_address),
  m_is_in_group(false)
{}

MulticastListener::~MulticastListener()
{
  if (m_is_in_group && kmps_leave_mcast(m_sock, m_interface_address, m_group_address) < 0) {
      kmps_print_error();
  }

  if (m_sock > -1) {
    kmps_close(m_sock);
  }
}

int MulticastListener::init()
{
  if ((m_sock = kmps_socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
    std::cout << "Failed to create socket\n";
    kmps_print_error();
    return -1;
  }

  if (kmps_join_mcast(m_sock, m_interface_address, m_group_address, m_port) == INVALID_SOCKET) {
    std::cout << "Failed to join multicast group\n";
    kmps_print_error();
    return -1;
  }
  m_is_in_group = true;
  return 0;
}

int MulticastListener::receive_data(char *buffer, size_t buffer_len) {
  return recv(m_sock, buffer, buffer_len, 0);
}
