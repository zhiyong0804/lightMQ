#ifndef EVENTLOOP_HPP
#define EVENTLOOP_HPP

#include "utilx.hpp"

#include "Event.hpp"
#include "Poller.hpp"


namespace socketx
{
    class EventLoop
    {
        public:
            EventLoop();
            ~EventLoop();

            /* Main Loop
            *  The Loop will stop when stop falg is set to false
            */
            void loop();
            void quit() { stop = true; }

            /*Update events by invoke poller's function*/
            void updateEvent(std::shared_ptr<Event> event);
            void deleteEvent(std::shared_ptr<Event> event);

        private:
            std::vector<std::shared_ptr<Event>> activeEvents;
            std::shared_ptr<socketx::Poller> poller;
            std::atomic<bool> stop;
    };
}
#endif