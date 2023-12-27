#include <unistd.h>
#include <cstring>
#include <functional>
#include <iostream>

#include "Connection.h"
#include "Socket.h"
#include "ThreadPool.h"

void OneClient(int msgs, int wait) {
  Socket *sock = new Socket();
  sock->Create();
  sock->Connect("127.0.0.1", 9999);
  Connection *conn = new Connection(sock->GetFd(), nullptr);
  sleep(wait);

  int count = 0;
  while (count < msgs) {
    conn->SetSendBuffer("I'm client!");
    conn->Write();
    if (conn->GetState() == Connection::State::Closed) {
      conn->Close();
      break;
    }
    conn->Read();
    std::cout << "msg count " << count++ << ": " << conn->GetReadBuffer()->ConstStr() << std::endl;
  }
  delete conn;
  delete sock;
}

int main(int argc, char *argv[]) {
  int threads = 100;
  int msgs = 100;
  int wait = 0;
  int o = 0;
  const char *optstring = "t:m:w:";
  while ((o = getopt(argc, argv, optstring)) != -1) {
    switch (o) {
      case 't':
        threads = atoi(optarg);
        break;
      case 'm':
        msgs = atoi(optarg);
        break;
      case 'w':
        wait = atoi(optarg);
        break;
      case '?':
        printf("error optopt: %c\n", optopt);
        printf("error opterr: %d\n", opterr);
        break;
      default:
        break;
    }
  }

  ThreadPool *poll = new ThreadPool(threads);
  std::function<void()> func = std::bind(OneClient, msgs, wait);
  for (int i = 0; i < threads; ++i) {
    poll->Add(func);
  }
  delete poll;
  return 0;
}