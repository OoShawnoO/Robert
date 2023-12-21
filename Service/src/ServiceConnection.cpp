/**
  ******************************************************************************
  * @file           : ServiceConnection.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/24
  ******************************************************************************
  */

#include <opencv2/highgui.hpp>
#include <future>
#include <iostream>
#include "ServiceConnection.h"      /* ServiceConnection */

const std::string ServiceConnectionChan = "ServiceConnection";

int hzd::ServiceConnection::Init(
        const hzd::ConnectionInitParameter &connectionInitParameter
)
{
    return Connection::Init(connectionInitParameter);
}

int hzd::ServiceConnection::Close() {
    Clear();
    return Connection::Close();
}

hzd::Connection::CallbackReturnType hzd::ServiceConnection::ReadCallback() {
    std::string temp;
    if(TCPSocket::Recv(temp,sizeof(ControlPacket),false)<0){
        LOG_ERROR(ServiceConnectionChan,"recv ControlPacket failed");
        return FAILED;
    }
    controlPacket = *(ControlPacket*)temp.c_str();
    return Connection::ReadCallback();
}

#define ADD_BODYPART(bodyPart) do {                                     \
    input.itemVecMap[person.bodyPart.classId].emplace_back(cv::Rect{    \
    person.bodyPart.position.x - bodyPartSize / 2,                      \
    person.bodyPart.position.y - bodyPartSize / 2,                      \
    bodyPartSize,bodyPartSize                                           \
});                                                                     \
}while(0)

