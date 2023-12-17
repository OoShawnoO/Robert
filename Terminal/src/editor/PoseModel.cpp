/**
  ******************************************************************************
  * @file           : PoseModel.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/17
  ******************************************************************************
  */

#include "PoseModel.h"

namespace hzd {
    std::unordered_map<std::string,int> PoseModel::poseIdMap{
            {"鼻子",0}, {"左眼",1},{"右眼",2},{"左耳",3},{"右耳",4},{"左肩",5},
            {"右肩",6},{"左肘",7},{"右肘",8},{"左腕",9},{"右腕",10},
            {"左臀",11},{"右臀",12},{"左膝",13},{"右膝",14},{"左脚",15},{"右脚",16}
    };

    PoseModel::PoseModel() : BaseModel("人体部位","人体部位") {
        layout.addWidget(&poseCombo);
        layout.addWidget(&idEdit);
        widget.setLayout(&layout);

        idEdit.setReadOnly(true);
        connect(&poseCombo,&QRowComboBox::currentTextChanged,this,[&]{
            BaseModel::broadcaster.EraseMap(lastName);
            realName = poseCombo.GetCurrentText();
            lastName = realName;
            id = poseIdMap.at(realName);
            idEdit.SetEditText(std::to_string(id));
            BaseModel::broadcaster.UpdateMap(realName,id);
        });
    }

    PoseModel::~PoseModel() {

    }

    QJsonObject PoseModel::save() const {
        auto modelJson = BaseModel::save();
        modelJson["名称"] = poseCombo.GetCurrentText().c_str();
        modelJson["ID"] = idEdit.GetEditText().c_str();
        return modelJson;
    }

    QWidget *PoseModel::embeddedWidget() {
        return BaseModel::embeddedWidget();
    }

    void PoseModel::load(const QJsonObject &p) {
        PARSE_MODEL_JSON(p,poseCombo,"名称");
        PARSE_MODEL_JSON(p,idEdit,"ID");
    }
} // hzd