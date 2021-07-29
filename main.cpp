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
using std::cout;
using std::endl;

bool is_running = true;
int main()
{
    const int port{25535};
    cout<<"Configuration started"<<endl;
    cout<<"Server port: "<<port<<endl;

    //opening logfile for write
    auto fout_ptr = std::make_shared<std::ofstream>("LogFile.txt",std::ios_base::out|std::ios_base::trunc);
    if(!fout_ptr->good()){
        cout<<"Can't open file for writing"<<endl;
        return -5;
    }
    std::mutex mtx;

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
        cout<<"Waiting for signal"<<endl;
        auto client_socket = pserver.get_connection();
        if(client_socket.handler<0){
            cout<<"Invalid connection"<<endl;
            continue;
        }
        cout<<"Got connection"<<endl;
        std::thread t([](int fd,std::shared_ptr<std::ostream> fout){
            const size_t buffer_size=128;
            char buffer[128];
            while(true){
                auto size = read(fd,buffer,512);
                if(size<=0){
                    *fout<<"Read error\n"<<errno;
                    continue;
                }
                if(size==1){
                    *fout<<"End of stream";
                    close(fd);
                    break;
                }
                fout->write(buffer,size);
                fout->flush();
            }
        },client_socket.handler,fout_ptr);
        t.detach();

    }
    return 0;
}
