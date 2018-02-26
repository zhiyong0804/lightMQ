#include "EventLoop.hpp"
#include "Server.hpp"

class EchoServer{
    public:
        EchoServer(socketx::EventLoop *loop, std::string port)
        :loop_(loop), port_(port),
        server_(std::make_shared<socketx::Server>(loop,port)){
            server_->setHandleConnectionFunc(std::bind(&EchoServer::handleConnection, this, std::placeholders::_1));
            server_->setHandleCloseEvents(std::bind(&EchoServer::handleCloseEvents, this, std::placeholders::_1));
        }

        void start(){
            server_->start();
        }

        void handleConnection(std::shared_ptr<socketx::Connection> conn){
            printf("New connection comes, we are going to set read events!!!\n");
            server_->setHandleReadEvents(std::bind(&EchoServer::handleReadEvents, this,  std::placeholders::_1));
        }
        void handleReadEvents(std::shared_ptr<socketx::Connection> conn){
            socketx::Message msg = conn->recvmsg();
            if(msg.getSize()==0){
                conn->handleClose();
                return;
            }else
            std::cout<<std::string(msg.getData())<<std::endl;
            conn->sendmsg(msg);
        }
        void handleCloseEvents(std::shared_ptr<socketx::Connection> conn){
            printf("Close connection...\n");
        }

    private:
        socketx::EventLoop *loop_;
        std::shared_ptr<socketx::Server> server_;
        std::string port_;
};


int main(int argc, char **argv){
    if(argc!=2){
        fprintf(stderr,"usage: %s <port>\n", argv[0]);
        exit(0);
    }

    std::string port(argv[1]);
    socketx::EventLoop loop;
    EchoServer server(&loop,port);
    server.start(); 
    loop.loop();

    return 0;
}