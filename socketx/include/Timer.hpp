#ifndef TIMER_HPP
#define TIMER_HPP

#include "utilx.hpp"
#include "Event.hpp"

namespace socketx{

    /*Forward declaration*/
    class EventLoop;

    class Timer{
        public:
            Timer(EventLoop *loop);
            ~Timer();
            /*Add a timer by users.
            * User should set an user defined function
            * for handling events when timer expitation has occured.
            */
            void addTimer(size_t start, size_t interval, const std::function<void()> &func);

            /*Clear all timer events*/
            void clear();

            void handleTimer(){
                uint64_t exp = 0;
                int n = ::read(fd_, &exp, sizeof(uint64_t));
                if(n == sizeof(uint64_t))
                    handleTimerFunc();
                else 
                    printf("Error: Timer events wrong...\n");
            }

        private: 
            EventLoop *loop_;
            int fd_;
            std::shared_ptr<Event> event_;
            std::function<void()> handleTimerFunc;
    };
}

#endif