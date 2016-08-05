#include "events.hpp"

namespace game_events
{

events::events(const std::string& name) :
    queue_name{ name }
{
    LOG3("Running the game queue: ",
         queue_name);
}

void events::push(event_type_ptr new_event)
{
    std::lock_guard<std::mutex> lock(change_mtx);
    LOG1("Pushing new event id (",queue_name,"): ",
         new_event->get_id());
    event_queue.push(new_event);
}

event_type_ptr events::front()
{
    return event_queue.front();
}

void events::pop()
{
    std::lock_guard<std::mutex> lock(change_mtx);
    LOG1("Popping the next event (",queue_name,"): ",
         event_queue.front()->get_id());
    event_queue.pop();
}

std::size_t events::size()
{
    return event_queue.size();
}

void events::clear()
{
    std::lock_guard<std::mutex> lock(change_mtx);
    while(!event_queue.empty())
    {
        LOG3("Queue clear, removing event (",queue_name,"): ",
             event_queue.front()->get_id());
        event_queue.pop();
    }
}

bool events::empty()
{
    return event_queue.empty();
}

/*
 * The object which is willing to process the queue
 * shall take all the actual events in a separate queue
 * and leave the event_queue ready to new push as fast
 * as possible
 */
uint32_t events::move_events(event_queue_container_t& destination_queue)
{
    std::lock_guard<std::mutex> lock(change_mtx);
    uint32_t queue_size = event_queue.size();
    LOG1("Copying the content of the queue (",queue_name,"), size:",
         queue_size);
    while(!event_queue.empty())
    {
        destination_queue.push(event_queue.front());
        event_queue.pop();
    }
    return queue_size;
}


}
