//
// Created by rjj on 12/4/23.
//

#include "include/EventLoop.h"

EventLoop::EventLoop() { poller_ = std::make_unique<Poller>(); }

void EventLoop::Loop() const {
  while (true) {
    for (auto *it : poller_->Poll()) {
      it->HandleEvent();
    }
  }
}

void EventLoop::UpdateChannel(Channel *ch) const { poller_->UpdateChannel(ch); }

void EventLoop::DeleteChannel(Channel *ch) const { poller_->DeleteChannel(ch); }
