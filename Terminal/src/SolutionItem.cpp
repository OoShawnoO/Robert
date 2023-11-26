/**
  ******************************************************************************
  * @file           : SolutionItem.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/26
  ******************************************************************************
  */

// You may need to build the project (run Qt uic code generator) to get "ui_SolutionItem.h" resolved

#include "SolutionItem.h"
#include "ui_SolutionItem.h"

namespace hzd {
    SolutionItem::SolutionItem(QWidget *parent) :
            QWidget(parent), ui(new Ui::SolutionItem) {
        ui->setupUi(this);
    }

    SolutionItem::~SolutionItem() {
        delete ui;
    }
} // hzd
