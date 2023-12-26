/**
  ******************************************************************************
  * @file           : MissionImpl.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/21
  ******************************************************************************
  */

#include "AsyncLogger.h"                        /* AsyncLogger */
#include "MissionReactor/MissionImpl.h"         /* MissionImpl */

namespace hzd {

#define MISSION_STANDARD_BEGIN do {                                             \
    for(const auto& startSignal : startSignals) {                               \
        if(!signalMap[startSignal]) return 0;                                   \
    }                                                                           \
                                                                                \
    auto durationFrameCount = fps * duration / 1000;                            \
    if(records.size() >= durationFrameCount) {                                  \
        if(fps > 1 && rightCount > tolerance * durationFrameCount){             \
            LOG_TRACE(mainMissionName,"[" + name + "事件完成]"                    \
                          + std::to_string(rightCount * 1000 / fps) + "ms "     \
                          + "ItemAID:" + std::to_string(itemA) + ","            \
                          + "ItemBID:" + std::to_string(itemB) + ","            \
                          + "Bound.x:" + std::to_string(bound.x) + ","          \
                          + "Bound.y:" + std::to_string(bound.y) + ","          \
                          + "Bound.width" + std::to_string(bound.width) + ","   \
                          + "Bound.height" + std::to_string(bound.height));     \
                                                                                \
            records.clear();                                                    \
            rightCount = 0;                                                     \
            bool isInterrupt = false;                                           \
            for(const auto& endSignal : endSignals) {                           \
                if(endSignal == 1024){ isInterrupt = true; continue;}           \
                endSignal >= 0 ?                                                \
                signalMap[endSignal] = true                                     \
                :                                                               \
                signalMap[-endSignal] = false;                                  \
            }                                                                   \
            if(isInterrupt) return -1;                                          \
            return 1;                                                           \
        }                                                                       \
    }                                                                           \
}while(0)

