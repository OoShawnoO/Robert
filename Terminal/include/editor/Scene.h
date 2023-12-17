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

using QtNodes::DataFlowGraphModel;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::NodeDelegateModelRegistry;

namespace hzd {

    class Scene : public DataFlowGraphicsScene {
    Q_OBJECT
    public:
        explicit Scene(DataFlowGraphModel& graphModel,QObject* parent = nullptr);

        ~Scene() override;
        static std::shared_ptr<NodeDelegateModelRegistry> registerDataModels();
        static bool GenerateItem(MissionPacker& packer,QJsonObject& objectJson);
        static bool GeneratePose(MissionPacker& packer,QJsonObject& objectJson);
        static bool GenerateSettlement(MissionPacker& packer,QJsonObject& objectJson);
        static bool GenerateStayMission(MissionPacker& packer,QJsonObject& objectJson);
        static bool GenerateContactMission(MissionPacker& packer,QJsonObject& objectJson);
        static bool GenerateDistanceMission(MissionPacker& packer,QJsonObject& objectJson);
        static bool GenerateDirectedDistanceMission(MissionPacker& packer,QJsonObject& objectJson);

    public slots:
        void SaveAndGenerate();

    private:
        std::unordered_map<
            std::string,
            std::function<bool(MissionPacker&,QJsonObject&)>
        > generateMap{
                {"结算器", GenerateSettlement},
                {"重要组件", GenerateItem},
                {"停留事件", GenerateStayMission},
                {"接触事件", GenerateContactMission},
                {"人体部位", GeneratePose},
                {"距离事件", GenerateDistanceMission},
                {"定向距离事件", GenerateDirectedDistanceMission}
        };
        bool Generate(const std::string& fileName);

    };

} // hzd

#endif //ROBERT_SCENE_H
