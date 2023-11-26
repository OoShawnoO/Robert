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
    uniqueLock.lock();
    // 当还没有读到该帧时
    if(expectIndex - frontIndex > matQueue.size()) {
        // 当缓存满时等待
        if(matQueue.size() >= 64) {
            uniqueLock.unlock();
            fullSem.wait();
            uniqueLock.lock();
        }

        matQueue.emplace_back();
        readCountQueue.emplace_back(workerCount);
        if(!capture.read(matQueue.back())){
            LOG_ERROR(CaptureStreamChan,"read new mat failed");
            return false;
        }
        readCountQueue.back()--;
        // 由于Mat有引用计数，比较安全。
        if(readCountQueue.back() == 0){
            frontIndex = expectIndex;
            frame = std::move(matQueue.front());
            matQueue.pop_front();
            readCountQueue.pop_front();
        }else{
            frame = readCountQueue.back();
        }
    }
    // 当已经读到该帧
    else{
        readCountQueue[expectIndex - frontIndex]--;

        if(expectIndex == frontIndex
        && readCountQueue.front() == 0) {
            frontIndex++;
            frame = std::move(matQueue.front());
            matQueue.pop_front();
            readCountQueue.pop_front();
            if(matQueue.size() == 63)  fullSem.signal();
        }else{
            frame = matQueue[expectIndex - frontIndex];
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
