#include <iostream>
#include "kmps_socket.h"
#include "MulticastSender.h"

#ifdef _WIN32
#include <windows.h>
#define MILLI_SLEEP(x) Sleep(x)
#else
#include <unistd.h>
#define MILLI_SLEEP(x) usleep(x*1000)
#endif


int main()
{
  const uint16_t PORT = htons(45612);
  const char LOOPBACK_IP[] = "127.0.0.1";
  uint32_t loopback_addr = 0;
  if (kmps_inet_pton(AF_INET, LOOPBACK_IP, &loopback_addr) != 1) {
    std::cout << "Bad IP string " << LOOPBACK_IP << "\n";
  }
  const char MCAST_GROUP[] = "239.3.2.1";
  uint32_t group_address = 0;
  if (kmps_inet_pton(AF_INET, MCAST_GROUP, &group_address) != 1) {
    std::cout << "Bad IP string " << MCAST_GROUP << "\n";
  }

  MulticastSender multicast_sender(group_address, PORT, loopback_addr);
  if (multicast_sender.init() < 0) {
    std::cout << "Failed to initialize MulticastSender\n";
    return -1;
  }
  std::cout << "Listening for messages\n";

  constexpr size_t MESSAGE_COUNT = 100;
  constexpr size_t MESSAGE_MAX_LEN = 100;
  for(int i = 0; i < MESSAGE_COUNT; i++) {
    char message[MESSAGE_MAX_LEN];
    snprintf(message, MESSAGE_MAX_LEN, "Multicast hello number %d.", i);
    int bytes_sent = multicast_sender.send_data((char *)message, sizeof(message));
    std::cout << "Sent " <<  bytes_sent << " bytes: " << message << "\n";
    MILLI_SLEEP(50);
  }
  kmps_clean_up();
  std::cout << "Good bye from Multicast talker!\n";
  return 0;
}
