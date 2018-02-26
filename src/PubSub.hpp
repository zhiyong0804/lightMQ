#ifndef _PUBSUB_HPP__
#define _PUBSUB_HPP__

#include "socketx.hpp"
#include "Protocol.hpp"


class PubSub{

    public:
        PubSub()=default;

        void subscriberStub(std::shared_ptr<Connection> conn, const Message &msg);
        void publisherStub(std::shared_ptr<Connection> conn, const Message &msg);
        void filter(std::shared_ptr<Connection> conn, const Message &msg);
        
    private:
        void regist(std::shared_ptr<Connection> conn);
        void subscribe(std::shared_ptr<Connection> conn, const std::string &topic);
        void unsubscribe(std::shared_ptr<Connection>, const std::string &topic);

        void publish(const std::string &topic, const Message &msg);
        void unpublish(std::shared_ptr<Connection> conn, const std::string &topic);

        void broadcast(const string &topic);

        void checker();
        void publishMsg(const std::string &topic);

        /*
         * key : topic值
         * value : 消息队列，一个topic对应一个单独的msg queue，
         *         msg queue确保把所有的消息发送所有的订阅者，而且仅发送一次
         */
        std::unordered_map<std::string, std::shared_ptr<socketx::MessageQueue>> queueMap_;

        
        std::unordered_map<std::string, std::vector<std::shared_ptr<Connection>>> channelMap_;


        /*
         * key : 连接的客户端
         * value : 一个集合，所有订阅的主题
         */
        std::unordered_map<std::shared_ptr<Connection>, std::set<std::string>> subscriberMap_;
        socketx::ThreadPool pool_;
};
#endif
