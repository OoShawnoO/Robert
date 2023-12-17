/**
  ******************************************************************************
  * @file           : QRowButton.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/17
  ******************************************************************************
  */

#ifndef ROBERT_QROWBUTTON_H
#define ROBERT_QROWBUTTON_H

#include <QWidget>

namespace hzd {
    QT_BEGIN_NAMESPACE
    namespace Ui { class QRowButton; }
    QT_END_NAMESPACE

    class QRowButton : public QWidget {
    Q_OBJECT

    public:
        explicit QRowButton(const std::string& buttonText,QWidget *parent = nullptr);

        ~QRowButton() override;
    signals:
        void clicked();
    private:
        Ui::QRowButton *ui;
    };
} // hzd

#endif //ROBERT_QROWBUTTON_H
