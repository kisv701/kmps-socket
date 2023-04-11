#include "stdio.h"
#include "kmps_socket.h"

#ifdef _WIN32
#include <windows.h>
#define MILLI_SLEEP(x) Sleep(x)
#else
#include <unistd.h>
#include <errno.h>
#include <string.h>
#define MILLI_SLEEP(x) usleep(x*1000)
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

  // struct sockaddr_in looks the same on both Linux and Windows
  struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_port = PORT,
    .sin_addr.s_addr = loopback_addr
  };
  printf("Will send 100 messages over UDP\n");

  for(int i = 0; i < 100; i++) {
    char message[100];
    int msg_len = sprintf((char *)message, "Talker is sending message number %d", i);
    int bytes_sent = sendto(sock, (char *)message, msg_len+1, 0, (struct sockaddr *)&addr, sizeof(addr));
    if (bytes_sent < 0) {
      kmps_print_error();
    }
    printf("Sent %d bytes: %s\n", bytes_sent, message);
    MILLI_SLEEP(10);
  }
  kmps_close(sock);
  kmps_clean_up();
  printf("Good bye from UDP talker! %u\n", loopback_addr);
  return 0;
}