#define MISSION_STANDARD_END do {                                       \
    if(flag) rightCount++;                                              \
    records.emplace_back(flag);                                         \
    if(fps > 1 && records.size() > duration * fps / 1000 + 1) {         \
        if(records.front()) rightCount--;                               \
        records.pop_front();                                            \
    }                                                                   \
}while(0)

    StayMissionImpl::StayMissionImpl() : MissionInterface("停留") {}

    int StayMissionImpl::operator()(
            const ItemVecMap &itemVecMap,
            int fps,
            const std::string &mainMissionName,
            SignalMap &signalMap
    )
    {
        if(!isInit) {
            startSignalCount = parameters[0].asInt32();
            endSignalCount = parameters[1].asInt32();
            itemA = parameters[2].asInt32();
            itemB = parameters[3].asInt32();
            duration = parameters[4].asInt32();
            bound = {
                    parameters[5].asInt32(),
                    parameters[6].asInt32(),
                    parameters[7].asInt32(),
                    parameters[8].asInt32()
            };
            anyoneTriggle = parameters[9].asBool();
            for(auto index = 1;index <= startSignalCount;index++ ){
                startSignals.push_back(parameters[9+index].asInt32());
            }
            for(auto index = 1;index <= endSignalCount;index++ ){
                startSignals.push_back(parameters[9+startSignalCount+index].asInt32());
            }
            isInit = true;
        }
        MISSION_STANDARD_BEGIN;
        bool flag = false;
        if(itemB != -1) {
            auto iterA = itemVecMap.find(itemA);
            auto iterB = itemVecMap.find(itemB);
            if(iterA != itemVecMap.end() && iterB != itemVecMap.end()) {
                const auto& itemAs = iterA->second;
                const auto& itemBs = iterB->second;
                if(anyoneTriggle) {
                    for (const auto &itmA: itemAs) {
                        for (const auto &itmB: itemBs) {
                            flag |= Item::Consist(itmB, itmA);
                            if (flag) break;
                        }
                        if (flag) break;
                    }
                }else{
                    for (const auto &itmA: itemAs) {
                        for (const auto &itmB: itemBs) {
                            if(itmA.area() > itmB.area()){
                                flag = false;
                                break;
                            }
                            flag &= Item::Consist(itmB, itmA);
                            if(!flag) break;
                        }
                        if(!flag) break;
                    }
                }
            }
        }else if(!bound.empty()) {
            auto iterA = itemVecMap.find(itemA);
            if(iterA != itemVecMap.end()) {
                if(anyoneTriggle) {
                    for(const auto& itmA : iterA->second) {
                        if(itmA.area() > bound.area()) continue;
                        flag |= Item::Consist(bound,itmA);
                        if(flag) break;
                    }
                }else {
                    for(const auto& itmA : iterA->second) {
                        if(itmA.area() > bound.area()) {
                            flag = false;
                            break;
                        }
                        flag &= Item::Consist(bound,itmA);
                        if(!flag) break;
                    }
                }
            }
        }else {
            auto iterA = itemVecMap.find(itemA);
            flag = (iterA != itemVecMap.end() && !iterA->second.empty());
        }

        MISSION_STANDARD_END;
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
        if(!isInit) {
            startSignalCount = parameters[0].asInt32();
            endSignalCount = parameters[1].asInt32();
            itemA = parameters[2].asInt32();
            itemB = parameters[3].asInt32();
            duration = parameters[4].asInt32();
            bound = {
                    parameters[5].asInt32(),
                    parameters[6].asInt32(),
                    parameters[7].asInt32(),
                    parameters[8].asInt32()
            };
            anyoneTriggle = parameters[9].asBool();
            for(auto index = 1;index <= startSignalCount;index++ ){
                startSignals.push_back(parameters[9+index].asInt32());
            }
            for(auto index = 1;index <= endSignalCount;index++ ){
                endSignals.push_back(parameters[9+startSignalCount+index].asInt32());
            }
            isInit = true;
        }

        MISSION_STANDARD_BEGIN;
        bool flag = false;
        if(itemB != -1) {
            auto iterA = itemVecMap.find(itemA);
            auto iterB = itemVecMap.find(itemB);
            if(iterA != itemVecMap.end() && iterB != itemVecMap.end()) {
                const auto& itemAs = iterA->second;
                const auto& itemBs = iterB->second;
                if(anyoneTriggle) {
                    for (const auto &itmA: itemAs) {
                        for (const auto &itmB: itemBs) {
                            flag |= Item::Cross(itmB, itmA);
                            if (flag) break;
                        }
                        if (flag) break;
                    }
                }else{
                    for (const auto &itmA: itemAs) {
                        for (const auto &itmB: itemBs) {
                            if(itmA.area() > itmB.area()){
                                flag = false;
                                break;
                            }
                            flag &= Item::Cross(itmB, itmA);
                            if(!flag) break;
                        }
                        if(!flag) break;
                    }
                }
            }
        }
        else if(!bound.empty()) {
            auto iterA = itemVecMap.find(itemA);
            if(iterA != itemVecMap.end()) {
                if(anyoneTriggle) {
                    for(const auto& itmA : iterA->second) {
                        if(itmA.area() > bound.area()) continue;
                        flag |= Item::Cross(bound,itmA);
                        if(flag) break;
                    }
                }else {
                    for(const auto& itmA : iterA->second) {
                        if(itmA.area() > bound.area()) {
                            flag = false;
                            break;
                        }
                        flag &= Item::Cross(bound,itmA);
                        if(!flag) break;
                    }
                }
            }
        }
        else {
            auto iterA = itemVecMap.find(itemA);
            flag = (iterA != itemVecMap.end() && !iterA->second.empty());
        }

        MISSION_STANDARD_END;

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
        if(!isInit) {
            startSignalCount = parameters[0].asInt32();
            endSignalCount = parameters[1].asInt32();
            itemA = parameters[2].asInt32();
            itemB = parameters[3].asInt32();
            duration = parameters[4].asInt32();
            bound = {
                    parameters[5].asInt32(),
                    parameters[6].asInt32(),
                    parameters[7].asInt32(),
                    parameters[8].asInt32()
            };
            anyoneTriggle = parameters[9].asBool();
            minThreshold = parameters[10].asDouble();
            maxThreshold = parameters[11].asDouble();
            for(auto index = 1;index <= startSignalCount;index++ ){
                startSignals.push_back(parameters[11+index].asInt32());
            }
            for(auto index = 1;index <= endSignalCount;index++ ){
                endSignals.push_back(parameters[11+startSignalCount+index].asInt32());
            }
            isInit = true;
        }

        MISSION_STANDARD_BEGIN;

        bool flag = false;
        if(itemB != -1) {
            auto iterA = itemVecMap.find(itemA);
            auto iterB = itemVecMap.find(itemB);
            if (iterA != itemVecMap.end() && iterB != itemVecMap.end()) {
                const auto &itemAs = iterA->second;
                const auto &itemBs = iterB->second;
                if (anyoneTriggle) {
                    for (const auto &itmA: itemAs) {
                        for (const auto &itmB: itemBs) {
                            auto distance = Item::Distance(itmA, itmB);
                            if (distance >= minThreshold * itmB.width
                                && distance <= maxThreshold * itmB.width) {
                                flag = true;
                                break;
                            }
                        }
                        if(flag) break;
                    }
                } else {
                    auto minDistance = DBL_MIN;
                    const Item *minItemB = nullptr;
                    for (const auto &itmA: itemAs) {
                        for (const auto &itmB: itemBs) {
                            auto distance = Item::Distance(itmA, itmB);
                            if (minDistance > distance) {
                                minDistance = distance;
                                minItemB = &itmB;
                            }
                        }
                    }
                    if (minItemB
                        && minDistance >= minThreshold * minItemB->width
                        && minDistance <= maxThreshold * minItemB->width) {
                        flag = true;
                    }
                }
            }
        }
        else if(!bound.empty()) {
            auto iterA = itemVecMap.find(itemA);
            if(iterA != itemVecMap.end()) {
                if(anyoneTriggle) {
                    for(const auto& itmA : iterA->second) {
                        auto distance = Item::Distance(itmA,bound);
                        if (distance >= minThreshold * bound.width
                            && distance <= maxThreshold * bound.width) {
                            flag = true;
                            break;
                        }
                    }
                }else {
                    auto minDistance = DBL_MAX;
                    for(const auto& itmA : iterA->second) {
                        minDistance = std::min(minDistance,Item::Distance(itmA,bound));
                    }
                    if (minDistance >= minThreshold * bound.width
                    && minDistance <= maxThreshold * bound.width) {
                        flag = true;
                    }
                }
            }
        }
        MISSION_STANDARD_END;
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
        if(!isInit) {
            startSignalCount = parameters[0].asInt32();
            endSignalCount = parameters[1].asInt32();
            itemA = parameters[2].asInt32();
            itemB = parameters[3].asInt32();
            duration = parameters[4].asInt32();
            bound = {
                    parameters[5].asInt32(),
                    parameters[6].asInt32(),
                    parameters[7].asInt32(),
                    parameters[8].asInt32()
            };
            anyoneTriggle = parameters[9].asBool();
            minThreshold = parameters[10].asDouble();
            maxThreshold = parameters[11].asDouble();
            vecA = {(double)parameters[12].asInt32(),(double)parameters[13].asInt32()};
            vecB = {(double)parameters[14].asInt32(),(double)parameters[15].asInt32()};
            for(auto index = 1;index <= startSignalCount;index++ ){
                startSignals.push_back(parameters[15+index].asInt32());
            }
            for(auto index = 1;index <= endSignalCount;index++ ){
                endSignals.push_back(parameters[15+startSignalCount+index].asInt32());
            }
            isInit = true;
        }

        MISSION_STANDARD_BEGIN;

        bool flag = false;
        if(itemB != -1) {
            auto iterA = itemVecMap.find(itemA);
            auto iterB = itemVecMap.find(itemB);
            if (iterA != itemVecMap.end() && iterB != itemVecMap.end()) {
                const auto &itemAs = iterA->second;
                const auto &itemBs = iterB->second;
                if (anyoneTriggle) {
                    for (const auto &itmA: itemAs) {
                        for (const auto &itmB: itemBs) {
                            double x = itmA.x + (double)itmA.width / 2 - itmB.x - (double)itmB.width / 2;
                            double y = itmA.y + (double)itmA.height / 2 - itmB.y - (double)itmB.height / 2;
                            if(!Vec::IsInArea(x,y,vecA,vecB)) continue;

                            auto distance = Item::Distance(itmA, itmB);
                            if (distance >= minThreshold * itmB.width
                                && distance <= maxThreshold * itmB.width) {
                                flag = true;
                                break;
                            }
                        }
                        if(flag) break;
                    }
                } else {
                    auto minDistance = DBL_MIN;
                    const Item *minItemB = nullptr;
                    for (const auto &itmA: itemAs) {
                        for (const auto &itmB: itemBs) {
                            double x = itmA.x + (double)itmA.width / 2 - itmB.x - (double)itmB.width / 2;
                            double y = itmA.y + (double)itmA.height / 2 - itmB.y - (double)itmB.height / 2;
                            if(!Vec::IsInArea(x,y,vecA,vecB)) continue;

                            auto distance = Item::Distance(itmA, itmB);
                            if (minDistance > distance) {
                                minDistance = distance;
                                minItemB = &itmB;
                            }
                        }
                    }
                    if (minItemB
                        && minDistance >= minThreshold * minItemB->width
                        && minDistance <= maxThreshold * minItemB->width) {
                        flag = true;
                    }
                }
            }
        }
        else if(!bound.empty()) {
            auto iterA = itemVecMap.find(itemA);
            if(iterA != itemVecMap.end()) {
                if(anyoneTriggle) {
                    for(const auto& itmA : iterA->second) {
                        double x = itmA.x + (double)itmA.width / 2 - bound.x - (double)bound.width / 2;
                        double y = itmA.y + (double)itmA.height / 2 - bound.y - (double)bound.height / 2;
                        if(!Vec::IsInArea(x,y,vecA,vecB)) continue;

                        auto distance = Item::Distance(itmA,bound);
                        if (distance >= minThreshold * bound.width
                            && distance <= maxThreshold * bound.width) {
                            flag = true;
                            break;
                        }
                    }
                }else {
                    auto minDistance = DBL_MAX;
                    for(const auto& itmA : iterA->second) {
                        double x = itmA.x + (double)itmA.width / 2 - bound.x - (double)bound.width / 2;
                        double y = itmA.y + (double)itmA.height / 2 - bound.y - (double)bound.height / 2;
                        if(!Vec::IsInArea(x,y,vecA,vecB)) continue;

                        minDistance = std::min(minDistance,Item::Distance(itmA,bound));
                    }
                    if (minDistance >= minThreshold * bound.width
                        && minDistance <= maxThreshold * bound.width) {
                        flag = true;
                    }
                }
            }
        }
        MISSION_STANDARD_END;

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

    DirectedDistanceMissionImpl::Vec::Vec(double _x, double _y) : x(_x),y(_y) {}
} // hzd