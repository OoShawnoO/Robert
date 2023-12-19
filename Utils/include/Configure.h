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
        static bool isInit;

        /**
         * @brief 全局单例配置文件 / Get global single configure object
         * @retval reference of single configure object
         */
        static Configure& Get(const std::string& filePath = "../etc/config.json");

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
        auto operator[](const std::string& key) const -> json::value_type;

        /**
          * @brief 重新导入配置文件 / Reload configure file
          * @param configPath 配置文件路径 / Configure file path
          * @retval true 成功 / false 失败
          */
        bool Reload(json& config);
        bool Reload(const std::string& configPath);

    private:
        json conf{};

        Configure() = default;
        explicit Configure(const std::string& configPath);
    };

    /**
     * @brief 服务器相关配置 / Server configure
     */
    struct ServerConfigure {
        // IP地址 / IP Address
        std::string ip{};
        // 端口 / Port
        unsigned short port{};
        // epoll超时时间 / Epoll timeout duration
        int epollTimeout{};
        // Reactor数量 / Reactor count
        int reactorCount{1};
        // 是否使用边沿触发 / Edge trigger or not
        bool isEdgeTrigger{false};
        // 是否使用One shot / One shot or not
        bool isOneShot{true};
        // 是否非阻塞 / None block or not
        bool isNonblock{true};
        // 放行IP列表 / Allow Ip list
        std::vector<std::string> allow{};
        // 禁止IP列表 / Deny Ip list
        std::vector<std::string> deny{};

        bool Load(const Configure& configure);
    };
    /**
     * @brief 日志相关配置 / Log configure
     */
    struct LogConfigure {
        // 日志文件保存根路径 / Log file root save path
        std::string                                 saveRootPath{};
        // 频道日志等级 / Channel log level
        std::unordered_map<std::string,std::string> channelLevel{};

        bool Load(const Configure& configure);
    };
    /**
     * @brief Yolo相关配置 / Yolo configure
     */
    struct YoloConfigure {
        // 名称 / Name
        std::string                     name{};
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
        // 身体部位物理大小 / Bodypart hit-box size
        int                             bodyPartSize{20};
        static std::vector<YoloConfigure> Load(const Configure& configure);
    };
    /**
     * @brief 交互相关配置 / Interflow configure
     */
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

        bool Load(const Configure& configure);
    };
    /**
      * @brief MissionReactor相关配置 / MissionReactor configure
      */
    struct MissionReactorConfigure {
        // 二进制任务文件路径 / Binary mission file path
        std::string                     missionFilePath{};
        // 分析帧数 / Frame count for analysis
        int                             analysisFrameCount{10};
        // 开始结算容忍度 / Start settlement tolerance
        double                          startTolerance{0.8};
        // 结束结算容忍度 / End settlement tolerance
        double                          endTolerance{0.9};
        // 保存历史结果数量 / Count of saving-result
        int                             saveResultCount{8192};

        bool Load(const Configure& configure);
    };

    /**
     * @brief Package of all configure part
     */
     struct ConfigurePackage {
         std::string                    name{"新方案"};
         std::string                    description{"新方案"};
         ServerConfigure                serverConfigure{};
         LogConfigure                   logConfigure{};
         std::vector<YoloConfigure>     yoloConfigures{};
         InterflowConfigure             interflowConfigure{};
         MissionReactorConfigure        missionReactorConfigure{};

         bool Deserialize(const std::string& path);
         void Serialize(const std::string& path);
         std::string toJson();
     };
} // hzd

#endif //UTILS_CONFIGURE_H
