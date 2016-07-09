#include "events.hpp"

namespace game_events
{

events::events(){
    LOG3("Running the game main queue!");
}

void events::push(event_type_ptr new_event)
{
    LOG1("Pushing new event id: ",new_event->get_id());
    event_queue.push(new_event);
}

event_type_ptr events::front()
{
    return event_queue.front();
}

void events::pop()
{
    LOG1("Popping the next event: ",event_queue.front()->get_id());
    event_queue.pop();
}

std::size_t events::size()
{
    return event_queue.size();
}

void events::clear()
{
    while(!event_queue.empty())
    {
        LOG3("Queue clear, removing event: ",event_queue.front()->get_id());
        event_queue.pop();
    }
}

bool events::empty()
{
    return event_queue.empty();
}


}
