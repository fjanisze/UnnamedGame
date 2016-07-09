#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <queue>
#include <random>
#include <utility>
#include <memory>

namespace random_engine
{

class random;

using random_engine_ptr = std::shared_ptr<random>;

using ldouble = long double;
using llong   = long long;

class random
{
    std::queue<ldouble> ldouble_queue;
    std::queue<llong>   llong_queue;

    std::random_device         rd;
    std::mt19937_64            eng;
    std::uniform_int_distribution<llong> dist;
public:
    random();
    void generate();

    ldouble get_canonical();
    llong   get_llong();

    std::pair<llong,llong> get_llong_pair();
};

}

#endif
