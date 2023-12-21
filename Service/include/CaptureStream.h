/**
  ******************************************************************************
  * @file           : CaptureStream.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/26
  ******************************************************************************
  */

#ifndef ROBERT_CAPTURESTREAM_H
#define ROBERT_CAPTURESTREAM_H

#include <queue>                    /* deque */
#include <opencv2/videoio.hpp>      /* VideoCapture */

#include "Channel.h"                /* Channel */

namespace hzd {

    class CaptureStream {
    public:
        CaptureStream() = default;
        ~CaptureStream();
        /**
          * @brief 通过url打开摄像头或视频 / Open the camera or video through the URL
          * @param url 摄像头/视频URL
          * @param workerCount 使用该视频流的工作者个数
          * @retval true 打开成功 / false 打开失败
          */
        bool Open(const std::string& url,unsigned int workerCount);
        /**
          * @brief 通过url打开摄像头索引 / Open the camera through index
          * @param url 摄像头索引
          * @param workerCount 使用该视频流的工作者个数
          * @retval true 打开成功 / false 打开失败
          */
        bool Open(int captureIndex,unsigned int workerCount);
        /**
          * @brief 读取视频帧 / Read video frame
          * @param frame 需要被读取到的Mat
          * @param expectIndex 期待获取的帧号
          * @retval true 读取成功 / false 读取失败
          */
        bool Read(cv::Mat& frame,unsigned int expectIndex);
        /**
          * @brief 视频流对象是否打开 / Is the video stream object open
          * @retval true 已经打开 / false 没有打开
          */
        bool isOpened() const;

        void Release();
    private:
        struct _Mat {
            unsigned int index;
            cv::Mat mat;
        };
        cv::VideoCapture            capture{};
        Semaphore                   fullSem{0};
        unsigned int                workerCount{0};
        unsigned int                frontIndex{0};
        std::mutex                  readMutex{};
        std::deque<_Mat>         matQueue{};
        std::deque<unsigned int>    readCountQueue{};

    };

} // hzd

#endif //ROBERT_CAPTURESTREAM_H
