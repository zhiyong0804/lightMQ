
#include "MessageQueue.hpp"

namespace socketx
{
    ssize_t MessageQueue::send()
    {
        if(sendList.size()==0) return -1;
        Message msg;
        bool flag = queue_.try_pop(msg);
        if(flag)
        {
            for(auto it=sendList.begin(); it!=sendList.end(); ++it)
            {
                (*it)->sendmsg(msg);
            }
            return 1;
        }
        
        return -1;
    }

    void MessageQueue::recv(Message msg)
    {
        queue_.push(msg);
    }

    void MessageQueue::addConnection(std::shared_ptr<Connection> conn)
    {
        if(std::find(sendList.begin(),sendList.end(),conn)==sendList.end())
        {
            sendList.push_back(conn);
        }
        else
        {
            printf("This connection is existed...\n");
        }
    }
    
    void MessageQueue::removeConnection(std::shared_ptr<Connection> conn)
    {
        auto it = std::find(sendList.begin(),sendList.end(),conn);
        if(it==sendList.end())
        {
            printf("No such a connection in MessageQueue...\n");
        }
        else
        {
            sendList.erase(it);
        }
    }
}
