#include "EventLoop.hpp"
#include "Client.hpp"
#include "Connection.hpp"

class Client_test{
    public:
        Client_test(socketx::EventLoop *loop, std::string hostname, std::string port)
        :loop_(loop), hostname_(hostname),port_(port),
        client_(new socketx::Client(loop,hostname,port)){
            client_->setHandleConnectionFunc(std::bind(&Client_test::handleConnection, this, std::placeholders::_1));
            client_->setHandleCloseEvents(std::bind(&Client_test::handleCloseEvents, this, std::placeholders::_1));
        }

        ~Client_test(){
            delete client_;
        }

        void start(){
            client_->start();
        }

        void handleConnection(std::shared_ptr<socketx::Connection> conn){
            printf("New connection comes, we are going to set read events!!!\n");
            client_->setHandleReadEvents(std::bind(&Client_test::handleReadEvents, this, std::placeholders::_1));
        }
        void handleReadEvents(std::shared_ptr<socketx::Connection> conn){
            printf("Read events...\n");
        }
        void handleCloseEvents(std::shared_ptr<socketx::Connection> conn){
            printf("Close connection...\n");
        }

    private:
        socketx::EventLoop *loop_;
        socketx::Client *client_;
        std::string hostname_;
        std::string port_;
};


int main(int argc, char **argv){
    if(argc!=3){
        fprintf(stderr,"usage: %s <port>\n", argv[0]);
        exit(0);
    }
    std::string hostname(argv[1]);
    std::string port(argv[2]);
    socketx::EventLoop loop;
    Client_test client(&loop,hostname,port);
    client.start();
    loop.loop();

    return 0;
}