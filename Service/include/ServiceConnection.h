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

#include "Connection.h"     /* Connection */
#include "Interflow.h"      /* Interflow */

namespace hzd {

    template<typename T>
    using Ptr = std::shared_ptr<T>;

    /**
      * @brief 负责摄像头管理、视频流处理、推理服务及流程纠错等 / Responsible for camera management, video stream processing, inference services, and process error correction, etc
      */
    struct ServiceConnection : public Connection {
        bool                        isInterflowInit{false};
        ControlType                 currentControl;
        ControlPacket               controlPacket{};
        InterflowConfigure          interflowConfigure;
        Ptr<Interflow>              ptrInterflow{nullptr};


        int Init(const ConnectionInitParameter &connectionInitParameter) override;

        int Close() override;

    protected:
        CallbackReturnType ReadCallback() override;

        CallbackReturnType AfterReadCallback() override;

        CallbackReturnType WriteCallback() override;

        CallbackReturnType AfterWriteCallback() override;
    };
    
} // hzd


#endif //ROBERT_SERVICECONNECTION_H
