//
// Created by rjj on 12/4/23.
//

#ifndef DAY01_SERVER_H
#define DAY01_SERVER_H
#include <unistd.h>
#include <cstring>
#include <functional>
#include <unordered_map>
#include "Acceptor.h"
#include "Channel.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "common.h"

#define READ_BUFFER 1024

class TCPServer {
 private:
  std::unique_ptr<EventLoop> main_reactor_;               //只负责接收连接，然后分发给一个subReactor
  std::unique_ptr<Acceptor> acceptor_;                    //用于接收TCP连接
  std::vector<std::unique_ptr<EventLoop>> sub_reactors_;  //负责处理事件循环
  std::unordered_map<int, std::unique_ptr<Connection>> connections_;  // fd到TCP连接的映射
  std::unique_ptr<ThreadPool> thread_pool_;                         //线程池

  std::function<void(Connection *)> on_connect_;
  std::function<void(Connection *)> on_recv_;

 public:
  DISALLOW_COPY_AND_MOVE(TCPServer);
  TCPServer();
  ~TCPServer() = default;
  void Start();

  RC NewConnection(int fd);     //新建TCP连接
  RC DeleteConnection(int fd);  //断开TCP连接

  void OnConnect(std::function<void(Connection *)> fn);
  void OnRecv(std::function<void(Connection *)> fn);
};

#endif  // DAY01_SERVER_H