hzd::Connection::CallbackReturnType hzd::ServiceConnection::AfterReadCallback() {
    currentControl = controlPacket.control;
    switch(currentControl) {
        /**
         *  客户端开始: 发送Config包 -> 接收Config响应 -> 发送mission文件 -> 发送Start包 ->
         *            接收帧数据 ...
         *            [1] -> 发送 ACK[mark:Right/Timeout/Stop]
         *                  [mark:Right] => 接收帧数据 -> ...
         *                  [mark:Timeout] => 接收帧数据 -> ...
         *                  [mark:Stop] => 等待下一步 指令
         *            [2] -> 发送 Change包 -> 发送Config包 -> 循环开始状态
         *            [3] -> 发送 Wave包 -> 接收Wave响应 -> 关闭
         *
         *  服务端开始: 接收Config包 -> 解析Config包 -> 发送Config响应 -> 接收mission文件 ->
         *            接收Start包 -> 发送帧数据...
         *            [1] -> 接收 ACK[mark]
         *                  [mark:Right] => 继续发送帧数据 ...
         *                  [mark:Timeout] => 发送上一帧数据 ...
         *                  [mark:Stop] => 停止工作，等待下一步指令
         *            [2] -> 接收 Change包 -> 接收Config包 -> 循环开始状态
         *            [3] -> 接收 Wave包 -> 发送Wave包响应 -> 关闭本连接
         *
         */

        // 客户端返回
        case Ack: {
            switch (controlPacket.mark) {
                case Timeout : {
                    // 跳过获取新帧操作
                    return Connection::AfterReadCallback();
                }
                case Right : {
                    // 跳出switch -> 执行Start对应操作获取下一帧数据
                    break;
                }
            }
        }
        // 客户端选择开始
        case Work: {
            if(!captureStream.Read(frame,++currentFrameCount)){
                LOG_ERROR(ServiceConnectionChan,"read frame failed");
                return FAILED;
            }
            MissionReactor::ReactInput input;
            input.frameNo = currentFrameCount;
            for(auto& worker : workers) {
                if(worker.yoloConfigure.isPose) {
                    int bodyPartSize = worker.yoloConfigure.bodyPartSize;
                    poseItems.emplace_back();
                    worker.poseChan->pop(poseItems.back());
                    if(poseItems.back().frameID != currentFrameCount){
                        LOG_ERROR(ServiceConnectionChan,"bad sequence");
                        return FAILED;
                    }
                    worker.yolo.PaintPersons(frame,poseItems.back().persons);
                    for(const auto& person : poseItems.back().persons) {
                        ADD_BODYPART(nose);
                        ADD_BODYPART(leftEye);
                        ADD_BODYPART(rightEye);
                        ADD_BODYPART(leftEar);
                        ADD_BODYPART(rightEar);
                        ADD_BODYPART(leftShoulder);
                        ADD_BODYPART(rightShoulder);
                        ADD_BODYPART(leftElbow);
                        ADD_BODYPART(rightElbow);
                        ADD_BODYPART(leftWrist);
                        ADD_BODYPART(rightWrist);
                        ADD_BODYPART(leftHip);
                        ADD_BODYPART(rightHip);
                        ADD_BODYPART(leftKnee);
                        ADD_BODYPART(rightKnee);
                        ADD_BODYPART(leftAnkle);
                        ADD_BODYPART(rightAnkle);
                    }
                }else{
                    detectionItems.emplace_back();
                    worker.detectChan->pop(detectionItems.back());
                    if(detectionItems.back().frameID != currentFrameCount){
                        LOG_ERROR(ServiceConnectionChan,"bad sequence");
                        return FAILED;
                    }
                    worker.yolo.PaintDetections(frame,detectionItems.back().detections);
                    for(auto& detection : detectionItems.back().detections){
                        auto iter = worker.yoloConfigure.transport.find(detection.classId);
                        if(iter != worker.yoloConfigure.transport.end()){
                            detection.classId = iter->second;
                        }
                        input.itemVecMap[detection.classId].emplace_back(detection.box);
                    }
                }
            }
            result = missionReactor.React(input);
            break;
        }
        // 客户端选择配置
        case Config : {
            // recv config json
            std::string temp;
            if(!TCPSocket::RecvWithHeader(temp, false)) {
                LOG_ERROR(ServiceConnectionChan,"recv config failed");
                return FAILED;
            }
            auto config = json::parse(temp);
            if(config.empty() || !Configure::Get().Reload(config)){
                LOG_ERROR(ServiceConnectionChan,"config file parse failed");
                return FAILED;
            }

            Clear();

            // load config json
            if(!interflowConfigure.Load(Configure::Get())){
                if(TCPSocket::Send("no",2) < 0) {
                    LOG_ERROR(ServiceConnectionChan,"send config response failed");
                }
                LOG_ERROR(ServiceConnectionChan,"config file have no Interflow item");
                return FAILED;
            }
            else if(!missionReactorConfigure.Load(Configure::Get())) {
                if(TCPSocket::Send("no",2) < 0) {
                    LOG_ERROR(ServiceConnectionChan,"send config response failed");
                }
                LOG_ERROR(ServiceConnectionChan,"config file have no MissionReactor item");
                return FAILED;
            }
            else if(!captureStreamConfigure.Load(Configure::Get())){
                if(TCPSocket::Send("no",2) < 0) {
                    LOG_ERROR(ServiceConnectionChan,"send config response failed");
                }
                LOG_ERROR(ServiceConnectionChan,"config file have no CaptureStream item");
                return FAILED;
            }
            else{
                if(TCPSocket::Send("ok",2) < 0) {
                    LOG_ERROR(ServiceConnectionChan,"send config response failed");
                    return FAILED;
                }
                if(TCPSocket::RecvFileWithHeader(missionReactorConfigure.missionFilePath) < 0){
                    if(TCPSocket::Send("no",2) < 0) {
                        LOG_ERROR(ServiceConnectionChan,"send mission file response failed");
                    }
                    LOG_ERROR(ServiceConnectionChan,"recv mission file failed");
                    return FAILED;
                }

                if(!missionReactor.LoadMission(
                        missionReactorConfigure.missionFilePath,
                        missionReactorConfigure.analysisFrameCount,
                        missionReactorConfigure.startTolerance,
                        missionReactorConfigure.endTolerance,
                        missionReactorConfigure.saveResultCount
                )){
                    if(TCPSocket::Send("no",2) < 0) {
                        LOG_ERROR(ServiceConnectionChan,"send mission file response failed");
                    }
                    LOG_ERROR(ServiceConnectionChan,"mission file not found");
                    return FAILED;
                }
                if(TCPSocket::Send("ok",2) < 0) {
                    LOG_ERROR(ServiceConnectionChan,"send config response failed");
                    return FAILED;
                }
            }
            ptrInterflow = std::make_shared<Interflow>(
                    interflowConfigure.isProducer,
                    interflowConfigure.isTcp,
                    interflowConfigure.myIpAddr,
                    interflowConfigure.myPort,
                    interflowConfigure.destIpAddr,
                    interflowConfigure.destPort,
                    interflowConfigure.shareKey,
                    interflowConfigure.producerSemKey,
                    interflowConfigure.consumerSemKey
            );

            auto yoloConfigures = YoloConfigure::Load(Configure::Get());

            if(captureStreamConfigure.captureIndex != -1){
                if(!captureStream.Open(captureStreamConfigure.captureIndex,yoloConfigures.size() + 1)){
                    LOG_ERROR(ServiceConnectionChan,"open capture index failed");
                }
            }else{
                if(!captureStream.Open(captureStreamConfigure.captureUrl,yoloConfigures.size() + 1)){
                    LOG_ERROR(ServiceConnectionChan,"open capture url failed");
                }
            }

            for(auto& yoloConfigure : yoloConfigures) {
                AddWorker(yoloConfigure);
            }
            break;
        }
        // 客户端选择关闭
        case Wave: {
            ptrInterflow->NotifyEnd();
            ptrInterflow.reset();
            Clear();
            break;
        }
    }
    return Connection::AfterReadCallback();
}

