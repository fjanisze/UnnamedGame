#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <string>
#include <map>
#include <logger/logger.hpp>

namespace game_configuration
{

struct configuration_loader;

using game_config_ptr = std::shared_ptr<configuration_loader>;

class configuration_loader
{
    std::string configuration_filename;
    //Those data are coming from the configuration file
    std::map<std::string,std::string> configuration_data;
    void read_conf_file();
public:
    configuration_loader(const std::string& conf_file_name) :
        configuration_filename{conf_file_name}{
        LOG3("Running the game conf. loader, Opening and reading configuration file ",
             conf_file_name.c_str());
        read_conf_file();
    }
    std::string get_option(const std::string& option);
};

}

#endif
