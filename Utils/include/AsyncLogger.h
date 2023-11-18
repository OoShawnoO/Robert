/**
  ******************************************************************************
  * @file           : AsyncLogger.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/18
  ******************************************************************************
  */

#ifndef UTILS_ASYNCLOGGER_H
#define UTILS_ASYNCLOGGER_H

#include <string>           /* string */
#include <thread>           /* thread */
#include <unordered_map>    /* unordered_map */

#include "Configure.h"      /* Configure */
#include "Channel.h"        /* BlockChannel */

namespace hzd {

#define CANCEL_COLOR_SCHEME "\033[0m"
#define LOG_BUF_SIZE 4096

    enum LogLevel {
        L_TRACE,    /* 跟踪 */
        L_DEBUG,    /* 调试 */
        L_INFO,     /* 信息 */
        L_WARN,     /* 警告 */
        L_ERROR,    /* 错误 */
        L_FATAL     /* 致命错误 */
    };

    /**
      * @brief 异步日志工具 / Async log tool
      */
    class AsyncLogger {
    public:
        ~AsyncLogger();
        const static std::unordered_map<std::string,LogLevel> strLevelMap;
        static std::unordered_map<std::string,LogLevel> channelLevelMap;
        /**
          * @brief 日志工作循环 / Log loop
          * @param AsyncLogger* a ptr of global single configure object
          * @retval None
          */
        static void LogLoop(AsyncLogger*);
        /**
          * @brief 提交日志 / Submit log
          * @param level 日志等级 / log level
          * @param channel 日志分类 / log channel
          * @param msg 日志内容 / content of log
          * @param line 日志所在行 / line of log
          * @param filename 日志所在文件 / file of log
          * @param function 日志所在函数 / function of log
          * @retval None
          */
        static void Log(
                LogLevel                level,
                const std::string&      channel,
                unsigned int            line,
                std::string             msg,
                const char*             filename,
                const char*             function
        );
    private:
        struct LogItem {
            // 日志分类 / log channel
            std::string         channel;
            // 日志等级 / log level
            LogLevel            level;
            // 日志所在行 / line of log
            unsigned int        line;
            // 日志内容 / content of log
            std::string         msg;
            // 日志所在文件 / file name of log
            const char*         filename;
            // 日志所在函数 / function of log
            const char*         function;
            // 日志时间 / time of log
            char                time[20];
        };

        const std::unordered_map<LogLevel,const char*> levelMap {
                {L_TRACE,"TRACE"},
                {L_DEBUG,"DEBUG"},
                {L_INFO,"INFO"},
                {L_WARN,"WARN"},
                {L_ERROR,"ERROR"},
                {L_FATAL,"FATAL"}
        };

        const std::unordered_map<LogLevel,const char*> colorScheme {
                {L_TRACE,"\033[36m"},
                {L_DEBUG,"\033[34m"},
                {L_INFO,"\033[32m"},
                {L_WARN,"\033[33m"},
                {L_ERROR,"\033[31m"},
                {L_FATAL,"\033[31m"}
        };

        std::string                     logSaveRootPath = "log";
        char                            buffer[LOG_BUF_SIZE] = {0};
        char                            writeBuffer[LOG_BUF_SIZE * 10] = {0};
        int                             writeCursor{0};
        FILE*                           fp{nullptr};
        BlockChannel<LogItem>           logChan{};
        std::thread                     logThread{};
        bool                            isStop{false};

        explicit AsyncLogger(std::string  logSaveRootPath = "log");
        void writeLogItem(LogItem& item);
    };

#define PRIVATE_LOG(level,channel,msg) do {                                         \
    if(level >= hzd::AsyncLogger::channelLevelMap[channel]){                        \
        hzd::AsyncLogger::Log(level,channel,__LINE__,msg,__FILE__,__FUNCTION__);    \
    }\
}while(0)

#ifndef NO_AYSNC_LOG
#define LOG_TRACE(channel,msg)  PRIVATE_LOG(hzd::LogLevel::L_TRACE,channel,msg)
#define LOG_DEBUG(channel,msg)  PRIVATE_LOG(hzd::LogLevel::L_DEBUG,channel,msg)
#define LOG_INFO(channel,msg)   PRIVATE_LOG(hzd::LogLevel::L_INFO,channel,msg)
#define LOG_WARN(channel,msg)   PRIVATE_LOG(hzd::LogLevel::L_WARN,channel,msg)
#define LOG_ERROR(channel,msg)  PRIVATE_LOG(hzd::LogLevel::L_ERROR,channel,msg)
#define LOG_FATAL(channel,msg)  PRIVATE_LOG(hzd::LogLevel::L_FATAL,channel,msg)
#else
#define LOG_TRACE(channel,msg)
#define LOG_DEBUG(channel,msg)
#define LOG_INFO(channel,msg)
#define LOG_WARN(channel,msg)
#define LOG_ERROR(channel,msg)
#define LOG_FATAL(channel,msg)
#endif

} // hzd

#endif //UTILS_ASYNCLOGGER_H
