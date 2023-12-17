/**
  ******************************************************************************
  * @file           : QRowEdit.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/15
  ******************************************************************************
  */

// You may need to build the project (run Qt uic code generator) to get "ui_QRowEdit.h" resolved

#include "QRowEdit.h"
#include "ui_QRowEdit.h"

namespace hzd {

    QRowEdit::QRowEdit(
            const std::string &labelText,
            QWidget *parent
    ):
    QWidget(parent),
    ui(new Ui::QRowEdit)
    {
        ui->setupUi(this);
        ui->label->setText(labelText.c_str());
    }

    QRowEdit::~QRowEdit() {
        delete ui;
    }

    std::string QRowEdit::GetEditText() const {
        return ui->lineEdit->text().toStdString();
    }

    void QRowEdit::setReadOnly(bool yes) {
        ui->lineEdit->setReadOnly(yes);
    }

    void QRowEdit::SetEditText(const std::string &text) {
        ui->lineEdit->setText(text.c_str());
    }

    void QRowEdit::SetCurrentText(QString text) {
        ui->lineEdit->setText(text);
    }
} // hzd
