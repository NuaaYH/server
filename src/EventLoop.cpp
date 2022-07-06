/******************************
*   author: yuesong-feng
*   
*
*
******************************/
#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include "timermg.h"
#include <vector>

EventLoop::EventLoop() : ep(nullptr), quit(false){
    ep = new Epoll();
}

EventLoop::~EventLoop(){
    delete ep;
}


void EventLoop::loop(){
    while(!quit){
        int chs;
        int timeMS=timer_->getNextHandle();
        //printf("%d\n",timeMS);
        chs = ep->poll(timeMS);
        for(int i = 0; i < chs; ++i){
            Channel *ch = (Channel*)ep->getEvent(i).data.ptr;
            ch->setReady(ep->getEvent(i).events);
            ch->handleEvent();
        }
    }
}

void EventLoop::updateChannel(Channel *ch){
    ep->updateChannel(ch);
}

void EventLoop::deleteChannel(Channel *ch){
    ep->deleteChannel(ch);
}

