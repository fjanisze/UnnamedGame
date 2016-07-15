
#include "objects.hpp"
#include <utility>

namespace objects
{

celestial_body::celestial_body(const std::string &name,
                             const celestial_body_spec &specifics,
                             const object_coordinates &position)
{
    body_info.body_specifics = specifics;
    body_info.body_position = position;
}

const object_coordinates &celestial_body::get_body_coordinates()
{
    return body_info.body_position;
}

object_info::object_info():
    body_type{celestial_body_types::celestial_body_none},
    body_unique_id{0}
{   }

}
