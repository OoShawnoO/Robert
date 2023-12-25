/**
  ******************************************************************************
  * @file           : Scene.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/15
  ******************************************************************************
  */

#include <QFileDialog>
#include <QDir>
#include <QJsonArray>
#include <QMessageBox>
#include <utility>
#include "Scene.h"
#include "ItemModel.h"
#include "PoseModel.h"
#include "SettlementModel.h"
#include "StayMissionModel.h"
#include "ContactMissionModel.h"
#include "DistanceMissionModel.h"
#include "DirectedDistanceMissionModel.h"

namespace hzd {
    using Parameter = MissionInterface::Parameter;

    static std::unordered_map<
            std::string,
            std::function<bool(MissionPacker&,QJsonObject&)>
    > generateMap{
            {"结算器", Scene::GenerateSettlement},
            {"重要组件", Scene::GenerateItem},
            {"停留事件", Scene::GenerateStayMission},
            {"接触事件", Scene::GenerateContactMission},
            {"人体部位", Scene::GeneratePose},
            {"距离事件", Scene::GenerateDistanceMission},
            {"指定方向距离事件", Scene::GenerateDirectedDistanceMission}
    };

    Scene::Scene(DataFlowGraphModel &graphModel,SolutionItem& _solutionItem, QObject *parent)
    : solutionItem(_solutionItem), DataFlowGraphicsScene(graphModel, parent) {
    }

    Scene::~Scene() = default;

    std::shared_ptr<NodeDelegateModelRegistry> Scene::registerDataModels() {
        auto ret = std::make_shared<NodeDelegateModelRegistry>();
        ret->registerModel<ItemModel>("重要组件");
        ret->registerModel<PoseModel>("重要组件");
        ret->registerModel<SettlementModel>("结算器");
        ret->registerModel<StayMissionModel>("事件");
        ret->registerModel<ContactMissionModel>("事件");
        ret->registerModel<DistanceMissionModel>("事件");
        ret->registerModel<DirectedDistanceMissionModel>("事件");
        return ret;
    }

    bool Scene::GenerateItem(MissionPacker &packer, QJsonObject &objectJson) {
        auto object =objectJson["internal-data"].toObject();
        packer.WriteItem(object["名称"].toString().toStdString(),object["ID"].toString().toInt());
        return true;
    }

    bool Scene::GeneratePose(MissionPacker &packer, QJsonObject &objectJson) {
        auto object = objectJson["internal-data"].toObject();
        packer.WriteItem(object["名称"].toString().toStdString(),object["ID"].toString().toInt());
        return true;
    }

    bool Scene::GenerateSettlement(MissionPacker &packer, QJsonObject &objectJson) {
        const static std::unordered_map<QString,Settlement::StartType> startMap{
                {"重要物体进入画面",Settlement::StartType::StartItemAppear},
                {"重要物体进入方框",Settlement::StartType::StartItemAppearInBound},
                {"重要物体停留方框",Settlement::StartType::StartItemStayInBound}
        };
        const static std::unordered_map<QString,Settlement::EndType> endMap{
                {"重要物体离开画面",Settlement::EndType::EndItemDisappear},
                {"重要物体进入方框",Settlement::EndType::EndItemAppearInBound},
                {"重要物体停留方框",Settlement::EndType::EndItemStayInBound},
                {"重要物体离开方框",Settlement::EndType::EndItemDisappearInBound}
        };
        auto object = objectJson["internal-data"].toObject();
        auto start = startMap.at(object["开始结算事件"].toString());
        auto end = endMap.at(object["结束结算事件"].toString());
        auto startDuration = object["开始参考时间"].toString().toInt();
        auto endDuration = object["结束参考时间"].toString().toInt();
        auto startItem = BaseModel::broadcaster.At(object["开始结算组件"].toString().toStdString());
        auto endItem = BaseModel::broadcaster.At(object["结束结算组件"].toString().toStdString());
        auto startBound = object["开始结算边界"].toString().split(' ');
        auto endBound = object["结束结算边界"].toString().split(' ');
        if(start != Settlement::StartType::StartItemAppear && startBound.size() != 4
        || end != Settlement::EndItemDisappear && endBound.size() != 4) {
            QMessageBox::critical(nullptr,"错误","边界格式x y w h填写错误");
            return false;
        }
        Item startBoundItem,endBoundItem;
        if(start != Settlement::StartType::StartItemAppear){
            startBoundItem = {
                    startBound[0].toInt(),startBound[1].toInt(),startBound[2].toInt(),startBound[3].toInt()
            };
        }
        if(end != Settlement::EndType::EndItemDisappear) {
            endBoundItem = {
                    endBound[0].toInt(),endBound[1].toInt(),endBound[2].toInt(),endBound[3].toInt()
            };
        }
        packer.WriteSettlement(start,end,{startItem},{endItem},startBoundItem,endBoundItem,startDuration,endDuration);
        return true;
    }

