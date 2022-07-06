/******************************
*   author: yuesong-feng
*   
*
*
******************************/
#pragma once
#include <functional>
class EventLoop;
//class Datahandle;
class Socket;
class Channel;
class Buffer;
class Connection
{
private:
    EventLoop *loop;
    Socket *sock;
    Channel *channel;
    std::function<void(int)> deleteConnectionCallback;
    Buffer *readBuffer;
    //Datahandle *datasql;
public:
    Connection(EventLoop *_loop, Socket *_sock);
    ~Connection();
    
    void echo(int sockfd);
    //void handHttp(int sockfd);
    void setDeleteConnectionCallback(std::function<void(int)>);
    void send(int sockfd,int res);
    enum METHOD
    {
        GET = 0,
        POST,
        HEAD,
        PUT,
        DELETE,
        TRACE,
        OPTIONS,
        CONNECT,
        PATH
    };
    enum CHECK_STATE
    {
        CHECK_STATE_REQUESTLINE = 0,
        CHECK_STATE_HEADER,
        CHECK_STATE_CONTENT
    };
    enum HTTP_CODE
    {
        NO_REQUEST,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION
    };
    enum LINE_STATUS
    {
        LINE_OK = 0,
        LINE_BAD,
        LINE_OPEN
    };
};

