#include "stdio.h"
#include "kmps_socket.h"

#ifdef _WIN32
#include <windows.h>
#define sleep(x) Sleep(x*1000)
#else
#include <unistd.h> // sleep
#endif

int main()
{
  const uint16_t PORT = htons(51234);
  const char LOOPBACK_IP[] = "127.0.0.1";
  uint32_t loopback_addr = 0;
  int sock = kmps_socket(AF_INET, SOCK_DGRAM, 0);
  if (kmps_inet_pton(AF_INET, LOOPBACK_IP, &loopback_addr) != 1) {
    printf("Bad IP string %s\n", LOOPBACK_IP);
  }

  printf("Listening for messages\n");
  // struct sockaddr_in looks the same on both Linux and Windows
  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = PORT;
  addr.sin_addr.s_addr = INADDR_ANY;
  bind(sock, (struct sockaddr *)&addr, sizeof(addr));

  for(int i = 0; i < 100; i++) {
    char message[100];
    int bytes_received = recv(sock, (char *)message, sizeof(message), 0);
    printf("Received %d bytes: %s\n", bytes_received, message);
  }
  kmps_close(sock);
  kmps_clean_up();
  printf("Good bye from UDP listener! %u\n", loopback_addr);
  return 0;
}
