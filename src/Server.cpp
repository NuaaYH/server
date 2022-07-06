/******************************
*   author: yuesong-feng
*   
*
*
******************************/
#include "Server.h"
#include "Socket.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"
#include "EventLoop.h"
#include "skipdb.h"
#include "timermg.h"
#include <unistd.h>
#include <fstream>
#include <functional>

Server::Server(EventLoop *_loop) : mainReactor(_loop), acceptor(nullptr){ 
    
    std::string name="/home/bianyetong/桌面/c++/30day/myserver/src/store/dumpFile";
    std::fstream f;
    f.open(name,std::ios::in);
    if(f) skipList.load_file();
    //skipList.display_list();

    acceptor = new Acceptor(mainReactor);
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);

    int size = std::thread::hardware_concurrency();
    thpool = new ThreadPool(size);
    for(int i = 0; i < size; ++i){
        subReactors.push_back(new EventLoop());
    }

    for(int i = 0; i < size; ++i){
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, subReactors[i]);  //绑定eventloop类的成员函数，需要用该类的实例作为第一个参数
        thpool->add(sub_loop);
    }
    
}

Server::~Server(){
    delete acceptor;
    delete thpool;
}


void Server::newConnection(Socket *sock){
    if(sock->getFd() != -1){
        int random = sock->getFd() % subReactors.size();
        std::shared_ptr<Connection> conn = std::make_shared<Connection>(subReactors[random], sock);
        std::function<void(int)> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
        conn->setDeleteConnectionCallback(cb);
        connections[sock->getFd()] = conn;

        timer_->addTimer(sock->getFd(),MAX_TIME,std::bind(&Server::deleteConnection, this, sock->getFd()));

    }
}

void Server::deleteConnection(int sockfd){
    if(sockfd != -1){
        auto it = connections.find(sockfd);
        if(it != connections.end()){
            std::shared_ptr<Connection> conn = connections[sockfd];
            connections.erase(sockfd);
            if(connections.empty()){
                skipList.dump_file();
            }
            //conn->loop->deleteChannel(conn->channel);
            //close(sockfd);       //正常
            //delete conn;         //会Segmant fault
        }
    }
}