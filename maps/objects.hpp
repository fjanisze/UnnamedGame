#ifndef OBJECTS_HPP
#define OBJECTS_CPP

#include "position.hpp"
#include <string>
#include <memory>

namespace objects
{
using namespace coordinates;

struct celestial_body;

using celestial_body_ptr = std::shared_ptr<celestial_body>;
using celestial_body_cptr = std::shared_ptr<const celestial_body>;

enum class celestial_body_types
{
    celestial_body_none,
    celestial_body_planet,
    celestial_body_star
};

struct celestial_body_spec
{
    uint32_t diameter;

    celestial_body_spec():
        diameter{0}
    {}
};

struct object_info
{
    celestial_body_types body_type;
    std::string          body_name;
    uint32_t             body_unique_id;
    object_coordinates   body_position;
    celestial_body_spec  body_specifics;

    object_info();
};

class celestial_body
{
    object_info body_info;
public:
    explicit celestial_body(const std::string& name,
                  const celestial_body_spec& specifics,
                  const object_coordinates& position);

    const object_coordinates& get_body_coordinates();

    template<typename...ARGS>
    static celestial_body_ptr create(ARGS...args);

};

template<typename...ARGS>
celestial_body_ptr celestial_body::create(ARGS...args)
{
    return std::make_shared<celestial_body>(std::forward<ARGS>(args)...);
}


}

#endif
