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
    generate();
}

void random::generate()
{
    static const uint32_t generation_limit{ 10000 };
    if(llong_queue.size()>=generation_limit &&
       ldouble_queue.size()>=generation_limit)
    {
        return;
    }
    for(uint32_t i{0};i<1000;i++){
        if(llong_queue.size() < generation_limit){
            llong_queue.push(dist(eng));
        }
        if(ldouble_queue.size() < generation_limit){
            ldouble_queue.push( std::generate_canonical<ldouble,10>(eng));
        }
    }
    LOG1("Random data, llong queue:",llong_queue.size(),
         ", ldouble queue:",ldouble_queue.size());
}

ldouble random::get_canonical()
{

}

llong random::get_llong()
{

}

std::pair<llong, llong> random::get_llong_pair()
{

}

}
