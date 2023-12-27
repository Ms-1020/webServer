//
// Created by rjj on 12/7/23.
//

#ifndef DAY01_THREADPOOL_H
#define DAY01_THREADPOOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
#include "common.h"

class ThreadPool {
 private:
  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> tasks_;     //任务队列
  std::mutex queue_mutex_;                      //任务队列访问互斥量
  std::condition_variable condition_variable_;  //任务队列条件遍历，通知线程有新任务可以执行
  std::atomic<bool> stop_{false};

 public:
  DISALLOW_COPY_AND_MOVE(ThreadPool);
  explicit ThreadPool(unsigned int size = std::thread::hardware_concurrency());
  ~ThreadPool();

  template <class F, class... Args>
  auto Add(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>;
};

template <class F, class... Args>
auto ThreadPool::Add(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type> {
  using return_type = typename std::result_of<F(Args...)>::type;  //定义返回类型

  auto task =
      std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  std::future<return_type> res = task->get_future();
  {
    std::unique_lock<std::mutex> lock(queue_mutex_);

    if (stop_) {
      throw std::runtime_error("enqueue on stopped ThreadPool");
    }

    tasks_.emplace([task]() { (*task)(); });
  }
  condition_variable_.notify_one();
  return res;
}

#endif  // DAY01_THREADPOOL_H
