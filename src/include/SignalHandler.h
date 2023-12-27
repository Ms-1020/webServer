//
// Created by rjj on 12/21/23.
//

#pragma once
#include <signal.h>
#include <functional>
#include <map>

std::map <int, std::function<void()>> handlers;
void SignalHandler(int sig) { handlers[sig](); }

struct Signal {
  static void signal(int sig, const std::function<void()> &handler) {
    handlers[sig] = handler;
    ::signal(sig, SignalHandler);
  }
};
