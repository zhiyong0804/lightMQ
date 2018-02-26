#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include "utilx.hpp"
#include "Event.hpp"
 
namespace socketx
{
    class Socket
    {
        protected:
            struct addrinfo hints;
            struct sockaddr_storage hostaddr;
            socklen_t hostlen;

            /*Files descriptors*/
            int socketfd;
        public:
            /*Constructor*/
            Socket();
            Socket(int fd):socketfd(fd){}

            virtual ~Socket();

            /*Return the hostname and port of the host it connect currently*/
            std::string getHostname();
            std::string getPeername();
            size_t getPort();
            int getFD(){  return socketfd; }
            int closeConn();            
    };

    class ServerSocket: public Socket
    {
        public:
            ServerSocket(EventLoop *loop, std::string port);
            ~ServerSocket();

            /*Listen to a port*/
            int listen();

            /*Accept a connection.
            * Return a file descriptor.
            */
            int accept();

            /*Regist events*/
            void registEvents()
            {
                event_->setFD(socketfd);
                event_->enableReading();
            }

            void setNewConnctionFunc(const std::function<void(int)> &func){ newConnectionFunc = func; }
            void handleAccept();
        private:
            std::string port_;
            std::shared_ptr<Event> event_;
            EventLoop *loop_;

            std::function<void(int)> newConnectionFunc;
    };

    class ClientSocket: public Socket
    {
        public:
            ClientSocket(std::string hostname, std::string port);
            ~ClientSocket();

            /*Connect to a host*/
            int connect();
            
            void handleConnect();
        private:
            std::string hostname_;
            std::string port_;
    };
}

#endif