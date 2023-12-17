/**
  ******************************************************************************
  * @file           : QRowEdit.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/15
  ******************************************************************************
  */

#ifndef ROBERT_QROWEDIT_H
#define ROBERT_QROWEDIT_H

#include <QWidget>

namespace hzd {
    QT_BEGIN_NAMESPACE
    namespace Ui { class QRowEdit; }
    QT_END_NAMESPACE

    class QRowEdit : public QWidget {
    Q_OBJECT

    public:
        explicit QRowEdit(
                const std::string& labelText,
                QWidget *parent = nullptr
        );

        ~QRowEdit() override;
        std::string GetEditText() const;
        void setReadOnly(bool yes);
        void SetEditText(const std::string& text);
        void SetCurrentText(QString text);
    private:
        Ui::QRowEdit *ui;
    };
} // hzd

#endif //ROBERT_QROWEDIT_H
