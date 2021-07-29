#include "tcpserver.h"

#include <netinet/in.h>

#include <errno.h>
#include <stdexcept>
#include <unistd.h>
TcpServer::TcpServer()
{
    _handler = socket(AF_INET,SOCK_STREAM,0);
}

TcpServer::~TcpServer()
{
    if(_handler>=0){
        close(_handler);
    }
}

int TcpServer::handler() const
{
    return _handler;
}

void TcpServer::listen(unsigned short port)
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = in_addr{0};//in_addr{INADDR_LOOPBACK};
    if(::bind(_handler,reinterpret_cast<sockaddr*>(&addr),sizeof(addr))){
        throw std::runtime_error("Failed to setup socket");
    }
    ::listen(_handler,100);
}

s_socket TcpServer::get_connection()
{
    s_socket sck;
    sck.handler = accept(_handler,reinterpret_cast<sockaddr*>(&sck.data),&sck.size);
    return sck;
}
