//
// Created by rjj on 12/5/23.
//

#ifndef DAY01_CONNECTION_H
#define DAY01_CONNECTION_H

#include <cstdio>
#include "Buffer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "common.h"

class EventLoop;
class Socket;
class Channel;
class Buffer;
class Connection {
 public:
  enum State {
    Invalid = 0,
    Handshaking,
    Connected,
    Closed,
  };
  DISALLOW_COPY_AND_MOVE(Connection);
  Connection(int fd, EventLoop *loop);
  ~Connection() = default;

  RC Read();
  RC Write();
  RC Send(std::string msg);

  void SetDeleteConnectionCallback(std::function<void(int)> const &callback);
  void SetOnRecv(std::function<void(Connection *)> const &callback);

  State GetState() const;
  Socket *GetSocket() const;

  void Close();

  void SetSendBuffer(const char *str);
  Buffer *GetReadBuffer();
  Buffer *GetSendBuffer();


 private:
  void Business();
  void EchoMessage();
  RC ReadNonBlocking();
  RC WriteNonBlocking();
  RC ReadBlocking();
  RC WriteBlocking();

  std::unique_ptr<Socket> sock_;
  std::unique_ptr<Channel> channel_{nullptr};

  State state_{State::Invalid};
  std::unique_ptr<Buffer> read_buffer_{nullptr};
  std::unique_ptr<Buffer> send_buffer_{nullptr};

  std::function<void(int)> delete_connection_callback_;

  std::function<void(Connection *)> on_recv_callback_;
};

#endif  // DAY01_CONNECTION_H
