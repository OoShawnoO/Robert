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
#include "YoloConfigPage.h"

namespace hzd {
    ConfigPage::ConfigPage(
            QWidget *parent,
            ConfigurePackage *_configurePackage
    )
    :QWidget(parent),
    configurePackage(_configurePackage),
    ui(new Ui::ConfigPage)
    {
        ui->setupUi(this);
        setAttribute(Qt::WA_StyledBackground);

        if(configurePackage) {
            auto& logConfigure = configurePackage->logConfigure;
            auto& serverConfigure = configurePackage->serverConfigure;
            auto& missionReactorConfigure = configurePackage->missionReactorConfigure;
            auto& interflowConfigure = configurePackage->interflowConfigure;
            auto& yoloConfigures = configurePackage->yoloConfigures;

            ui->nameEdit->setText(configurePackage->name.c_str());
            ui->descEdit->setText(configurePackage->description.c_str());
            ui->logEdit->setText(logConfigure.saveRootPath.c_str());
            ui->ipEdit->setText(serverConfigure.ip.c_str());
            ui->portEdit->setText(std::to_string(serverConfigure.port).c_str());
            ui->epollEdit->setText(std::to_string(serverConfigure.epollTimeout).c_str());
            ui->reactorEdit->setText(std::to_string(serverConfigure.reactorCount).c_str());
            if(serverConfigure.isEdgeTrigger){
                ui->etCheckBox->setCheckState(Qt::CheckState::Checked);
            }else{
                ui->etCheckBox->setCheckState(Qt::CheckState::Unchecked);
            }
            if(serverConfigure.isOneShot){
                ui->oneshotCheckBox->setCheckState(Qt::CheckState::Checked);
            }else{
                ui->oneshotCheckBox->setCheckState(Qt::CheckState::Unchecked);
            }
            ui->fileEdit->setText(missionReactorConfigure.missionFilePath.c_str());
            ui->analysisEdit->setText(std::to_string(missionReactorConfigure.analysisFrameCount).c_str());
            ui->startEdit->setText(std::to_string(missionReactorConfigure.startTolerance).c_str());
            ui->endEdit->setText(std::to_string(missionReactorConfigure.endTolerance).c_str());
            if(interflowConfigure.isProducer){
                ui->producerCheckBox->setCheckState(Qt::CheckState::Checked);
            }else{
                ui->producerCheckBox->setCheckState(Qt::CheckState::Unchecked);
            }
            if(interflowConfigure.isTcp){
                ui->tcpCheckBox->setCheckState(Qt::CheckState::Checked);
            }else{
                ui->tcpCheckBox->setCheckState(Qt::CheckState::Unchecked);
            }
            ui->shareEdit->setText(interflowConfigure.shareKey.c_str());
            ui->producerEdit->setText(interflowConfigure.producerSemKey.c_str());
            ui->consumerEdit->setText(interflowConfigure.consumerSemKey.c_str());
            ui->myIpEdit->setText(interflowConfigure.myIpAddr.c_str());
            ui->myPortEdit->setText(std::to_string(interflowConfigure.myPort).c_str());
            ui->destIpEdit->setText(interflowConfigure.destIpAddr.c_str());
            ui->destPortEdit->setText(std::to_string(interflowConfigure.destPort).c_str());
            tempYoloConfigures = yoloConfigures;

            for(const auto& yoloConfigure : tempYoloConfigures) {
                ui->modelList->addItem(yoloConfigure.name.c_str());
            }
        }

        // 完成按钮
        connect(
        ui->finish,
        &QPushButton::clicked,
        this,
        [&]
        {
            configurePackage->name = ui->nameEdit->text().toStdString();
            configurePackage->description = ui->descEdit->text().toStdString();

            auto& logConfigure = configurePackage->logConfigure;
            auto& serverConfigure = configurePackage->serverConfigure;
            auto& missionReactorConfigure = configurePackage->missionReactorConfigure;
            auto& interflowConfigure = configurePackage->interflowConfigure;
            auto& yoloConfigures = configurePackage->yoloConfigures;

            logConfigure.saveRootPath = ui->logEdit->text().toStdString();
            serverConfigure.ip = ui->ipEdit->text().toStdString();
            serverConfigure.port = ui->portEdit->text().toUInt();
            serverConfigure.epollTimeout = ui->epollEdit->text().toInt();
            serverConfigure.reactorCount = ui->reactorEdit->text().toInt();
            serverConfigure.isEdgeTrigger = ui->etCheckBox->isChecked();
            serverConfigure.isOneShot = ui->oneshotCheckBox->isChecked();
            missionReactorConfigure.missionFilePath = ui->fileEdit->text().toStdString();
            missionReactorConfigure.analysisFrameCount = ui->analysisEdit->text().toInt();
            missionReactorConfigure.startTolerance = ui->startEdit->text().toDouble();
            missionReactorConfigure.endTolerance = ui->endEdit->text().toDouble();
            interflowConfigure.isProducer = ui->producerCheckBox->isChecked();
            interflowConfigure.isTcp = ui->tcpCheckBox->isChecked();
            interflowConfigure.shareKey = ui->shareEdit->text().toStdString();
            interflowConfigure.producerSemKey = ui->producerEdit->text().toStdString();
            interflowConfigure.consumerSemKey = ui->consumerEdit->text().toStdString();
            interflowConfigure.myIpAddr = ui->myIpEdit->text().toStdString();
            interflowConfigure.myPort = ui->myPortEdit->text().toUInt();
            interflowConfigure.destIpAddr = ui->destIpEdit->text().toStdString();
            interflowConfigure.destPort = ui->destPortEdit->text().toUInt();
            yoloConfigures = std::move(tempYoloConfigures);

            emit finish();

            close();
        });
        // 取消按钮
        connect(
          ui->cancel,
          &QPushButton::clicked,
          this,
          [&]{
              tempYoloConfigures.clear();
              close();
          }
        );
        // 新建按钮
        connect(
            ui->newModel,
            &QPushButton::clicked,
            this,
            [&]{
                tempYoloConfigures.emplace_back();
                auto* yoloConfigPage = new YoloConfigPage(nullptr,&tempYoloConfigures.back());
                connect(
                        yoloConfigPage,
                        &YoloConfigPage::finished,
                        this,
                        [&]{
                            if(!tempYoloConfigures.back().name.empty()){
                                ui->modelList->addItem(tempYoloConfigures.back().name.c_str());
                            }
                        }
                );
                yoloConfigPage->show();
            }
        );
        // 删除按钮
        connect(
          ui->deleteModel,
          &QPushButton::clicked,
          this,
          [&]{
              tempYoloConfigures.erase(tempYoloConfigures.begin() + ui->modelList->currentIndex().row());
              delete ui->modelList->currentItem();
          }
        );
        // item双击
        connect(
            ui->modelList,
            &QListWidget::itemDoubleClicked,
            this,
            [&]{
                auto* yoloConfigPage = new YoloConfigPage(nullptr,&tempYoloConfigures[ui->modelList->currentIndex().row()]);
                yoloConfigPage->show();
            }
        );
    }

    ConfigPage::~ConfigPage() {
        delete ui;
    }
} // hzd
