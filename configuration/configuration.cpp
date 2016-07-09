#include "configuration.hpp"
#include <fstream>

namespace game_configuration
{

void configuration_loader::read_conf_file()
{
    std::ifstream in_file(configuration_filename.c_str());
    if(in_file){
        std::string current_line;
        std::size_t pos,equal_pos,line_number{1};
        while(std::getline(in_file,current_line)){
            if(current_line.empty())
                continue;
            //Remove trailing spaces and looking the '=' symbol
            pos = 0;
            equal_pos = 0;
            for(std::size_t i{0};i<current_line.size();i++){
                if(current_line[i]==' ')
                    continue;
                else if(pos != i){
                    current_line[pos] = current_line[i];
                }
                if(current_line[pos]=='='){
                    if(equal_pos!=0){
                        WARN1("Multiple '=' found in conf. file at line ",
                              line_number,", will not parse this line anymore");
                        equal_pos = 0;
                        break;
                    }
                    equal_pos = pos;
                }
                ++pos;
            }
            current_line.erase(pos);
            if(current_line.empty())
                continue;
            LOG1("Conf. line without spaces: ", current_line.c_str());
            if(current_line[0]=='#'){
                //Skipping comment line
                continue;
            }
            if(equal_pos==0){
                WARN2("Configuration file malformed, unable to parse the configuration line ",
                      line_number);
            }
            else{
                std::string option,value;
                option = current_line.substr(0,equal_pos);
                value = current_line.substr(equal_pos+1);
                LOG3("Pushing configuration option: ",option.c_str()," = ",value.c_str());
                configuration_data[option] = value;
            }
            current_line.clear();
            ++line_number;
        }
    }else{
        ERR("Unable to open the configuration file!");
    }
}

std::string configuration_loader::get_option(const std::string& option)
{
    std::string value;
    auto conf_it = configuration_data.find(option);
    if(conf_it==configuration_data.end()){
        WARN2("Unable to find the requested option: ",
             option.c_str());
    }else{
        value = conf_it->second;
    }
    return value;
}

}
