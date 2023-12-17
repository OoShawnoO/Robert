/**
  ******************************************************************************
  * @file           : QRowButton.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/17
  ******************************************************************************
  */

// You may need to build the project (run Qt uic code generator) to get "ui_QRowButton.h" resolved

#include "QRowButton.h"
#include "ui_QRowButton.h"

namespace hzd {
    QRowButton::QRowButton(const std::string& buttonText,QWidget *parent) :
            QWidget(parent), ui(new Ui::QRowButton) {
        ui->setupUi(this);
        ui->pushButton->setText(buttonText.c_str());
        connect(
            ui->pushButton,
            &QPushButton::clicked,
            this,
            [&] {
                emit clicked();
            }
        );
    }

    QRowButton::~QRowButton() {
        delete ui;
    }
} // hzd
