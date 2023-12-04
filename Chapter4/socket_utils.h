#pragma once
#include <iostream>
#include <string>
#include <unordered_set>
#include <sstream>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <fcntl.h>

#include "macros.h"

#include "logging.h"

namespace Common
{
  struct SocketCfg
  {
    std::string ip_;
    std::string iface_;
    int port_ = -1;
    bool is_udp_ = false;
    bool is_listening_ = false;
    bool needs_so_timestamp_ = false;

    auto toString() const
    {
      std::stringstream ss;
      ss << "SocketCfg[ip:" << ip_
         << " iface:" << iface_
         << " port:" << port_
         << " is_udp:" << is_udp_
         << " is_listening:" << is_listening_
         << " needs_SO_timestamp:" << needs_so_timestamp_
         << "]";

      return ss.str();
    }
  };

  /// Represents the maximum number of pending / unaccepted TCP connections.
  constexpr int MaxTCPServerBacklog = 1024;

  /// Convert interface name "eth0" to ip "123.123.123.123".
  // The interface may look like in this way:
  /* lo: flags=73<UP,LOOPBACK,RUNNING>  mtu 65536
        inet 127.0.0.1  netmask 255.0.0.0
wlp4s0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 192.168.10.104  netmask 255.255.255.0  broadcast 192.168.10.255
  */
  // trailing return type
  inline auto getIfaceIP(const std::string &iface) -> std::string
  {
    // NI_MAXHOST is defined in netdb.h and is the maximum length of a host name.
    char buf[NI_MAXHOST] = {'\0'};
    ifaddrs *ifaddr = nullptr;

    if (getifaddrs(&ifaddr) != -1)
    {
      for (ifaddrs *ifa = ifaddr; ifa; ifa = ifa->ifa_next)
      {
        // check if the address is not 'nullptr', ensures it's an IPv4 address 'AF_INET' and compares the interface name with the
        // requested interface name.
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET && iface == ifa->ifa_name)
        {
          // convert the network address into a human-readable string.
          getnameinfo(ifa->ifa_addr, sizeof(sockaddr_in), buf, sizeof(buf), NULL, 0, NI_NUMERICHOST);
          break;
        }
      }
      // free the memory allocated by getifaddrs().
      freeifaddrs(ifaddr);
    }

