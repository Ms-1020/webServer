//
// Created by rjj on 12/5/23.
//

#include "include/Connection.h"

Connection::Connection(int fd, EventLoop *loop) {
  sock_ = std::make_unique<Socket>();
  sock_->SetFd(fd);
  if (loop != nullptr) {
    channel_ = std::make_unique<Channel>(loop, fd);  //根据客户端socket创建一个新的channel
    channel_->EnableReading();                       //打开读监听事件
    channel_->EnableET();
  }

  read_buffer_ = std::make_unique<Buffer>();
  send_buffer_ = std::make_unique<Buffer>();
  state_ = State::Connected;
}

RC Connection::Read() {
  if (state_ != State::Connected) {
    perror("Connection is not connected, can not read");
    return RC::RC_CONNECTION_ERROR;
  }
  assert(state_ == State::Connected && "connection state is disconnected!");
  read_buffer_->Clear();
  RC rc = RC_UNDEFINED;
  if (sock_->IsNonBlocking()) {
    rc = ReadNonBlocking();
  } else {
    rc = ReadBlocking();
  }
  return rc;
}

RC Connection::Write() {
  if (state_ != State::Connected) {
    perror("Connection is not connected, can not read");
    return RC::RC_CONNECTION_ERROR;
  }
  RC rc = RC_UNDEFINED;
  if (sock_->IsNonBlocking()) {
    rc = WriteNonBlocking();
  } else {
    rc = WriteBlocking();
  }
  send_buffer_->Clear();
  return rc;
}

RC Connection::ReadNonBlocking() {
  int sockfd = sock_->GetFd();
  char buf[1024];
  while (true) {
    memset(&buf, 0, sizeof(buf));                         //缓冲区清0
    ssize_t read_bytes = read(sockfd, buf, sizeof(buf));  //从客户端socket读取到缓冲区，得到读取大小
    if (read_bytes > 0) {
      read_buffer_->Append(buf, read_bytes);
    } else if (read_bytes == -1 && errno == EINTR) {  //客户端正常中断、继续读取
      printf("continue reading\n");
      continue;
    } else if (read_bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {  //表示数据全部读取完毕
      break;
    } else if (read_bytes == 0) {  // EOF，客户端断开连接
      printf("read EOF, client fd %d disconnected\n", sockfd);
      state_ = State::Closed;
      Close();
      break;
    } else {
      printf("Other error on client fd %d\n", sockfd);
      state_ = State::Closed;
      Close();
      break;
    }
  }
  return RC_SUCCESS;
}

RC Connection::WriteNonBlocking() {
  int sockfd = sock_->GetFd();
  char buf[send_buffer_->Size()];
  memcpy(buf, send_buffer_->ConstStr(), send_buffer_->Size());
  int data_size = send_buffer_->Size();
  int data_left = data_size;
  while (data_left > 0) {
    ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left);
    if (bytes_write == -1 && errno == EINTR) {
      printf("continue writing\n");
      continue;
    }
    if (bytes_write == -1 && errno == EAGAIN) {
      break;
    }
    if (bytes_write == -1) {
      printf("Other error on client fd %d\n", sockfd);
      state_ = State::Closed;
      break;
    }
    data_left -= bytes_write;
  }
  return RC_SUCCESS;
}

RC Connection::ReadBlocking() {
  int sockfd = sock_->GetFd();
  char buf[1024];
  ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
  if (bytes_read > 0) {
    read_buffer_->Append(buf, bytes_read);
  } else if (bytes_read == 0) {
    printf("read EOF, blocking client fd %d disconnected\n", sockfd);
    state_ = State::Closed;
  } else if (bytes_read == -1) {
    printf("Other error on blocking client fd %d\n", sockfd);
    state_ = State::Closed;
  }
  return RC_SUCCESS;
}

RC Connection::WriteBlocking() {
  // 没有处理send_buffer_数据大于TCP写缓冲区，的情况，可能会有bug
  int sockfd = sock_->GetFd();
  ssize_t bytes_write = write(sockfd, send_buffer_->ConstStr(), send_buffer_->Size());
  if (bytes_write == -1) {
    printf("Other error on blocking client fd %d\n", sockfd);
    state_ = State::Closed;
  }
  return RC_SUCCESS;
}

RC Connection::Send(std::string msg) {
  SetSendBuffer(msg.c_str());
  Write();
  return RC_SUCCESS;
}

void Connection::Business() {
  Read();
  if(state_ == Connected) {
    on_recv_callback_(this);
  }
}

void Connection::Close() {
  delete_connection_callback_(sock_->GetFd());
}

Connection::State Connection::GetState() const { return state_; }

void Connection::SetSendBuffer(const char *str) { send_buffer_->SetBuf(str); }

Buffer *Connection::GetReadBuffer() { return read_buffer_.get(); }
Buffer *Connection::GetSendBuffer() { return send_buffer_.get(); }

void Connection::SetDeleteConnectionCallback(std::function<void(int)> const &callback) {
  delete_connection_callback_ = std::move(callback);
}

void Connection::SetOnRecv(std::function<void(Connection *)> const &callback) {
  on_recv_callback_ = std::move(callback);
  std::function<void()> fn = std::bind(&Connection::Business, this);
  channel_->SetReadCallback(fn);
}

Socket *Connection::GetSocket() const { return sock_.get(); }