/**
  ******************************************************************************
  * @file           : homepage.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/26
  ******************************************************************************
  */

// You may need to build the project (run Qt uic code generator) to get "ui_HomePage.h" resolved

#include <QScrollBar>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QGraphicsDropShadowEffect>
#include <dirent.h>
#include <iostream>
#include <QMessageBox>
#include <QJsonArray>
#include "HomePage.h"
#include "ui_HomePage.h"
#include "SolutionItem.h"
#include "ConfigPage.h"
#include "Editor.h"
#include "BaseModel.h"

namespace hzd {

    void AddTableItem(QTableWidget* table,const QString& time,const QString& procedure,bool isSuccess,const QString& reason = "") {
        table->insertRow(0);
        table->setItem(0,0,new QTableWidgetItem(time));
        table->setItem(0,1,new QTableWidgetItem(procedure));
        table->setItem(0,2,new QTableWidgetItem(isSuccess ? "success" : "failed"));
        table->setItem(0,3,new QTableWidgetItem(reason));
        for(int i=0;i<4;i++) {
            table->item(0,i)->setTextAlignment(Qt::AlignCenter);
        }
        table->resizeColumnsToContents();
    }

    HomePage::HomePage(QWidget *parent) :
            QWidget(parent), ui(new Ui::HomePage) {

        ui->setupUi(this);
        /* region scroll area style */
        ui->resultTable
        ->verticalScrollBar()
        ->setStyleSheet("QScrollBar:vertical {\n"
                        "          border-width: 0px;\n"
                        "          border: none;\n"
                        "          background:rgba(64, 65, 79, 0);\n"
                        "          width:12px;\n"
                        "          margin: 0px 0px 0px 0px;\n"
                        "      }\n"
                        "      QScrollBar::handle:vertical {\n"
                        "          background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
                        "          stop: 0 #aaaaff, stop: 0.5 #aaaaff, stop:1 #aaaaff);\n"
                        "          min-height: 20px;\n"
                        "          max-height: 20px;\n"
                        "          margin: 0 0px 0 0px;\n"
                        "          border-radius: 6px;\n"
                        "      }\n"
                        "      QScrollBar::add-line:vertical {\n"
                        "          background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
                        "          stop: 0 rgba(64, 65, 79, 0), stop: 0.5 rgba(64, 65, 79, 0),  stop:1 rgba(64, 65, 79, 0));\n"
                        "          height: 0px;\n"
                        "          border: none;\n"
                        "          subcontrol-position: bottom;\n"
                        "          subcontrol-origin: margin;\n"
                        "      }\n"
                        "      QScrollBar::sub-line:vertical {\n"
                        "          background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
                        "          stop: 0  rgba(64, 65, 79, 0), stop: 0.5 rgba(64, 65, 79, 0),  stop:1 rgba(64, 65, 79, 0));\n"
                        "          height: 0 px;\n"
                        "          border: none;\n"
                        "          subcontrol-position: top;\n"
                        "          subcontrol-origin: margin;\n"
                        "      }\n"
                        "      QScrollBar::sub-page:vertical {\n"
                        "      background: rgba(64, 65, 79, 0);\n"
                        "      }\n"
                        "\n"
                        "      QScrollBar::add-page:vertical {\n"
                        "      background: rgba(64, 65, 79, 0);\n"
                        "      }");
        ui->scrollArea
        ->verticalScrollBar()
        ->setStyleSheet("QScrollBar:vertical {\n"
               "          border-width: 0px;\n"
               "          border: none;\n"
               "          background:rgba(64, 65, 79, 0);\n"
               "          width:12px;\n"
               "          margin: 0px 0px 0px 0px;\n"
               "      }\n"
               "      QScrollBar::handle:vertical {\n"
               "          background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
               "          stop: 0 #aaaaff, stop: 0.5 #aaaaff, stop:1 #aaaaff);\n"
               "          min-height: 20px;\n"
               "          max-height: 20px;\n"
               "          margin: 0 0px 0 0px;\n"
               "          border-radius: 6px;\n"
               "      }\n"
               "      QScrollBar::add-line:vertical {\n"
               "          background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
               "          stop: 0 rgba(64, 65, 79, 0), stop: 0.5 rgba(64, 65, 79, 0),  stop:1 rgba(64, 65, 79, 0));\n"
               "          height: 0px;\n"
               "          border: none;\n"
               "          subcontrol-position: bottom;\n"
               "          subcontrol-origin: margin;\n"
               "      }\n"
               "      QScrollBar::sub-line:vertical {\n"
               "          background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
               "          stop: 0  rgba(64, 65, 79, 0), stop: 0.5 rgba(64, 65, 79, 0),  stop:1 rgba(64, 65, 79, 0));\n"
               "          height: 0 px;\n"
               "          border: none;\n"
               "          subcontrol-position: top;\n"
               "          subcontrol-origin: margin;\n"
               "      }\n"
               "      QScrollBar::sub-page:vertical {\n"
               "      background: rgba(64, 65, 79, 0);\n"
               "      }\n"
               "\n"
               "      QScrollBar::add-page:vertical {\n"
               "      background: rgba(64, 65, 79, 0);\n"
               "      }");
    /* endregion */

        DIR    *dir;
        struct    dirent    *ptr;
        dir = opendir("solutions");
        while((ptr = readdir(dir)) != nullptr)
        {
            if(std::string(ptr->d_name) == "." || std::string(ptr->d_name) == "..") continue;
            if(ptr->d_type == DT_DIR){
                AddSolution(std::string("./solutions/") + ptr->d_name + "/" + ptr->d_name);
            }
        }
        closedir(dir);

        // 添加方案
        connect(
          ui->addSolution,
          &QPushButton::clicked,
          this,
          [&] {
              AddSolution();
          }
        );

        qRegisterMetaType<cv::Mat>("cv::Mat");
        connect(
          ui->videoWidget->videoThread.get(),
          &VideoThread::newFrame,
          ui->videoWidget,
          &VideoWidget::paintFrame
        );

        connect(
          this,
          &HomePage::config,
          ui->videoWidget->videoThread.get(),
          &VideoThread::config
        );

        connect(
                this,
                &HomePage::stop,
                ui->videoWidget->videoThread.get(),
                &VideoThread::stop
        );

        auto* loginForm = new LoginForm(ui->videoWidget->videoThread->client,this);
        loginForm->show();
        ui->videoWidget->videoThread->start();
    }

