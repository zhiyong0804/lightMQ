
#include "Publisher.hpp"
#include <glog/logging.h>

class PublisherClient
{
    public:
        PublisherClient(socketx::EventLoop *loop, std::string hostname, std::string port)
        : loop_(loop), 
          hostname_(hostname),
          port_(port),
          client_(std::make_shared<socketx::Client>(loop, hostname, port))
        {
            client_->setHandleConnectionFunc(std::bind(&PublisherClient::handleConnection, this, std::placeholders::_1));
            client_->setHandleCloseEvents(std::bind(&PublisherClient::handleCloseEvents, this, std::placeholders::_1));
            /*Get file descriptor of stdin and regist it into EventLoop*/
            cout<<"Input <Cmd> <Channel> <Message>: " << endl;
            int fd = fileno(stdin);
            stdinConn = std::make_shared<socketx::Connection>(loop_, fd);
            stdinConn->setHandleReadEvents(std::bind(&PublisherClient::stdinReadEvents, this, std::placeholders::_1));
            stdinConn->registReadEvents();
        }

        void start()
        {
            client_->start();
        }

        void stdinReadEvents(std::shared_ptr<socketx::Connection> conn)
        {
            std::string cmd, topic, message;
            if( cin >> cmd >> topic >> message)
            {
                if(cmd=="pub") publisher_->publish(topic, message);
                else if(cmd=="unpub") publisher_->unpublish(topic);
            }
            else
                printf("Read error from stdin....\n");
        }

        void handleConnection(std::shared_ptr<socketx::Connection> conn)
        {
            printf("New connection comes, we are going to set read events!!!\n");
            client_->setHandleReadEvents(std::bind(&PublisherClient::handleReadEvents, this, std::placeholders::_1));
            publisher_ = std::make_shared<Publisher>(conn);
        }

        void handleReadEvents(std::shared_ptr<socketx::Connection> conn)
        {
            socketx::Message msg = conn->recvmsg();
            if(msg.getSize()==0) conn->handleClose();
        }

        void handleCloseEvents(std::shared_ptr<socketx::Connection> conn)
        {
            printf("Close connection...\n");
            loop_->quit();
        }

    private:
        std::shared_ptr<Publisher> publisher_;
        std::shared_ptr<socketx::Connection> stdinConn;
        socketx::EventLoop *loop_;
        std::shared_ptr<socketx::Client> client_;
        std::string hostname_;
        std::string port_;
};


int main(int argc, char **argv)
{
    if(argc!=3)
    {
        fprintf(stderr,"usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }

    FLAGS_log_dir = "./log";
    google::InitGoogleLogging(argv[0]);
    google::SetLogDestination(google::GLOG_INFO, "./PUBCLI_");
    google::SetStderrLogging(google::GLOG_INFO);
    google::SetLogFilenameExtension("log_");
    FLAGS_colorlogtostderr = true;  // Set log color
    FLAGS_logbufsecs = 0;  // Set log output speed(s)
    FLAGS_max_log_size = 1024;  // Set max log file size
    FLAGS_stop_logging_if_full_disk = true;  // If disk is full

    std::string hostname(argv[1]);
    std::string port(argv[2]);
    socketx::EventLoop loop;
    PublisherClient client(&loop,hostname,port);
    client.start();
    loop.loop();

    return 0;
}