    bool Scene::GenerateStayMission(MissionPacker &packer, QJsonObject &objectJson) {
        auto object = objectJson["internal-data"].toObject();

        auto startSignals = object["开始信号"].toString().split(' ');
        auto endSignals = object["结束信号"].toString().split(' ');
        std::vector<Parameter> parameters(10);
        parameters[0] = startSignals.size();
        parameters[1] = endSignals.size();
        parameters[2] = BaseModel::broadcaster.At(object["组件A"].toString().toStdString());
        parameters[3] = BaseModel::broadcaster.At(object["组件B"].toString().toStdString());
        parameters[4] = object["参考时间"].toString().toInt();
        auto list = object["边界"].toString().split(' ');
        if(parameters[3].asInt32() == -1 && list.size() != 4){
            QMessageBox::critical(nullptr,"错误","边界应该为x y w h格式书写");
            return false;
        }else{
            parameters[5] = list[0].toInt();
            parameters[6] = list[1].toInt();
            parameters[7] = list[2].toInt();
            parameters[8] = list[3].toInt();
        }
        parameters[9] = object["任意个体"] == "是";
        for(const auto& startSignal : startSignals) {
            parameters.push_back(Parameter(startSignal.toInt()));
        }
        for(const auto& endSignal : endSignals) {
            parameters.push_back(Parameter(endSignal.toInt()));
        }
        packer.WriteFunction("停留事件",parameters);
        return true;
    }

    bool Scene::GenerateContactMission(MissionPacker &packer, QJsonObject &objectJson) {
        auto object = objectJson["internal-data"].toObject();

        auto startSignals = object["开始信号"].toString().split(' ');
        auto endSignals = object["结束信号"].toString().split(' ');
        std::vector<Parameter> parameters(10);
        parameters[0] = startSignals.size();
        parameters[1] = endSignals.size();
        parameters[2] = BaseModel::broadcaster.At(object["组件A"].toString().toStdString());
        parameters[3] = BaseModel::broadcaster.At(object["组件B"].toString().toStdString());
        parameters[4] = object["参考时间"].toString().toInt();
        auto list = object["边界"].toString().split(' ');
        if(parameters[3].asInt32() == -1 && list.size() != 4){
            QMessageBox::critical(nullptr,"错误","边界应该为x y w h格式书写");
            return false;
        }else{
            parameters[5] = list[0].toInt();
            parameters[6] = list[1].toInt();
            parameters[7] = list[2].toInt();
            parameters[8] = list[3].toInt();
        }
        parameters[9] = object["任意个体"] == "是";
        for(const auto& startSignal : startSignals) {
            parameters.push_back(Parameter(startSignal.toInt()));
        }
        for(const auto& endSignal : endSignals) {
            parameters.push_back(Parameter(endSignal.toInt()));
        }
        packer.WriteFunction("接触事件",parameters);
        return true;
    }

    bool Scene::GenerateDistanceMission(MissionPacker &packer, QJsonObject &objectJson) {
        auto object = objectJson["internal-data"].toObject();

        auto startSignals = object["开始信号"].toString().split(' ');
        auto endSignals = object["结束信号"].toString().split(' ');
        std::vector<Parameter> parameters(12);
        parameters[0] = startSignals.size();
        parameters[1] = endSignals.size();
        parameters[2] = BaseModel::broadcaster.At(object["组件A"].toString().toStdString());
        parameters[3] = BaseModel::broadcaster.At(object["组件B"].toString().toStdString());
        parameters[4] = object["参考时间"].toString().toInt();
        auto list = object["边界"].toString().split(' ');
        if(parameters[3].asInt32() == -1 && list.size() != 4){
            QMessageBox::critical(nullptr,"错误","边界应该为x y w h格式书写");
            return false;
        }else{
            parameters[5] = list[0].toInt();
            parameters[6] = list[1].toInt();
            parameters[7] = list[2].toInt();
            parameters[8] = list[3].toInt();
        }
        parameters[9] = object["任意个体"] == "是";
        parameters[10] = object["最小距离权值"].toString().toDouble();
        parameters[11] = object["最大距离权值"].toString().toDouble();
        for(const auto& startSignal : startSignals) {
            parameters.push_back(Parameter(startSignal.toInt()));
        }
        for(const auto& endSignal : endSignals) {
            parameters.push_back(Parameter(endSignal.toInt()));
        }
        packer.WriteFunction("距离事件",parameters);
        return true;
    }

