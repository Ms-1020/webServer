#pragma once
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <string>
#include "common.h"

class Socket {
 public:
  DISALLOW_COPY_AND_MOVE(Socket);
  Socket();
  ~Socket();

  void SetFd(int fd);
  int GetFd() const;
  std::string GetAddr() const;
  RC Create();
  RC Bind(const char *ip, uint16_t port) const;
  RC Listen() const;
  RC Accept(int &clnt_fd) const;

  RC Connect(const char *ip, uint16_t port) const;
  RC SetNonBlocking() const;
  bool IsNonBlocking() const;

  size_t RecvBufSize() const;

 private:
  int fd_{-1};
};
