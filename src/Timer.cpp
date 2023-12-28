//
// Created by rjj on 12/27/23.
//



#include "include/Timer.h"
void Timer::AdjustTimer(int id, int new_expires) {
  assert(!heap_.empty() && ref_.count(new_expires));
  heap_[ref_[id]].expires = Clock::now() + MS(new_expires);
  SiftDown(ref_[id], heap_.size());                   //从上往下调整一次
}

void Timer::AddTimer(int id, int timeout, const std::function<void()> &cb) {

}

void Timer::DoWork(int id) {}

void Timer::Clear() {
  ref_.clear();
  heap_.clear();
}

void Timer::Tick() {}

void Timer::PopTimer() {
  assert(!heap_.empty());
  DelTimer(0);            //将堆顶的Timer出去
}

int Timer::GetNextTick() { return 0; }

void Timer::DelTimer(size_t index) {}

void Timer::SiftUp(size_t index) {}

bool Timer::SiftDown(size_t index, size_t n) { return false; }

void Timer::SwapNode(size_t index1, size_t index2) {
  assert(index1 >=0 && index1 <= heap_.size());
  assert(index2 >=0 && index2 <= heap_.size());
  std::swap(heap_[index1],heap_[index2]);
  ref_[heap_[index1].id] = index1;
  ref_[heap_[index2].id] = index2;
}
