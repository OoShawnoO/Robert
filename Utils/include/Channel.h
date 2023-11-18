/**
  ******************************************************************************
  * @file           : Channel.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/18
  ******************************************************************************
  */

#ifndef ROBERT_CHANNEL_H
#define ROBERT_CHANNEL_H

#include <deque>                /* deque */
#include <mutex>                /* mutex */
#include <condition_variable>   /* condition variable */

namespace hzd {
    struct Semaphore {
        explicit Semaphore(long count = 0);
        /**
          * @brief 信号量自增一 / semaphore plus 1
          * @retval None
          */
        void signal();
        /**
          * @brief 信号量小于一将阻塞,否则自减一 / block if semaphore less than 1,else semaphore sub 1
          * @retval None
          */
        void wait();
    private:
        long count;
        std::mutex mtx;
        std::condition_variable cond;
    };

    template<class T>
    struct Channel {
        /**
          * @brief 元素入队，动态分配内存将转移所有权 / Element queuing, dynamically allocating memory will transfer ownership
          * @param itemRRef 模板对象右值引用 / Template element right reference object
          * @retval None
          */
        void push(T&& itemRRef) {
            std::lock_guard<std::mutex> guard(mtx);
            data.emplace_back(std::move(itemRRef));
        }
        /**
          * @brief 元素入队 / Element queuing
          * @param itemRef 模板对象引用 / Template element reference object
          * @retval None
          */
        void push(T& itemRef) {
            std::lock_guard<std::mutex> guard(mtx);
            data.emplace_back(itemRef);
        }
        /**
          * @brief 元素出队 / Element dequeue
          * @param destRef 目的模板对象引用 / Target template object reference
          * @retval None
          */
        bool pop(T& destRef) {
            std::lock_guard<std::mutex> guard(mtx);
            if(data.empty()) return false;
            destRef = std::move(data.front());
            data.pop_front();
            return true;
        }
        /**
          * @brief 判空 / Judging container empty
          * @retval true 空 / false 非空
          */
        bool empty() {
            std::lock_guard<std::mutex> guard(mtx);
            return data.empty();
        }
        /**
          * @brief 容器内元素个数 / Amount of container elements
          * @retval 元素个数 / amount of element
          */
        size_t size() {
            std::lock_guard<std::mutex> guard(mtx);
            return data.size();
        }

    private:
        std::mutex mtx;
        std::deque<T> data;
    };

    template<class T>
    struct BlockChannel {
        /**
          * @brief 解除阻塞 / unblock
          * @retval None
          */
        void push() {
            sem.signal();
        }
        /**
          * @brief 元素入队，动态分配内存将转移所有权 / Element queuing, dynamically allocating memory will transfer ownership
          * @param itemRRef 模板对象右值引用 / Template element right reference object
          * @retval None
          */
        void push(T&& itemRRef) {
            chan.push(std::forward<T&&>(itemRRef));
            sem.signal();
        }
        /**
          * @brief 元素入队 / Element queuing
          * @param itemRef 模板对象引用 / Template element reference object
          * @retval None
          */
        void push(T& itemRef) {
            chan.push(std::forward<T&>(itemRef));
            sem.signal();
        }
        /**
          * @brief 元素出队,当容器为空时将阻塞 / Element dequeue,Block if container empty
          * @param destRef 目的模板对象引用 / Target template object reference
          * @retval None
          */
        bool pop(T& destRef) {
            sem.wait();
            return chan.pop(destRef);
        }
        /**
          * @brief 判空 / Judging container empty
          * @retval true 空 / false 非空
          */
        bool empty() {
            return chan.empty();
        }
        /**
          * @brief 容器内元素个数 / Amount of container elements
          * @retval 元素个数 / amount of element
          */
        size_t size() {
            return chan.size();
        }
    private:
        Semaphore sem;
        Channel<T> chan;
    };
} // hzd

#endif //ROBERT_CHANNEL_H
