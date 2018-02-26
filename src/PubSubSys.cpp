#include "PubSub.hpp"
#include <glog/logging.h>

//#include "zookeeperService.h"

class PubSubSys
{
    public:
        PubSubSys(socketx::EventLoop *loop, std::string port)
        : loop_(loop),
          port_(port),
          pubsub_(std::make_shared<PubSub>()),
          server_(std::make_shared<socketx::Server>(loop, port))
        {
            server_->setHandleConnectionFunc(std::bind(&PubSubSys::handleConnection, this, std::placeholders::_1));
            server_->setHandleCloseEvents(std::bind(&PubSubSys::handleCloseEvents, this, std::placeholders::_1));
        }

        void start()
        {
            server_->start();
            LOG(INFO) << "server is running."; 
        }

        void handleConnection(std::shared_ptr<socketx::Connection> conn)
        {
            LOG(INFO) << "New connection comes, we are going to set read events!!!";
            server_->setHandleReadEvents(std::bind(&PubSubSys::handleReadEvents, this, std::placeholders::_1));
        }

        void handleReadEvents(std::shared_ptr<socketx::Connection> conn)
        {
            socketx::Message msg = conn->recvmsg();
            if(msg.getSize()==0)
            {
                conn->handleClose();
                LOG(ERROR) << "msg content is empty, close connection";
                return;
            }

            pubsub_->filter(conn, msg);
        }
 
        void handleCloseEvents(std::shared_ptr<socketx::Connection> conn)
        {
            LOG(INFO) << "Close connection...";
        }

    private:
        std::shared_ptr<PubSub> pubsub_;
        socketx::EventLoop *loop_;
        std::shared_ptr<socketx::Server> server_;
        std::string port_;
};

int main(int argc, char **argv)
{
    if(argc!=2)
	{
        fprintf(stderr,"usage: %s <port>\n", argv[0]);
        exit(0);
    }

    FLAGS_log_dir = ".";
    google::InitGoogleLogging(argv[0]);
    google::SetLogDestination(google::GLOG_INFO, "./MASTER_");
    google::SetStderrLogging(google::GLOG_INFO);
    google::SetLogFilenameExtension("log_");
    FLAGS_colorlogtostderr = true;  // Set log color
    FLAGS_logbufsecs = 0;  // Set log output speed(s)
    FLAGS_max_log_size = 1024;  // Set max log file size
    FLAGS_stop_logging_if_full_disk = true;  // If disk is full
    
    std::string port(argv[1]);
    socketx::EventLoop loop;
    //ZooKeeperService zks;
    //zks.zks_init("127.0.0.1", 2181, "127.0.0.1", atoi(argv[1]));
    PubSubSys server(&loop,port);
    server.start();
    loop.loop();

    return 0;
}
