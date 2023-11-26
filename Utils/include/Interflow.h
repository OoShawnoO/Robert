/**
  ******************************************************************************
  * @file           : Interflow.h
  * @author         : huzhida
  * @brief          : 通用交互工具类库
  * @date           : 2023/11/18
  ******************************************************************************
  */

#ifndef UTILS_INTERFLOW_H
#define UTILS_INTERFLOW_H


#include <semaphore.h>      /* sem_t */
#include <opencv2/core.hpp> /* Mat */

#include "Socket.h"         /* Socket */

namespace hzd {
    /**
     * @brief 控制类型 / Control type
     */
    enum ControlType {
        // 配置
        Config,
        // 确认成功收到
        Ack,
        // 开始工作
        Start,
        // 切换配置
        Change,
        // Interflow挥手
        Wave
    };

    enum MarkType {
        // 正确接收
        Right,
        // 暂停
        Stop,
        // 超时
        Timeout
    };

    struct ControlPacket {
        // 帧号
        size_t      frameID;
        // 操作类型
        ControlType control;
        // 标记
        MarkType    mark;
    };


    /**
      * @brief 共享内存 / Share memory
      */
    class ShareMemory {
    public:
        ShareMemory(
                bool               isProducer,
                const std::string& shareKey,
                long               capacity,
                const std::string& producerSemKey,
                const std::string& consumerSemKey
        );
        ~ShareMemory();
        /**
          * @brief 等待生产者信号量 / Wait producer semaphore
          * @retval None
          */
        inline void WaitProducerSem() { sem_wait(producerSem); }
        /**
          * @brief 等待消费者信号量 / Wait consumer semaphore
          * @retval None
          */
        inline void WaitConsumerSem() { sem_wait(consumerSem); }
        /**
          * @brief 激活生产者信号量 / Post producer semaphore
          * @retval None
          */
        inline void PostProducerSem() { sem_post(producerSem); }
        /**
          * @brief 激活消费者信号量 / Post consumer semaphore
          * @retval None
          */
        inline void PostConsumerSem() { sem_post(consumerSem); }
        /**
          * @brief 尝试等待生产者信号量 / Try to wait producer semaphore
          * @retval None
          */
        inline bool TryWaitProducerSem() {
            return producerSem && sem_trywait(producerSem) == 0;
        }
        /**
          * @brief 尝试等待消费者信号量 / Try to wait consumer semaphore
          * @retval None
          */
        inline bool TryWaitConsumerSem() {
            return consumerSem && sem_trywait(consumerSem) == 0;
        };
        /**
          * @brief 获取共享内存地址 / Get share memory address
          * @retval None
          */
        unsigned char* GetShareMemory();
        /**
          * @brief 清空共享内存 / Clean share memory
          * @retval None
          */
        void Clear();
    private:
        bool            isProducer{false};
        int             shareId{-1};
        std::string     shareKey{};
        unsigned char*  sharePtr{nullptr};
        long            shareCapacity;
        std::string     producerSemKey{};
        sem_t*          producerSem{nullptr};
        std::string     consumerSemKey{};
        sem_t*          consumerSem{nullptr};
    };

    /**
      * @brief 集成交互工具 / Integrated Interflow tool
      */
    class Interflow {
    public:
        Interflow() = default;

        Interflow(
            bool               isProducer,
            bool               isTcp,
            const std::string& myIpAddr,
            unsigned short     myPort,
            const std::string& destIpAddr = "",
            unsigned short     destPort = 0,
            const std::string& shareKey = "",
            const std::string& producerSemKey = "",
            const std::string& consumerSemKey = ""
        );

#define MAX_SHARE_MAT_SIZE (1920*1080*3 + 4)
        struct ShareMat {
            // 行数 / row count
            int rows;
            // 列数 / col count
            int cols;
            // 通道数 / channel count
            int channels;
            // 存储空间 / memory space
            unsigned char data[MAX_SHARE_MAT_SIZE];
        };

        struct TcpMatProp {
            // 压缩后大小 / zipped size
            size_t size;
            // 行数 / row count
            int rows;
            // 列数 / col count
            int cols;
            // 通道数 / channel count
            int channels;
        };

        struct CacheQueueItem {
            size_t                      frameID{};
            TcpMatProp                  tcpMatProp{};
            std::vector<unsigned char>  matBuffer{};
            size_t                      resultSize{};
            std::string                 result{};

            CacheQueueItem() = default;
            CacheQueueItem(CacheQueueItem&&) noexcept ;
            CacheQueueItem& operator=(CacheQueueItem&&) noexcept;
        };

        struct CacheQueue {
            void EmplaceBack(CacheQueueItem&& item);
            CacheQueueItem& Adjust(size_t frameID);

        private:
            std::deque<CacheQueueItem>  queue{};
        };

        /**
          * @brief 发送Mat + json / Send mat and json
          * @param mat 需要发送的Mat / the Mat which need to send
          * @param json 需要发送的json / the json which need to send
          * @retval true 成功 / false 失败
          */
        bool SendItem(const cv::Mat& mat,const nlohmann::json& json);
        /**
          * @brief 接收Mat + json / Receive mat and json
          * @param mat 用于保存接收的Mat / use for save received Mat
          * @param json 用于保存接收的json / use for save received json
          * @retval true 成功 / false 失败
          */
        bool ReceiveItem(cv::Mat& mat,nlohmann::json& json);
        /**
          * @brief 激活消费者信号量 / Post consumer semaphore
          * @note 只有在使用共享内存时需要 / Just use when using share memory
          * @retval None
          */
        void NotifyEnd();

    private:
        bool                        isInit{false};
        bool                        isTcp;
        ShareMat*                   shareMatPtr;
        ShareMemory                 shareMemory;
        CacheQueue                  cacheQueue;
        CacheQueueItem              cacheQueueItem;
        std::vector<int>            encodeParams;
        UDPSocket                   udp;
        TCPSocket                   tcp;

        // tcp 发送Mat + json / tcp Send mat + json data
        bool TcpSendItem(const cv::Mat& mat,const json& json);
        // tcp 接收Mat + json / tcp Receive mat + json data
        bool TcpReceiveItem(cv::Mat& mat,json& json);
        // udp 发送json数据 / udp Send json data
        bool UdpSendJson(const nlohmann::json& json);
        // udp 接收json数据 / udp Receive json data
        bool UdpReceiveJson(nlohmann::json& json);
        // 使用共享内存发送cv::Mat / Using share memory send cv::Mat
        bool ShareMemorySendMat(const cv::Mat& mat);
        // 使用共享内存接收cv::Mat / Using share memory receive cv::Mat
        bool ShareMemoryReceiveMat(cv::Mat& mat);
    };
} // hzd

#endif //UTILS_INTERFLOW_H
