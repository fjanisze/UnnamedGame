#ifndef OBJECTS_HPP
#define OBJECTS_CPP

#include <string>
#include <memory>

namespace objects
{

struct object_planet;

using object_planet_ptr = std::shared_ptr<object_planet>;

struct object_position
{
    uint32_t x,
             y,
             diameter;

    object_position():
        x{0},y{0},diameter{0}
    {}
};

/*
 * The common base for all the objects is monstly
 * important in order to allow a uniform way of
 * drawing those entities.
 */
class drawable_object
{
protected:
    object_position position;
public:
    drawable_object();

    virtual ~drawable_object();
};


class object_planet : public drawable_object
{
public:
    explicit object_planet(const std::string& name,
                  uint32_t diameter,
                  uint32_t x_coord,
                  uint32_t y_coord);

    template<typename...ARGS>
    static object_planet_ptr create(ARGS...args);

};

template<typename...ARGS>
object_planet_ptr object_planet::create(ARGS...args)
{
    return std::make_shared<object_planet>(std::forward<ARGS>(args)...);
}


}

#endif
