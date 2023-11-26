/**
  ******************************************************************************
  * @file           : MissionReactor.h
  * @author         : huzhida
  * @brief          : 任务流程监测控制
  * @date           : 2023/11/20
  ******************************************************************************
  */

#ifndef ROBERT_MISSIONREACTOR_H
#define ROBERT_MISSIONREACTOR_H

#include <chrono>                               /* chrono */
#include <vector>                               /* vector */

#include "AsyncLogger.h"                        /* AsyncLogger */
#include "MissionReactor/MissionFactory.h"      /* MissionFactory */
#include "MissionReactor/MissionPacker.h"       /* MissionPacker */
#include "MissionReactor/MissionInterface.h"    /* SignalMap */

namespace hzd {
    using FrameNo = unsigned int;
    template<typename T>
    using Ptr = std::shared_ptr<T>;

    class MissionReactor {
    public:
        struct ReactInput {
            FrameNo    frameNo;
            ItemVecMap itemVecMap;
        };

        enum ResStatus {
            None = 0,
            Success = 1,
            Stop = 2,
            Timeout = 3,
            NotSuccess = 4,
        };

        struct Res {
            ResStatus                          status;
            std::vector<GlobalMissionIndex> errorMissions;
        };

        int                                                     analysisFrameCount{};
        double                                                  startTolerance{};
        double                                                  endTolerance{};
        int                                                     saveResultCount{};
        Settlement                                              settlement;
        std::string                                             mainMissionName{};
        std::vector<Ptr<MissionInterface>>                      globalMissions{};
        std::vector<Ptr<MissionInterface>>                      workMissions{};
        std::unordered_map<std::string,ItemID>                  nameItemMap{};
        std::unordered_map<ItemID,std::string>                  itemNameMap{};
        std::unordered_map<std::string,Ptr<MissionInterface>>   nameMissionMap{};


        MissionReactor();
        void Clear();
        Res React(const ReactInput& input);
        bool LoadMission(
                const std::string& missionFilePath,
                int _analysisFrameCount = 10,
                double _startTolerance = 0.8,
                double _endTolerance = 0.9,
                int _saveResultCount = 8196
        );

    private:
        int                                     fps{};
        int                                     flagCount{0};
        FrameNo                                 frameNo{};
        FrameNo                                 startFrameNo{};
        SignalMap                               signalMap {{0,true}};
        std::deque<bool>                        historyResult{};
        std::vector<bool>                       finished{};
        std::vector<std::vector<int>>           results{};
        std::vector<GlobalMissionIndex>         runtimeProcess{};
        std::chrono::system_clock::time_point   frameClock{};

        bool verify(Res& res) const;
        void registerItem(const std::string& name,ItemID id);
        Ptr<MissionInterface> registerMission(const std::string& missionName);
    };

} // hzd

#endif //ROBERT_MISSIONREACTOR_H
