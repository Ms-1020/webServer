//
// Created by rjj on 12/4/23.
//

#ifndef DAY01_EVENTLOOP_H
#define DAY01_EVENTLOOP_H
#include <vector>
#include "Channel.h"
#include "Poller.h"
#include "ThreadPool.h"
#include "common.h"

class EventLoop {
 private:
  std::unique_ptr<Poller> poller_;

 public:
  DISALLOW_COPY_AND_MOVE(EventLoop);
  EventLoop();
  ~EventLoop() = default;

  void Loop() const;
  void UpdateChannel(Channel *ch) const;
  void DeleteChannel(Channel *ch) const;

  // void addThread(std::function<void()>);
};

#endif  // DAY01_EVENTLOOP_H
