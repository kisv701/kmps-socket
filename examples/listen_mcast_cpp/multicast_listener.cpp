#include <iostream>
#include "kmps_socket.h"
#include "MulticastListener.h"

int main()
{
  const uint16_t port = htons(45612);
  const char loopback_ip[] = "127.0.0.1";
  uint32_t loopback_addr = 0;
  if (kmps_inet_pton(AF_INET, loopback_ip, &loopback_addr) != 1) {
    std::cout << "Bad IP string " << loopback_ip << "\n";
  }
  const char MCAST_GROUP[] = "239.3.2.1";
  uint32_t group_address = 0;
  if (kmps_inet_pton(AF_INET, MCAST_GROUP, &group_address) != 1) {
    std::cout << "Bad IP string " << MCAST_GROUP << "\n";
  }

  MulticastListener multicast_listener(group_address, port, loopback_addr);
  if (multicast_listener.init() < 0) {
    std::cout << "Failed to initialize MulticastListener\n";
    return -1;
  }
  std::cout << "Listening for messages\n";

  for(int i = 0; i < 100; i++) {
    char message[100];
    int bytes_received = multicast_listener.receive_data((char *)message, sizeof(message));
    std::cout << "Received " <<  bytes_received << " bytes: " << message << "\n";
  }
  kmps_clean_up();
  std::cout << "Good bye from Multicast listener!\n";
  return 0;
}
