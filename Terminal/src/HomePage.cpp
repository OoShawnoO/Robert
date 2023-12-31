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
        // 注册 Meta 数据
        qRegisterMetaType<cv::Mat>("cv::Mat");
        qRegisterMetaType<ConfigurePackage>("ConfigurePackage");
        // table widget column 宽度
        ui->resultTable->setColumnWidth(0,100);
        ui->resultTable->setColumnWidth(1,100);
        ui->resultTable->setColumnWidth(2,100);
        ui->resultTable->setColumnWidth(3,500);
        // 读取已存在方案
        DIR *dir;
        struct dirent *ptr;
        dir = opendir("solutions");
        while ((ptr = readdir(dir)) != nullptr) {
            if (ptr->d_type != DT_DIR) continue;
            std::string name = ptr->d_name;
            if (name == "." || name == "..") continue;
            AddSolution(std::stol(ptr->d_name),true);
        }
        closedir(dir);

        // 添加方案
        connect(
                ui->addSolution,
                &QPushButton::clicked,
                this,
                [&] {
                    AddSolution(time(nullptr));
                }
        );
        // 更新视频显示
        connect(
                ui->videoWidget->videoThread.get(),
                &VideoThread::newFrame,
                ui->videoWidget,
                &VideoWidget::paintFrame
        );
        // 切换运行方案
        connect(
                this,
                &HomePage::config,
                ui->videoWidget->videoThread.get(),
                &VideoThread::config
        );
        // 添加新纪录
        connect(
                ui->videoWidget->videoThread.get(),
                &VideoThread::addTableItem,
                this,
                &HomePage::AddTableItem
        );
        // 暂停运行方案
        connect(
                this,
                &HomePage::stop,
                ui->videoWidget->videoThread.get(),
                &VideoThread::stop
        );
        // 报错信息
        connect(
                ui->videoWidget->videoThread.get(),
                &VideoThread::error,
                this,
                [&](QString errorStr) {
                    QMessageBox::critical(this, "错误", errorStr);
                }
        );
        // 通知信息
        connect(
                ui->videoWidget->videoThread.get(),
                &VideoThread::info,
                this,
                [&](QString infoStr) {
                    QMessageBox::information(this, "info", infoStr);
                }
        );

//        auto* loginForm = new LoginForm(ui->videoWidget->videoThread->client,this);
//        loginForm->show();
        if (!ui->videoWidget->videoThread->client.Connect("127.0.0.1", 9999)) {
            QMessageBox::critical(this, "失败", "连接失败...");
            exit(-1);
        }
        ui->videoWidget->videoThread->client.isConnected = true;
        QMessageBox::information(nullptr, "成功", "连接成功!");

        ui->videoWidget->videoThread->start();
    }

    HomePage::~HomePage() {
        // 等待视频线程退出
        ui->videoWidget->videoThread->Wait();
        delete ui;
    }

    void HomePage::AddSolution(long id,bool isExist) {
        static const std::string rootPath = "./solutions/";
        std::string idStr = std::to_string(id);
        std::string fileNameWithOutTypePath = rootPath + idStr + "/" + idStr;

        if(!isExist) { std::filesystem::create_directories(rootPath + idStr); }

        auto* solution = new SolutionItem(id,fileNameWithOutTypePath);
        solutionMap[id] = solution;
        if(isExist) {
            if(!solution->configurePackage.Deserialize(fileNameWithOutTypePath + ".json")){
                QMessageBox::critical(nullptr,"错误","无法打开配置文件!");
                return;
            }
            // 读取.flow文件
            QFile file((fileNameWithOutTypePath + ".flow").c_str());
            if (file.open(QIODevice::ReadOnly)) {
                solution->editorFlowJson = QJsonDocument::fromJson(file.readAll()).object();
                file.close();
                // 导入flow文件内容
                auto nodes = solution->editorFlowJson["nodes"];
                auto nodesArray = nodes.toArray();
                for (const auto &node: nodesArray) {
                    auto internalData = node.toObject()["internal-data"].toObject();
                    QString modelName = internalData["model-name"].toString();
                    if (modelName == "结算器") {
                        BaseModel::broadcaster.Deserialize(internalData["nameIdMap"]);
                        break;
                    }
                }
            }
            solution->updateUI();
        }
        // 新记录
        ui->solutions->insertWidget(0, solution);

        // 编辑配置
        connect(
            solution,
            &SolutionItem::editConfigSignal,
            this,
            [=] {
                auto *configPage = new ConfigPage(nullptr, &solution->configurePackage);
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
            [=] {
                auto *editor = new Editor(*solution);
                editor->setAttribute(Qt::WA_DeleteOnClose);
                editor->show();
            }
        );
        // 运行
        connect(
            solution,
            &SolutionItem::runSignal,
            this,
            [=, this] {
                if (lastSolutionId != -1 && lastSolutionId != solution->id) {
                    solutionMap[lastSolutionId]->HideRunButton();
                    ClearTableItem();
                }
                emit config(solution->id, solution->configurePackage, solution->editorFlowJson);
                lastSolutionId = solution->id;
            }
        );
        // 暂停
        connect(
            solution,
            &SolutionItem::stopSignal,
            this,
            [&] {
                emit stop();
            }
        );
        // 删除
        connect(
            solution,
            &SolutionItem::deleteSignal,
            this,
            [=, this] {
                solutionMap[solution->id]->hide();
                solutionMap.erase(solution->id);
                ui->solutions->update();
                if(!std::filesystem::remove_all(fileNameWithOutTypePath.substr(0,fileNameWithOutTypePath.find_last_of('/')).c_str())) {
                    QMessageBox::critical(nullptr,"错误",fileNameWithOutTypePath.substr(0,fileNameWithOutTypePath.find_last_of('/')).c_str());
                    return;
                }
            }
        );
    }

    void HomePage::AddTableItem(QString time, QString procedure, bool isSuccess, QString reason) {
        ui->resultTable->insertRow(0);
        ui->resultTable->setItem(0, 0, new QTableWidgetItem(time));
        ui->resultTable->setItem(0, 1, new QTableWidgetItem(procedure));
        ui->resultTable->setItem(0, 2, new QTableWidgetItem(isSuccess ? "success" : "failed"));
        if (isSuccess) {
            ui->resultTable->item(0, 2)->setBackground(QBrush(QColor{0, 255, 0}));
            successCount++;
            ui->successRecord->setText(std::to_string(successCount).c_str());
        } else {
            ui->resultTable->item(0, 2)->setBackground(QBrush(QColor{255, 0, 0}));
            failedCount++;
            ui->successRecord->setText(std::to_string(failedCount).c_str());
        }
        ui->resultTable->setItem(0, 3, new QTableWidgetItem(reason));
        for (int i = 0; i < 4; i++) {
            ui->resultTable->item(0, i)->setTextAlignment(Qt::AlignCenter);
        }
        ui->resultTable->resizeColumnsToContents();
    }

    void HomePage::ClearTableItem() {
        successCount = 0;
        failedCount = 0;
        ui->resultTable->clearContents();
    }

} // hzd
