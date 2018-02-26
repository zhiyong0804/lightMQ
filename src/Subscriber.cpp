#include "Subscriber.hpp"

bool Subscriber::isSubscribed(std::string topic)
{
    auto it = std::find(topics_.begin(),topics_.end(),topic);
    if(it==topics_.end())
        return false;
    return true;
}

void Subscriber::printer(const std::string &topic, const std::string &msg)
{
    cout<<"topic: "<<topic<<endl;
    cout<<"Message: "<<msg<<endl;
}

void Subscriber::filter(const Message &msg)
{
    Protocol pro;
    pro.deserialization(msg);
    std::string cmd = pro.getCmd();
    printf("Received a message of command cmd %s.\n", cmd.c_str());
    if(cmd=="broadcast")
    {
        std::string topic = pro.getTopic();
        if(isSubscribed(topic)) return;
        else
        {
            printf("Subscribe topic %s <Y/N> ?:",topic.c_str());
            std::string str;
            cin>>str;
            if(str=="Y") subscribe(topic);
            else return;
        }
    }
    else if(cmd=="pub")
    {
        std::string topic = pro.getTopic();
        std::string msg = pro.getMsg();
        printer(topic,msg);
    }
}

void Subscriber::regist()
{
    Protocol pro("regist","","");
    socketx::Message msg = pro.serialization();
    conn_->sendmsg(msg);
}

void Subscriber::subscribe(std::string topic)
{
    if(isSubscribed(topic))
    {
        printf("topic %s was subscribed before...\n",topic.c_str());
        return;
    }
    else
        topics_.push_back(topic);

    Protocol pro("sub",topic,"");
    socketx::Message msg = pro.serialization();
    conn_->sendmsg(msg);
    printf("Subsribe topic %s \n", topic.c_str());
}

void Subscriber::unsubscribe(std::string theme)
{
    if(!isSubscribed(theme))
    {
        printf("Theme %s was unsubscribed before...\n",theme.c_str());
        return;
    }
    else
        topics_.erase(std::find(topics_.begin(),topics_.end(),theme));

    Protocol pro("unsub",theme,"");
    socketx::Message msg = pro.serialization();
    conn_->sendmsg(msg);
    printf("Unsubsribe Theme %s \n", theme.c_str());
}
