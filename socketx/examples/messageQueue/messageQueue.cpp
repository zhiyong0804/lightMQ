#include "socketx.hpp"

std::shared_ptr<socketx::Connection> stdoutConn;
socketx::MessageQueue q;

void stdinReadEvents(std::shared_ptr<socketx::Connection> conn){
    std::string line;
    if(std::getline(std::cin,line)){
        socketx::Message msg(line);
        q.recv(msg);
        q.send();
        //stdoutConn->sendmsg(msg);
        printf("\n");
    }
    else
        printf("Read error from stdin....\n");   
}


int main(){
    socketx::EventLoop loop;

    int stdoutfd = fileno(stdout);
    stdoutConn = std::make_shared<socketx::Connection>(&loop,stdoutfd);
    q.addConnection(stdoutConn);

    int stdinfd = fileno(stdin);
    std::shared_ptr<socketx::Connection>  stdinConn = std::make_shared<socketx::Connection>(&loop,stdinfd);
    stdinConn->setHandleReadEvents(std::bind(stdinReadEvents, std::placeholders::_1));
    stdinConn->registReadEvents();

    loop.loop();
    return 0;
}