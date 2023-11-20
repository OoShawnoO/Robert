/**
  ******************************************************************************
  * @file           : MissionInterface.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/20
  ******************************************************************************
  */
#include <iostream>

#include "MissionReactor/MissionInterface.h"

namespace hzd {

    unsigned int MissionInterface::staticMissionCount = 0;

    MissionInterface::MissionInterface(std::string _name) : name(std::move(_name)) {
        if(name.size() >= 64) {
            std::cerr << "[FATAL] mission name must less than 64 Byte." << std::endl;
            exit(-1);
        }
        name += std::to_string(MissionInterface::staticMissionCount++);
    }
} // hzd
