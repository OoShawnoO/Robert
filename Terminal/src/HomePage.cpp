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
#include "HomePage.h"
#include "ui_HomePage.h"
#include "SolutionItem.h"
#include "ConfigPage.h"
#include "YoloConfigPage.h"

namespace hzd {
#define MARGIN_MIN_SIZE 0
#define MARGIN_MAX_SIZE 20


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

        connect(
          ui->addSolution,
          &QPushButton::clicked,
          this,
          [&]{
            auto* solution = new SolutionItem(nullptr,solutionIndex);
            solutionMap[solutionIndex] = solution;
            ui->solutions->insertWidget(0,solution);
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
            solutionIndex++;
          }
        );
    }

    HomePage::~HomePage() {
        delete ui;
    }
} // hzd
