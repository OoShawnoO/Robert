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

#include <chrono>   /* chrono */
#include <vector>   /* vector */

#include "MissionInterface.h"   /* SignalMap */

namespace hzd {
    using FrameNo = unsigned int;
    template<typename T>
    using Ptr = std::shared_ptr<T>;

    class MissionReactor {
    public:
        enum ResStatus {
            None,
            Timeout
        };

        struct ReactInput {
            FrameNo    frameNo;
            ItemVecMap itemVecMap;
        };

        struct Res {
            ResStatus                          status;
            std::vector<GlobalMissionIndex> errorMissions;
        };

        struct Settlement {
            enum StartType {
                StartItemAppear,
                StartItemAppearInBound,
                StartItemStayInBound,
            };
#define SETTLEMENT_STARTTYPE_SIZE sizeof(hzd::MissionReactor::Settlement::StartType);
            enum EndType {
                EndItemDisappear,
                EndItemAppearInBound,
                EndItemStayInBound,
                EndItemDisappearInBound
            };
#define SETTLEMENT_ENDTYPE_SIZE sizeof(hzd::MissionReactor::Settlement::EndType);
            enum Status {
                Wait,
                Start,
                Running,
                End
            };
            Status                          status{Wait};
            int                             startDuration{0};
            int                             endDuration{0};
            Item                            startBound{};
            Item                            endBound{};
            StartType                       startType{StartItemAppear};
            EndType                         endType{EndItemDisappear};
            std::vector<ItemID>             startItems{};
            std::vector<ItemID>             endItems{};
            std::vector<GlobalMissionIndex> rightMissions{};

        }settlement;


        int                                                     analysisFrameCount{};
        double                                                  startTolerance{};
        double                                                  endTolerance{};
        int                                                     saveResultCount{};
        std::string                                             mainMissionName{};
        std::vector<Ptr<MissionInterface>>                      globalMissions{};
        std::vector<Ptr<MissionInterface>>                      workMissions{};
        std::unordered_map<std::string,ItemID>                  nameItemMap{};
        std::unordered_map<ItemID,std::string>                  itemNameMap{};
        std::unordered_map<std::string,Ptr<MissionInterface>>   nameMissionMap{};


        MissionReactor();
        bool LoadMission(const std::string& missionFilePath);
        Res React(const ReactInput& input);
        void Clear();

    private:
        int                                     fps{};
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
