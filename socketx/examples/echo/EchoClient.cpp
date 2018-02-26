#include "EventLoop.hpp"
#include "Client.hpp"
#include "Connection.hpp"

class EchoClient{
    public:
        EchoClient(socketx::EventLoop *loop, std::string hostname, std::string port)
        :loop_(loop), hostname_(hostname),port_(port),
        client_(std::make_shared<socketx::Client>(loop,hostname,port)){
            client_->setHandleConnectionFunc(std::bind(&EchoClient::handleConnection, this, std::placeholders::_1));
            client_->setHandleCloseEvents(std::bind(&EchoClient::handleCloseEvents, this, std::placeholders::_1));
            /*Get file descriptor of stdin and regist it into EventLoop*/
            int fd = fileno(stdin);
            stdinConn = std::make_shared<socketx::Connection>(loop_,fd);
            stdinConn->setHandleReadEvents(std::bind(&EchoClient::stdinReadEvents, this, std::placeholders::_1));
            stdinConn->registReadEvents();
        }

        void start(){
            client_->start();
        }

        void stdinReadEvents(std::shared_ptr<socketx::Connection> conn){
            std::string line;
            if(std::getline(std::cin,line)){
                line += '\n';
                socketx::Message msg(line);
                clientConn->sendmsg(msg);
            }
            else
                printf("Read error from stdin....\n");
            
        }

        void handleConnection(std::shared_ptr<socketx::Connection> conn){
            printf("New connection comes, we are going to set read events!!!\n");
            client_->setHandleReadEvents(std::bind(&EchoClient::handleReadEvents, this, std::placeholders::_1));
            clientConn = conn;
        }
        void handleReadEvents(std::shared_ptr<socketx::Connection> conn){
            socketx::Message msg = conn->recvmsg();
            if(msg.getSize()==0){
                conn->handleClose();
                return;
            }else
            std::cout<<std::string(msg.getData())<<std::endl;
        }
        void handleCloseEvents(std::shared_ptr<socketx::Connection> conn){
            printf("Close connection...\n");
            loop_->quit();
        }

    private:
        std::shared_ptr<socketx::Connection> stdinConn;
        std::shared_ptr<socketx::Connection> clientConn;
        socketx::EventLoop *loop_;
        std::shared_ptr<socketx::Client> client_;
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