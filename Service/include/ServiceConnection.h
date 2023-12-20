/**
  ******************************************************************************
  * @file           : ServiceConnection.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/24
  ******************************************************************************
  */

#ifndef ROBERT_SERVICECONNECTION_H
#define ROBERT_SERVICECONNECTION_H

#include "Connection.h"                     /* Connection */
#include "Interflow.h"                      /* Interflow */
#include "InferenceWorker.h"                /* InferenceWorker */
#include "MissionReactor/MissionReactor.h"  /* MissionReactor*/

namespace hzd {

    template<typename T>
    using Ptr = std::shared_ptr<T>;

    /**
      * @brief 负责摄像头管理、视频流处理、推理服务及流程纠错等 / Responsible for camera management, video stream processing, inference services, and process error correction, etc
      */
    struct ServiceConnection : public Connection {
        ControlType                 currentControl;
        ControlPacket               controlPacket{};
        InterflowConfigure          interflowConfigure;
        Ptr<Interflow>              ptrInterflow{nullptr};

        CaptureStreamConfigure      captureStreamConfigure{};
        CaptureStream               captureStream{};
        cv::Mat                     frame{};
        unsigned int                currentFrameCount{0};

        MissionReactor::Res         result;
        MissionReactor              missionReactor;
        MissionReactorConfigure     missionReactorConfigure;

        std::vector<DetectionItem>  detectionItems{};
        std::vector<PoseItem>       poseItems{};

        bool                        isStop{false};
        std::deque<InferenceWorker> workers{};
        std::deque<std::thread>     threads{};


        int Init(const ConnectionInitParameter &connectionInitParameter) override;

        int Close() override;

    protected:
        CallbackReturnType ReadCallback() override;

        CallbackReturnType AfterReadCallback() override;

        CallbackReturnType WriteCallback() override;

        CallbackReturnType AfterWriteCallback() override;
    private:
        void AddWorker(YoloConfigure& yoloConfigure);
        void Clear();
    };
    
} // hzd


#endif //ROBERT_SERVICECONNECTION_H
