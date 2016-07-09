#ifndef UI_HPP
#define UI_HPP

#include <string>
#include <memory>
#include "../logger/logger.hpp"
#include "../configuration/configuration.hpp"
#include "../events/events.hpp"

namespace game_graphics
{

struct ui;

using game_ui_pointer = std::shared_ptr<ui>;

struct drawing_statistics
{
    uint32_t num_of_draw_call{0},
             num_of_reshape_call{0},
             num_of_idle_func_call{0};
};

enum class mouse_button
{
    left_button,
    middle_button,
    right_button
};

enum class mouse_button_state
{
    up,
    down
};

struct mouse_information
{
    mouse_button_state left_button_state,
                       right_button_state;

    void press_left(){
        left_button_state = mouse_button_state::down;
    }
    void release_left(){
        left_button_state = mouse_button_state::up;
    }
    void press_right(){
        right_button_state = mouse_button_state::down;
    }
    void release_right(){
        right_button_state = mouse_button_state::up;
    }

    mouse_information():
        left_button_state{mouse_button_state::up},
        right_button_state{mouse_button_state::up}
    {}
};

/*
 * Objec responsible for the creation and handling of
 * the game window
 */
class ui
{
    game_configuration::game_config_ptr game_conf;
    game_events::game_evt_pointer       game_events_queue;

    drawing_statistics draw_stats;
    mouse_information  mouse_state;

    uint32_t ui_window_height,
             ui_window_width;

    void init_ui_window();
    void display_drawing_stats();
    void draw_string(uint32_t x_pos,
                     uint32_t y_pos,
                     const std::string& text);
public:
    ui(game_configuration::game_config_ptr conf_info,
       game_events::game_evt_pointer event_queue);
    void draw();
    void window_reshape(uint32_t width,
                        uint32_t height);
    void mouse_click_down(mouse_button button,
                     uint32_t x, uint32_t y);
    void mouse_click_up(mouse_button button,
                     uint32_t x, uint32_t y);
    void mouse_move_with_trigger(uint32_t x,
                                 uint32_t y);
    void idle_function();
};

}

#endif
