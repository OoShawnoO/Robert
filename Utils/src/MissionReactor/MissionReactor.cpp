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
    const std::string MissionReactorChan = "MissionReactor";

    MissionReactor::MissionReactor() {
        analysisFrameCount = 10;
        startTolerance = 0.8;
        endTolerance = 0.9;
        saveResultCount = 8196;
    }

    bool MissionReactor::LoadMission(
            const std::string &missionFilePath,
            int _analysisFrameCount,
            double _startTolerance,
            double _endTolerance,
            int _saveResultCount
    ) {
        Clear();
        mainMissionName = missionFilePath.substr(missionFilePath.find_last_of('/') + 1);
        mainMissionName = mainMissionName.substr(0,mainMissionName.size() - 8);
        analysisFrameCount = _analysisFrameCount;
        startTolerance = _startTolerance;
        endTolerance = _endTolerance;
        saveResultCount = _saveResultCount;

        std::ifstream in(missionFilePath);
        if(!in.is_open()) {
            LOG_ERROR(MissionReactorChan,"open file failed");
            return false;
        }
        char choice[5] = {0};
        while(!in.eof()) {
            bzero(choice,5);
            in.read(choice,4);
            std::string choiceStr = choice;
            if(choiceStr.empty()) return true;
            if(choiceStr == "func") {
                char buffer[64] = {0};
                MissionInterface::Parameter param;
                in.read(buffer,64);
                auto mission = registerMission(buffer);
                if(!mission) {
                    LOG_ERROR(MissionReactorChan,"failed to create mission:" + std::string(buffer));
                    return false;
                }
                size_t size;
                in.read((char*)&size,sizeof(size_t));
                for(auto index = 0;index < size;index++){
                    in.read((char*)&param,PARAMETER_SIZE);
                    mission->parameters.emplace_back(param);
                }
            }
            else if(choiceStr == "item") {
                ItemID id;
                char buffer[64] = {0};
                in.read(buffer,64);
                in.read((char*)&id,sizeof(ItemID));
                registerItem(buffer,id);
            }
            else if(choiceStr == "sett") {
                size_t size;
                in.read((char*)&settlement.startType,SETTLEMENT_STARTTYPE_SIZE);
                in.read((char*)&size,sizeof(size_t));
                settlement.startItems.resize(size);
                for(auto index = 0; index < size; index++) {
                    in.read((char*)&settlement.startItems[index],sizeof(ItemID));
                }
                switch (settlement.startType) {
                    case Settlement::StartItemAppear : { break; }
                    case Settlement::StartItemAppearInBound : {
                        in.read((char*)&settlement.startBound.x,sizeof(int));
                        in.read((char*)&settlement.startBound.y,sizeof(int));
                        in.read((char*)&settlement.startBound.width,sizeof(int));
                        in.read((char*)&settlement.startBound.height,sizeof(int));
                        break;
                    }
                    case Settlement::StartItemStayInBound   : {
                        in.read((char*)&settlement.startBound.x,sizeof(int));
                        in.read((char*)&settlement.startBound.y,sizeof(int));
                        in.read((char*)&settlement.startBound.width,sizeof(int));
                        in.read((char*)&settlement.startBound.height,sizeof(int));

                        in.read((char*)&settlement.startDuration,sizeof(int));
                        break;
                    }
                }

                in.read((char*)&settlement.endType,SETTLEMENT_ENDTYPE_SIZE);
                in.read((char*)&size,sizeof(size_t));
                settlement.endItems.resize(size);
                for(auto index = 0; index < size; index++) {
                    in.read((char*)&settlement.endItems[index],sizeof(ItemID));
                }
                switch(settlement.endType) {
                    case Settlement::EndItemDisappear : { break; }
                    case Settlement::EndItemAppearInBound :
                    case Settlement::EndItemDisappearInBound : {
                        in.read((char*)&settlement.endBound.x,sizeof(int));
                        in.read((char*)&settlement.endBound.y,sizeof(int));
                        in.read((char*)&settlement.endBound.width,sizeof(int));
                        in.read((char*)&settlement.endBound.height,sizeof(int));
                        break;
                    }
                    case Settlement::EndItemStayInBound : {
                        in.read((char*)&settlement.endBound.x,sizeof(int));
                        in.read((char*)&settlement.endBound.y,sizeof(int));
                        in.read((char*)&settlement.endBound.width,sizeof(int));
                        in.read((char*)&settlement.endBound.height,sizeof(int));

                        in.read((char*)&settlement.endDuration,sizeof(int));
                        break;
                    }
                }
            }
            else if(choiceStr == "\001") {
                return true;
            }
            else {
                in.close();
                LOG_ERROR(MissionReactorChan,"mission file bad format");
                return false;
            }
        }
        in.close();
        return true;
    }

