//
// Created by rjj on 12/27/23.
//

#pragma once

#include <queue>
#include <cerrno>
#include <cassert>
#include <functional>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <chrono>
#include <unordered_map>
#include <sys/socket.h>
#include <signal.h>

typedef std::chrono::high_resolution_clock Clock;
typedef Clock::time_point TimeStamp;
typedef std::chrono::milliseconds MS;

struct TimerNode{
  int id;
  TimeStamp expires;                      //超时时间
  std::function<void()> time_callback;    //任务回调函数
  bool operator<(const TimerNode& t) {
    return expires < t.expires;
  }
};
class Timer {
public:
  Timer(){heap_.reserve(64);}
  ~Timer(){Clear();}
  void AdjustTimer(int id, int new_expires);

  void AddTimer(int id, int timeout, const std::function<void()> &cb);

  void DoWork(int id);

  void Clear();

  void Tick();

  void PopTimer();

  int GetNextTick();
private:
  void DelTimer(size_t index);

  void SiftUp(size_t index);

  bool SiftDown(size_t index, size_t n);

  void SwapNode(size_t index1, size_t index2);

  std::vector<TimerNode> heap_;
  std::unordered_map<int, size_t> ref_;     //建立id到下标的映射
};

