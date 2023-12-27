//
// Created by rjj on 11/29/23.
//

#include "include/Channel.h"

const short Channel::READ_EVENT = 1;
const short Channel::WRITE_EVENT = 2;
const short Channel::ET = 4;

Channel::Channel(EventLoop *loop, int fd) : loop_(loop), fd_(fd) {}
Channel::~Channel() { loop_->DeleteChannel(this); }

void Channel::HandleEvent() const{
  if (ready_events_ & READ_EVENT) {
    read_callback_();
  }
  if (ready_events_ & WRITE_EVENT) {
    write_callback_();
  }
}

void Channel::EnableReading() {
  listen_events_ |= READ_EVENT;
  loop_->UpdateChannel(this);  //更新，打开允许读事件
}

void Channel::EnableWriting() {
  listen_events_ |= WRITE_EVENT;
  loop_->UpdateChannel(this);  //更新，打开允许写事件
}

void Channel::EnableET() {
  listen_events_ |= ET;
  loop_->UpdateChannel(this);
}

int Channel::GetFd() const { return fd_; }

short Channel::GetListenEvents() const { return listen_events_; }

short Channel::GetReadyEvents() const { return ready_events_; }

void Channel::SetReadyEvents(short ev) {
  if (ev & READ_EVENT) {
    ready_events_ |= READ_EVENT;
  }
  if (ev & WRITE_EVENT) {
    ready_events_ |= WRITE_EVENT;
  }
  if (ev & ET) {
    ready_events_ |= ET;
  }
}

bool Channel::GetExist() { return exist_; }

void Channel::SetExist(bool _in) { exist_ = _in; }

void Channel::SetReadCallback(std::function<void()> const &callback) { read_callback_ = std::move(callback); }

void Channel::SetWriteCallback(std::function<void()> const &callback) { write_callback_ = std::move(callback); }
