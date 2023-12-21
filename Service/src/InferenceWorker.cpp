/**
  ******************************************************************************
  * @file           : InferenceWorker.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/26
  ******************************************************************************
  */

#include "InferenceWorker.h"        /* InferenceWorker */
#include "AsyncLogger.h"            /* AsyncLogger */

#include <utility>

namespace hzd {

    const std::string InferenceWorkerChan = "InferenceWorker";

    InferenceWorker::InferenceWorker(
            YoloConfigure                       &_yoloConfigure,
            bool                                &_isStop,
            CaptureStream                       &_captureStream,
            Ptr<BlockChannel<DetectionItem>>    _detectChan,
            Ptr<BlockChannel<PoseItem>>         _poseChan
    ):
    yoloConfigure(_yoloConfigure),
    isStop(_isStop),
    captureStream(_captureStream),
    detectChan(std::move(_detectChan)),
    poseChan(std::move(_poseChan))
    {
        if(yoloConfigure.isPose) {
            if(yoloConfigure.version == 8){
                yoloVersion = Yolo::YoloVersion::PoseV8;
            }
            else if(yoloConfigure.version == 5) {
                yoloVersion = Yolo::YoloVersion::PoseV5;
            }
        }
        else{
            if(yoloConfigure.version == 8){
                yoloVersion = Yolo::YoloVersion::DetectV8;
            }
            else if(yoloConfigure.version == 5) {
                yoloVersion = Yolo::YoloVersion::DetectV5;
            }
        }
        if(!yolo.Reload(
                yoloConfigure.weightFilePath,
                yoloVersion,
                yoloConfigure.size,
                yoloConfigure.cuda,
                yoloConfigure.deviceId,
                yoloConfigure.confThreshold,
                yoloConfigure.iouThreshold
        )){
            LOG_ERROR(InferenceWorkerChan,"reload yolo object failed");
            exit(-1);
        }
    }

    void InferenceWorker::Work(InferenceWorker* _worker) {
        auto& worker = *_worker;
        while(!worker.isStop) {
            cv::Mat frame;
            if(!worker.captureStream.Read(frame,++worker.currentFrameCount)){
                LOG_ERROR(InferenceWorkerChan,"read frame failed");
                return;
            }

            switch (worker.yoloVersion) {
                case Yolo::DetectV8 :
                case Yolo::DetectV5 : {
                    DetectionItem item;
                    item.frameID = worker.currentFrameCount;
                    if(!worker.yolo.Detect(frame,item.detections)){
                        LOG_ERROR(InferenceWorkerChan,"yolo detect failed");
                        return;
                    }
                    worker.detectChan->push(std::move(item));
                    break;
                }
                case Yolo::PoseV8 :
                case Yolo::PoseV5 : {
                    PoseItem item;
                    item.frameID = worker.currentFrameCount;
                    if(!worker.yolo.Pose(frame,item.persons)){
                        LOG_ERROR(InferenceWorkerChan,"yolo pose failed");
                        return;
                    }
                    worker.poseChan->push(std::move(item));
                    break;
                }

            }
        }
    }
} // hzd