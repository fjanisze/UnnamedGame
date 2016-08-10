/*#include <GL/glut.h>
#include "logger/logger.hpp"
#include "runner/runner.hpp"
#include <iostream>
#include <thread>
#include <cstdio>


int main(int argc,char** argv)
{
    std::ios_base::sync_with_stdio(false);
    SET_LOG_THREAD_NAME("MAIN");

    glutInit(&argc,argv);

    game_runner::runner runner;
    runner.start();
}
*/
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
// Std. Includes
#include <iostream>
#include <map>
#include <string>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H
// GL includes
// Properties

#include "graphics/shaders.hpp"
#include "graphics/text_rendering.hpp"

// Properties
const GLuint WIDTH = 800, HEIGHT = 600;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    FT_Pos Advance;    // Horizontal offset to advance to next glyph
};

GLuint VAO, VBO;


void RenderText(game_shaders::shader &shader,
                text_renderer::font_texture_ptr font,
                std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

static void GLFW_error(int error, const char* description)
{
    std::cout<<"ERROR: "<<error<<", "<<description<<std::endl;
}

// The MAIN function, from here we start our application and run the Game loop
int main()
{
    glewExperimental = GL_TRUE;
    // Init GLFW
    glfwInit();
    glfwSetErrorCallback(GLFW_error);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr); // Windowed
    if(window==NULL){
        std::cout<<"Init window failed\n";
        return 0;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW to setup the OpenGL Function pointers

    GLenum er = glewInit();
    if(er){
        std::cout<<"GLEW Init failed: "<<er<<"\n";
        return 0;
    }


    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);

    // Set OpenGL options
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Compile and setup the shader
    game_shaders::shader shader;
    shader.load_from_path("shaders/text.vs", "shaders/text.frag");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(WIDTH), 0.0f, static_cast<GLfloat>(HEIGHT));
    shader.use_shader();
    glUniformMatrix4fv(glGetUniformLocation(shader.get_shader_program(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    text_renderer::font_texture_loader fonts;
    text_renderer::font_texture_ptr font_texture = fonts.get_texture(
                "/usr/share/fonts/truetype/freefont/FreeMono.ttf");
    text_renderer::font_texture_ptr font_texture2 = fonts.get_texture(
                "/usr/share/fonts/truetype/freefont/FreeSerifBold.ttf");
    if(font_texture == nullptr){
        ERR("Loading failed!");
        return 1;
    }

    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Check and call events
        glfwPollEvents();

        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        RenderText(shader,font_texture2, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
        RenderText(shader,font_texture2, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

        // Swap the buffers
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void RenderText(game_shaders::shader &shader,
                text_renderer::font_texture_ptr font,
                std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // Activate corresponding render state
    shader.use_shader();
    glUniform3f(glGetUniformLocation(shader.get_shader_program(), "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        text_renderer::character_data ch = font->charset[*c];

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
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
