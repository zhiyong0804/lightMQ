#include "EventLoop.hpp"
#include "Server.hpp"

class Server_test{
    public:
        Server_test(socketx::EventLoop *loop, std::string port)
        :loop_(loop), port_(port),
        server_(new socketx::Server(loop,port)){
            server_->setHandleConnectionFunc(std::bind(&Server_test::handleConnection, this, std::placeholders::_1));
            server_->setHandleCloseEvents(std::bind(&Server_test::handleCloseEvents, this, std::placeholders::_1));
        }

        ~Server_test(){
            delete server_;
        }

        void start(){
            server_->start();
        }

        void handleConnection(std::shared_ptr<socketx::Connection> conn){
            printf("New connection comes, we are going to set read events!!!\n");
            server_->setHandleReadEvents(std::bind(&Server_test::handleReadEvents, this,  std::placeholders::_1));
        }
        void handleReadEvents(std::shared_ptr<socketx::Connection> conn){
            printf("Read events...\n");
        }
        void handleCloseEvents(std::shared_ptr<socketx::Connection> conn){
            printf("Close connection...\n");
        }

    private:
        socketx::EventLoop *loop_;
        socketx::Server *server_;
        std::string port_;
};


int main(int argc, char **argv){
    if(argc!=2){
        fprintf(stderr,"usage: %s <port>\n", argv[0]);
        exit(0);
    }

    std::string port(argv[1]);
    socketx::EventLoop loop;
    Server_test server(&loop,port);
    server.start(); 
    loop.loop();

    return 0;
}