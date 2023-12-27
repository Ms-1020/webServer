#pragma once
#include <sys/epoll.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <vector>
#include "Channel.h"
#include "Socket.h"
#include "common.h"

class Poller {
 private:
  int epfd_{-1};

  struct epoll_event *events_{nullptr};

 public:
  DISALLOW_COPY_AND_MOVE(Poller);
  Poller();
  ~Poller();

  // void addFd(int fd,uint32_t op);
  void UpdateChannel(Channel *ch);
  void DeleteChannel(Channel *ch);

  std::vector<Channel *> Poll(long timeout = -1) const;
};
