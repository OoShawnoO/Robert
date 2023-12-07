/**
  ******************************************************************************
  * @file           : ConfigPage.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/6
  ******************************************************************************
  */

// You may need to build the project (run Qt uic code generator) to get "ui_ConfigPage.h" resolved

#include "ConfigPage.h"
#include "ui_ConfigPage.h"

namespace hzd {
    ConfigPage::ConfigPage(QWidget *parent) :
            QWidget(parent), ui(new Ui::ConfigPage) {
        ui->setupUi(this);
        setAttribute(Qt::WA_StyledBackground);
    }

    ConfigPage::~ConfigPage() {
        delete ui;
    }
} // hzd
