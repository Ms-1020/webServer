//
// Created by rjj on 12/4/23.
//

#include "include/TCPServer.h"

TCPServer::TCPServer() {
  main_reactor_ = std::make_unique<EventLoop>();
  acceptor_ = std::make_unique<Acceptor>(main_reactor_.get());  // Acceptor只由mainReactor负责
  std::function<void(int)> cb = std::bind(&TCPServer::NewConnection, this, std::placeholders::_1);
  acceptor_->SetNewConnectionCallback(cb);

  unsigned int size_ = std::thread::hardware_concurrency();  //获取线程数量，也是subReactor数量
  thread_pool_ = std::make_unique<ThreadPool>(size_);        //创造线程池
  for (unsigned int i = 0; i < size_; ++i) {
    std::unique_ptr<EventLoop> sub_reactor = std::make_unique<EventLoop>();
    sub_reactors_.emplace_back(std::move(sub_reactor));  //每个线程是一个EventLoop
  }
}

void TCPServer::Start() {
  for (int i = 0; i < sub_reactors_.size(); ++i) {
    std::function<void()> sub_loop = std::bind(&EventLoop::Loop, sub_reactors_[i].get());
    thread_pool_->Add(std::move(sub_loop));
  }
  main_reactor_->Loop();
}

RC TCPServer::NewConnection(int fd) {
  assert(fd != -1);
  uint64_t random_num = fd % sub_reactors_.size();

  std::unique_ptr<Connection> conn = std::make_unique<Connection>(fd, sub_reactors_[random_num].get());
  std::function<void(int)> cb = std::bind(&TCPServer::DeleteConnection, this, std::placeholders::_1);

  conn->SetDeleteConnectionCallback(cb);
  conn->SetOnRecv(on_recv_);

  connections_[fd] = std::move(conn);

  if (on_connect_) {
    on_connect_(connections_[fd].get());
  }
  return RC_SUCCESS;
}

RC TCPServer::DeleteConnection(int fd) {
  auto it = connections_.find(fd);
  assert(it != connections_.end());
  connections_.erase(fd);
  return RC_SUCCESS;
}

void TCPServer::OnConnect(std::function<void(Connection *)> fn) { on_connect_ = std::move(fn); }

void TCPServer::OnRecv(std::function<void(Connection *)> fn) { on_recv_ = std::move(fn); }