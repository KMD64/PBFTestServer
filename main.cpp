#include <iostream>

#include "tcpserver.h"
#include "worker.h"

#include <arpa/inet.h>
#include <fstream>
#include <memory>
#include <thread>
#include <signal.h>
#include <unistd.h>
#include <mutex>
#include <system_error>

using std::cout;
using std::endl;

bool is_running = true;

int main(int argc, char **argv)
{
    if(argc<2)return EINVAL;
    const int port{atoi(argv[1])};
    cout<<"Configuration started"<<endl;
    cout<<"Server port: "<<port<<endl;

    //opening logfile for write
    auto fout_ptr = std::make_shared<std::ofstream>("LogFile.txt",std::ios_base::out|std::ios_base::trunc);
    if(!fout_ptr->good()){
        cout<<"Can't open file for writing"<<endl;
        return -5;
    }
    auto mtx_ptr = std::make_shared<std::mutex>();

    //creating server
    TcpServer pserver;
    try{
        pserver.listen(port);
    }catch(std::runtime_error e){
        std::cerr<<e.what()<<endl;
        return errno;
    }

    cout<<"Configuration finished. Listening port "<<port<<endl;
    while(true){
        cout<<"Waiting for connection"<<endl;
        auto client_socket = pserver.get_connection();
        if(client_socket.handler<0){
            cout<<"Connection failed"<<endl;
            continue;
        }
        cout<<"Got connection "<<inet_ntoa(client_socket.data.sin_addr)<<":"<<client_socket.data.sin_port<<endl;

        //creating thread for handling client socket
        std::thread t([](int fd,std::shared_ptr<std::ostream> fout,std::shared_ptr<std::mutex> mtx){
            const size_t buffer_size=128;
            char buffer[128];
            while(true){
                auto size = read(fd,buffer,512);
                if(size<=0){
                    //Error. Closing connection
                    close(fd);
                    break;
                }
                //Block the stream for seamless writing
                std::unique_lock<std::mutex> lk(*mtx);
                fout->write(buffer,size);
                *fout<<'\n';
                fout->flush();
            }
        },client_socket.handler,fout_ptr,mtx_ptr);
        t.detach();
    }
    return 0;
}
