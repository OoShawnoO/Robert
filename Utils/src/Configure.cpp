/**
  ******************************************************************************
  * @file           : Configure.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/18
  ******************************************************************************
  */

#include <fstream>
#include <iostream>
#include "Configure.h"

namespace hzd {

    std::vector<std::string> Configure::Keys() {
        std::vector<std::string> k;
        for(auto it=conf.begin();it != conf.end();it++){
            k.emplace_back(it.key());
        }
        return k;
    }

    auto Configure::operator[](const std::string &key) -> json::value_type {
        return conf[key];
    }

    Configure &Configure::Get() {
        static Configure config("../etc/config.json");
        return config;
    }

    Configure::Configure(const std::string &configPath) {
        std::ifstream in(configPath);
        if(!in.is_open()) {
            std::cerr << "[FATAL] bad configure file path." << std::endl;
            exit(-1);
        }
        conf = json::parse(in);
        in.close();
    }

    bool Configure::Reload(const std::string &configPath) {
        std::ifstream in(configPath);
        if(!in.is_open()) {
            return false;
        }
        conf = json::parse(in);
        in.close();
        return true;
    }

    void Configure::Reload(json &config) {
        conf = config;
    }

} // hzd
