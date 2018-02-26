#include "Client.hpp"

namespace socketx{

    Client::Client(EventLoop *loop, std::string hostname, std::string port)
        :loop_(loop),
        hostname_(hostname),
        port_(port),
       socket_(std::make_shared<ClientSocket>(hostname,port)){

    }

    Client::~Client(){
    }

     /*Start to connect to a host. 
    * This function should retry the connection
    * until the connection is established or terminated by users.
    */
    void Client::start(){
        socket_->handleConnect();
        newConnection(socket_->getFD());
    }

    void Client::newConnection(int fd){
        currentConn = std::make_shared<Connection>(loop_, fd);
        if(!connectionsMap.count(fd)){
            connectionsMap[fd] = currentConn;
            currentConn->setHandleCloseEvents(std::bind(&Client::removeConnection, this, std::placeholders::_1));
            /* Run user defined function for new connection*/
            handleConnectionFunc(currentConn);
        }
        else{
            printf("Error: existing file descriptor for a new connection!\n");
        }
    }

    void Client::removeConnection(std::shared_ptr<Connection> conn){
        /*Run user defined function for closing connection*/
        handleCloseEvents(conn);
    }
}