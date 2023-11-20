/**
  ******************************************************************************
  * @file           : Configure.h
  * @author         : huzhida
  * @brief          : 配置文件相关
  * @date           : 2023/11/18
  ******************************************************************************
  */

#ifndef UTILS_CONFIGURE_H
#define UTILS_CONFIGURE_H

#include <opencv2/core.hpp>     /* cv::Size */

#include "json.hpp"             /* nlohmann::json */

namespace hzd {

    using json = nlohmann::json;
    /**
      * @brief 配置文件工具 / Configure tool
      */
    class Configure {
    public:

        struct LogConfigure {
            // 日志文件保存根路径 / Log file root save path
            std::string                                 saveRootPath{};
            // 频道日志等级 / Channel log level
            std::unordered_map<std::string,std::string> channelLevel{};
        };

        struct YoloConfigure {
            // 权重文件路径 / Weight file path
            std::string                     weightFilePath{};
            // 是否为yolo-pose / Whether yolo-pose or not
            bool                            isPose{false};
            // yolo版本 / Yolo version
            int                             version{8};
            // 推理图像大小 / Inference image size
            cv::Size                        size{640,640};
            // 是否启用cuda / Using cuda or not
            bool                            cuda{false};
            // 使用cuda的设备索引 / Device index for using cuda
            int                             deviceId{0};
            // 置信度阈值 / Confidence threshold
            float                           confThreshold{0.4};
            // IOU阈值 / IOU threshold
            float                           iouThreshold{0.4};
            // 转换表 / Transport map
            std::unordered_map<int,int>     transport;
        };

        struct InterflowConfigure {
            // 是否为生产者   / Whether producer or not
            bool                            isProducer{false};
            // 是否使用Tcp / Whether tcp or not
            bool                            isTcp{true};
            // 共享内存键值 / Share memory key
            std::string                     shareKey{};
            // 生产者信号量键值 / Producer semaphore key
            std::string                     producerSemKey{};
            // 消费者信号量键值 / Consumer semaphore key
            std::string                     consumerSemKey{};
            // 本机地址 / Self address
            std::string                     myIpAddr{};
            // 本机端口 / Self port
            unsigned short                  myPort{0};
            // 目标地址 / Destination address
            std::string                     destIpAddr{};
            // 目标端口 / Destination port`
            unsigned short                  destPort{0};
        };

        LogConfigure                log;
        std::vector<YoloConfigure>  yolos;
        InterflowConfigure          interflow;

        /**
          * @brief 获取配置文件中的键名 / Get configure file's keys
          * @retval 配置文件中的键名组 / A series of keys of configure file
          */
        std::vector<std::string> Keys();
        /**
          * @brief 获取配置文件对应建的值 / Get the correspond value of configure file's key
          * @param key 需要获取的值的键名 / the key of correspond value you need
          * @retval 对应值 / correspond value
          */
        auto operator[](const std::string& key) -> json::value_type;
        /**
          * @brief 重新导入配置文件 / Reload configure file
          * @param configPath 配置文件路径 / Configure file path
          * @retval true 成功 / false 失败
          */
        bool Reload(const std::string& configPath);
        bool Reload(json& config);
        /**
          * @brief 获取全局单例配置文件对象 / Get global single configure object
          * @retval reference of global single configure object
          */
        static Configure& Get();
    private:
        json conf{};
        explicit Configure(const std::string& configPath);
        bool modify();
    };
} // hzd

#endif //UTILS_CONFIGURE_H
