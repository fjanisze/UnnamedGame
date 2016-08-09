#include <GL/glew.h>
#include <GL/glut.h>
#include "ui.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"

namespace game_graphics
{

game_fonts::game_fonts() :
    VAO{0},
    VBO{0}
{
    LOG3("Init game_fonts! Setting up the text rendering environment.");


    //Init the FT library
    FT_Error ft_error = FT_Init_FreeType(&ft_library);
    if(ft_error){
        ERR("Unable to initialize the freetype library, error: ",
            ft_error);
        //TODO: This look weird!
        throw std::runtime_error("freetype init failed!");
    }
    //Load the font
    ft_error = FT_New_Face(ft_library,
                           "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
                           0,
                           &ft_face);
    if(ft_error){
        ERR("Unable to initialize the font face, error: ",
            ft_error);
        throw std::runtime_error("freetype init failed!");
    }
    //Set default font size
    FT_Set_Pixel_Sizes(ft_face,0,48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //Load the charset
    load_charset();

    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat) * 6 * 4,
                 NULL,
                 GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void game_fonts::load_charset()
{
    LOG1("Loading first 128 ASCII characters.");
    bool loading_error = false;
    for(GLchar charcode{0};charcode<127;++charcode)
    {
        FT_Error ft_error = FT_Load_Char(ft_face,
                                         charcode,
                                         FT_LOAD_RENDER);
        if(ft_error){
            loading_error = true;
            break;
        }

        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    ft_face->glyph->bitmap.width,
                    ft_face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    ft_face->glyph->bitmap.buffer
                    );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_MAG_FILTER,
                        GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(ft_face->glyph->bitmap.width,
                ft_face->glyph->bitmap.rows),
            glm::ivec2(ft_face->glyph->bitmap_left,
                ft_face->glyph->bitmap_top),
            ft_face->glyph->advance.x
        };
        charset[charcode]=character;
    }

    if(loading_error){
         ERR("Error when loading the charset textures!");
    }
    else
    {
        LOG3("Charset loaded correctly! Releasing ft_face and ft_library");
        glBindTexture(GL_TEXTURE_2D, 0);
        // Destroy FreeType once we're finished
        FT_Done_Face(ft_face);
        FT_Done_FreeType(ft_library);
    }
}

void game_fonts::render_text(const std::string &text,
                             uint32_t x,
                             uint32_t y,
                             float scale = 1)
{
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    for(auto current:text)
    {
        Character ch = charset[current];
        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER,
                     VBO);
        glBufferSubData(GL_ARRAY_BUFFER,
                        0,
                        sizeof(vertices),
                        vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

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
    if(ImGui::IsMouseHoveringAnyWindow() == false){
        //Handle the click only if the mouse is not
        //over a ImGui window
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
    else{
        //Call the ImGui Handler for further processing
        ImGui_ImplGlfw_MouseButtonCallback(window,button,
                                           action,mods);
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
    //Create the ui queue
    ui_events_queue = std::make_shared<game_events::events>("UI QUEUE");
    //Save the pointer to this object
    ui_instance_pointer = this;
    //Start the initialization
    init_ui_window();
    //Init GLEW
    GLenum gl_error = glewInit();
    if(gl_error != GLEW_OK)
    {
        ERR("Unable to load GLEW, error: ",
            gl_error);
        throw std::runtime_error("Unable to load GLEW!");
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,
                GL_ONE_MINUS_SRC_ALPHA);

    //Init the text rendered
    fonts = std::make_shared<game_fonts>();
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
                        false);

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
    style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PopupBg]               = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Header]                = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Column]                = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
    style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);

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
        ui_events_queue->push(mouse_click_evt);
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
        ui_events_queue->push(mouse_click_evt);
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

void ui::process_ui_events()
{
    if(ui_events_queue->empty()){
        return; //Nothing to do now
    }
    game_events::event_queue_container_t ui_events;
    ui_events_queue->move_events(ui_events);
    while(!ui_events.empty())
    {
        auto evt = ui_events.front();
        ui_events.pop();
        switch(evt->get_id())
        {
        case game_events::events_ids::ui_mouse_left_button_down:
            {
                auto mouse_evt = std::dynamic_pointer_cast<game_events::mouse_left_button_down_evt,
                    game_events::game_event_type>(evt);
                LOG1("Handling left mouse button down, coord: ",
                     mouse_evt->x_coord,"/",mouse_evt->y_coord);
            }
            break;
        case game_events::events_ids::ui_mouse_left_button_up:
            {
                auto mouse_evt = std::dynamic_pointer_cast<game_events::mouse_left_button_up_evt,
                        game_events::game_event_type>(evt);
                LOG1("Handling left mouse button up, coord: ",
                     mouse_evt->x_coord,"/",mouse_evt->y_coord);
            }
            break;
        default:
            WARN1("Unable to process the event with ID ",
                  evt->get_id());
        }
    }
}

void ui::loop()
{

    bool load_texture{ false };
    bool close_window{ false };
    bool p_open;

    glViewport(0, 0, ui_window_width,
                     ui_window_height);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);



   /*     ImGui_ImplGlfw_NewFrame();

        //Let the UI process the UI events
        process_ui_events();

        if(close_window == false){
            ImGui::SetNextWindowSize(ImVec2(450,100),ImGuiSetCond_FirstUseEver);
            ImGui::Begin("What do you want to do?",
                         &p_open, ImGuiWindowFlags_NoResize);
            if(ImGui::Button("Load the texture"))
                load_texture = true;
            if(ImGui::Button("Close Window"))
                close_window = true;
            ImGui::End();
        }*/

    //    ImGui::ShowTestWindow();

        fonts->render_text("Text",10,10);


        //ImGui::Render();
        glfwSwapBuffers(window);
    }
}

}
