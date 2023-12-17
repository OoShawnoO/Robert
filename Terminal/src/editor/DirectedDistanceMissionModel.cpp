/**
  ******************************************************************************
  * @file           : DirectedDistanceMissionModel.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/17
  ******************************************************************************
  */

#include "DirectedDistanceMissionModel.h"

namespace hzd {
    DirectedDistanceMissionModel::DirectedDistanceMissionModel() : BaseModel("指定方向距离事件","指定方向距离事件") {
        layout.addWidget(&itemACombo);
        layout.addWidget(&itemBCombo);
        layout.addWidget(&durationEdit);
        layout.addWidget(&boundEdit);
        layout.addWidget(&minThresholdEdit);
        layout.addWidget(&maxThresholdEdit);
        layout.addWidget(&vectorAEdit);
        layout.addWidget(&vectorBEdit);
        layout.addWidget(&anyTriggerCombo);
        layout.addWidget(&startSignalEdit);
        layout.addWidget(&endSignalEdit);

        widget.setLayout(&layout);
        connect(&broadcaster,&ModelBroadcaster::updated,this,[&]{
            std::vector<std::string> keys = broadcaster.Keys();
            itemACombo.SetComboItems(keys);
            itemBCombo.SetComboItems(keys);
        });
        emit broadcaster.updated();
    }

    DirectedDistanceMissionModel::~DirectedDistanceMissionModel() {

    }

    QJsonObject DirectedDistanceMissionModel::save() const {
        auto modelJson = BaseModel::save();
        modelJson["组件A"] = itemACombo.GetCurrentText().c_str();
        modelJson["组件B"] = itemBCombo.GetCurrentText().c_str();
        modelJson["参考时间"] = durationEdit.GetEditText().c_str();
        modelJson["边界"] = boundEdit.GetEditText().c_str();
        modelJson["最小距离权值"] = minThresholdEdit.GetEditText().c_str();
        modelJson["最大距离权值"] = maxThresholdEdit.GetEditText().c_str();
        modelJson["向量A"] = vectorAEdit.GetEditText().c_str();
        modelJson["向量B"] = vectorBEdit.GetEditText().c_str();
        modelJson["任意个体"] = anyTriggerCombo.GetCurrentText().c_str();
        modelJson["开始信号"] = startSignalEdit.GetEditText().c_str();
        modelJson["结束信号"] = endSignalEdit.GetEditText().c_str();
        return modelJson;
    }

    QWidget *DirectedDistanceMissionModel::embeddedWidget() {
        return BaseModel::embeddedWidget();
    }

    void DirectedDistanceMissionModel::load(const QJsonObject &p) {
        PARSE_MODEL_JSON(p,itemACombo,"组件A");
        PARSE_MODEL_JSON(p,itemBCombo,"组件B");
        PARSE_MODEL_JSON(p,durationEdit,"参考时间");
        PARSE_MODEL_JSON(p,boundEdit,"边界");
        PARSE_MODEL_JSON(p,minThresholdEdit,"最小距离权值");
        PARSE_MODEL_JSON(p,maxThresholdEdit,"最大距离权值");
        PARSE_MODEL_JSON(p,vectorAEdit,"向量A");
        PARSE_MODEL_JSON(p,vectorBEdit,"向量B");
        PARSE_MODEL_JSON(p,anyTriggerCombo,"任意个体");
        PARSE_MODEL_JSON(p,startSignalEdit,"开始信号");
        PARSE_MODEL_JSON(p,endSignalEdit,"结束信号");
    }
} // hzd