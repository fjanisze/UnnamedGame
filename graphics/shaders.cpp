#include "../logger/logger.hpp"
#include "shaders.hpp"

namespace game_shaders
{

shader::shader()
{
    LOG3("New shader created!");
}

void shader::load_from_path(const GLchar *vertex_path,
                            const GLchar *fragment_path)
{
    vertex_code_path = vertex_path;
    fragment_code_path = fragment_path;
    if(load_shader_code())
    {
        compile_shader();
    }
    else{
        ERR("Shader creation failed!");
    }
}

void shader::load_from_string(const std::string& vertex_shader_code,
                              const std::string& frag_shader_code)
{
    vertex_code = vertex_shader_code;
    fragment_code = frag_shader_code;
    compile_shader();
}

bool shader::compile_shader()
{
    LOG3("Compiling shader code.");
    bool operation_success = true;
    const GLchar* vertex_shader_code = vertex_code.c_str();
    const GLchar * frag_shader_code = fragment_code.c_str();
    // 2. Compile shaders
    GLuint vertex, fragment;
    GLint success;
    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_shader_code, NULL);
    glCompileShader(vertex);
    // Print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, info_log_buffer);
        ERR("Shader compilation failed! ",
            info_log_buffer);
        operation_success = false;
    }
    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &frag_shader_code, NULL);
    glCompileShader(fragment);
    // Print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, info_log_buffer);
        ERR("Shader compilation failed! ",
            info_log_buffer);
        operation_success = false;
    }
    // Shader Program
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex);
    glAttachShader(shader_program, fragment);
    glLinkProgram(shader_program);
    // Print linking errors if any
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log_buffer);
        ERR("Shader linking failed! ",
            info_log_buffer);
        operation_success = false;
    }
    // Delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return operation_success;
}

bool shader::load_shader_code()
{
    LOG3("Loading shader code: ",
         vertex_code_path, " , ",
         fragment_code_path);
    bool operation_success = true;
    // 1. Retrieve the vertex/fragment source code from filePath
    std::ifstream vertex_shader_file;
    std::ifstream frag_shader_file;
    // ensures ifstream objects can throw exceptions:
    vertex_shader_file.exceptions (std::ifstream::badbit);
    frag_shader_file.exceptions (std::ifstream::badbit);
    try{
        // Open files
        vertex_shader_file.open(vertex_code_path);
        frag_shader_file.open(fragment_code_path);
        std::stringstream vShaderStream, fShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vertex_shader_file.rdbuf();
        fShaderStream << frag_shader_file.rdbuf();
        // close file handlers
        vertex_shader_file.close();
        frag_shader_file.close();
        // Convert stream into string
        vertex_code = vShaderStream.str();
        fragment_code = fShaderStream.str();
    }
    catch (std::ifstream::failure)
    {
        ERR("Unable to load the shader code! Operation failed!");
        operation_success = false;
    }
    return operation_success;
}

void shader::use_shader()
{
    glUseProgram(shader_program);
}

GLuint shader::get_shader_program()
{
    return shader_program;
}

}
