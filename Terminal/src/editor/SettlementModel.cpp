/**
  ******************************************************************************
  * @file           : SettlementModel.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/17
  ******************************************************************************
  */

#include "SettlementModel.h"

namespace hzd {
    SettlementModel::SettlementModel() : BaseModel("结算器","结算器") {

        layout.addWidget(&startCombo);
        layout.addWidget(&startItemCombo);
        layout.addWidget(&startRect);
        layout.addWidget(&startDuration);
        layout.addWidget(&endCombo);
        layout.addWidget(&endItemCombo);
        layout.addWidget(&endRect);
        layout.addWidget(&endDuration);
        widget.setLayout(&layout);

        connect(&BaseModel::broadcaster,&ModelBroadcaster::updated,this,[&]{
            std::vector<std::string> keys = broadcaster.Keys();
            startItemCombo.SetComboItems(keys);
            endItemCombo.SetComboItems(keys);
        });
        emit BaseModel::broadcaster.updated();
    }

    SettlementModel::~SettlementModel() {

    }

    QJsonObject SettlementModel::save() const {
        auto modelJson =  BaseModel::save();
        broadcaster.Serialize(modelJson);

        modelJson["开始结算事件"] = startCombo.GetCurrentText().c_str();
        modelJson["结束结算事件"] = endCombo.GetCurrentText().c_str();
        modelJson["开始参考时间"] = startDuration.GetEditText().c_str();
        modelJson["结束参考时间"] = endDuration.GetEditText().c_str();
        modelJson["开始结算组件"] = startItemCombo.GetCurrentText().c_str();
        modelJson["结束结算组件"] = endItemCombo.GetCurrentText().c_str();
        modelJson["开始结算边界"] = startRect.GetEditText().c_str();
        modelJson["结束结算边界"] = endRect.GetEditText().c_str();

        return modelJson;
    }

    QWidget *SettlementModel::embeddedWidget() {
        return BaseModel::embeddedWidget();
    }

    void SettlementModel::load(const QJsonObject &p) {
        if(p.find("nameIdMap") != p.end()) {
            BaseModel::broadcaster.Deserialize(p["nameIdMap"]);
        }

        PARSE_MODEL_JSON(p,startCombo,"开始结算事件");
        PARSE_MODEL_JSON(p,endCombo,"结束结算事件");
        PARSE_MODEL_JSON(p,startDuration,"开始参考时间");
        PARSE_MODEL_JSON(p,endDuration,"结束参考时间");
        PARSE_MODEL_JSON(p,startItemCombo,"开始结算组件");
        PARSE_MODEL_JSON(p,endItemCombo,"结束结算组件");
        PARSE_MODEL_JSON(p,startRect,"开始结算边界");
        PARSE_MODEL_JSON(p,endRect,"结束结算边界");
    }
} // hzd