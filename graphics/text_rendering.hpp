#ifndef TEXT_RENDERING_HPP
#define TEXT_RENDERING_HPP

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H
//Generic
#include <string>
#include <unordered_map>
#include <memory>
#include <utility>
//Game
#include "shaders.hpp"
#include "../logger/logger.hpp"

namespace text_renderer
{

/// Holds all state information relevant to a character as loaded using FreeType
struct character_data {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    FT_Pos Advance;    // Horizontal offset to advance to next glyph
};

/*
 * Contains the information required to render a font set
 */
struct font_texture
{
    std::string font_name;
    std::unordered_map<GLchar,character_data> charset;
};

using font_texture_ptr = std::shared_ptr<font_texture>;

/*
 * Instead of passing everywhere the font name
 * the code will identify the various font by
 * a unique ID
 */
using font_type_id = int;

class font_texture_loader
{
    FT_Library freetype_lib;
    font_type_id next_id;
    std::unordered_map<font_type_id,font_texture_ptr> fonts;
public:
    font_texture_loader();
    std::pair<font_type_id,font_texture_ptr> load_new_textureset(const std::string& font_name);
    font_texture_ptr get_texture(font_type_id id);
};



}

#endif //TEXT_RENDERING_HPP
