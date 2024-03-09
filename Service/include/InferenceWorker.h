/**
  ******************************************************************************
  * @file           : InferenceWorker.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/26
  ******************************************************************************
  */

#ifndef ROBERT_INFERENCEWORKER_H
#define ROBERT_INFERENCEWORKER_H

#include "Configure.h"              /* Configure */
#include "Yolo.h"                   /* Yolo */
#include "CaptureStream.h"          /* CaptureStream */


namespace hzd {

    template<typename T>
    using Ptr = std::shared_ptr<T>;

    struct DetectionItem {
        unsigned int frameID{};
        std::vector<Detection>   detections{};
    };

    struct PoseItem {
        unsigned int frameID{};
        std::vector<Person>      persons{};
    };

    class InferenceWorker {
    public:
        Yolo                                    yolo;
        YoloConfigure                           yoloConfigure;
        Ptr<BlockChannel<DetectionItem>>        detectChan{};
        Ptr<BlockChannel<PoseItem>>             poseChan{};

        InferenceWorker(
                YoloConfigure&                          yoloConfigure,
                bool&                                   isStop,
                CaptureStream&                          captureStream,
                Ptr<BlockChannel<DetectionItem>>        detectChan = nullptr,
                Ptr<BlockChannel<PoseItem>>             poseChan = nullptr
        );
        static void Work(InferenceWorker*);
    private:
        Yolo::YoloVersion                       yoloVersion;
        bool&                                   isStop;
        CaptureStream&                          captureStream;
        unsigned int                            currentFrameCount{0};
    };

} // hzd

#endif //ROBERT_INFERENCEWORKER_H
