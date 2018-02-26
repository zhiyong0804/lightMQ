#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "utilx.hpp"
#include "Socket.hpp"
#include "Connection.hpp"

namespace socketx{

    class Client{
        public:
            Client(EventLoop *loop, std::string hostname, std::string port);
            
            ~Client();

            /*Start to connect to a host. 
            * This function should retry the connection
            * until the connection is established or terminated by users.
            */
            void start();

            /* Remove the connection, called by an Connection*/
            void removeConnection(std::shared_ptr<Connection> conn);


            /* Provide an API for users 
            *  to handle new connections
            */
            void setHandleConnectionFunc(const std::function<void(std::shared_ptr<Connection>)> &func){
                handleConnectionFunc = func;
            }

            /* Provide an API for users
            *  to handle events of a connection.
            *  Users need to set these functions in their codes
            *  to regist corresponding events.
            */
            void setHandleReadEvents(const std::function<void(std::shared_ptr<Connection>)> &func){
                handleReadEvents = func;
                currentConn->setHandleReadEvents(handleReadEvents);
                currentConn->registReadEvents();
            }
            void setHandleWriteEvents(const std::function<void(std::shared_ptr<Connection>)> &func){
                handleWriteEvents = func;
                currentConn->setHandleWriteEvents(handleWriteEvents);
                currentConn->registWriteEvents();
            }
            void setHandleCloseEvents(const std::function<void(std::shared_ptr<Connection>)> &func){
                handleCloseEvents = func;
            }

            /* This function need reture the current connection*/
            std::shared_ptr<Connection> getCurrentConnection(){
                return currentConn;
            }

        private:

            /* Handle new connections
            * This function is used to create a new object
            * Connection as an event listened by EventLoop
            */
            void newConnection(int fd);

            EventLoop *loop_;
            std::string hostname_;
            std::string port_;
            std::shared_ptr<ClientSocket> socket_;
            std::shared_ptr<Connection> currentConn;
            std::map<int, std::shared_ptr<Connection>> connectionsMap;
            
            std::function<void(std::shared_ptr<Connection>)> handleConnectionFunc;
            std::function<void(std::shared_ptr<Connection>)> handleReadEvents;
            std::function<void(std::shared_ptr<Connection>)> handleWriteEvents;
            std::function<void(std::shared_ptr<Connection>)> handleCloseEvents;
    };

}


#endif