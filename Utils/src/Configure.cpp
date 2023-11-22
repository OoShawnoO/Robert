/**
  ******************************************************************************
  * @file           : Configure.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/18
  ******************************************************************************
  */

#include <fstream>      /* ifstream */
#include <iostream>     /* cerr */

#include "Configure.h"  /* Configure*/

namespace hzd {

    bool Configure::isInit = true;

    std::vector<std::string> Configure::Keys() {
        std::vector<std::string> k;
        for(auto it=conf.begin();it != conf.end();it++){
            k.emplace_back(it.key());
        }
        return k;
    }

    auto Configure::operator[](const std::string &key) const -> json::value_type {
        return conf.at(key);
    }

    Configure::Configure(const std::string &configPath) {
        std::ifstream in(configPath);
        if(!in.is_open()) {
            std::cerr << "[FATAL] bad configure file path." << std::endl;
            exit(-1);
        }
        conf = json::parse(in);
        in.close();
    }

    bool Configure::Reload(const std::string &configPath) {
        std::ifstream in(configPath);
        if(!in.is_open()) {
            return false;
        }
        conf = json::parse(in);
        in.close();
        return true;
    }

    bool Configure::Reload(json &config) {
        conf = config;
        return true;
    }

    Configure &Configure::Get(const std::string &filePath) {
        static Configure configure("../etc/config.json");
        return configure;
    }

    bool LogConfigure::Load(const Configure &conf) {
        try{
            channelLevel = conf["Log"]["channels"];
            saveRootPath = conf["Log"]["path"];
            return true;
        }catch(...){
            return false;
        }
    }

    std::vector<YoloConfigure> YoloConfigure::Load(const Configure &conf) {
        std::vector<YoloConfigure> yolos;
        for(auto yolo : conf["Yolo"]) {
            YoloConfigure yoloConf;

            yoloConf.weightFilePath = yolo["weightFilePath"];
            yoloConf.version = yolo["version"];
            yoloConf.isPose = yolo["isPose"];
            yoloConf.cuda = yolo["cuda"];
            yoloConf.deviceId = yolo["deviceId"];
            yoloConf.size = {yolo["size"][0],yolo["size"][1]};
            yoloConf.confThreshold = yolo["confThreshold"];
            yoloConf.iouThreshold = yolo["iouThreshold"];
            auto _transport = yolo["transport"];
            for(auto item = _transport.begin();item != _transport.end(); item++)
            {
                yoloConf.transport[stoi(item.key())] = stoi(std::string(item.value()));
            }
            yolos.emplace_back(yoloConf);
        }
        return yolos;
    }

    bool InterflowConfigure::Load(const Configure &conf) {
        try{
            isProducer = conf["Interflow"]["isProducer"];
            isTcp = conf["Interflow"]["isTcp"];
            shareKey = conf["Interflow"]["shareKey"];
            producerSemKey = conf["Interflow"]["producerSemKey"];
            consumerSemKey = conf["Interflow"]["consumerSemKey"];
            myIpAddr = conf["Interflow"]["myIpAddr"];
            myPort = conf["Interflow"]["myPort"];
            destIpAddr = conf["Interflow"]["destIpAddr"];
            destPort = conf["Interflow"]["destPort"];
            return true;
        }catch(...){
            return false;
        }
    }
} // hzd
