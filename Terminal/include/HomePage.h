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

namespace hzd {
    QT_BEGIN_NAMESPACE
    namespace Ui { class HomePage; }
    QT_END_NAMESPACE

    class HomePage : public QWidget {
    Q_OBJECT

    public:
        explicit HomePage(QWidget *parent = nullptr);

        ~HomePage() override;

    private:
        Ui::HomePage *ui;
    };
} // hzd

#endif //ROBERT_HOMEPAGE_H
