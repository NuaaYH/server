/******************************
*   author: yuesong-feng
*   
*
*
******************************/
#include "Connection.h"
#include "Socket.h"
#include "Channel.h"
#include "util.h"
#include "Buffer.h"
#include "skipdb.h"
#include "timermg.h"
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <vector>
#include <iostream>

Connection::Connection(EventLoop *_loop, Socket *_sock) : loop(_loop), sock(_sock), channel(nullptr), readBuffer(nullptr){
    channel = new Channel(loop, sock->getFd());
    channel->enableRead();
    channel->useET();
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
    channel->setReadCallback(cb);
    readBuffer = new Buffer();
}

Connection::~Connection(){
    delete channel;
    delete sock;
    delete readBuffer;
}

void Connection::setDeleteConnectionCallback(std::function<void(int)> _cb){
    deleteConnectionCallback = _cb;
}

/*void handHttp(int sockfd){
    char buff[1024*1024]={0};
    bzero(&buff, sizeof(buff));
    ssize_t bytes_read = read(sockfd, buff, sizeof(buff));
    if(bytes_read>0){
        printf("accept request from client:%s\n",buff);
    }
    char fileName[20]={0};
    sscanf(buff,"GET /%s",fileName);
    char* mime=NULL;
    if(strstr(fileName,".html"))
    {
        mime="text/html";
    }else if(strstr(fileName,"jpg"))
    {
        mime="image/jpg";
    }
    char response[1024*1024]={0};

    sprintf(response,"HTTP/1.1 200 ok\r\nContent-Type: %s\r\n\r\n",mime);
    int responseLen =strlen(response);
    int fileFd=open(fileName,O_RDONLY);
    int fileLen=read(fileFd,responseLen+response,sizeof(response)-responseLen);
    write(sockfd,response,response+responseLen);
    close(fileFd);
    sleep(1);
}*/

void Connection::echo(int sockfd){
    char buf[1024];     //这个buf大小无所谓
    
    while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0){
            readBuffer->append(buf, bytes_read);
        } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
            printf("continue reading\n");
            continue;
        } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
            timer_->update(sockfd,MAX_TIME);
            std::string str[3];
            std::istringstream is(readBuffer->c_str());
            is>>str[0]>>str[1]>>str[2];
            
            if(str[0]=="0"){
                int res=skipList.insert_element(str[1],str[2]);
                if(res==0){
                    printf("message from client fd %d: %s\n", sockfd, readBuffer->c_str());
                }else{
                    printf("message from client fd %d: %s\n", sockfd, readBuffer->c_str());
                }
                send(sockfd,res);
            }
            else if(str[0]=="1"){
                int res=skipList.verify_element(sockfd,str[1],str[2]);
                if(res==0){
                    printf("message from client fd %d: %s\n", sockfd, readBuffer->c_str());
                }else{
                    printf("message from client fd %d: %s\n", sockfd, readBuffer->c_str());
                } 
                send(sockfd,res);
            }           
            readBuffer->clear();
            break;
        } else if(bytes_read == 0){  //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            skipList.reset_element(sockfd);
            deleteConnectionCallback(sockfd);           
            break;
        } else {
            printf("Connection reset by peer\n");
            deleteConnectionCallback(sockfd);         
            break;
        }
    }
    //datasql.closemysql();
}

void Connection::send(int sockfd,int res){
    const char* str;
    if(res==0)
    {
        str="operation success";
    }else{
        str="operation fail";
    }
    char buf[strlen(str)];
    strcpy(buf, str);
    int  data_size = strlen(str); 
    int data_left = data_size; 
    while (data_left > 0) 
    { 
        ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left); 
        if (bytes_write == -1 && errno == EAGAIN) { 
            break;
        }
        data_left -= bytes_write; 
    }
    /*char buf[readBuffer->size()];
    strcpy(buf, readBuffer->c_str());
    int  data_size = readBuffer->size(); 
    int data_left = data_size; 
    while (data_left > 0) 
    { 
        ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left); 
        if (bytes_write == -1 && errno == EAGAIN) { 
            break;
        }
        data_left -= bytes_write; 
    }*/
}
