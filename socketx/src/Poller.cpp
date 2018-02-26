#include "Poller.hpp"

namespace socketx{

    Poller::Poller():
        timeout_(INFTIM)
    {

    }

    Poller::~Poller()
    {
         
    }

    /*Wrapper function of Linux/Unix poll
    * Return a vector of active events
    */
    std::vector<std::shared_ptr<Event>> Poller::poll()
    {
        int num = ::poll(&*pollfdList.begin(),pollfdList.size(),timeout_);
        
        /*Construct active events vector*/
        std::vector<std::shared_ptr<Event>> vec;
        for(auto it=pollfdList.begin();it!=pollfdList.end();++it)
        {
            if(it->revents>0)
            {
                auto events = eventsMap[it->fd];
                events->setRevents(it->revents); 
                vec.push_back(events);
            }
        }
        return vec;
    }

    /*Update or delete an event from eventList*/
    void Poller::updateEvent(std::shared_ptr<Event> event)
    {
        int eventfd = event->getFD();
        if(eventsMap.count(eventfd))
        {
            /*Update pollfdList*/
            auto it = std::find_if(pollfdList.begin(),pollfdList.end(),
                                    [eventfd](struct pollfd &pollfd_){return eventfd==pollfd_.fd;});
            it->events = event->getEvents();
            it->revents = event->getRevents();
        }
        else
        {
            /*Update eventsMap*/
            eventsMap[eventfd] = event;
            /*Update eventsList*/
            //eventsList.push_back(event);
            /*Update pollfdList*/
            struct pollfd pollfd_; 
            pollfd_.fd = eventfd;
            pollfd_.events = event->getEvents();
            pollfd_.revents = event->getRevents();
            pollfdList.push_back(pollfd_);
        }
    }
    
    void Poller::deleteEvent(std::shared_ptr<Event> event)
    {
        int eventfd = event->getFD();
        if(eventsMap.count(eventfd))
        {
            auto it = std::find_if(pollfdList.begin(),pollfdList.end(),
                                    [eventfd](struct pollfd &pollfd_){return eventfd==pollfd_.fd;});
            pollfdList.erase(it);
            eventsMap.erase(eventfd);
        }
        else
        {
            printf("Poller::deleteEvent error! No such an event...\n");
        }
    }
}
