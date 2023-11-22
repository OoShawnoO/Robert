/**
  ******************************************************************************
  * @file           : MissionInterface.h
  * @author         : huzhida
  * @brief          : 定义任务接口
  * @date           : 2023/11/20
  ******************************************************************************
  */

#ifndef UTILS_MISSIONINTERFACE_H
#define UTILS_MISSIONINTERFACE_H

#include <cstdint>                  /* int32_t */
#include <string>                   /* string */
#include <vector>                   /* vector */
#include <deque>                    /* deque */
#include <unordered_map>            /* unordered_map */

#include "MissionReactor/Item.h"    /* Item */

namespace hzd {
    using SignalID = int32_t;
    using ItemVecMap = std::unordered_map<ItemID,std::vector<Item>>;
    using SignalMap = std::unordered_map<SignalID,bool>;
    using GlobalMissionIndex = unsigned int;

    struct MissionInterface {
        enum ParameterType {
            BOOL,INT32,DOUBLE
        };

        struct Parameter {
            union Data{
                bool        _bool;
                int32_t     _int32;
                double      _double;

                Data() = default;
                explicit Data(bool _b) : _bool(_b) {}
                explicit Data(int32_t _i32) : _int32(_i32) {}
                explicit Data(double _d) : _double(_d) {}
            }data{};
            ParameterType   type{BOOL};

            Parameter() = default;
            explicit Parameter(bool _bool) : data(_bool) { type = BOOL; }
            explicit Parameter(int32_t _int32) : data(_int32) { type = INT32; }
            explicit Parameter(double _double) : data(_double) { type = DOUBLE; }

            inline bool&    asBool()  {
                assert(type == BOOL);
                return data._bool;
            }
            inline int32_t& asInt32() {
                assert(type == INT32);
                return data._int32;
            }
            inline double& asDouble() {
                assert(type == DOUBLE);
                return data._double;
            }
        };

#define PARAMETER_SIZE sizeof(hzd::MissionInterface::Parameter)

        static unsigned int staticMissionCount;
        // 检测容忍度 / check tolerance
        const double tolerance = 0.6;

        int                             index{-1};
        int                             duration{0};
        int                             rightCount{0};
        int                             startSignalCount{0};
        int                             endSignalCount{0};
        bool                            isInit{false};
        std::string                     name{};
        std::vector<Parameter>          parameters{};
        ItemID                          itemA{-1};
        ItemID                          itemB{-1};
        Item                            bound{};
        bool                            anyoneTriggle{true};
        std::deque<bool>                records{};
        std::vector<SignalID>           startSignals{};
        std::vector<SignalID>           endSignals{};

        virtual int operator()(
                const ItemVecMap&       itemVecMap,
                int                     fps,
                const std::string&      rootMissionName,
                SignalMap&              signalMap
        ) = 0;

        MissionInterface(const MissionInterface&) = delete;
        MissionInterface& operator=(const MissionInterface&) = delete;

    protected:
        explicit MissionInterface(std::string name);
    };

    struct Settlement {
        enum StartType {
            StartItemAppear,
            StartItemAppearInBound,
            StartItemStayInBound,
        };
#define SETTLEMENT_STARTTYPE_SIZE sizeof(hzd::Settlement::StartType)
        enum EndType {
            EndItemDisappear,
            EndItemAppearInBound,
            EndItemStayInBound,
            EndItemDisappearInBound
        };
#define SETTLEMENT_ENDTYPE_SIZE sizeof(hzd::Settlement::EndType)
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

    };

} // hzd

#endif //UTILS_MISSIONINTERFACE_H
