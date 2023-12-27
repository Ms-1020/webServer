//
// Created by rjj on 11/29/23.
//

#pragma once
#include <functional>
#include <iostream>
#include "EventLoop.h"
#include "Poller.h"
#include "common.h"

class EventLoop;
class Channel {
 private:
  EventLoop *loop_;
  int fd_;
  short listen_events_{0};
  short ready_events_{0};
  bool exist_{false};  // 表示当前Channel是否已经在epoll红黑树中
  std::function<void()> read_callback_;
  std::function<void()> write_callback_;

 public:
  DISALLOW_COPY_AND_MOVE(Channel);
  Channel(EventLoop *loop, int fd);
  ~Channel();
  void HandleEvent() const;
  void EnableReading();
  void EnableWriting();

  int GetFd() const;
  short GetListenEvents() const;
  short GetReadyEvents() const;
  bool GetExist();
  void SetExist(bool _in = true);
  void EnableET();

  void SetReadyEvents(short _ev);
  void SetReadCallback(std::function<void()> const &callback);
  void SetWriteCallback(std::function<void()> const &callback);

  static const short READ_EVENT;
  static const short WRITE_EVENT;
  static const short ET;
};