    bool Scene::GenerateDirectedDistanceMission(MissionPacker &packer, QJsonObject &objectJson) {
        auto object = objectJson["internal-data"].toObject();

        auto startSignals = object["开始信号"].toString().split(' ');
        auto endSignals = object["结束信号"].toString().split(' ');
        auto vectorA = object["向量A"].toString().split(' ');
        auto vectorB = object["向量B"].toString().split(' ');
        std::vector<Parameter> parameters(16);
        parameters[0] = startSignals.size();
        parameters[1] = endSignals.size();
        parameters[2] = BaseModel::broadcaster.At(object["组件A"].toString().toStdString());
        parameters[3] = BaseModel::broadcaster.At(object["组件B"].toString().toStdString());
        parameters[4] = object["参考时间"].toString().toInt();
        auto list = object["边界"].toString().split(' ');
        if(parameters[3].asInt32() == -1 && list.size() != 4){
            QMessageBox::critical(nullptr,"错误","边界应该为x y w h格式书写");
            return false;
        }else{
            parameters[5] = list[0].toInt();
            parameters[6] = list[1].toInt();
            parameters[7] = list[2].toInt();
            parameters[8] = list[3].toInt();
        }
        parameters[9] = object["任意个体"] == "是";
        parameters[10] = object["最小距离权值"].toString().toDouble();
        parameters[11] = object["最大距离权值"].toString().toDouble();
        parameters[12] = vectorA[0].toInt();
        parameters[13] = vectorA[1].toInt();
        parameters[14] = vectorB[0].toInt();
        parameters[15] = vectorB[1].toInt();
        for(const auto& startSignal : startSignals) {
            parameters.push_back(Parameter(startSignal.toInt()));
        }
        for(const auto& endSignal : endSignals) {
            parameters.push_back(Parameter(endSignal.toInt()));
        }
        packer.WriteFunction("指定方向距离事件",parameters);
        return true;
    }

    void Scene::Save() {
        auto json = QJsonDocument(_graphModel.save()).toJson();
        if(!json.contains("结算器")){
            QMessageBox::critical(nullptr,"错误","flow文件中至少要有一个结算器组件");
            return;
        }
        SaveJson(solutionItem.editorFlowJson);
        QFile file((solutionItem.fileName + ".flow").c_str());
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(nullptr, "错误", "无法创建文件");
            return;
        }
        file.write(json);
        file.close();
    }

    void Scene::SaveAndGenerate() {
        auto json = QJsonDocument(_graphModel.save()).toJson();
        if(!json.contains("结算器")){
            QMessageBox::critical(nullptr,"错误","flow文件中至少要有一个结算器组件");
            return;
        }

        QString _fileName = QFileDialog::getSaveFileName(
                nullptr,
                tr("Open Flow Scene"),
                QDir::homePath(),
                tr("Flow Scene Files (*.flow)")
        );

        if (!_fileName.isEmpty()) {
            if (!_fileName.endsWith("flow", Qt::CaseInsensitive))
                _fileName += ".flow";

            QFile file(_fileName);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(json);
            }
            file.close();
        }
        Generate(_fileName.toStdString());
    }

    bool Scene::Generate(const QJsonObject &jsonObject,const std::string& name) {
        MissionPacker packer(name + ".mission");
        QJsonArray nodeArray = jsonObject["nodes"].toArray();
        for(const auto node : nodeArray) {
            auto data = node.toObject()["internal-data"].toObject();
            if(data["model-name"] == "结算器") {
                BaseModel::broadcaster.Deserialize(data["nameIdMap"]);
                break;
            }
        }
        for(const auto node : nodeArray){
            auto internalData = node.toObject()["internal-data"].toObject();
            QString modelName = internalData["model-name"].toString();
            auto n = node.toObject();
            generateMap.at(modelName.toStdString())(packer,n);
        }
        packer.Save();
        return true;
    }

    bool Scene::Generate(const std::string &_fileName) {
        QString fileName = _fileName.c_str();
        QFile file(fileName);
        QString missionName;
        if(fileName.endsWith(".flow")){
            missionName = fileName.left(fileName.size()-5);
        }
        MissionPacker packer(missionName.toStdString() + ".mission");
        if(!file.open(QIODevice::ReadOnly)){
            QMessageBox::critical(nullptr,"错误","读取文件失败");
            return false;
        }
        QByteArray data = file.readAll();
        file.close();
        QJsonParseError jsonError{};
        QJsonDocument jsonDocument(QJsonDocument::fromJson(data,&jsonError));
        if(jsonError.error != QJsonParseError::NoError){
            QMessageBox::critical(nullptr,"错误","Json 解析错误");
            return false;
        }
        QJsonObject modelJson = jsonDocument.object();
        QJsonArray nodeArray = modelJson["nodes"].toArray();
        for(const auto node : nodeArray){
            auto internalData = node.toObject()["internal-data"].toObject();
            QString modelName = internalData["model-name"].toString();
            auto n = node.toObject();
            generateMap.at(modelName.toStdString())(packer,n);
        }
        packer.Save();
        return true;
    }

    void Scene::Load() {
        LoadJson(solutionItem.editorFlowJson);
    }

    void Scene::LoadJson(QJsonObject &jsonObject) {
        clearScene();
        _graphModel.load(jsonObject);
        Q_EMIT sceneLoaded();
    }

    void Scene::SaveJson(QJsonObject &jsonObject) {
        jsonObject = _graphModel.save();
    }

} // hzd