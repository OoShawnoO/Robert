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

    std::vector<std::string> Configure::Keys() {
        std::vector<std::string> k;
        for(auto it=conf.begin();it != conf.end();it++){
            k.emplace_back(it.key());
        }
        return k;
    }

    auto Configure::operator[](const std::string &key) -> json::value_type {
        return conf[key];
    }

    Configure &Configure::Get() {
        static Configure config("../etc/config.json");
        return config;
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
        return modify();
    }

    bool Configure::Reload(json &config) {
        conf = config;
        return modify();
    }

    bool Configure::modify() {
        log.channelLevel = conf["Log"]["channels"];
        log.saveRootPath = conf["Log"]["path"];

        interflow.isProducer = conf["Interflow"]["isProducer"];
        interflow.isTcp = conf["Interflow"]["isTcp"];
        interflow.shareKey = conf["Interflow"]["shareKey"];
        interflow.producerSemKey = conf["Interflow"]["producerSemKey"];
        interflow.consumerSemKey = conf["Interflow"]["consumerSemKey"];
        interflow.myIpAddr = conf["Interflow"]["myIpAddr"];
        interflow.myPort = conf["Interflow"]["myPort"];
        interflow.destIpAddr = conf["Interflow"]["destIpAddr"];
        interflow.destPort = conf["Interflow"]["destPort"];

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
            auto transport = yolo["transport"];
            for(auto item = transport.begin();item != transport.end(); item++)
            {
                yoloConf.transport[stoi(item.key())] = stoi(std::string(item.value()));
            }
            yolos.emplace_back(yoloConf);
        }

        return true;
    }

} // hzd
