#include "utilx.hpp"
#include "EventLoop.hpp"

int main(int argc, char ** argv){
    socketx::EventLoop loop;
    loop.loop();
    return 0;
}