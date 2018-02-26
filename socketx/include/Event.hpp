#ifndef EVENT_HPP
#define EVENT_HPP

#include "utilx.hpp"


namespace socketx
{

    /*Forward declaration*/
    class EventLoop;

    class Event:public std::enable_shared_from_this<Event>
    {
        public:
            Event(EventLoop *loop, int fd);
            ~Event();

            /*Handle the event according to revents*/
            void handleEvent();
            
            /*Regist Read and Write events*/
            void enableReading();
            void enableWriting();
            void disableReading();
            void disableWriting();

            /*Unregiest events*/
            void deleteEvent();
            
            /*Set callback function*/
            void setReadFunc(const std::function<void()> &func)
            {
                readFunc = func;
            }
            
            void setWriteFunc(const std::function<void()> &func)
            {
                writeFunc = func;
            }
            
            void setErrorFunc(const std::function<void()> &func)
            {
                errorFunc = func;
            }

            /*Get or set some data member */
            int getFD()
            {
                return fd_;
            }
            
            int getEvents()
            {
                return events_;
            }
            
            int getRevents()
            {
                return revents_;
            }
            
            void setFD(int fd)
            {
                fd_ = fd;
            }
            
            void setEvents(int events)
            {
                events_ = events;
            }
            
            void setRevents(int revents)
            {
                revents_ = revents;
            }
        
        private:
            std::function<void()> readFunc;
            std::function<void()> writeFunc;
            std::function<void()> errorFunc;

            EventLoop *loop_;
            int fd_;
            int events_;
            int revents_;
    };

}
#endif
