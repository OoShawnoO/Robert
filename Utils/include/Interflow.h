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
        // 确认成功收到
        Ack,
        // Interflow握手
        Handshake,
        // 开始传输
        Start,
        // 暂停传输
        Stop,
        // 切换配置
        Change,
        // Interflow挥手
        Wave
    };

    enum MarkType {
        // 正确接收
        Right,
        // 图像等待超时
        MatTimeout,
        // 结果等待超时
        ResultTimeout
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
            // 帧号 / frame id
            size_t frameID;
            // 压缩后大小 / zipped size
            size_t size;
            // 行数 / row count
            int rows;
            // 列数 / col count
            int cols;
            // 通道数 / channel count
            int channels;
        };
        /**
          * @brief 激活消费者信号量 / Post consumer semaphore
          * @note 只有在使用共享内存时需要 / Just use when using share memory
          * @retval None
          */
        void NotifyEnd();
        /**
          * @brief 发送cv::Mat / Send cv::Mat
          * @param mat 需要发送的Mat / the Mat which need to send
          * @note 当初始化时使用TCP时，将使用TCP进行传输，否则将使用共享内存
          * @retval true 成功 / false 失败
          */
        bool SendMat(const cv::Mat& mat);
        /**
          * @brief 接收cv::Mat / Receive cv::Mat
          * @param mat 用于保存接收的Mat / use for save received Mat
          * @note 当初始化时使用TCP时，将使用TCP进行传输，否则将使用共享内存
          * @retval true 成功 / false 失败
          */
        bool ReceiveMat(cv::Mat& mat);
        /**
          * @brief 发送json数据 / Send json data
          * @param json 需要发送的json / the json which need to send
          * @note 当初始化时使用TCP时，将使用TCP进行传输，否则将使用UDP
          * @retval true 成功 / false 失败
          */
        bool SendJson(nlohmann::json& json);
        /**
          * @brief 接收json数据 / Receive json data
          * @param json 用于保存接收的json / use for save received json
          * @note 当初始化时使用TCP时，将使用TCP进行传输，否则将使用UDP
          * @retval true 成功 / false 失败
          */
        bool ReceiveJson(nlohmann::json& json);

    private:
        bool                        isInit{false};
        bool                        isTcp;
        ShareMat*                   shareMatPtr;
        ShareMemory                 shareMemory;
        TcpMatProp                  tcpMatProp{};
        std::vector<unsigned char>  buffer;
        std::vector<int>            encodeParams;
        UDPSocket                   udp;
        TCPSocket                   tcp;

        // udp 发送json数据 / udp Send json data
        bool UdpSendJson(nlohmann::json& json);
        // udp 接收json数据 / udp Receive json data
        bool UdpReceiveJson(nlohmann::json& json);
        // tcp 发送json数据 / tcp Send json data
        bool TcpSendJson(nlohmann::json& json);
        // tcp 接收json数据 / tcp Receive json data
        bool TcpReceiveJson(nlohmann::json& json);
        // 使用TCP发送cv::Mat / Using tcp send cv::Mat
        bool TcpSendMat(const cv::Mat& mat);
        // 使用TCP接收cv::Mat / Using tcp receive cv::Mat
        bool TcpReceiveMat(cv::Mat& mat);
        // 使用共享内存发送cv::Mat / Using share memory send cv::Mat
        bool ShareMemorySendMat(const cv::Mat& mat);
        // 使用共享内存接收cv::Mat / Using share memory receive cv::Mat
        bool ShareMemoryReceiveMat(cv::Mat& mat);
    };
} // hzd

#endif //UTILS_INTERFLOW_H
