/**
  ******************************************************************************
  * @file           : YoloConfigPage.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/7
  ******************************************************************************
  */

// You may need to build the project (run Qt uic code generator) to get "ui_YoloConfigPage.h" resolved

#include "YoloConfigPage.h"
#include "ui_YoloConfigPage.h"

namespace hzd {
    YoloConfigPage::YoloConfigPage(QWidget *parent) :
            QWidget(parent), ui(new Ui::YoloConfigPage) {
        ui->setupUi(this);
    }

    YoloConfigPage::~YoloConfigPage() {
        delete ui;
    }
} // hzd
