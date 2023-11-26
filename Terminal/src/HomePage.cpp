/**
  ******************************************************************************
  * @file           : homepage.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/26
  ******************************************************************************
  */

// You may need to build the project (run Qt uic code generator) to get "ui_HomePage.h" resolved

#include "HomePage.h"
#include "ui_HomePage.h"
#include "SolutionItem.h"

namespace hzd {
    HomePage::HomePage(QWidget *parent) :
            QWidget(parent), ui(new Ui::HomePage) {
        ui->setupUi(this);
        ui->solutions->addWidget(new SolutionItem);
        ui->solutions->addWidget(new SolutionItem);
        ui->solutions->addWidget(new SolutionItem);
    }

    HomePage::~HomePage() {
        delete ui;
    }
} // hzd
