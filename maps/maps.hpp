#ifndef MAPS_HPP
#define MAPS_HPP

#include "position.hpp"
#include "objects.hpp"
#include <vector>

namespace game_maps
{

using namespace coordinates;
using namespace objects;

struct uni_map_specifics
{
    uint32_t universe_width,
             universe_height;

    uni_map_specifics() :
        universe_height{0},
        universe_width{0}
    {}
    uni_map_specifics(uint32_t width,
                      uint32_t height):
        universe_width{width},
        universe_height{height}
    {}
};

class universe_map
{
    std::vector<celestial_body_ptr> celestial_bodies;
    uni_map_specifics               universe_specification;
public:
    universe_map();
    void set_universe_size(uint32_t width,
                           uint32_t height);

    uint32_t add_celestial_body(celestial_body_ptr new_body);

    std::vector<celestial_body_cptr> find_bodies_within(uint32_t top_left_y,
                                                        uint32_t top_left_x,
                                                        uint32_t bottom_right_x,
                                                        uint32_t bottom_right_y);
};

}

#endif
