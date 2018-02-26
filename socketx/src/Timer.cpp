#include "Timer.hpp"

namespace socketx{

    Timer::Timer(EventLoop *loop):
        loop_(loop), fd_(-1),
        event_(std::make_shared<Event>(loop,fd_)){
    }

    Timer::~Timer(){
        clear();
    }

    void Timer::addTimer(size_t start, size_t interval, const std::function<void()> &func){
        /*Construct a timerfd*/
        struct itimerspec new_value;
        struct timespec curr;
        if(clock_gettime(CLOCK_REALTIME,&curr)==-1)
            printf("Error: clock_gettime\n");
        new_value.it_value.tv_sec = curr.tv_sec + start;
        new_value.it_value.tv_nsec = curr.tv_nsec;
        new_value.it_interval.tv_sec = interval;
        new_value.it_interval.tv_nsec = 0;
        int fd = timerfd_create(CLOCK_REALTIME,0);
        if(fd==-1) 
            printf("Error: timerfd_create\n");
        if(timerfd_settime(fd,TFD_TIMER_ABSTIME,&new_value,NULL)==-1)
            printf("Error: timerfd_settime\n");
        
        /*Regist timer events*/
        fd_ = fd;
        event_->setFD(fd_);
        event_->enableReading();
        event_->setReadFunc(std::bind(&Timer::handleTimer,this));
        handleTimerFunc = func;
    }

    void Timer::clear(){
        ::close(fd_);
        event_->deleteEvent();
    }

}