    return buf;
  }

  /// Sockets will not block on read, but instead return immediately if data is not available.
  inline auto setNonBlocking(int fd) -> bool
  {
    // fcntl is a POSIX system call for changing the properties of a file descriptor.
    const auto flags = fcntl(fd, F_GETFL, 0);
    if (flags & O_NONBLOCK)
      return true;
    return (fcntl(fd, F_SETFL, flags | O_NONBLOCK) != -1);
  }

  /// Disable Nagle's algorithm and associated delays.
  inline auto disableNagle(int fd) -> bool
  {
    int one = 1;
    return (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<void *>(&one), sizeof(one)) != -1);
  }
  /*
  int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
  sockfd: socket descriptor
  level : SOL_SOCKET, IPPROTO_TCP, IPPROTO_IP, IPPROTO_IPV6 the level at which the option is defined.
  optname: SO_REUSEADDR, SO_REUSEPORT, SO_TIMESTAMP, TCP_NODELAY, IP_ADD_MEMBERSHIP, IP_MULTICAST_LOOP, IP_MULTICAST_TTL, IP_MULTICAST_IF
  the name of the option to set. 
  optval: a pointer to the value of the option.
  optlen: the length of the option value
  */
  /// Allow software receive timestamps on incoming packets.
  inline auto setSOTimestamp(int fd) -> bool
  {
    int one = 1;
    return (setsockopt(fd, SOL_SOCKET, SO_TIMESTAMP, reinterpret_cast<void *>(&one), sizeof(one)) != -1);
  }

  /// Add / Join membership / subscription to the multicast stream specified and on the interface specified.
  inline auto join(int fd, const std::string &ip) -> bool
  {
    const ip_mreq mreq{{inet_addr(ip.c_str())}, {htonl(INADDR_ANY)}};
    // inet_addr: convers an IPV4 address in its standard text format into its dot-decimal format "like "224.0.0.1"
    // htonl: coverts the "host byte order" to "network byte order" Thsi is important for network communications to ensure consistent byte 
    // ordering arcoss different systems. 
    return (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) != -1);
  }

  /// Create a TCP / UDP socket to either connect to or listen for data on or listen for connections on the specified interface and IP:port information.
  // the [[nondiscard]] attribute in c++ is a feature introduced in c++17. It is used to tell the compiler that the return value of the function should 
  // not be discarded. 
  [[nodiscard]] inline auto createSocket(Logger &logger, const SocketCfg &socket_cfg) -> int
  {
    std::string time_str;

    const auto ip = socket_cfg.ip_.empty() ? getIfaceIP(socket_cfg.iface_) : socket_cfg.ip_;
    logger.log("%:% %() % cfg:%\n", __FILE__, __LINE__, __FUNCTION__,
               Common::getCurrentTimeStr(&time_str), socket_cfg.toString());

    const int input_flags = (socket_cfg.is_listening_ ? AI_PASSIVE : 0) | (AI_NUMERICHOST | AI_NUMERICSERV);
    const addrinfo hints{input_flags, AF_INET, socket_cfg.is_udp_ ? SOCK_DGRAM : SOCK_STREAM,
                         socket_cfg.is_udp_ ? IPPROTO_UDP : IPPROTO_TCP, 0, 0, nullptr, nullptr};

    addrinfo *result = nullptr;
    const auto rc = getaddrinfo(ip.c_str(), std::to_string(socket_cfg.port_).c_str(), &hints, &result);
    ASSERT(!rc, "getaddrinfo() failed. error:" + std::string(gai_strerror(rc)) + "errno:" + strerror(errno));

    int socket_fd = -1;
    int one = 1;
    for (addrinfo *rp = result; rp; rp = rp->ai_next)
    {
      ASSERT((socket_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) != -1, "socket() failed. errno:" + std::string(strerror(errno)));

      ASSERT(setNonBlocking(socket_fd), "setNonBlocking() failed. errno:" + std::string(strerror(errno)));

      if (!socket_cfg.is_udp_)
      { // disable Nagle for TCP sockets.
        ASSERT(disableNagle(socket_fd), "disableNagle() failed. errno:" + std::string(strerror(errno)));
      }

      if (!socket_cfg.is_listening_)
      { // establish connection to specified address.
        ASSERT(connect(socket_fd, rp->ai_addr, rp->ai_addrlen) != 1, "connect() failed. errno:" + std::string(strerror(errno)));
      }

      if (socket_cfg.is_listening_)
      { // allow re-using the address in the call to bind()
        ASSERT(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&one), sizeof(one)) == 0, "setsockopt() SO_REUSEADDR failed. errno:" + std::string(strerror(errno)));
      }

      if (socket_cfg.is_listening_)
      {
        // bind to the specified port number.
        const sockaddr_in addr{AF_INET, htons(socket_cfg.port_), {htonl(INADDR_ANY)}, {}};
        ASSERT(bind(socket_fd, socket_cfg.is_udp_ ? reinterpret_cast<const struct sockaddr *>(&addr) : rp->ai_addr, sizeof(addr)) == 0, "bind() failed. errno:%" + std::string(strerror(errno)));
      }

      if (!socket_cfg.is_udp_ && socket_cfg.is_listening_)
      { // listen for incoming TCP connections.
        ASSERT(listen(socket_fd, MaxTCPServerBacklog) == 0, "listen() failed. errno:" + std::string(strerror(errno)));
      }

      if (socket_cfg.needs_so_timestamp_)
      { // enable software receive timestamps.
        ASSERT(setSOTimestamp(socket_fd), "setSOTimestamp() failed. errno:" + std::string(strerror(errno)));
      }
    }

    return socket_fd;
  }
}
