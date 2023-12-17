/**
  ******************************************************************************
  * @file           : MissionFactory.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/21
  ******************************************************************************
  */

#include "MissionReactor/MissionFactory.h"      /* MissionFactory */

namespace hzd {

#define REGISTER_MISSION(missionImplClass,name) do {        \
    if(missionName == name) {                               \
        return std::make_shared<missionImplClass>();        \
    }                                                       \
}while(0)

    Ptr<MissionInterface> MissionFactory::Create(const std::string &missionName) {
        REGISTER_MISSION(StayMissionImpl,"停留事件");
        REGISTER_MISSION(ContactMissionImpl,"接触事件");
        REGISTER_MISSION(DistanceMissionImpl,"距离事件");
        REGISTER_MISSION(DirectedDistanceMissionImpl,"指定方向距离事件");
        return nullptr;
    }
} // hzd