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
        static Configure configure(filePath);
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
            yoloConf.bodyPartSize = yolo["bodyPartSize"];
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

    bool ServerConfigure::Load(const Configure &conf) {
        try {
            ip = conf["Server"]["ip"];
            port = conf["Server"]["port"];
            epollTimeout = conf["Server"]["epollTimeout"];
            reactorCount = conf["Server"]["reactorCount"];
            isEdgeTrigger = conf["Server"]["isEdgeTrigger"];
            isOneShot = conf["Server"]["isOneshot"];
            isNonblock = conf["Server"]["isNonblock"];
            for(const auto& allowItem : conf["Server"]["allow"]) allow.emplace_back(allowItem);
            for(const auto& denyItem : conf["Server"]["deny"]) deny.emplace_back(denyItem);
        }catch(...) {
            return false;
        }
        return true;
    }

    bool MissionReactorConfigure::Load(const Configure &conf) {
        try {
            missionFilePath = conf["MissionReactor"]["missionFilePath"];
            analysisFrameCount = conf["MissionReactor"]["analysisFrameCount"];
            startTolerance = conf["MissionReactor"]["startTolerance"];
            endTolerance = conf["MissionReactor"]["endTolerance"];
            saveResultCount = conf["MissionReactor"]["saveResultCount"];
        }catch(...){
            return false;
        }
        return true;
    }

    bool CaptureStreamConfigure::Load(const Configure &conf) {
        try {
            captureIndex = conf["CaptureStream"]["index"];
            captureUrl = conf["CaptureStream"]["url"];
        }catch(...) {
            return false;
        }
        return true;
    }

    void ConfigurePackage::Serialize(const std::string& path) {
        std::ofstream out(path,std::ios::out);
        auto jsonStr = toJson();
        out.write(jsonStr.c_str(),jsonStr.size());
        out.close();
    }

    bool ConfigurePackage::Deserialize(const std::string &path) {
        std::ifstream in(path);
        json conf;
        try{
            conf = json::parse(in);
            in.close();
            name = conf["Name"];
            description = conf["Description"];
            logConfigure.channelLevel = conf["Log"]["channels"];
            logConfigure.saveRootPath = conf["Log"]["path"];
            interflowConfigure.isProducer = conf["Interflow"]["isProducer"];
            interflowConfigure.isTcp = conf["Interflow"]["isTcp"];
            interflowConfigure.shareKey = conf["Interflow"]["shareKey"];
            interflowConfigure.producerSemKey = conf["Interflow"]["producerSemKey"];
            interflowConfigure.consumerSemKey = conf["Interflow"]["consumerSemKey"];
            interflowConfigure.myIpAddr = conf["Interflow"]["myIpAddr"];
            interflowConfigure.myPort = conf["Interflow"]["myPort"];
            interflowConfigure.destIpAddr = conf["Interflow"]["destIpAddr"];
            interflowConfigure.destPort = conf["Interflow"]["destPort"];
            serverConfigure.ip = conf["Server"]["ip"];
            serverConfigure.port = conf["Server"]["port"];
            serverConfigure.epollTimeout = conf["Server"]["epollTimeout"];
            serverConfigure.reactorCount = conf["Server"]["reactorCount"];
            serverConfigure.isEdgeTrigger = conf["Server"]["isEdgeTrigger"];
            serverConfigure.isOneShot = conf["Server"]["isOneshot"];
            serverConfigure.isNonblock = conf["Server"]["isNonblock"];
            for(const auto& allowItem : conf["Server"]["allow"]) serverConfigure.allow.emplace_back(allowItem);
            for(const auto& denyItem : conf["Server"]["deny"]) serverConfigure.deny.emplace_back(denyItem);
            missionReactorConfigure.missionFilePath = conf["MissionReactor"]["missionFilePath"];
            missionReactorConfigure.analysisFrameCount = conf["MissionReactor"]["analysisFrameCount"];
            missionReactorConfigure.startTolerance = conf["MissionReactor"]["startTolerance"];
            missionReactorConfigure.endTolerance = conf["MissionReactor"]["endTolerance"];
            missionReactorConfigure.saveResultCount = conf["MissionReactor"]["saveResultCount"];
            for(auto yolo : conf["Yolo"]) {
                YoloConfigure yoloConf;
                yoloConf.name = yolo["name"];
                yoloConf.weightFilePath = yolo["weightFilePath"];
                yoloConf.version = yolo["version"];
                yoloConf.isPose = yolo["isPose"];
                yoloConf.cuda = yolo["cuda"];
                yoloConf.deviceId = yolo["deviceId"];
                yoloConf.size = {yolo["size"][0],yolo["size"][1]};
                yoloConf.confThreshold = yolo["confThreshold"];
                yoloConf.iouThreshold = yolo["iouThreshold"];
                yoloConf.bodyPartSize = yolo["bodyPartSize"];
                auto _transport = yolo["transport"];
                for(auto item = _transport.begin();item != _transport.end(); item++)
                {
                    yoloConf.transport[stoi(item.key())] = stoi(std::string(item.value()));
                }
                yoloConfigures.emplace_back(yoloConf);
            }
            captureStreamConfigure.captureIndex = conf["CaptureStream"]["index"];
            captureStreamConfigure.captureUrl = conf["CaptureStream"]["url"];
        }catch(...){
            in.close();
            return false;
        }

        return true;
    }

    std::string ConfigurePackage::toJson() {
        json conf;
        conf["Name"] = name;
        conf["Description"] = description;
        conf["Log"]["path"] = logConfigure.saveRootPath;
        conf["Log"]["channels"] = {{"MAIN", "INFO"}};
        conf["Server"]["ip"] = serverConfigure.ip;
        conf["Server"]["port"] = serverConfigure.port;
        conf["Server"]["epollTimeout"] = serverConfigure.epollTimeout;
        conf["Server"]["reactorCount"] = serverConfigure.reactorCount;
        conf["Server"]["isEdgeTrigger"] = serverConfigure.isEdgeTrigger;
        conf["Server"]["isOneshot"] = serverConfigure.isOneShot;
        conf["Server"]["isNonblock"] = serverConfigure.isNonblock;
        conf["Server"]["allow"] = json::array();
        conf["Server"]["deny"] = json::array();
        conf["Interflow"]["isProducer"] = interflowConfigure.isProducer;
        conf["Interflow"]["isTcp"] = interflowConfigure.isTcp;
        conf["Interflow"]["shareKey"] = interflowConfigure.shareKey;
        conf["Interflow"]["producerSemKey"] = interflowConfigure.producerSemKey;
        conf["Interflow"]["consumerSemKey"] = interflowConfigure.consumerSemKey;
        conf["Interflow"]["myIpAddr"] = interflowConfigure.myIpAddr;
        conf["Interflow"]["myPort"] = interflowConfigure.myPort;
        conf["Interflow"]["destIpAddr"] = interflowConfigure.destIpAddr;
        conf["Interflow"]["destPort"] = interflowConfigure.destPort;
        conf["Yolo"] = json::array();
        for(const auto& yolo : yoloConfigures) {
            json yoloJson;
            yoloJson["name"] = yolo.name;
            yoloJson["weightFilePath"] = yolo.weightFilePath;
            yoloJson["version"] = yolo.version;
            yoloJson["isPose"] = yolo.isPose;
            yoloJson["cuda"] = yolo.cuda;
            yoloJson["deviceId"] = yolo.deviceId;
            yoloJson["size"] = json::array();
            yoloJson["size"].push_back(yolo.size.width);
            yoloJson["size"].push_back(yolo.size.height);
            yoloJson["confThreshold"] = yolo.confThreshold;
            yoloJson["iouThreshold"] = yolo.iouThreshold;
            yoloJson["bodyPartSize"] = yolo.bodyPartSize;
            yoloJson["transport"] = json();
            for(const auto& p : yolo.transport) {
                yoloJson["transport"][std::to_string(p.first)] = std::to_string(p.second);
            }
            conf["Yolo"].push_back(yoloJson);
        }
        conf["MissionReactor"]["missionFilePath"] = missionReactorConfigure.missionFilePath;
        conf["MissionReactor"]["analysisFrameCount"] = missionReactorConfigure.analysisFrameCount;
        conf["MissionReactor"]["startTolerance"] = missionReactorConfigure.startTolerance;
        conf["MissionReactor"]["endTolerance"] = missionReactorConfigure.endTolerance;
        conf["MissionReactor"]["saveResultCount"] = missionReactorConfigure.saveResultCount;
        conf["CaptureStream"]["index"] = captureStreamConfigure.captureIndex;
        conf["CaptureStream"]["url"] = captureStreamConfigure.captureUrl;
        return to_string(conf);
    }
} // hzd
