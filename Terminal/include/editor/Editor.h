/**
  ******************************************************************************
  * @file           : Editor.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/17
  ******************************************************************************
  */

#ifndef ROBERT_EDITOR_H
#define ROBERT_EDITOR_H

#include <QMainWindow>
#include <ConnectionStyle>
#include <DataFlowGraphModel>
#include <DataFlowGraphicsScene>
#include <GraphicsView>
#include <NodeData>
#include <NodeDelegateModelRegistry>

#include "Scene.h"
#include "SolutionItem.h"

using QtNodes::ConnectionStyle;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;
using QtNodes::NodeDelegateModelRegistry;

namespace hzd {
    QT_BEGIN_NAMESPACE
    namespace Ui { class Editor; }
    QT_END_NAMESPACE

    class Editor : public QMainWindow {
    Q_OBJECT

    public:
        explicit Editor(SolutionItem& solutionItem,QWidget *parent = nullptr);
        ~Editor() override;

    private:
        Ui::Editor *ui;
        std::shared_ptr<NodeDelegateModelRegistry> registry;
        std::shared_ptr<DataFlowGraphModel>        dataFlowGraphModel;
        std::shared_ptr<Scene>                     scene;
        std::shared_ptr<GraphicsView>              view;
    };
} // hzd

#endif //ROBERT_EDITOR_H
