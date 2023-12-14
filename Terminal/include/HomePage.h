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
    private:
        Ui::HomePage *ui;
        int solutionIndex{0};
        std::unordered_map<int,SolutionItem*> solutionMap;
    };
} // hzd

#endif //ROBERT_HOMEPAGE_H
