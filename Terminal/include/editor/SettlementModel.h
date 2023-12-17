/**
  ******************************************************************************
  * @file           : SettlementModel.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/17
  ******************************************************************************
  */

#ifndef ROBERT_SETTLEMENTMODEL_H
#define ROBERT_SETTLEMENTMODEL_H

#include "BaseModel.h"
#include "QRowComboBox.h"

namespace hzd {

    class SettlementModel : public BaseModel {
    Q_OBJECT
    public:
        SettlementModel();
        ~SettlementModel() override;
        QJsonObject save() const override;
        QWidget *embeddedWidget() override;
        void load(const QJsonObject &p) override;
    private:
        QRowComboBox startCombo{"开始事件",{
            "重要物体进入画面","重要物体进入方框","重要物体停留方框"
        },nullptr};
        QRowComboBox startItemCombo{"开始事件组件",{"无"},nullptr};
        QRowEdit    startDuration{"开始事件间隔",nullptr};
        QRowEdit    startRect{"开始事件区域",nullptr};
        QRowComboBox endCombo{"结束事件",{
            "重要物体离开画面","重要物体进入方框","重要物体停留方框","重要物体离开方框"
        },nullptr};
        QRowComboBox endItemCombo{"结束事件组件",{"无"},nullptr};
        QRowEdit    endDuration{"结束事件间隔",nullptr};
        QRowEdit    endRect{"结束事件区域",nullptr};
    };

} // hzd

#endif //ROBERT_SETTLEMENTMODEL_H
