/**
  ******************************************************************************
  * @file           : PoseModel.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/17
  ******************************************************************************
  */

#ifndef ROBERT_POSEMODEL_H
#define ROBERT_POSEMODEL_H

#include "BaseModel.h"
#include "QRowComboBox.h"

namespace hzd {

    class PoseModel : public BaseModel {
        Q_OBJECT
    public:
        PoseModel();
        ~PoseModel() override;
        QJsonObject save() const override;
        QWidget *embeddedWidget() override;
        void load(const QJsonObject &p) override;
    private:
        static std::unordered_map<std::string,int> poseIdMap;
        QRowComboBox    poseCombo{"部位",{
            "鼻子","左眼","右眼","左耳","右耳","左肩","右肩","左肘","右肘","左腕","右腕",
            "左臀","右臀","左膝","右膝","左脚","右脚"
            },nullptr
        };
        QRowEdit        idEdit{"ID:",nullptr};
        std::string     lastName{};
        std::string     realName;
        int             id{};
    };

} // hzd

#endif //ROBERT_POSEMODEL_H
