#ifndef HPP_RUNNER
#define HPP_RUNNER

#include <memory>
#include "../events/events.hpp"
#include "../chrono/chrono.hpp"
#include "../graphics/ui.hpp"
#include "../configuration/configuration.hpp"
#include "../random/random.hpp"
#include "../maps/maps.hpp"

namespace game_runner
{

using namespace game_configuration;
using namespace game_events;
using namespace random_engine;
using namespace game_maps;

using game_chrono_pointer = std::shared_ptr<game_chrono::chrono>;
using game_ui_pointer = std::shared_ptr<game_graphics::ui>;

class game_engine
{
    universe_map star_chart;
public:
    game_engine();
};

using game_engine_ptr = std::shared_ptr<game_engine>;

class runner
{
    game_evt_pointer    game_event_queue;
    game_chrono_pointer game_time;
    game_config_ptr     game_conf;
    game_ui_pointer     game_ui;
    random_engine_ptr   game_random_engine;

    game_engine_ptr     game;

    void setup_logger();
    void game_loop();
public:
    runner();
};

}

#endif
