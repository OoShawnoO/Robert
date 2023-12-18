/**
  ******************************************************************************
  * @file           : Scene.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/15
  ******************************************************************************
  */

#ifndef ROBERT_SCENE_H
#define ROBERT_SCENE_H

#include <DataFlowGraphicsScene>
#include <MissionReactor/MissionPacker.h>
#include "SolutionItem.h"

using QtNodes::DataFlowGraphModel;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::NodeDelegateModelRegistry;

namespace hzd {

    class Scene : public DataFlowGraphicsScene {
    Q_OBJECT
    public:
        explicit Scene(DataFlowGraphModel& graphModel,SolutionItem& solutionItem,QObject* parent = nullptr);
        ~Scene() override;
        static std::shared_ptr<NodeDelegateModelRegistry> registerDataModels();
        static bool Generate(const std::string& fileName);
        static bool Generate(const QJsonObject& jsonObject,const std::string& name);
        static bool GenerateItem(MissionPacker& packer,QJsonObject& objectJson);
        static bool GeneratePose(MissionPacker& packer,QJsonObject& objectJson);
        static bool GenerateSettlement(MissionPacker& packer,QJsonObject& objectJson);
        static bool GenerateStayMission(MissionPacker& packer,QJsonObject& objectJson);
        static bool GenerateContactMission(MissionPacker& packer,QJsonObject& objectJson);
        static bool GenerateDistanceMission(MissionPacker& packer,QJsonObject& objectJson);
        static bool GenerateDirectedDistanceMission(MissionPacker& packer,QJsonObject& objectJson);
    public slots:
        void Load();
        void Save();
        void SaveAndGenerate();

    private:
        SolutionItem& solutionItem;
        void SaveJson(QJsonObject& jsonObject);
        void LoadJson(QJsonObject& jsonObject);
    };

} // hzd

#endif //ROBERT_SCENE_H
