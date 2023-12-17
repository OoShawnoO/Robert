/**
  ******************************************************************************
  * @file           : QRowComboBox.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/15
  ******************************************************************************
  */

// You may need to build the project (run Qt uic code generator) to get "ui_QRowComboBox.h" resolved

#include "QRowComboBox.h"
#include "ui_QRowComboBox.h"

namespace hzd {
    QRowComboBox::QRowComboBox(
            const std::string& labelText,
            const std::vector<std::string>& comboItems,
            QWidget *parent
    ) :QWidget(parent), ui(new Ui::QRowComboBox) {
        ui->setupUi(this);

        ui->label->setText(labelText.c_str());
        SetComboItems(comboItems);
        ui->comboBox->setMaxVisibleItems(6);

        connect(
                ui->comboBox,
                &QComboBox::currentTextChanged,
                this,
                [&] {
                    emit currentTextChanged();
                }
        );
    }

    QRowComboBox::~QRowComboBox() {
        delete ui;
    }

    std::string QRowComboBox::GetCurrentText() const {
        return ui->comboBox->currentText().toStdString();
    }

    void QRowComboBox::SetComboItems(const std::vector<std::string> &comboItems) {
        bool flag = true;
        auto lastItem = ui->comboBox->currentText();
        ui->comboBox->clear();
        QStringList stringList;
        for(const auto& item : comboItems) {
            stringList << item.c_str();
        }
        ui->comboBox->addItems(stringList);
        if(stringList.contains(lastItem)) {
            ui->comboBox->setCurrentText(lastItem);
        }
    }

    void QRowComboBox::SetCurrentText(QString text) {
        ui->comboBox->setCurrentText(text);
    }
} // hzd
