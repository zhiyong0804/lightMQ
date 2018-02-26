#include "EventLoop.hpp"
#include "Timer.hpp"


void handleReadEvents(){
    printf("Read events...\n");
}

int main(int argc, char **argv){
    if(argc!=3){
        fprintf(stderr,"usage: %s <start> <interval>\n", argv[0]);
        exit(0);
    }

    size_t start = atoi(argv[1]);
    size_t interval = atoi(argv[2]);
    socketx::EventLoop loop;
    socketx::Timer timer(&loop);
    timer.addTimer(start,interval,handleReadEvents);
    loop.loop(); 

    return 0;
}