#include <iostream>
#include "SignalHandler.h"
#include "TCPServer.h"

int main() {
  TCPServer *server = new TCPServer();

  Signal::signal(SIGINT, [&] {
    delete server;
    std::cout << "\nServer exit!" << std::endl;
    exit(0);
  });

  server->OnConnect(
      [](Connection *conn) { std::cout << "New connection fd: " << conn->GetSocket()->GetFd() << std::endl; });

  server->OnRecv([](Connection *conn) {
    std::cout << "Message from client: " << conn->GetReadBuffer()->ConstStr() << std::endl;
    conn->Send(conn->GetReadBuffer()->ConstStr());
  });

  server->Start();
  delete server;
  return 0;
}
