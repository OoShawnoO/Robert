/**
  ******************************************************************************
  * @file           : CaptureStream.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/26
  ******************************************************************************
  */

#include <iostream>
#include "CaptureStream.h"      /* CaptureStream */
#include "AsyncLogger.h"        /* AsyncLogger */

const std::string CaptureStreamChan = "CaptureStream";

hzd::CaptureStream::~CaptureStream() {
    if(capture.isOpened()) {
        capture.release();
    }
}

bool hzd::CaptureStream::Open(const std::string &url, unsigned int _workerCount) {
    if(capture.isOpened()) {
        capture.release();
        matQueue.clear();
    }
    workerCount = _workerCount;
    return capture.open(url);
}

bool hzd::CaptureStream::Open(int captureIndex, unsigned int _workerCount) {
    if(capture.isOpened()) {
        fullSem.signal();
        capture.release();
        matQueue.clear();
    }
    workerCount = _workerCount;
    return capture.open(captureIndex);
}

bool hzd::CaptureStream::Read(cv::Mat &frame, unsigned int expectIndex) {
BLOCK_REIN:
    std::unique_lock<std::mutex> uniqueLock(readMutex);
    // 当还没有读到该帧时
    if(matQueue.empty() || expectIndex > matQueue.back().index) {
        // 当缓存满时等待
        if(matQueue.size() >= 256) {
            uniqueLock.unlock();
            fullSem.wait();
            if(!capture.isOpened()) return false;
            goto BLOCK_REIN;
        }
        matQueue.emplace_back();
        matQueue.back().index = expectIndex;
        matQueue.back().readCount = workerCount - 1;
        if(!capture.read(matQueue.back().mat)){
            matQueue.pop_back();
            LOG_ERROR(CaptureStreamChan,"read new mat failed");
            return false;
        }
        // 由于Mat有引用计数，比较安全。
        if(matQueue.front().readCount == 0){
            frame = std::move(matQueue.front().mat);
            matQueue.pop_front();
        }else{
            frame = matQueue.back().mat;
        }
    }
    // 当已经读过该帧
    else{
        auto iter = std::find_if(matQueue.begin(),matQueue.end(),[=](const _Mat& _mat){
           return _mat.index == expectIndex;
        });
        if(iter == matQueue.end())  {
            LOG_ERROR(CaptureStreamChan,"read new mat failed");
            return false;
        }
        --iter->readCount;
        frame = iter->mat;
        bool flag = false;
        if(matQueue.size() >= 256) flag = true;
        while(!matQueue.empty() && matQueue.front().readCount <= 0) {
            matQueue.pop_front();
        }
        if(flag && matQueue.size() < 256) fullSem.signalAll();
    }
    return true;

}

bool hzd::CaptureStream::isOpened() const {
    return capture.isOpened();
}

void hzd::CaptureStream::Release() {
    if(capture.isOpened()){
        fullSem.signal();
        capture.release();
        workerCount = 0;
        matQueue.clear();
    }
}
