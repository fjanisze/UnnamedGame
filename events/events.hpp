#ifndef EVENTS_HPP
#define EVENTS_HPP

#include "../logger/logger.hpp"
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <iostream>
#include <typeinfo>
#include <mutex>

namespace game_events
{

using event_id_t = long;

class game_event_type;
class events;

using event_type_ptr = std::shared_ptr<game_event_type>;
using game_evt_pointer = std::shared_ptr<events>;

/*
 * The generic base class for all the possible
 * events.
 */
class game_event_type
{
    const event_id_t id;
public:
    game_event_type(event_id_t event_id):
        id{ event_id }
    {}
    virtual void process() {}
    virtual event_id_t get_id() = 0;

    template<typename T>
    std::shared_ptr<T> cast_pointer(event_type_ptr pointer){
        return std::dynamic_pointer_cast<T,
                game_events::game_event_type>(pointer);
    }

    virtual ~game_event_type() {}
};

class mouse_left_button_down_evt : public game_event_type
{
    uint32_t x_coord,
             y_coord;
public:
    explicit mouse_left_button_down_evt(uint32_t x,uint32_t y):
        game_event_type(1),
        x_coord{x},
        y_coord{y}
    {}

    event_id_t get_id(){
        return 1;
    }

    ~mouse_left_button_down_evt(){}
};


class mouse_left_button_up_evt : public game_event_type
{
    uint32_t x_coord,
             y_coord;
public:
    explicit mouse_left_button_up_evt(uint32_t x,uint32_t y):
        game_event_type(2),
        x_coord{x},
        y_coord{y}
    {}

    event_id_t get_id(){
        return 2;
    }

    ~mouse_left_button_up_evt(){}
};

template<typename EVENT_TYPE>
class event_factory
{
public:
    template<typename...EVENT_ARGS>
    static event_type_ptr create(EVENT_ARGS...event_args)
    {
        LOG1("Creating new event, name: ",typeid(EVENT_TYPE).name());
        return std::make_shared<EVENT_TYPE>(std::forward<EVENT_ARGS>(event_args)...);
    }
};

using event_queue_container_t = std::queue<event_type_ptr>;

class events
{
    event_queue_container_t event_queue;
    std::mutex change_mtx;
public:
    events();
    void push(event_type_ptr new_event);
    event_type_ptr front();
    void pop();
    std::size_t size();
    void clear();
    bool empty();
    uint32_t move_events(event_queue_container_t& destination_queue);
};

}

#endif
