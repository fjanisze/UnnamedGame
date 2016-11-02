#include <logger/logger.hpp>
#include "maps.hpp"

namespace game_maps
{

universe_map::universe_map()
{
    LOG3("Creating the universe map");
}

void universe_map::set_universe_size(uint32_t width,
                                     uint32_t height)
{
    LOG3("Setting the universe size to ",
         width,"x",height);

    universe_specification.universe_height = height;
    universe_specification.universe_width = width;
}

uint32_t universe_map::add_celestial_body(celestial_body_ptr new_body)
{
    auto body_position = new_body->get_body_coordinates();
    if(body_position.x < universe_specification.universe_width &&
       body_position.y < universe_specification.universe_height)
    {
        celestial_bodies.push_back(new_body);
    }
    return celestial_bodies.size();
}

std::vector<celestial_body_cptr> universe_map::find_bodies_within(uint32_t top_left_y,
                                                                  uint32_t top_left_x,
                                                                  uint32_t bottom_right_x,
                                                                  uint32_t bottom_right_y)
{
    std::vector<celestial_body_cptr> bodies;
    for(auto element:celestial_bodies)
    {
        auto body_coord = element->get_body_coordinates();
        if(body_coord.x>=top_left_x && body_coord.x<=bottom_right_x &&
           body_coord.y>=top_left_y && body_coord.y<=bottom_right_y)
        {
            bodies.push_back(element);
        }
    }
    return bodies;
}

}


