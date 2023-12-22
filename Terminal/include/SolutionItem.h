/**
  ******************************************************************************
  * @file           : SolutionItem.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/26
  ******************************************************************************
  */

#ifndef ROBERT_SOLUTIONITEM_H
#define ROBERT_SOLUTIONITEM_H

#include <QWidget>
#include <QJsonObject>
#include "Configure.h"

namespace hzd {
    QT_BEGIN_NAMESPACE
    namespace Ui { class SolutionItem; }
    QT_END_NAMESPACE

    class SolutionItem : public QWidget {
    Q_OBJECT

    public:
        explicit SolutionItem(QWidget *parent = nullptr,int id = 0);
        int id{0};
        ~SolutionItem() override;
        ConfigurePackage configurePackage;
        QJsonObject      editorFlowJson;
        void HideRunButton();
        signals:
        void editConfigSignal();
        void editMissionSignal();
        void runSignal();
        void stopSignal();
        void deleteSignal();
    public slots:
        void updateUI();
    private:
        Ui::SolutionItem *ui;
    };
} // hzd

#endif //ROBERT_SOLUTIONITEM_H
