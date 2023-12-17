/**
  ******************************************************************************
  * @file           : ItemModel.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/15
  ******************************************************************************
  */

#ifndef ROBERT_ITEMMODEL_H
#define ROBERT_ITEMMODEL_H

#include "BaseModel.h"

namespace hzd {

    class ItemModel : public BaseModel {
        Q_OBJECT
    public:
        ItemModel();
        ~ItemModel() override;
        QJsonObject save() const override;
        QWidget *embeddedWidget() override;
        void load(const QJsonObject &p) override;
    private:
        QRowEdit nameEdit{"名称:",nullptr};
        QRowEdit idEdit{"ID:",nullptr};
        QRowButton finishButton{"完成",nullptr};
        QRowButton editButton{"修改",nullptr};
    };

} // hzd

#endif //ROBERT_ITEMMODEL_H
