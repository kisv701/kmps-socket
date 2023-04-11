/**
 * @file kmps_socket.h
 * @author Kim Svensson
 * @brief Header only wrapper for sockets on Linux and Windows systems.
 * @version 0.1
 * @date 2023-04-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef KMPS_SOCKET_H
#define KMPS_SOCKET_H

#include <inttypes.h>
#include <ctype.h>

#ifdef _WIN32
// Windows
// Note CMake line: target_link_libraries(example_app PRIVATE ws2_32.lib)
#define WIN32_LEAN_AND_MEAN
#undef TEXT
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>

static WSADATA wsaData;
static uint8_t is_initialized = 0;

#else
// Linux
#define sprintf_s sprintf
typedef int SOCKET;
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <errno.h>
#include <string.h> // strerror

static const int INVALID_SOCKET = -1;
static const int SOCKET_ERROR   = -1;
// No need to initialize on Linux.
static uint8_t is_initialized = 1;

#endif // Windows or Linux includes

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Converts IPv4 string representation to uint32_t representation.
 *
 * @param[in] src IPv4 address string in null terminated dotted decimal format (ddd.ddd.ddd.ddd).
 * @param[out] dst IPv4 address in network byte-order.
 * @return int 0 if src contains invalid ip string. 1 is returned on success.
 */
extern inline int kmps_ip4_pton(const char *src, uint32_t *dst)
{
  // MinGW does not seem to have InetPton??
  // Add this simple implementation to cover (ddd.ddd.ddd.ddd) case.
  uint32_t parts[4] = {0};
  uint32_t value = 0;
  char current = *src;
  int dots = 0;
  const uint8_t IP4_MAX_LEN = 16;
  for (int i = 0; i < IP4_MAX_LEN; i++) {
    if (src[i] == '\0') {
      // Found end of string, done.
      parts[dots] = value;
      break;
    }
    if (src[i] == '.') {
      parts[dots] = value;
      value = 0;
      dots++;
      if (dots > 3) {
        // Only 3 dots allowed in ddd.ddd.ddd.ddd
        return 0;
      }
    }

    if (isdigit(src[i])) {
      value = (value * 10) + (uint32_t)(src[i] - '0');

      if (value > 255) {
        return 0;
      }
    }
  }
  if (dots != 3) {
    return 0;
  }

  // Construct IP address. Value already has least significant byte.
  value |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
  if(dst) {
    *dst = htonl(value);
  }
  return 1;
}

extern inline int kmps_socket(int domain, int type, int protocol)
{
  #ifdef _WIN32
    // Initialize Winsock
    if (!is_initialized) {
      int result = WSAStartup(MAKEWORD(2,2), &wsaData);
      if (result != 0) {
          return SOCKET_ERROR;
      }
      is_initialized = 1;
    }
    return socket(domain, type, protocol);

  #else
  // Linux
    return socket(domain, type, protocol);
  #endif
}

extern inline int kmps_close(int sock)
{
  #ifdef _WIN32
    return closesocket(sock);
  #else
    return close(sock);
  #endif
}

extern inline void kmps_clean_up()
{
  #ifdef _WIN32
    WSACleanup();
  #endif
}

extern inline int kmps_setsockopt(int sock, int level, int option_name, const char *option_value, int option_len)
{
  #ifdef _WIN32
    return setsockopt((SOCKET)sock, level, option_name, option_value, option_len);
  #else
    return setsockopt(sock, level, option_name, (const void *)option_value, (socklen_t)option_len);
  #endif
}

extern inline int kmps_join_mcast(int sock, uint32_t interface_ip, uint32_t group_ip, uint16_t port)
{
  struct sockaddr_in local_sock = {0};
  local_sock.sin_family = AF_INET;
  local_sock.sin_port = port;
  local_sock.sin_addr.s_addr = INADDR_ANY;

  if(bind(sock, (struct sockaddr*)&local_sock, sizeof(local_sock)) == SOCKET_ERROR) {
    return SOCKET_ERROR;
  }

  /* Enable SO_REUSEADDR to allow multiple instances of this
     application to receive copies of the multicast datagrams. */
  int reuse = 1;
  if(kmps_setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) == SOCKET_ERROR)
  {
    return SOCKET_ERROR;
  }

  struct ip_mreq group = {0};
  group.imr_interface.s_addr = interface_ip;
  group.imr_multiaddr.s_addr = group_ip;
  return kmps_setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group));
}

extern inline int kmps_leave_mcast(int sock, uint32_t interface_ip, uint32_t group_ip)
{
  struct ip_mreq group = {0};
  group.imr_interface.s_addr = interface_ip;
  group.imr_multiaddr.s_addr = group_ip;
  return kmps_setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&group, sizeof(group));
}

/**
 * @brief convert IPv4 and IPv6 (Linux only) addresses from string to uint32_t binary form.
 *
 * @param af AF_INET for IPv4 and AF_INET6 for IPv6 (Linux only).
 * @param[in] src input string such in dotted decimal format (ddd.ddd.ddd.ddd).
 * @param[out] dst pointer to destination buffer.
 * @return int 1 on success. 0 is returned if src does not contain a valid string.
 */
extern inline int kmps_inet_pton(int af, const char *src, void *dst)
{
  #ifdef _WIN32
    if (af == AF_INET) {
      return kmps_ip4_pton(src, (uint32_t *)dst);
    } else {
      // Failure, we don't support IPv6.
      return 0;
    }
  #else
    return inet_pton(af, src, dst);
  #endif
}

extern inline const char *kmps_get_error_text(int error_code)
{
  #ifdef _WIN32
    static char message[256] = {0};
    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
        0, error_code, 0, message, 256, 0);
    char *nl = strrchr(message, '\n');
    if (nl) *nl = 0;
    return message;
  #else
      return strerror(error_code);
  #endif
}

extern inline void kmps_print_error()
{
  #ifdef _WIN32
    int err = WSAGetLastError();
    printf("[ERROR] (%d): %s\n", err, kmps_get_error_text(err));
  #else
    printf("[ERROR] (%d): %s\n", errno, kmps_get_error_text(errno));
  #endif
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // KMPS_SOCKET_H