hzd::Connection::CallbackReturnType hzd::ServiceConnection::WriteCallback() {
    switch(controlPacket.control){
        case Ack:
        case Work: {
            json resultJson;
            resultJson["code"] = (int)result.status;
            for(const auto& index : result.errorMissions){
                const auto& mission = *missionReactor.globalMissions[index];
                std::string reason =
                        "[任务未完成] 任务名:" + mission.name.substr(0,mission.name.size()-1) + "\n"
                        + "ItemA:" + missionReactor.itemNameMap[mission.itemA] + "\n";
                if(mission.itemB != -1) {
                    reason += "ItemB:" + missionReactor.itemNameMap[mission.itemB] + "\n";
                }else{
                    reason += "Bound:{" + std::to_string(mission.bound.x) + ","
                            + std::to_string(mission.bound.y) + ","
                            + std::to_string(mission.bound.width) + ","
                            + std::to_string(mission.bound.height) + "}\n";
                }
                resultJson["failedReason"].emplace_back(std::move(reason));
            }
            if(!ptrInterflow->SendItem(frame,resultJson)){
                LOG_ERROR(ServiceConnectionChan,std::string{"send item failed,error:"} + strerror(errno));
                return FAILED;
            }
            break;
        }
        default: break;
    }
    return Connection::WriteCallback();
}

hzd::Connection::CallbackReturnType hzd::ServiceConnection::AfterWriteCallback() {
    return Connection::AfterWriteCallback();
}

void hzd::ServiceConnection::AddWorker(hzd::YoloConfigure &yoloConfigure) {
    if(yoloConfigure.isPose){
        workers.emplace_back(
                yoloConfigure,
                 isStop,
                 captureStream,
                 nullptr,
                 std::make_shared<BlockChannel<PoseItem>>()
        );
        threads.emplace_back(InferenceWorker::Work,&workers.back());
    }else{
        workers.emplace_back(
                yoloConfigure,
                isStop,
                captureStream,
                std::make_shared<BlockChannel<DetectionItem>>(),
                nullptr
        );
        threads.emplace_back(InferenceWorker::Work,&workers.back());
    }

}

void hzd::ServiceConnection::Clear() {
    if(captureStream.isOpened()){
        isStop = true;
        currentFrameCount = 0;
        if(ptrInterflow.get()) {
            ptrInterflow->Close();
            ptrInterflow.reset();
        }
        captureStream.Release();
        for(auto& thread : threads) {
            thread.join();
        }
        workers.clear();
        threads.clear();
        detectionItems.clear();
        poseItems.clear();
    }
    isStop = false;
}

