/**
  ******************************************************************************
  * @file           : MissionImpl.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/21
  ******************************************************************************
  */

#ifndef ROBERT_MISSIONIMPL_H
#define ROBERT_MISSIONIMPL_H

#include "MissionReactor/MissionInterface.h"        /* MissionInterface */

namespace hzd {

    /**
      * @brief 停留事件 (A必须包含B,或 A被Bound包含) / Stay event
      * @note
      * 1. 当传入Item A,B时,检测对象为A与B的停留事件 \n
      * 2. 当传入Item A,没有传入B且传入了边界，检测对象为A与边界 \n
      * 3. 当仅传入Item A时,检测对象为整个可视区域 \n
      * @note
      * 传入参数:                                \n
      * parameters[0] => 开始检测信号个数          \n
      * parameters[1] => 成功后发出信号个数         \n
      * parameters[2] => Item A id              \n
      * parameters[3] => Item B id              \n
      * parameters[4] = > 停留时间(ms)            \n
      * parameters[5~8] => Bound x,y,w,h        \n
      * parameters[9] => 是否任意个体都可以触发成功  \n
      * parameters( 9,9+parameters[0] ] 开始信号   \n
      * parameters( 9+parameters[0],9+parameters[0]+parameters[1] ] 结束信号 \n
      */
    struct StayMissionImpl : MissionInterface {
        StayMissionImpl();
        int operator()(
                const ItemVecMap& itemVecMap,
                int fps,
                const std::string& mainMissionName,
                SignalMap& signalMap
        ) override;
    };
    /**
      * @brief 接触事件 (A与B/Bound相交) / Contact event
      * @note
      * 1. 当传入Item A,B时,检测对象为A与B的接触事件 \n
      * 2. 当传入Item A,没有传入B且传入了边界，检测对象为A与边界 \n
      * 3. 当仅传入Item A时,检测对象为整个可视区域 \n
      * @note
      * 传入参数:                                \n
      * parameters[0] => 开始检测信号个数          \n
      * parameters[1] => 成功后发出信号个数         \n
      * parameters[2] => Item A id              \n
      * parameters[3] => Item B id              \n
      * parameters[4] = > 接触时间(ms)            \n
      * parameters[5~8] => Bound x,y,w,h        \n
      * parameters[9] => 是否任意个体都可以触发成功  \n
      * parameters( 9,9+parameters[0] ] 开始信号   \n
      * parameters( 9+parameters[0],9+parameters[0]+parameters[1] ] 结束信号 \n
      */
    struct ContactMissionImpl : MissionInterface {
        ContactMissionImpl();
        int operator()(
                const ItemVecMap& itemVecMap,
                int fps,
                const std::string& mainMissionName,
                SignalMap& signalMap
        ) override;
    };
    /**
      * @brief 距离事件 (A到B/Bound的中心距离,或A到Bound的中心距离) / Distance event
      * @note
      * 1. 当传入Item A,B时,检测对象为A与B的相距事件 \n
      * 2. 当传入Item A,没有传入B且传入了边界，检测对象为A与边界 \n
      * @note
      * 传入参数:                                \n
      * parameters[0] => 开始检测信号个数          \n
      * parameters[1] => 成功后发出信号个数         \n
      * parameters[2] => Item A id              \n
      * parameters[3] => Item B id              \n
      * parameters[4] => 保持距离区间的时间间隔      \n
      * parameters[5~8] => Bound x,y,w,h        \n
      * parameters[9] => 是否任意个体都可以触发成功  \n
      * parameters[10] => 最小距离阈值权值Ω 计算距离时使用 Ω * Item B.width \n
      * parameters[11] => 最大距离阈值权值Ω 计算距离时使用 Ω * Item B.width \n
      * parameters( 11,11+parameters[0] ] 开始信号   \n
      * parameters( 11+parameters[0],11+parameters[0]+parameters[1] ] 结束信号 \n
      *
      */
    struct DistanceMissionImpl : MissionInterface {
        double                          minThreshold{};
        double                          maxThreshold{};
        DistanceMissionImpl();
        int operator()(
                const ItemVecMap& itemVecMap,
                int fps,
                const std::string& mainMissionName,
                SignalMap& signalMap
        ) override;
    };
    /**
      * @brief 指定方向距离事件 (A到B/Bound的中心距离,或A到Bound的中心距离) / Distance event
      * @note
      * 1. 当传入Item A,B时,检测对象为A与B的相距事件 \n
      * 2. 当传入Item A,没有传入B且传入了边界，检测对象为A与边界 \n
      * @note
      * 传入参数:                                \n
      * parameters[0] => 开始检测信号个数          \n
      * parameters[1] => 成功后发出信号个数         \n
      * parameters[2] => Item A id              \n
      * parameters[3] => Item B id              \n
      * parameters[4] => 保持距离区间的时间间隔      \n
      * parameters[5~8] => Bound x,y,w,h        \n
      * parameters[9] => 是否任意个体都可以触发成功  \n
      * parameters[10] => 最小距离阈值权值Ω 计算距离时使用 Ω * Item B.width \n
      * parameters[11] => 最大距离阈值权值Ω 计算距离时使用 Ω * Item B.width \n
      * parameters[12~15] =>
      * 以物体A为中心到点{parameters[12],parameters[13]的向量
      * 顺时针扫至{parameters[14],parameters[15]}的向量空间上的距离才会触发成功 \n
      * parameters( 15,15+parameters[0] ] 开始信号   \n
      * parameters( 15+parameters[0],15+parameters[0]+parameters[1] ] 结束信号 \n
      */
    struct DirectedDistanceMissionImpl : MissionInterface {
        struct Vec {
            double x{0},y{0};
            static bool IsInArea(double x,double y,const Vec& vecI,const Vec& vecII);
            Vec() = default;
            Vec(double x, double y);
        };

        double                          minThreshold{};
        double                          maxThreshold{};
        Vec                             vecA;
        Vec                             vecB;

        DirectedDistanceMissionImpl();

        int operator()(
                const ItemVecMap& itemVecMap,
                int fps,
                const std::string& mainMissionName,
                SignalMap& signalMap
        ) override;
    };

} // hzd

#endif //ROBERT_MISSIONIMPL_H
