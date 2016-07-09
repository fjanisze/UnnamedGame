#include <chrono>
#include <ctime>
#include <string>
#include <iomanip>
#include <iostream>
#include <cstring>

#ifndef CHRONO_HPP
#define CHRONO_HPP

namespace game_chrono
{

using ll = long long;

struct game_time_t
{
    std::tm game_time;
    ll      tick_count; //From the beginning of the game
    //Use the default game start date
    game_time_t(int day = 1,
                int month = 1,
                int year = 2230) :
        tick_count{0} {
        memset(&game_time,0,sizeof(std::tm));
        game_time.tm_mday = day;
        game_time.tm_mon = month - 1;
        game_time.tm_year = year - 1900;
    }
};

class chrono
{
    game_time_t current_time;
public:
    chrono(game_time_t starting_time = game_time_t());
    //Return the current game time
    game_time_t game_time();
    std::string format_game_time();
    //When time elapse, this function is called.
    void clock_tick();
    //Return the current time as std::string
    static std::string get_wallclock_time();
};

}

#endif
