/**
  ******************************************************************************
  * @file           : YoloConfigPage.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/7
  ******************************************************************************
  */

// You may need to build the project (run Qt uic code generator) to get "ui_YoloConfigPage.h" resolved

#include <QFileDialog>
#include "YoloConfigPage.h"
#include "ui_YoloConfigPage.h"

namespace hzd {
    YoloConfigPage::YoloConfigPage(
            QWidget *parent,
            YoloConfigure* _yoloConfigure
    )
    :QWidget(parent),
    yoloConfigure(_yoloConfigure),
    ui(new Ui::YoloConfigPage) {
        ui->setupUi(this);
        setAttribute(Qt::WA_StyledBackground);
        if(yoloConfigure){
            ui->nameEdit->setText(yoloConfigure->name.c_str());
            ui->weightFileEdit->setText(yoloConfigure->weightFilePath.c_str());
            if(yoloConfigure->version == 5){
                if(yoloConfigure->isPose){
                    ui->versionCombo->setCurrentText("yolov5-pose");
                }else{
                    ui->versionCombo->setCurrentText("yolov5-detect");
                }
            }
            else{
                if(yoloConfigure->isPose){
                    ui->versionCombo->setCurrentText("yolov8-pose");
                }else{
                    ui->versionCombo->setCurrentText("yolov8-detect");
                }
            }
            if(yoloConfigure->cuda) {
                ui->cudaCheck->setCheckState(Qt::CheckState::Checked);
            }
            else{
                ui->cudaCheck->setCheckState(Qt::CheckState::Unchecked);
            }

            ui->deviceEdit->setText(std::to_string(yoloConfigure->deviceId).c_str());
            ui->sizeEdit->setText(
                    (
                            std::to_string(yoloConfigure->size.width)
                            + " "
                            + std::to_string(yoloConfigure->size.height)
                    ).c_str()
            );
            ui->confEdit->setText(std::to_string(yoloConfigure->confThreshold).c_str());
            ui->iouEdit->setText(std::to_string(yoloConfigure->iouThreshold).c_str());
            ui->bodySizeEdit->setText(std::to_string(yoloConfigure->bodyPartSize).c_str());
            for(const auto& p : yoloConfigure->transport){
                ui->tableWidget->insertRow(0);
                ui->tableWidget->setItem(0,0,new QTableWidgetItem(std::to_string(p.first).c_str()));
                ui->tableWidget->setItem(0,1,new QTableWidgetItem(std::to_string(p.second).c_str()));
            }
        }

        // 选择文件
        connect(
            ui->weightFileButton,
            &QPushButton::clicked,
            this,
            [&]{
                ui->weightFileEdit->setText(QFileDialog::getOpenFileName(
                        this,
                        "选择权重文件路径(.onnx)",
                        "./",
                        "ONNX(*.onnx)"
                ));
            }
        );
        // 完成按钮
        connect(
          ui->finish,
          &QPushButton::clicked,
          this,
          [&]{
              yoloConfigure->name = ui->nameEdit->text().toStdString();
              yoloConfigure->weightFilePath = ui->weightFileEdit->text().toStdString();
              auto comboStr = ui->versionCombo->currentText();
              if(comboStr == "yolov5-detect"){
                  yoloConfigure->version = 5;
                  yoloConfigure->isPose = false;
              }else if(comboStr == "yolov5-pose"){
                  yoloConfigure->version = 5;
                  yoloConfigure->isPose = true;
              }else if(comboStr == "yolov8-detect"){
                  yoloConfigure->version = 8;
                  yoloConfigure->isPose = false;
              }else if(comboStr == "yolov8-pose"){
                  yoloConfigure->version = 8;
                  yoloConfigure->isPose = true;
              }
              yoloConfigure->cuda = ui->cudaCheck->isChecked();
              yoloConfigure->deviceId = ui->deviceEdit->text().toInt();
              auto sizeList = ui->sizeEdit->text().split(' ');
              yoloConfigure->size.width = sizeList[0].toInt();
              yoloConfigure->size.height = sizeList[1].toInt();
              yoloConfigure->confThreshold = ui->confEdit->text().toFloat();
              yoloConfigure->iouThreshold = ui->iouEdit->text().toFloat();
              yoloConfigure->bodyPartSize = ui->bodySizeEdit->text().toInt();

              auto rowCount = ui->tableWidget->rowCount();
              for(auto index = 0; index < rowCount; index ++) {
                  int ago = ui->tableWidget->item(index,0)->text().toInt();
                  int now = ui->tableWidget->item(index,1)->text().toInt();
                  yoloConfigure->transport[ago] = now;
              }
              emit finished();
              close();
          }
        );
        // 取消按钮
        connect(
          ui->cancel,
          &QPushButton::clicked,
          this,
          [&]{
              yoloConfigure->name.clear();
              close();
          }
        );
        // 新建按钮
        connect(
          ui->newPair,
          &QPushButton::clicked,
          this,
          [&]{
              ui->tableWidget->insertRow(0);
          }
        );
        // 删除按钮
        connect(
          ui->deletePair,
          &QPushButton::clicked,
          this,
          [&]{
              ui->tableWidget->removeRow(ui->tableWidget->currentRow());
          }
        );
    }

    YoloConfigPage::~YoloConfigPage() {
        delete ui;
    }
} // hzd
