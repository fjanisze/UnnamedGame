#include "text_rendering.hpp"

namespace text_renderer
{

font_texture_loader::font_texture_loader() :
    next_id{ 1 }
{
    LOG3("Creating the font loader object.");
    FT_Error error = FT_Init_FreeType(&freetype_lib);
    if(error){
        ERR("Unable to load the freetype library, error code: ",
            error);
        throw std::runtime_error("Unable to load the freetype library!");
    }
    else
    {
        LOG3("Freetype lib loaded correctly.");
        //Load the default font which shall have the ID 1
        auto default_font = load_new_textureset("/usr/share/fonts/truetype/freefont/FreeSerif.ttf");
        if(default_font.second == nullptr){
            ERR("Failed to load the default font.");
            throw std::runtime_error("Unable to load the default font!.");
        }
        default_font_id = default_font.first;
    }
}

/*
 * Attemp to load a new texture set from the path provided in
 * font_name. Return nullptr if the operations fails
 */

std::pair<font_type_id,font_texture_ptr>
font_texture_loader::load_new_textureset(const std::string &font_name)
{
    LOG3("Attempt to load the font: ",
         font_name);
    font_texture_ptr new_font = nullptr;
    font_type_id     new_font_id = 0;
    FT_Face          font_face;
    FT_Error error = FT_New_Face(freetype_lib,
                                 font_name.c_str(),
                                 0,
                                 &font_face);
    if(error)
    {
        ERR("Failed to load the font: ",
            font_name);
    }
    else
    {
        new_font = std::make_shared<font_texture>();
        new_font->font_name = font_name;
        // Set size to load glyphs as
        FT_Set_Pixel_Sizes(font_face, 0, 48);

        //Make sure that the proper alignment is set
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Load first 128 characters of ASCII set
        for (GLubyte current_char = 0; current_char < 128; current_char++)
        {
            // Load character glyph
            if (FT_Load_Char(font_face, current_char, FT_LOAD_RENDER))
            {
                ERR("Failed to load the Glyph for the requested font!");
                return {0,nullptr};
            }
            // Generate texture
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                font_face->glyph->bitmap.width,
                font_face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                font_face->glyph->bitmap.buffer
            );
            // Set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // Now store character for later use
            character_data character = {
                texture,
                glm::ivec2(font_face->glyph->bitmap.width,
                        font_face->glyph->bitmap.rows),
                glm::ivec2(font_face->glyph->bitmap_left,
                        font_face->glyph->bitmap_top),
                font_face->glyph->advance.x
            };
            new_font->charset.insert({current_char,character});
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        FT_Done_Face(font_face);

        new_font_id = next_id++;
        LOG1("Font ",font_name," loaded properly!");
        fonts.insert({new_font_id,
                      new_font});
    }
    return {new_font_id,
                new_font};
}


/*
 * Return the pointe to the texture information for the given font,
 * if those information are not available then try to load the font
 * from its location.
 *
 * font_name is actually a path.
 */
font_texture_ptr font_texture_loader::get_texture(font_type_id id)
{
    auto it = fonts.find(id);
    if(it == fonts.end()){
        ERR("Unable to find the font with id ",id);
        return nullptr;
    }
    return it->second;
}

font_type_id font_texture_loader::get_default_font_id()
{
    return default_font_id;
}


font_texture_loader renderable_text::font_loader;
game_shaders::shader  renderable_text::text_render_shader;

/*
 * Create a renderable test
 */
renderable_text::renderable_text(const std::string& text,
                                 glm::fvec2 position,
                                 GLfloat scale,
                                 glm::vec3 color) :
    text_position( position ),
    text_scale( scale ),
    text_color( color ),
    text_string( text )
{
    //Load the font texture
    font_texture = font_loader.get_texture(font_loader.get_default_font_id());
    if(font_texture == nullptr){
        ERR("Unable to create the renderable_text with the default font!");
        throw std::runtime_error("renderable_text::renderable_text");
    }
    //Load the shader if not loaded
    if(text_render_shader.get_shader_program() == 0){
        bool result = text_render_shader.load_from_path("shaders/text.vs",
                                                        "shaders/text.frag");
        if(false == result){
            ERR("Text creation failed!");
            throw std::runtime_error("renderable_text::renderable_text");
        }
    }
 }

void renderable_text::render_text()
{
    glm::mat4 projection = glm::ortho(0.0f,
                                      static_cast<GLfloat>(800),
                                      0.0f,
                                      static_cast<GLfloat>(600));
    text_render_shader.use_shader();
    glUniformMatrix4fv(glGetUniformLocation(text_render_shader.get_shader_program(),
                                            "projection"),
                                            1,
                                            GL_FALSE,
                                            glm::value_ptr(projection));

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

    text_render_shader.use_shader();
    glUniform3f(glGetUniformLocation(text_render_shader.get_shader_program(),
                                     "textColor"),
                                    text_color.x,
                                    text_color.y,
                                    text_color.z);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // Iterate through all characters
    std::string::const_iterator c;
    GLfloat x = text_position.x;
    for (std::size_t i{0 };i<text_string.size();++i)
    {
        char c = text_string[i];
        if(font_texture->charset.find(c)==font_texture->charset.end()){
            ERR("MERDA");
        }
        text_renderer::character_data ch = font_texture->charset[c];

        GLfloat xpos = text_position.x + ch.Bearing.x * text_scale;
        GLfloat ypos = text_position.y - (ch.Size.y - ch.Bearing.y) * text_scale;

        GLfloat w = ch.Size.x * text_scale;
        GLfloat h = ch.Size.y * text_scale;
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
        glBufferSubData(GL_ARRAY_BUFFER,
                        0,
                        sizeof(vertices),
                        vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * text_scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

}
