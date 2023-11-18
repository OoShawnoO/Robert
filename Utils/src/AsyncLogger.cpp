/**
  ******************************************************************************
  * @file           : AsyncLogger.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/18
  ******************************************************************************
  */

#include <utility>              /* pair */
#include <cstring>              /* strlen strcat */
#include <iostream>             /* cerr cout endl */
#include <sstream>              /* stringstream */

#include "AsyncLogger.h"        /* AsyncLogger */

namespace hzd {
    const std::unordered_map<std::string,LogLevel> AsyncLogger::strLevelMap{
            {"TRACE",L_TRACE},
            {"DEBUG",L_DEBUG},
            {"INFO",L_INFO},
            {"WARN",L_WARN},
            {"ERROR",L_ERROR},
            {"FATAL",L_FATAL}
    };

    std::unordered_map<std::string,LogLevel> AsyncLogger::channelLevelMap;

    AsyncLogger::AsyncLogger(std::string _logSaveRootPath)
    : logSaveRootPath(std::move(_logSaveRootPath)){
        auto channels = Configure::Get()["Log"]["channel"];
        for(auto iter = channels.begin();iter != channels.end();++iter ) {
            channelLevelMap[iter.key()] = strLevelMap.at(iter.value());
        }
        logThread = std::thread(LogLoop,this);
    }

    AsyncLogger::~AsyncLogger() {
        isStop = true;
        logChan.push();
        logThread.join();
        if(fp) {
            if(strlen(writeBuffer) > 0) {
                fwrite(writeBuffer,strlen(writeBuffer),1,fp);
            }
            fclose(fp);
            fp = nullptr;
        }
    }

    void AsyncLogger::LogLoop(AsyncLogger* ptrLogger) {
        auto& logger = *ptrLogger;
        LogItem item{};
        if(!logger.logSaveRootPath.empty() && !logger.fp) {
            logger.fp = fopen(logger.logSaveRootPath.c_str(),"a+");
        }
        while(!logger.isStop) {
            if(!logger.logChan.pop(item)) {
                std::cerr << "[FATAL] pop error" << std::endl;
                exit(-1);
            }
            logger.writeLogItem(item);
        }
        while(!logger.logChan.empty()) {
            if(logger.logChan.pop(item)) {
                logger.writeLogItem(item);
            }
        }
    }

    void AsyncLogger::Log(
            LogLevel                level,
            const std::string&      channel,
            unsigned int            line,
            std::string             msg,
            const char*             filename,
            const char*             function
    ) {
        static AsyncLogger asyncLogger(Configure::Get()["Log"]["path"]);
        static time_t last = 0;
        static char timeStr[20];
        std::time_t current = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        LogItem item {channel,level,line,std::move(msg),filename,function};
        if(current - last >= 1) {
            std::strftime(timeStr,sizeof(timeStr),"%Y-%m-%d %H:%M:%S",std::localtime(&current));
            last = current;
            strcpy(item.time,timeStr);
        }else{
            strcpy(item.time,timeStr);
        }
        asyncLogger.logChan.push(std::move(item));
    }

    void AsyncLogger::writeLogItem(AsyncLogger::LogItem &item) {
        bzero(buffer,LOG_BUF_SIZE);
        if(item.msg.size() + strlen(item.filename) + strlen(item.function) > LOG_BUF_SIZE - 30) {
            std::cerr << "[WARN] single log data memory bigger than 4kb." << std::endl;
            std::stringstream stream;
            stream  << "[" << item.time << "]"
                    << "[ " << item.channel << " ]"
                    << "[ " << levelMap.at(item.level) << " ] "
                    << item.msg
                    << "[" << item.filename << ":"
                    << item.function << "]\n";
            if(fp) {
                fwrite(writeBuffer,strlen(writeBuffer),1,fp);
                bzero(writeBuffer,LOG_BUF_SIZE*10);
                writeCursor = 0;
                fwrite(stream.str().c_str(),stream.str().size(),1,fp);
            }
            std::cout << colorScheme.at(item.level) << stream.str() << CANCEL_COLOR_SCHEME;
            return;
        }
        sprintf(buffer,
                "[%s][ %s ][ %s ] %s [%s:%d:%s]\n",
                item.time,
                item.channel.c_str(),
                levelMap.at(item.level),
                item.msg.c_str(),
                item.filename,
                item.line,
                item.function);
        std::cout << colorScheme.at(item.level) << buffer << CANCEL_COLOR_SCHEME;
        fflush(stdout);
        if(fp) {
            strcat(writeBuffer,buffer);
            writeCursor += (int)strlen(buffer);
            if(writeCursor >= LOG_BUF_SIZE * 9){
                fwrite(writeBuffer,strlen(writeBuffer),1,fp);
                bzero(writeBuffer,LOG_BUF_SIZE * 10);
                writeCursor = 0;
            }
        }
    }

} // hzd