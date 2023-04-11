#include <iostream>
#include "MulticastSender.h"

MulticastSender::MulticastSender(uint32_t group_address, uint16_t port, uint32_t interface_address) :
  m_sock(-1),
  m_group_address(group_address),
  m_port(port),
  m_interface_address(interface_address),
  m_target_sock()
{
  m_target_sock.sin_family = AF_INET;
  m_target_sock.sin_addr.s_addr = m_group_address;
  m_target_sock.sin_port = m_port;
}

MulticastSender::~MulticastSender()
{
  if (m_sock > -1) {
    kmps_close(m_sock);
  }
}
int MulticastSender::init()
{
  if ((m_sock = kmps_socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
    std::cout << "Failed to create socket\n";
    kmps_print_error();
    return -1;
  }

  struct in_addr interface = {0};
  interface.s_addr = m_interface_address;

  if(setsockopt(m_sock, IPPROTO_IP, IP_MULTICAST_IF, (char *)&interface, sizeof(interface)) == SOCKET_ERROR) {
    std::cout << "Failed to setup interface for IP Multicast\n";
    kmps_print_error();
    return -1;
  }

  return 0; // Success
}

int MulticastSender::send_data(char *buffer, size_t buffer_len)
{
  return sendto(m_sock, buffer, buffer_len, 0, (struct sockaddr*)&m_target_sock, sizeof(m_target_sock));
}
