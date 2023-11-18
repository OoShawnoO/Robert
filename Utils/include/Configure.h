/**
  ******************************************************************************
  * @file           : Configure.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/18
  ******************************************************************************
  */

#ifndef UTILS_CONFIGURE_H
#define UTILS_CONFIGURE_H

#include "json.hpp"     /* nlohmann::json */

namespace hzd {

    using json = nlohmann::json;

    class Configure {
    public:
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
        void Reload(json& config);
        /**
          * @brief 获取全局单例配置文件对象 / Get global single configure object
          * @retval reference of global single configure object
          */
        static Configure& Get();
    private:
        json conf{};
        Configure(const std::string& configPath);
    };
} // hzd

#endif //UTILS_CONFIGURE_H
