#ifndef EVENTS_HPP
#define EVENTS_HPP

#include <logger/logger.hpp>
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
 * Each event shall have its own unique identificator
 */
enum events_ids {
    ui_mouse_left_button_down,
    ui_mouse_left_button_up
};

/*
 * The generic base class for all the possible
 * events.
 */
class game_event_type
{
    const events_ids id;
public:
    game_event_type(events_ids event_id):
        id{ event_id }
    {}
    virtual void process() {}
    events_ids get_id(){
        return id;
    }

    virtual ~game_event_type() {}
};

class mouse_left_button_down_evt : public game_event_type
{
public:
    uint32_t x_coord{ 0 },
             y_coord{ 0 };
    mouse_left_button_down_evt() :
        game_event_type( events_ids::ui_mouse_left_button_down )
    {}
    explicit mouse_left_button_down_evt(uint32_t x,uint32_t y):
        game_event_type( events_ids::ui_mouse_left_button_down ),
        x_coord{x},
        y_coord{y}
    {}

    ~mouse_left_button_down_evt(){}
};


class mouse_left_button_up_evt : public game_event_type
{
public:
    uint32_t x_coord{ 0 },
             y_coord{ 0 };
    mouse_left_button_up_evt() :
        game_event_type( events_ids::ui_mouse_left_button_up )
    {}
    explicit mouse_left_button_up_evt(uint32_t x,uint32_t y):
        game_event_type( events_ids::ui_mouse_left_button_up ),
        x_coord{x},
        y_coord{y}
    {}

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
        return std::dynamic_pointer_cast<game_event_type>(
                    std::make_shared<EVENT_TYPE>(std::forward<EVENT_ARGS>(event_args)...));
    }
};

using event_queue_container_t = std::queue<event_type_ptr>;

class events
{
    event_queue_container_t event_queue;
    std::mutex change_mtx;
    std::string queue_name;
public:
    events(const std::string &queue_name);
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
