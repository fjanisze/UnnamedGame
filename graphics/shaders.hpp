#ifndef SHADERS_HPP
#define SHADERS_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

namespace game_shaders
{

class shader
{
    GLuint shader_program;
    std::string vertex_code_path,
                fragment_code_path;
    std::string vertex_code,
                fragment_code;
    //Buffer used to extract information
    //from the shader compiler
    GLchar info_log_buffer[512];
    bool load_shader_code();
    bool compile_shader();
public:
    // Constructor generates the shader on the fly
    shader();
    bool load_from_path(const GLchar* vertex_path,
                        const GLchar* fragment_path);
    void load_from_string(const std::string& vertex_shader_code,
                          const std::string& frag_shader_code);
    // Uses the current shader
    void use_shader();
    GLuint get_shader_program();
};

}
#endif
