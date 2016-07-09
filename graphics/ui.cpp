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

void glut_keyboard_func_callback(unsigned char ascii,
                                 int x,int y){
    ui_instance_pointer->keyboard_press(ascii,x,y);
}

void glut_keyboard_specialfunc_callback(int key,
                                        int x,int y){
    ui_instance_pointer->keyboard_special_press(key,x,y);
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
    glutKeyboardFunc(glut_keyboard_func_callback);
    glutSpecialFunc(glut_keyboard_specialfunc_callback);

    init_viewport();
}

void ui::display_ui_info()
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
    stat_string.str("");
    stat_string<<"W. Width:"<<ui_window_width<<",Height:"<<ui_window_height;
    draw_string(5,40,stat_string.str());
    stat_string.str("");
    stat_string<<"Viewport, X:"<<viewport.x_from<<"/"<<viewport.x_to;
    stat_string<<" Y:"<<viewport.y_from<<"/"<<viewport.y_to;
    draw_string(5,50,stat_string.str());
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

void ui::init_viewport()
{
    viewport.x_to = ui_window_width;
    viewport.y_to = ui_window_height;
    LOG3("Setting initial viewport to x:",
         viewport.x_from,"/",viewport.x_to,", y:",
         viewport.y_from,"/",viewport.y_to);
}

void ui::move_viewport(uint32_t new_x_from,
                       uint32_t new_y_from)
{
    viewport.x_from = new_x_from;
    viewport.y_from = new_y_from;
    viewport.x_to = new_x_from + ui_window_width;
    viewport.y_to = new_y_from + ui_window_height;
    LOG3("Setting the viewport to x:",
         viewport.x_from,"/",viewport.x_to,", y:",
         viewport.y_from,"/",viewport.y_to);

    draw();
}

void ui::update_viewport_size(uint32_t new_width,
                              uint32_t new_height)
{
    viewport.x_to = viewport.x_from + new_width;
    viewport.y_to = viewport.y_from + new_height;

    draw();
}

void ui::handle_arrow_key_press(arrow_key key)
{
    static int32_t default_vieport_shift{ 50 };
    int8_t dir{ 1 };
    if(key == arrow_key::left ||
       key == arrow_key::up)
        dir = -1;
    switch (key) {
    case arrow_key::down:
    case arrow_key::up:
        move_viewport(viewport.x_from,
                      std::max<int32_t>(0,viewport.y_from +
                                         dir*default_vieport_shift));
        break;
    case arrow_key::left:
    case arrow_key::right:
        move_viewport(std::max<int32_t>(0,viewport.x_from +
                                         dir*default_vieport_shift),
                      viewport.y_from);
    default:
        break;
    }
}

arrow_key ui::is_arrow_key(uint32_t key_code)
{
    arrow_key key = arrow_key::none;
    switch (key_code) {
    case GLUT_KEY_DOWN: key = arrow_key::down; break;
    case GLUT_KEY_UP:   key = arrow_key::up;   break;
    case GLUT_KEY_RIGHT:key = arrow_key::right;break;
    case GLUT_KEY_LEFT: key = arrow_key::left; break;
    default:
        break;
    }
    return key;
}

void ui::draw()
{
    ++draw_stats.num_of_draw_call;
    //Background color
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);



    display_ui_info();

    glutSwapBuffers();
}

void ui::window_reshape(uint32_t width, uint32_t height)
{
    ++draw_stats.num_of_reshape_call;

    ui_window_height = height;
    ui_window_width = width;

    update_viewport_size(width,height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0,0,ui_window_width,ui_window_height);
    gluOrtho2D(0.0,ui_window_width,ui_window_height,0);

    draw();
}

void ui::mouse_click_down(mouse_button button,
                     uint32_t x, uint32_t y)
{
    if(button == mouse_button::left_button)
    {
        auto mouse_click_evt = game_events::event_factory<
                game_events::mouse_left_button_down_evt>::create(
                                                viewport.x_from + x,
                                                viewport.y_from + y
                                                                );

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
                game_events::mouse_left_button_up_evt>::create(
                                                viewport.x_from + x,
                                                viewport.y_from + y
                                                               );

        game_events_queue->push(mouse_click_evt);

        mouse_state.release_left();
    }else{
        LOG1("Unsupported mouse click button!");
    }
}

void ui::mouse_move_with_trigger(uint32_t x, uint32_t y)
{

}

void ui::keyboard_press(unsigned char ascii,
                        uint32_t x, uint32_t y)
{
    LOG1("Keyboard input, ascii:",ascii,", x:",x,
         ", y:",y);
}

void ui::keyboard_special_press(uint32_t key,
                                uint32_t x, uint32_t y)
{
    LOG1("Keyboard special input, key:",key,", x:",x,
         ", y:",y);
    arrow_key arrow = is_arrow_key(key);
    if(arrow != arrow_key::none){
        handle_arrow_key_press(arrow);
    }
}

void ui::idle_function()
{
    ++draw_stats.num_of_idle_func_call;
}

}
