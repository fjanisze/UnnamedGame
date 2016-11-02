#include <logger/logger.hpp>
#include "chrono.hpp"

namespace game_chrono
{

chrono::chrono(game_time_t starting_time):
    current_time{starting_time}
{
    LOG3("Running chrono, current wallclock time: ",
         get_wallclock_time().c_str());
}


game_time_t chrono::game_time(){
    return current_time;
}

std::string chrono::format_game_time(){
    return  std::asctime(&current_time.game_time);
}

/*
 * The main game loop at each iteration call this
 * function to simulate the time flow in the game
 */
void chrono::clock_tick(){
    ++current_time.tick_count;
    ++current_time.game_time.tm_mday;
    std::mktime(&current_time.game_time);
}

/*
 * Return the current system time in the format: HH:MM:SS.xxx
 * where xxx are milliseconds
 */
std::string chrono::get_wallclock_time()
{
    auto cur_time = std::chrono::duration_cast<
            std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch());
    std::time_t cur_time_timet = std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::time_point(cur_time)
                );
    char buffer[13];
    std::size_t written = std::strftime(buffer,12,"%T",std::localtime(&cur_time_timet));
    std::string elapsed_ms = std::to_string(cur_time.count()%1000);
    buffer[written] = '.';
    std::size_t buf_pos{ written + 3 };
    for(std::size_t position{elapsed_ms.size()};buf_pos!=written;--buf_pos){
        if(position>0)
            buffer[buf_pos] = elapsed_ms[--position];
        else
            buffer[buf_pos] = '0';
    }
    buffer[12] = '\0';
    return std::string(buffer);
}

}
