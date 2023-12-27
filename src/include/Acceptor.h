//
// Created by rjj on 12/5/23.
//

#ifndef DAY01_ACCEPTOR_H
#define DAY01_ACCEPTOR_H
#include <cstdio>
#include <memory>
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "common.h"

class Acceptor {
 public:
  DISALLOW_COPY_AND_MOVE(Acceptor);
  explicit Acceptor(EventLoop *loop);
  ~Acceptor() = default;

  RC AcceptConnection() const;
  void SetNewConnectionCallback(std::function<void(int)> const &callback);

 private:
  std::unique_ptr<Socket> socket_;
  std::unique_ptr<Channel> channel_;
  std::function<void(int)> new_connection_callback_;
};

#endif  // DAY01_ACCEPTOR_H
