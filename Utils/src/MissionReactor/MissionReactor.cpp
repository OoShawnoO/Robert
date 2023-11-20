/**
  ******************************************************************************
  * @file           : MissionReactor.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/20
  ******************************************************************************
  */

#include "MissionReactor/MissionReactor.h"

namespace hzd {

    MissionReactor::MissionReactor() {
        analysisFrameCount = 10;
        startTolerance = 0.8;
        endTolerance = 0.95;
        saveResultCount = 8196;
        startFrameNo = 0;
    }

    bool MissionReactor::LoadMission(const std::string &missionFilePath) {
        return false;
    }

    MissionReactor::Res MissionReactor::React(const MissionReactor::ReactInput &input) {
        return MissionReactor::Res();
    }

    void MissionReactor::Clear() {
        fps = 0;
        startFrameNo = 0;
        finished.clear();
        itemNameMap.clear();
        nameItemMap.clear();
        historyResult.clear();
        runtimeProcess.clear();
        nameMissionMap.clear();
        workMissions.clear();
        globalMissions.clear();

        settlement.rightMissions.clear();
        settlement.startBound = {};
        settlement.endBound = {};
        settlement.startDuration = 0;
        settlement.endDuration = 0;
        settlement.startItems.clear();
        settlement.endItems.clear();
    }

    bool MissionReactor::verify(MissionReactor::Res &res) const {
        return false;
    }

    void MissionReactor::registerItem(const std::string &name, ItemID id) {

    }

    Ptr<MissionInterface> MissionReactor::registerMission(const std::string &missionName) {
        return hzd::Ptr<MissionInterface>();
    }
} // hzd