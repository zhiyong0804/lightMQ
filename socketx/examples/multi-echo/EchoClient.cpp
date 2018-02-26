#include "EventLoop.hpp"
#include "Client.hpp"
#include "Connection.hpp"
#include "Timer.hpp"
#include "Threadx.hpp"

class EchoClient{
    public:
        EchoClient(socketx::EventLoop *loop, std::string hostname, std::string port)
        :loop_(loop), hostname_(hostname),port_(port),
        client_(std::make_shared<socketx::Client>(loop,hostname,port)){
            client_->setHandleConnectionFunc(std::bind(&EchoClient::handleConnection, this, std::placeholders::_1));
            client_->setHandleCloseEvents(std::bind(&EchoClient::handleCloseEvents, this, std::placeholders::_1));
            
            /*Get a timer for sending messages to the server*/
            timer_ = std::make_shared<socketx::Timer>(loop_);
            timer_->addTimer(1,5,std::bind(&EchoClient::sendMessages, this));
        }

        ~EchoClient(){
        }

        void start(){
            client_->start();
        }

        void sendMessages(){
            std::string line = "Message from client fd " + std::to_string(clientConn->getFD()) + '\n';
            clientConn->send(line.c_str(),line.size());
        }

        void handleConnection(std::shared_ptr<socketx::Connection> conn){
            printf("New connection comes, we are going to set read events!!!\n");
            client_->setHandleReadEvents(std::bind(&EchoClient::handleReadEvents, this, std::placeholders::_1));
            clientConn = conn;
        }
        void handleReadEvents(std::shared_ptr<socketx::Connection> conn){
            std::string line = conn->readline();
            if(line.size()==0) conn->handleClose();
            else std::cout<<line;
        }
        void handleCloseEvents(std::shared_ptr<socketx::Connection> conn){
            printf("Close connection...\n");
        }

    private:
        std::shared_ptr<socketx::Connection> clientConn;
        socketx::EventLoop *loop_;
        std::shared_ptr<socketx::Client> client_;
        std::shared_ptr<socketx::Timer> timer_;
        std::string hostname_;
        std::string port_;
};


int main(int argc, char **argv){
    if(argc!=3){
        fprintf(stderr,"usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }
    std::string hostname(argv[1]);
    std::string port(argv[2]);
    socketx::EventLoop loop;
    
    EchoClient client(&loop,hostname,port);
    client.start();
    loop.loop();

    return 0;
}