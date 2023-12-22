/**
  ******************************************************************************
  * @file           : homepage.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/26
  ******************************************************************************
  */

#ifndef ROBERT_HOMEPAGE_H
#define ROBERT_HOMEPAGE_H

#include <QWidget>
#include "SolutionItem.h"
#include "Editor.h"
#include "LoginForm.h"

namespace hzd {

    QT_BEGIN_NAMESPACE
    namespace Ui { class HomePage; }
    QT_END_NAMESPACE

    class HomePage : public QWidget {
    Q_OBJECT
    public:
        explicit HomePage(QWidget *parent = nullptr);
        ~HomePage() override;
        void AddSolution(const std::string& path = "");
    signals:
        void stop();
        void config(int solutionId,ConfigurePackage configurePackage,QJsonObject flowJson);
    private:
        Ui::HomePage *ui;
        int solutionIndex{0};
        int lastSolutionIndex{-1};
        std::unordered_map<int,SolutionItem*> solutionMap;
    };
} // hzd

#endif //ROBERT_HOMEPAGE_H
