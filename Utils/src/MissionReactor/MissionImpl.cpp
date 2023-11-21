/**
  ******************************************************************************
  * @file           : MissionImpl.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/21
  ******************************************************************************
  */

#include "MissionReactor/MissionImpl.h"         /* MissionImpl */

namespace hzd {

    StayMissionImpl::StayMissionImpl() : MissionInterface("停留") {}

    int StayMissionImpl::operator()(
            const ItemVecMap &itemVecMap,
            int fps,
            const std::string &mainMissionName,
            SignalMap &signalMap
    )
    {
        return 0;
    }

    ContactMissionImpl::ContactMissionImpl() : MissionInterface("接触") {}

    int ContactMissionImpl::operator()(
            const ItemVecMap &itemVecMap,
            int fps,
            const std::string &mainMissionName,
            SignalMap &signalMap
    )
    {
        return 0;
    }

    DistanceMissionImpl::DistanceMissionImpl() : MissionInterface("相距") {}

    int DistanceMissionImpl::operator()(
            const ItemVecMap &itemVecMap,
            int fps,
            const std::string &mainMissionName,
            SignalMap &signalMap
    )
    {
        return 0;
    }

    DirectedDistanceMissionImpl::DirectedDistanceMissionImpl() : MissionInterface("向量相距") {}

    int DirectedDistanceMissionImpl::operator()(
            const ItemVecMap &itemVecMap,
            int fps,
            const std::string &mainMissionName,
            SignalMap &signalMap
    )
    {
        return 0;
    }

    bool DirectedDistanceMissionImpl::Vec::IsInArea(
            double x,
            double y,
            const DirectedDistanceMissionImpl::Vec &vecI,
            const DirectedDistanceMissionImpl::Vec &vecII
    )
    {
        Vec vecIII {x,y};
        Vec I_II = {vecII.x - vecI.x,vecII.y - vecI.y};
        Vec I_III = { vecIII.x - vecI.x,vecIII.y - vecI.y};
        double cross = I_II.x * I_III.y - I_II.y * I_III.y;
        return cross <= 0;
    }

    double DirectedDistanceMissionImpl::Vec::DotProduct(
            const DirectedDistanceMissionImpl::Vec &vI,
            const Vec &vII
    )
    {
        return vI.x * vII.x + vI.y * vII.y;
    }

    DirectedDistanceMissionImpl::Vec::Vec(double _x, double _y) : x(_x),y(_y) {}
} // hzd