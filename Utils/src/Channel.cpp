/**
  ******************************************************************************
  * @file           : Channel.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/18
  ******************************************************************************
  */

#include "Channel.h"    /* Channel */

namespace hzd{

    Semaphore::Semaphore(long _count) : count(_count) {}

    void Semaphore::signal(){
        std::unique_lock<std::mutex> unique(mtx);
        ++count;
        if (count <= 0) cond.notify_one();
    }


    void Semaphore::wait() {
        std::unique_lock<std::mutex> unique(mtx);
        --count;
        if (count < 0)  cond.wait(unique);
    }

    void Semaphore::signalAll() {
        std::unique_lock<std::mutex> unique(mtx);
        ++count;
        if (count <= 0) cond.notify_all();
    }

}
