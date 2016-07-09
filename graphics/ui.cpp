#include <GL/glut.h>
#include "ui.hpp"

namespace game_graphics
{

namespace{
    ui* ui_instance_pointer = nullptr;
}

namespace{

/*
 * Those are the callbacks for GLUT, they eventually
 * perform some computation on the data from GLUT and trigger
 * the proper action handler in the ui object
 */

void glut_draw_callback(){
    ui_instance_pointer->draw();
}

void glut_idle_func_callback(){
    ui_instance_pointer->idle_function();
}

void glut_window_reshape_callback(int width,int height){
    ui_instance_pointer->window_reshape(width,height);
}

void glut_motion_func_callback(int x,int y){
    ui_instance_pointer->mouse_move_with_trigger(x,y);
}

}

void glut_mouse_click_callback(int button, int state,
                               int x,int y){
    mouse_button_state sel_state;
    mouse_button sel_button;

    switch(button)
    {
    case GLUT_LEFT_BUTTON:
        sel_button = mouse_button::left_button;
        break;
    case GLUT_MIDDLE_BUTTON:
        sel_button = mouse_button::middle_button;
    case GLUT_RIGHT_BUTTON:
        sel_button = mouse_button::right_button;
    default:
        break;
    }

    switch(state)
    {
    case GLUT_UP:
        ui_instance_pointer->mouse_click_up(sel_button,
                                         x,y);
        break;
    case GLUT_DOWN:
        ui_instance_pointer->mouse_click_down(sel_button,
                                         x,y);
        break;
    default:
        break;
    }

}

ui::ui(game_configuration::game_config_ptr conf_info,
       game_events::game_evt_pointer event_queue) :
    game_conf{conf_info},
    game_events_queue{event_queue}
{
    LOG3("Running the UI object.");
    //Save the pointer to this object
    ui_instance_pointer = this;
    //Start the initialization
    init_ui_window();
}

void ui::init_ui_window()
{
    LOG3("Creating the main game window");
    std::string window_height{0},
             window_width{0};
    window_width = game_conf->get_option("window_width");
    window_height = game_conf->get_option("window_height");
    if(window_height.empty() || window_width.empty()){
        WARN1("Window size configuration not available, using default 800x600");
        window_height = "800";
        window_width = "600";
    }
    ui_window_height = std::stoi(window_height),
    ui_window_width = std::stoi(window_width);

    LOG1("Configuring window size to ", ui_window_height,"x",ui_window_width);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(0,0);
    glutInitWindowSize(ui_window_width,
                       ui_window_height);
    glutCreateWindow("Filip");
    glutReshapeFunc(glut_window_reshape_callback);
    glutDisplayFunc(glut_draw_callback);
    glutIdleFunc(glut_idle_func_callback);
    glutMouseFunc(glut_mouse_click_callback);
}

void ui::display_drawing_stats()
{
    glColor3f(1,1,1);
    std::stringstream stat_string;
    stat_string << "draw_call: "<<draw_stats.num_of_draw_call;
    draw_string(5,10,stat_string.str());
    stat_string.str("");
    stat_string << "reshape_call: "<<draw_stats.num_of_reshape_call;
    draw_string(5,20,stat_string.str());
    stat_string.str("");
    stat_string << "idle_func: "<<draw_stats.num_of_idle_func_call;
    draw_string(5,30,stat_string.str());
}

void ui::draw_string(uint32_t x_pos,
                     uint32_t y_pos,
                     const std::string &text)
{
    glRasterPos2i(x_pos,
                  y_pos);
    for( std::size_t i=0; i < text.size(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10,
                            text[i]); // Print a character on the screen
    }
}

void ui::draw()
{
    ++draw_stats.num_of_draw_call;
    //Background color
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);


    display_drawing_stats();

    glutSwapBuffers();
}

void ui::window_reshape(uint32_t width, uint32_t height)
{
    ++draw_stats.num_of_reshape_call;

    ui_window_height = height;
    ui_window_width = width;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0,0,ui_window_width,ui_window_height);
    gluOrtho2D(0.0,ui_window_width,ui_window_height,0);
}

void ui::mouse_click_down(mouse_button button,
                     uint32_t x, uint32_t y)
{
    if(button == mouse_button::left_button)
    {
        auto mouse_click_evt = game_events::event_factory<
                game_events::mouse_left_button_down_evt>::create(x,y);

        game_events_queue->push(mouse_click_evt);

        mouse_state.press_left();
    }else{
        LOG1("Unsupported mouse click button!");
    }
}

void ui::mouse_click_up(mouse_button button,
                     uint32_t x, uint32_t y)
{
    if(button == mouse_button::left_button)
    {
        auto mouse_click_evt = game_events::event_factory<
                game_events::mouse_left_button_up_evt>::create(x,y);

        game_events_queue->push(mouse_click_evt);

        mouse_state.release_left();
    }else{
        LOG1("Unsupported mouse click button!");
    }
}

void ui::mouse_move_with_trigger(uint32_t x, uint32_t y)
{

}

void ui::idle_function()
{
    ++draw_stats.num_of_idle_func_call;
}

}
