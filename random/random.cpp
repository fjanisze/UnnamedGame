#include "random.hpp"
#include "../logger/logger.hpp"
#include <limits>

namespace random_engine
{

random::random():
    eng(rd()),
    dist(std::numeric_limits<llong>::min(),
         std::numeric_limits<llong>::max())
{
    LOG3("Starting the game random engine.");

    llong_randoms.reserve(random_numbers_limis);
    ldouble_randoms.reserve(random_numbers_limis);
    //Setting this index to 0 means that we have no
    //more random numbers available now.
    llong_randoms_idx = 0;
    ldouble_randoms_idx = 0;
    generate();
}

void random::generate()
{
    while(llong_randoms_idx < random_numbers_limis - 1){
        llong_randoms[llong_randoms_idx++] = dist(eng);
    }

    while(ldouble_randoms_idx < random_numbers_limis - 1){
        ldouble_randoms[ldouble_randoms_idx++] = std::generate_canonical<ldouble,10>(eng);
    }
    LOG1("Random data, llong:",llong_randoms_idx,
         ", ldouble:",ldouble_queue.size());
}

llong random::get_llong()
{
    if(llong_randoms_idx == 0)
        generate();
    return llong_randoms[llong_randoms_idx--];
}

std::pair<llong, llong> random::get_llong_pair()
{
    if(llong_randoms_idx < 2)
        generate();
    //To avoid the "multiple unsequenced warning"
    llong_randoms_idx -= 2;
    return std::make_pair(llong_randoms[llong_randoms_idx + 1],
                          llong_randoms[llong_randoms_idx + 2]);
}

}
