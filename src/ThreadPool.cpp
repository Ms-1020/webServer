//
// Created by rjj on 12/7/23.
//

#include "include/ThreadPool.h"

ThreadPool::ThreadPool(unsigned int size) {
  for (unsigned int i = 0; i < size; ++i) {       //启动size个线程
    workers_.emplace_back(std::thread([this]() {  //定义每个线程的工作函数
      while (true) {
        std::function<void()> task;
        {  //在该作用域内给是mutex枷锁，出了作用域会自动解锁
          std::unique_lock<std::mutex> lock(queue_mutex_);
          condition_variable_.wait(lock, [this]() {  //等待条件变量，条件为任务队列不为空或者线程池停止
            return stop_ || !tasks_.empty();
          });
          if (stop_ && tasks_.empty()) {
            return;  //任务队列为空并且线程池停止，退出线程
          }
          task = tasks_.front();  //取出一个任务
          tasks_.pop();
        }
        task();
      }
    }));
  }
}

ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    stop_ = true;
  }
  condition_variable_.notify_all();  //唤醒所有等待中的线程
  for (std::thread &th : workers_) {
    if (th.joinable()) {
      th.join();  //等待线程执行完毕
    }
  }
}
