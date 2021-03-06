#include <GL/glut.h>
#include "ui.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"

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

void mousebutton_callback(GLFWwindow* window,
                               int button,
                               int action,
                               int mods)
{
    double x,y; //Mouse position
    glfwGetCursorPos(window,
                     &x,
                     &y);
    \
    mouse_button pressed_button;
    switch(button){
    case GLFW_MOUSE_BUTTON_LEFT:
        pressed_button = mouse_button::left_button;
        break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
        pressed_button = mouse_button::middle_button;
        break;
    case GLFW_MOUSE_BUTTON_RIGHT:
        pressed_button = mouse_button::right_button;
        break;
    default:
        ERR("Unable to understand what button is pressed! ",
            button,", ",action,", ",mods);
        return;
    }

    if(action == GLFW_PRESS){
        ui_instance_pointer->mouse_click_down(pressed_button,
                                              x,
                                              y);
    }else{
        ui_instance_pointer->mouse_click_up(pressed_button,
                                            x,
                                            y);
    }
}

/*
 * This is the callback used to report failures on
 * GLFW side
 */
void glfw_error_callback(int error, const char* description)
{
    ERR("Error reported by GLFW, code:",error,", description:",
        description);
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

    init_viewport();
    init_glfw_window();

    setup_ui_styles(false,1);
}

void ui::init_glfw_window()
{
    glfwSetErrorCallback(glfw_error_callback);

    if(!glfwInit()){
        ERR("Unable to init GLFW!");
        throw std::runtime_error("GLFW initialization failed");
    }

    window = glfwCreateWindow(ui_window_width,
                               ui_window_height,
                               "Unnamed Game",
                               NULL,
                               NULL);
    if(nullptr == window){
        ERR("Unable to create the window!");
        throw std::runtime_error("Window creation failed!");
    }

    // Setup ImGui binding
    ImGui_ImplGlfw_Init(window,
                        true);

    glfwMakeContextCurrent(window);

    //Setup all the callbacks
    glfwSetMouseButtonCallback(window,
                               mousebutton_callback);
}

void ui::setup_ui_styles(bool dark_style,
                         float alpha)
{
    ImGuiStyle& style = ImGui::GetStyle();

    // light style from Pacôme Danhiez (user itamago) https://github.com/ocornut/imgui/pull/511#issuecomment-175719267
    style.Alpha = 1.0f;
    style.FrameRounding = 3.0f;
    style.Colors[ImGuiCol_Text]                  = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Header]                = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Column]                = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
    style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

    if( dark_style )
    {
        for (int i = 0; i <= ImGuiCol_COUNT; i++)
        {
            ImVec4& col = style.Colors[i];
            float H, S, V;
            ImGui::ColorConvertRGBtoHSV( col.x, col.y, col.z, H, S, V );

            if( S < 0.1f )
            {
                V = 1.0f - V;
            }
            ImGui::ColorConvertHSVtoRGB( H, S, V, col.x, col.y, col.z );
            if( col.w < 1.00f )
            {
                col.w *= alpha;
            }
        }
    }
    else
    {
        for (int i = 0; i <= ImGuiCol_COUNT; i++)
        {
            ImVec4& col = style.Colors[i];
            if( col.w < 1.00f )
            {
                col.x *= alpha;
                col.y *= alpha;
                col.z *= alpha;
                col.w *= alpha;
            }
        }
    }
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

void ui::loop()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplGlfw_NewFrame();

        ImGui::SetNextWindowSize(ImVec2(150,100),ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Test Window");
        ImGui::Text("Hello World");
        ImGui::End();

        display_ui_info();

        glViewport(0, 0, ui_window_width,
                   ui_window_height);

        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Render();
        glfwSwapBuffers(window);
    }
}

}
