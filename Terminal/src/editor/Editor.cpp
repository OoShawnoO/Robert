/**
  ******************************************************************************
  * @file           : Editor.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/17
  ******************************************************************************
  */

// You may need to build the project (run Qt uic code generator) to get "ui_Editor.h" resolved

#include "Editor.h"
#include "ui_Editor.h"
#include "BaseModel.h"
#include <QScreen>

namespace hzd {
    Editor::Editor(SolutionItem& solutionItem,QWidget *parent) :
            QMainWindow(parent), ui(new Ui::Editor) {
        ui->setupUi(this);

        ConnectionStyle::setConnectionStyle(
                R"(
          {
            "ConnectionStyle": {
              "ConstructionColor": "gray",
              "NormalColor": "black",
              "SelectedColor": "gray",
              "SelectedHaloColor": "deepskyblue",
              "HoveredColor": "deepskyblue",

              "LineWidth": 3.0,
              "ConstructionLineWidth": 2.0,
              "PointDiameter": 10.0,

              "UseDataDefinedColors": true
            }
          }
        )"
        );

        registry = Scene::registerDataModels();
        dataFlowGraphModel = std::make_shared<DataFlowGraphModel>(registry);
        scene = new Scene(*dataFlowGraphModel,solutionItem,this);
        view = std::make_shared<GraphicsView>(scene,this);
        ui->verticalLayout->addWidget(view.get());
        adjustSize();

        scene->Load();
        connect(ui->loadAction, &QAction::triggered, scene, &DataFlowGraphicsScene::load);
        connect(ui->saveAnotherAction,&QAction::triggered,scene,&Scene::SaveAndGenerate);
        connect(ui->saveAction,&QAction::triggered,scene,&Scene::Save);
        connect(scene, &DataFlowGraphicsScene::sceneLoaded, view.get(), &GraphicsView::centerScene);

        this->move(QApplication::primaryScreen()->availableGeometry().center()
                   - this->rect().center());
    }

    Editor::~Editor() {
        delete ui;
    }
} // hzd
