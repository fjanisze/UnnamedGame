#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <queue>
#include <random>
#include <utility>
#include <memory>
#include <vector>

namespace random_engine
{

static const std::size_t random_numbers_limis = 10000;

class random;

using random_engine_ptr = std::shared_ptr<random>;

using ldouble = long double;
using llong   = long long;

class random
{
    std::queue<ldouble> ldouble_queue;
    std::queue<llong>   llong_queue;

    std::size_t          ldouble_randoms_idx;
    std::vector<ldouble> ldouble_randoms;

    std::size_t         llong_randoms_idx;
    std::vector<llong>  llong_randoms;

    std::random_device         rd;
    std::mt19937_64            eng;
    std::uniform_int_distribution<llong> dist;

    void generate();
public:
    random();

    llong   get_llong();

    std::pair<llong,llong> get_llong_pair();
};

}

#endif