    HomePage::~HomePage() {
        ui->videoWidget->videoThread->Wait();
        for(auto& solution : solutionMap) {
            const auto& name = solution.second->configurePackage.name;
            auto dirPath = "./solutions/" + name + "/";
            if(!std::filesystem::exists(dirPath) && !std::filesystem::create_directories(dirPath)){
                QMessageBox::critical(nullptr,"错误","无法创建目录");
                continue;
            }
            solution.second->configurePackage.Serialize(dirPath + name + ".json");
            auto json = QJsonDocument(solution.second->editorFlowJson).toJson();
            QFile file((dirPath + name + ".flow").c_str());
            if(!file.open(QIODevice::WriteOnly)){
                QMessageBox::critical(nullptr,"错误","无法创建文件");
                continue;
            }
            file.write(json);
            file.close();
            Scene::Generate(solution.second->editorFlowJson,dirPath + name);
        }
        delete ui;
    }

    void HomePage::AddSolution(const std::string& path) {
        auto* solution = new SolutionItem(nullptr,solutionIndex);
        solutionMap[solutionIndex] = solution;
        if(!path.empty()){
            if(!solution->configurePackage.Deserialize(path + ".json")) return;
            QFile file((path + ".flow").c_str());
            if(!file.open(QIODevice::ReadOnly)) return;
            solution->editorFlowJson = QJsonDocument::fromJson(file.readAll()).object();
            file.close();
            auto nodes = solution->editorFlowJson["nodes"];
            auto nodesArray = nodes.toArray();
            for(const auto& node : nodesArray) {
                auto internalData = node.toObject()["internal-data"].toObject();
                QString modelName = internalData["model-name"].toString();
                if(modelName == "结算器") {
                    BaseModel::broadcaster.Deserialize(internalData["nameIdMap"]);
                    break;
                }
            }

            solution->updateUI();
            if(std::filesystem::remove_all(path.substr(0,path.find_last_of('/')).c_str()) < 0) {
                QMessageBox::critical(nullptr,"错误",path.substr(0,path.find_last_of('/')).c_str());
                return;
            }
        }
        ui->solutions->insertWidget(0,solution);
        // 编辑配置
        connect(
                solution,
                &SolutionItem::editConfigSignal,
                this,
                [=] {
                    auto* configPage = new ConfigPage(nullptr,&solution->configurePackage);
                    connect(
                            configPage,
                            &ConfigPage::finish,
                            solution,
                            &SolutionItem::updateUI
                    );
                    configPage->show();
                }
        );
        // 编辑任务
        connect(
                solution,
                &SolutionItem::editMissionSignal,
                this,
                [=]{
                    auto* editor = new Editor(*solution);
                    editor->show();
                }
        );
        // 运行
        connect(
                solution,
                &SolutionItem::runSignal,
                this,
                [&,solution]{
                    emit config(solutionIndex,solution->configurePackage.toJson(),solution->editorFlowJson);
                }
        );
        // 暂停
        connect(
          solution,
          &SolutionItem::stopSignal,
          this,
          [&]{
              emit stop();
          }
        );
        // 删除
        connect(
          solution,
          &SolutionItem::deleteSignal,
          this,
          [=, this]{
              solutionMap[solution->id]->hide();
              solutionMap.erase(solution->id);
              ui->solutions->update();
          }
        );
        solutionIndex++;
    }

} // hzd
