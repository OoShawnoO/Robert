/**
  ******************************************************************************
  * @file           : ItemModel.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/15
  ******************************************************************************
  */

#include "ItemModel.h"
#include <QMessageBox>

namespace hzd {
    ItemModel::ItemModel() : BaseModel("重要组件","重要组件") {
        editButton.hide();

        connect(&finishButton,&QRowButton::clicked,this,[&]{
            if(BaseModel::broadcaster.HasKey(nameEdit.GetEditText())) {
                QMessageBox::critical(nullptr,"错误","已经存在同名组件,名称需要唯一");
                return;
            }
            finishButton.hide();
            editButton.show();
            nameEdit.setReadOnly(true);
            idEdit.setReadOnly(true);
            BaseModel::broadcaster.UpdateMap(nameEdit.GetEditText(),stoi(idEdit.GetEditText()));
        });
        connect(&editButton,&QRowButton::clicked,this,[&]{
            editButton.hide();
            finishButton.show();
            nameEdit.setReadOnly(false);
            idEdit.setReadOnly(false);
            BaseModel::broadcaster.EraseMap(nameEdit.GetEditText());
        });

        layout.addWidget(&nameEdit);
        layout.addWidget(&idEdit);
        layout.addWidget(&finishButton);
        layout.addWidget(&editButton);
        widget.setLayout(&layout);
    }

    ItemModel::~ItemModel() = default;

    QJsonObject ItemModel::save() const {
        auto modelJson = BaseModel::save();
        modelJson["名称"] = nameEdit.GetEditText().c_str();
        modelJson["ID"] = idEdit.GetEditText().c_str();
        if(finishButton.isHidden()) {
            modelJson["finishButton"] = "hidden";
        }else{
            modelJson["finishButton"] = "show";
        }
        return modelJson;
    }

    QWidget *ItemModel::embeddedWidget() {
        return &widget;
    }

    void ItemModel::load(const QJsonObject &p) {
        PARSE_MODEL_JSON(p,nameEdit,"名称");
        PARSE_MODEL_JSON(p,idEdit,"ID");
        auto finishObject = p["finishButton"];
        if(!finishObject.isUndefined()) {
            if(finishObject.toString() == "hidden") {
                finishButton.hide();
                editButton.show();
            }else{
                finishButton.show();
                editButton.hide();
            }
        }
    }



} // hzd