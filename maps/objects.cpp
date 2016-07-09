
#include "objects.hpp"
#include <utility>

namespace objects
{

drawable_object::drawable_object()
{

}

objects::drawable_object::~drawable_object()
{

}

object_planet::object_planet(const std::string &name,
                             uint32_t diameter,
                             uint32_t x_coord,
                             uint32_t y_coord)
{
    position.diameter = diameter;
    position.x = x_coord;
    position.y = y_coord;
}

}
