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
        // 客户端选择开始
        case Work: {
            if(!captureStream.Read(frame,++currentFrameCount)){
                LOG_ERROR(ServiceConnectionChan,"read frame failed");
                result.status = MissionReactor::ResStatus::End;
                return SUCCESS;
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
            if(!missionReactor.settlement.startBound.empty()){
                cv::rectangle(frame,missionReactor.settlement.startBound,cv::Scalar(0,0,123),2);
            }
            if(!missionReactor.settlement.endBound.empty()){
                cv::rectangle(frame,missionReactor.settlement.endBound,cv::Scalar(0,0,123),2);
            }
            for(const auto& pMission : missionReactor.workMissions) {
                if(pMission->bound.empty()) continue;
                cv::rectangle(frame,pMission->bound,cv::Scalar(0,0,123),2);
            }
            result = missionReactor.React(input);
            break;
        }
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
        // 客户端选择配置
        case Config : {
            Clear();
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
                remove(missionReactorConfigure.missionFilePath.c_str());
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
            Close();
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
            if(result.status != MissionReactor::ResStatus::None){
                auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                auto* time = localtime(&t);
                char timeStr[20] = {0};
                strftime(timeStr,20,"%Y-%m-%d %H:%M:%S",time);
                std::string timeStdStr = timeStr;
                resultJson["time"] = timeStdStr;
            }
            resultJson["procedure"] = missionReactor.mainMissionName;
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
            // 发送数据包
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
        missionReactor.Clear();
    }
    isStop = false;
}

