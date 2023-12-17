/**
  ******************************************************************************
  * @file           : StayMissionModel.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/17
  ******************************************************************************
  */

#ifndef ROBERT_STAYMISSIONMODEL_H
#define ROBERT_STAYMISSIONMODEL_H

#include "BaseModel.h"
#include "QRowComboBox.h"

namespace hzd {

    class StayMissionModel : public BaseModel {
        Q_OBJECT
    public:
        StayMissionModel();
        ~StayMissionModel() override;
        QJsonObject save() const override;
        QWidget *embeddedWidget() override;
        void load(const QJsonObject &p) override;
    private:
        QRowComboBox itemACombo{"组件A:",{"无(必要)"}};
        QRowComboBox itemBCombo{"组件B",{"无"}};
        QRowEdit durationEdit{"参考时间(ms):"};
        QRowEdit boundEdit{"边界(x,y,w,h):"};
        QRowComboBox anyTriggerCombo{"是否任意个体触发",{"是","否"}};
        QRowEdit startSignalEdit{"开始信号:"};
        QRowEdit endSignalEdit{"结束信号:"};
    };

} // hzd

#endif //ROBERT_STAYMISSIONMODEL_H
