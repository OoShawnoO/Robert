/**
  ******************************************************************************
  * @file           : ThreadPool.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/24
  ******************************************************************************
  */

#ifndef ROBERT_THREADPOOL_H
#define ROBERT_THREADPOOL_H

#include <atomic>           /* atomic */
#include <vector>           /* vector */
#include "AsyncLogger.h"    /* AsyncLogger */
#include "Channel.h"        /* Channel */


namespace hzd {
    template<class T>
    class ThreadPool {
        using Ptr = std::shared_ptr<T>                          ;
        int threadCount                                         {8};
        int maxWorkCount                                        {20000};
        BlockChannel<Ptr> workChan                              ;
        std::atomic<bool> isStop                                {false};
        std::vector<std::shared_ptr<std::thread>> threads       {};
        static void work(ThreadPool<T>* arg){
            arg->run();
        }
        void run(){
            Ptr ptr;
            while(!isStop){
                if(!workChan.pop(ptr))  continue;
                ptr->Run();
            }
        }
    public:
        explicit ThreadPool(int _threadCount = 8,int _maxWorkCount = 8196)
                : threadCount(_threadCount),maxWorkCount(_maxWorkCount){
            if(threadCount <= 0 || maxWorkCount <= 0){
                LOG_ERROR("ThreadPool","thread count or max process count should >= 0");
                exit(-1);
            }
            for(int i=0;i<threadCount;i++){
                threads.emplace_back(std::make_shared<std::thread>(work,this));
            }
        }
        bool Add(Ptr& ptr){
            if(workChan.size() >= maxWorkCount){
                LOG_WARN("ThreadPool","thread pool overload");
                return false;
            }
            workChan.push(ptr);
            return true;
        }
    };

} // hzd

#endif //ROBERT_THREADPOOL_H
