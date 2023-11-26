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

namespace hzd {
    QT_BEGIN_NAMESPACE
    namespace Ui { class SolutionItem; }
    QT_END_NAMESPACE

    class SolutionItem : public QWidget {
    Q_OBJECT

    public:
        explicit SolutionItem(QWidget *parent = nullptr);

        ~SolutionItem() override;

    private:
        Ui::SolutionItem *ui;
    };
} // hzd

#endif //ROBERT_SOLUTIONITEM_H
