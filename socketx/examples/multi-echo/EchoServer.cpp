#include "EventLoop.hpp"
#include "Server.hpp"
#include "Threadx.hpp"

socketx::cirqueue<std::string> data_queue(100);
std::vector<std::shared_ptr<socketx::Connection>> connectionList;
std::mutex mut;

void echo_send(){
    std::string line;
    while(1){
        data_queue.wait_pop(line);
        mut.lock();
        for(auto it=connectionList.begin();it!=connectionList.end();++it){
            (*it)->send(line.c_str(),line.size());
        }
        mut.unlock();
    }
} 

class Server_test{
    public:
        Server_test(socketx::EventLoop *loop, std::string port)
        :loop_(loop), port_(port),
        server_(std::make_shared<socketx::Server>(loop,port)){
            server_->setHandleConnectionFunc(std::bind(&Server_test::handleConnection, this, std::placeholders::_1));
            server_->setHandleCloseEvents(std::bind(&Server_test::handleCloseEvents, this, std::placeholders::_1));
        }

        void start(){
            server_->start();
        }

        void handleConnection(std::shared_ptr<socketx::Connection> conn){
            printf("New connection comes, we are going to set read events!!!\n");
            server_->setHandleReadEvents(std::bind(&Server_test::handleReadEvents, this,  std::placeholders::_1));
            mut.lock();
            connectionList.push_back(conn);
            mut.unlock();
        }
        void handleReadEvents(std::shared_ptr<socketx::Connection> conn){
            std::string line = conn->readline();
            if(line.size()==0){
                conn->handleClose();
                return;
            }else
            std::cout<<line;
            data_queue.wait_push(line);
        }
        void handleCloseEvents(std::shared_ptr<socketx::Connection> conn){
            printf("Close connection...\n");
            auto it = std::find(connectionList.begin(),connectionList.end(),conn);
            mut.lock();
            connectionList.erase(it);
            mut.unlock();
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
    Server_test server(&loop,port);

    /*Create a thread pool for handling sending fuctions*/
    socketx::ThreadPool pool;
    pool.submit(echo_send);
    
    /*Start server*/
    server.start(); 
    loop.loop();

    return 0;
}