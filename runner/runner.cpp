#include "../logger/logger.hpp"
#include "runner.hpp"
#include <string>
#include <thread>
#include <chrono>
#include <future>

namespace game_runner
{

runner::runner(){
    LOG3("Starting the game runner!");
    game_time = std::make_shared<game_chrono::chrono>();
    game_conf = std::make_shared<game_configuration::configuration_loader>("config.txt");
    game_random_engine = std::make_shared<random_engine::random>();
    game_event_queue = std::make_shared<game_events::events>("RUNNER QUEUE");
    game_ui = std::make_shared<game_graphics::ui>(game_conf,
                                                  game_event_queue);

    game = std::make_shared<game_engine>();

    setup_logger();

    auto game_loop_thread = std::thread{&runner::game_loop,this};
    game_loop_thread.detach();
}

void runner::start()
{
    game_ui->loop();
}

void runner::game_loop(){
    SET_LOG_THREAD_NAME("GLOOP");
    LOG3("Entering the game loop");
    while(1){
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
    }
}

/*
 * The configuration file may include some settings
 * for the logger, check the game configuration
 * and update the logger settings
 */
void runner::setup_logger(){
    std::string logging_level = game_conf->get_option("logging_level");
    logging::severity_type new_severity;
    if(!logging_level.empty()){
        LOG3("Request to set the logging level to: ",
             logging_level.c_str());
        new_severity = logging::severity_type(std::stoi(logging_level));
        log_inst.set_logging_level(new_severity);
    }
}

game_engine::game_engine()
{
    LOG3("Starting the game engine");
}

}
