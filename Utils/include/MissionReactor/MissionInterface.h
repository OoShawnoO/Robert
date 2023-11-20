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
            explicit Parameter(bool _bool) : data(_bool) {}
            explicit Parameter(int32_t _int32) : data(_int32) {}
            explicit Parameter(double _double) : data(_double) {}

            inline bool&    asBool()  { return data._bool; }
            inline int32_t& asInt32() { return data._int32; }
            inline double& asDouble() { return data._double; }
        };

        static unsigned int staticMissionCount;

        int                             index{-1};
        bool                            isInit{false};
        std::string                     name{};
        std::vector<Parameter>          parameters{};
        ItemID                          itemI{-1};
        ItemID                          itemII{-1};
        Item                            bound{};
        bool                            anyoneTriggle{true};
        std::deque<bool>                records{};
        SignalID                        startSignal{0};
        SignalID                        endSignal{0};

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

} // hzd

#endif //UTILS_MISSIONINTERFACE_H
