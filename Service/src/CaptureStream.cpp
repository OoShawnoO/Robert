/**
  ******************************************************************************
  * @file           : CaptureStream.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/26
  ******************************************************************************
  */

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
        readCountQueue.clear();
    }
    workerCount = _workerCount;
    return capture.open(url);
}

bool hzd::CaptureStream::Open(int captureIndex, unsigned int _workerCount) {
    if(capture.isOpened()) {
        fullSem.signal();
        capture.release();
        matQueue.clear();
        readCountQueue.clear();
    }
    workerCount = _workerCount;
    return capture.open(captureIndex);
}

bool hzd::CaptureStream::Read(cv::Mat &frame, unsigned int expectIndex) {
    std::unique_lock<std::mutex> uniqueLock(readMutex);
    // 当还没有读到该帧时
    if(expectIndex - matQueue.front().index >= matQueue.size()) {
        // 当缓存满时等待
        if(matQueue.size() >= 64) {
            uniqueLock.unlock();
            fullSem.wait();
            uniqueLock.lock();
        }

        matQueue.emplace_back();
        readCountQueue.emplace_back(workerCount-1);
        matQueue.back().index = expectIndex;
        if(!capture.read(matQueue.back().mat)){
            LOG_ERROR(CaptureStreamChan,"read new mat failed");
            return false;
        }
        // 由于Mat有引用计数，比较安全。
        if(readCountQueue.front() == 0){
            frame = std::move(matQueue.front().mat);
            matQueue.pop_front();
            readCountQueue.pop_front();
        }else{
            frame = matQueue.back().mat;
        }
    }
    // 当已经读过该帧
    else{
        readCountQueue[expectIndex - matQueue.front().index]--;

        if(expectIndex == matQueue.front().index
        && readCountQueue.front() == 0) {
            frontIndex++;
            frame = std::move(matQueue.front().mat);
            matQueue.pop_front();
            readCountQueue.pop_front();
            if(matQueue.size() == 63)  fullSem.signal();
        }else{
            frame = matQueue[expectIndex - matQueue.front().index].mat;
        }
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
        matQueue.clear();
        readCountQueue.clear();
    }
}
