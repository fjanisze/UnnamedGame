#ifndef POSITION_HPP
#define POSITION_HPP

#include <stdint.h>

namespace coordinates
{

struct object_coordinates
{
    uint32_t x,
             y;
    object_coordinates(uint32_t x_coord = 0,
                       uint32_t y_coord = 0) :
        x{x_coord},
        y{y_coord}
    {}
};

}

#endif
