#ifndef MESSAGEQUEUE_HPP
#define MESSAGEQUEUE_HPP

#include "socketx.hpp"

namespace socketx
{
    class MessageQueue
    {
        public:
            MessageQueue()=default;

            ssize_t send();
            void recv(Message msg);

            void addConnection(std::shared_ptr<Connection> conn);
            void removeConnection(std::shared_ptr<Connection> conn);
            bool empty() const{ return queue_.empty();}

        private:
            squeue<Message> queue_;
            std::vector<std::shared_ptr<Connection>> sendList;
    };
}

#endif