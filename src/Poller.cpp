#include "include/Poller.h"

#define MAX_EVENTS 1000

Poller::Poller() {
  epfd_ = epoll_create1(0);
  if(epfd_ == -1) {
    perror("epoll create error");
  }
  events_ = new epoll_event[MAX_EVENTS];
  memset(events_, 0, sizeof(*events_) * MAX_EVENTS);
}

Poller::~Poller() {
  if (epfd_ != -1) {
    close(epfd_);
    epfd_ = -1;
  }
  delete[] events_;
}

std::vector<Channel *> Poller::Poll(long timeout) const{
  std::vector<Channel *> active_channels;
  int nfds = epoll_wait(epfd_, events_, MAX_EVENTS, timeout);
  if(nfds == -1) {
    perror("epoll wait error");
  }
  for (int i = 0; i < nfds; ++i) {
    Channel *ch = (Channel *)events_[i].data.ptr;
    int events = events_[i].events;
    if (events & EPOLLIN) {
      ch->SetReadyEvents(Channel::READ_EVENT);
    }
    if (events & EPOLLOUT) {
      ch->SetReadyEvents(Channel::WRITE_EVENT);
    }
    if (events & EPOLLET) {
      ch->SetReadyEvents(Channel::ET);
    }
    active_channels.emplace_back(ch);
  }
  return active_channels;
}

void Poller::UpdateChannel(Channel *ch) {
  int fd = ch->GetFd();
  struct epoll_event ev {};
  ev.data.ptr = ch;  //指向channel
  if (ch->GetListenEvents() & Channel::READ_EVENT) {
    ev.events |= EPOLLIN | EPOLLPRI;
  }
  if (ch->GetListenEvents() & Channel::WRITE_EVENT) {
    ev.events |= EPOLLOUT;
  }
  if (ch->GetListenEvents() & Channel::ET) {
    ev.events |= EPOLLET;
  }
  if (!ch->GetExist()) {  //不在红黑树中
    if(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
      perror("epoll add error");
    }
    ch->SetExist();
  } else {
    if(epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev) == -1) {
      perror("epoll modify error");
    }
  }
}

void Poller::DeleteChannel(Channel *ch) {
  int fd = ch->GetFd();
  if(epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, nullptr) == -1) {
    perror("epoll delte error");
  }
  ch->SetExist(false);
}
