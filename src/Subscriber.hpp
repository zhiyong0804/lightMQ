#ifndef SUBSCRIBER_HPP
#define SUBSCRIBER_HPP

#include "socketx.hpp"
#include "Protocol.hpp"

class Subscriber{

    public: 
        Subscriber(std::shared_ptr<socketx::Connection> conn)
        : conn_(conn)
        {

        }

        void filter(const Message &msg);
        void printer(const std::string &topic, const std::string &msg);

        void regist();
        void subscribe(std::string topic);
        void unsubscribe(std::string topic);

        bool isSubscribed(std::string topic);

    private: 
        std::vector<std::string> topics_;
        std::shared_ptr<socketx::Connection> conn_;
};


#endif