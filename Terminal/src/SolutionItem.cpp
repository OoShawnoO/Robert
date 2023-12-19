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
    SolutionItem::SolutionItem(QWidget *parent,int _id) :
            QWidget(parent), ui(new Ui::SolutionItem),id(_id) {
        ui->setupUi(this);
        ui->stop->hide();
        connect(
            ui->editConfig,
            &QPushButton::clicked,
            this,
            [&]{
                emit editConfigSignal();
            }
        );
        connect(
                ui->editMission,
                &QPushButton::clicked,
                this,
                [&]{
                    emit editMissionSignal();
                }
        );
        connect(
                ui->start,
                &QPushButton::clicked,
                this,
                [&]{
                    ui->stop->show();
                    ui->start->hide();
                    emit runSignal();
                }
        );
        connect(
                ui->stop,
                &QPushButton::clicked,
                this,
                [&]{
                    ui->start->show();
                    ui->stop->hide();
                    emit stopSignal();
                }
        );
        connect(
          ui->deleteItem,
          &QPushButton::clicked,
          this,
          [&]{
              emit deleteSignal();
          }
        );
    }

    SolutionItem::~SolutionItem() {
        delete ui;
    }

    void SolutionItem::updateUI() {
        ui->title->setText(configurePackage.name.c_str());
        ui->description->setText(configurePackage.description.c_str());
    }
} // hzd
