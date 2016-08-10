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

}