#define JUDGE_APPEAR_START do {                                     \
    if(historyResult.size() >= analysisFrameCount){                 \
        if(flagCount > (long)(analysisFrameCount * startTolerance)){\
            settlement.status = Settlement::Start;                  \
            break;                                                  \
        }                                                           \
    }                                                               \
}while(0)

#define END_JUDGE_APPEAR_START do {                                 \
    if(flag) flagCount++;                                           \
    historyResult.push_back(flag);                                  \
    if(historyResult.size() > analysisFrameCount){                  \
        if(historyResult.front()) flagCount--;                      \
        historyResult.pop_front();                                  \
    }                                                               \
}while(0)

#define JUDGE_APPEAR_END do {                                       \
    if(historyResult.size() >= analysisFrameCount){                 \
        if(flagCount > (long)(analysisFrameCount * endTolerance)){  \
            settlement.status = Settlement::End;                    \
            break;                                                  \
        }                                                           \
    }                                                               \
}while(0)

#define END_JUDGE_APPEAR_END do {                                   \
    if(flag) flagCount++;                                           \
    historyResult.push_back(flag);                                  \
    if(historyResult.size() > analysisFrameCount){                  \
        if(historyResult.front()) flagCount--;                      \
        historyResult.pop_front();                                  \
    }                                                               \
}while(0)

    MissionReactor::Res MissionReactor::React(
            const MissionReactor::ReactInput &input
    ) {
        if(frameNo++ == analysisFrameCount) {
            auto duration =
            (double)std::chrono::duration_cast<std::chrono::milliseconds >(
                    std::chrono::system_clock::now() - frameClock
            ).count() / 1000;

            fps = (int)(analysisFrameCount / duration);
            frameNo = 0;
            frameClock = std::chrono::system_clock::now();
            LOG_TRACE(MissionReactorChan,"FPS:" + std::to_string(fps));
        }

        Res res {None,{}};

        if(finished.size() < workMissions.size()) finished.resize(workMissions.size(),false);
        switch(settlement.status) {
            case Settlement::Wait : {
                if(settlement.startItems[0] == -1) {
                    settlement.status = Settlement::Start;
                    break;
                }
                switch(settlement.startType) {
                    case Settlement::StartItemAppear : {
                        JUDGE_APPEAR_START;
                        bool flag = false;
                        for(const auto& itemId : settlement.startItems) {
                            flag = input.itemVecMap.find(itemId) != input.itemVecMap.end();
                            if(flag) break;
                        }
                        END_JUDGE_APPEAR_START;
                        break;
                    }
                    case Settlement::StartItemAppearInBound : {
                        JUDGE_APPEAR_START;
                        bool flag = false;
                        try {
                            const auto& items = input.itemVecMap.at(settlement.startItems[0]);
                            for(const auto& item : items ) {
                                flag = Item::Consist(settlement.startBound,item);
                                if(flag) break;
                            }
                        }catch (...){
                            flag = false;
                        }
                        END_JUDGE_APPEAR_START;
                        break;
                    }
                    case Settlement::StartItemStayInBound : {
                        if(historyResult.size() >= fps * settlement.startDuration) {
                            if(flagCount > (long)(fps * settlement.startDuration * startTolerance)){
                                settlement.status = Settlement::Status::Start;
                                break;
                            }
                        }
                        bool flag = false;
                        try {
                            auto items = input.itemVecMap.at(settlement.startItems[0]);
                            for(const auto& item : items ){
                                flag = Item::Consist(settlement.startBound,item);
                                if(flag) break;
                            }
                        }catch(...) {
                            flag = false;
                        }
                        if(flag) flagCount++;
                        if(historyResult.size() > fps * settlement.startDuration * 2) {
                            if(historyResult.front()) flagCount--;
                            historyResult.pop_front();
                        }
                        break;
                    }
                }
                break;
            }
            case Settlement::Start : {
                historyResult.clear();
                flagCount = 0;
                settlement.status = Settlement::Status::Running;
                startFrameNo = input.frameNo + 1;
                LOG_TRACE(MissionReactorChan,"开始帧号:" + std::to_string(startFrameNo));
                res.status = ResStatus::Start;
                break;
            }
            case Settlement::Running : {
                for(auto index = 0; index < workMissions.size(); index++) {
                    if (finished[index]) continue;
                    int ret = workMissions[index]->operator()(
                            input.itemVecMap,
                            fps,
                            mainMissionName,
                            signalMap
                    );
                    if (ret < 0) {
                        finished[index] = true;
                        runtimeProcess.emplace_back(workMissions[index]->index);
                        settlement.status = Settlement::End;
                    } else if (ret == 1) {
                        finished[index] = true;
                        runtimeProcess.emplace_back(workMissions[index]->index);
                        if (std::all_of(finished.begin(), finished.end(), [](bool b) { return b; })) {
                            settlement.status = Settlement::End;
                            break;
                        }
                    }
                }
                if(settlement.endItems[0] == -1) break;

                switch(settlement.endType) {
                    case Settlement::EndItemDisappear: {
                        JUDGE_APPEAR_END;
                        bool flag = true;
                        for(const auto& item : settlement.endItems) {
                            flag &= input.itemVecMap.find(item) == input.itemVecMap.end();
                        }
                        END_JUDGE_APPEAR_END;
                        break;
                    }
                    case Settlement::EndItemAppearInBound: {
                        JUDGE_APPEAR_END;
                        bool flag = true;
                        try {
                            auto items = input.itemVecMap.at(settlement.endItems[0]);
                            for(const auto& item : items ) {
                                flag = Item::Consist(settlement.endBound,item);
                                if(flag) break;
                            }
                        }catch (...) {
                            flag = false;
                        }
                        END_JUDGE_APPEAR_END;
                        break;
                    }
                    case Settlement::EndItemStayInBound: {
                        if(historyResult.size() >= fps * settlement.endDuration){
                            if(flagCount > (long)(fps * settlement.endDuration * endTolerance)){
                                settlement.status = Settlement::End;
                                break;
                            }
                        }
                        bool flag = false;
                        try {
                            auto items = input.itemVecMap.at(settlement.endItems[0]);
                            for(const auto& item : items ) {
                                flag = Item::Consist(settlement.endBound,item);
                                if(flag) break;
                            }
                        } catch(...) {
                            flag = false;
                        }
                        if(flag) flagCount++;
                        if(historyResult.size() > fps * settlement.endDuration * 2) {
                            if(historyResult.front()) flagCount--;
                            historyResult.pop_front();
                        }
                        break;
                    }
                    case Settlement::EndItemDisappearInBound: {
                        JUDGE_APPEAR_END;
                        bool flag = true;
                        try {
                            auto items = input.itemVecMap.at(settlement.endItems[0]);
                            for(const auto& item : items ) {
                                flag &= !Item::Consist(settlement.endBound,item);
                            }
                        }catch (...) {
                            flag = false;
                        }
                        END_JUDGE_APPEAR_END;
                        break;
                    }
                }
                break;
            }
            case Settlement::End : {
                LOG_WARN(MissionReactorChan,"结束帧号:" + std::to_string(input.frameNo));
                historyResult.clear();
                signalMap.clear();
                signalMap[0] = true;
                settlement.status = Settlement::Wait;
                verify(res);
                finished.clear();
                runtimeProcess.clear();
                return res;
            }
        }
        return res;
    }

    void MissionReactor::Clear() {
        fps = 0;
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
        for(const auto& mis : settlement.rightMissions) {
            if(std::find(runtimeProcess.begin(),runtimeProcess.end(),mis) == runtimeProcess.end()){
                res.status = NotSuccess;
                res.errorMissions.emplace_back(mis);
            }
            if(res.status == NotSuccess) return false;
        }
        res.errorMissions.clear();
        res.status = Success;
        return true;
    }

    void MissionReactor::registerItem(const std::string &name, ItemID id) {
        itemNameMap[id] = name;
        nameItemMap[name] = id;
    }

    Ptr<MissionInterface> MissionReactor::registerMission(const std::string &missionName) {
        auto mission = MissionFactory::Create(missionName);
        mission->index = (int)globalMissions.size();
        globalMissions.emplace_back(mission);
        nameMissionMap[mission->name] = mission;
        workMissions.emplace_back(mission);
        settlement.rightMissions.emplace_back(mission->index);
        return mission;
    }
} // hzd