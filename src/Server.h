/******************************
*   author: yuesong-feng
*   
*
*
******************************/
#pragma once
#include <map>
#include <vector>
#include <memory>
//#include "timer.h"
class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;
class Server
{
private:
    EventLoop *mainReactor;
    Acceptor *acceptor;
    std::map<int, std::shared_ptr<Connection>> connections;
    std::vector<EventLoop*> subReactors;
    ThreadPool *thpool;
public:
    Server(EventLoop*);
    ~Server();

    void handleReadEvent(int);
    void newConnection(Socket *sock);
    void deleteConnection(int sockfd);
};


