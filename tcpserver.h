#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <netinet/in.h>
struct s_socket{
    int handler;
    sockaddr_in data;
    socklen_t size;
};
class TcpServer
{
    int _handler{0};

public:
    TcpServer();
    ~TcpServer();
    int handler() const;

    void listen(unsigned short port);

    s_socket get_connection();
};

#endif // TCPSERVER_H
