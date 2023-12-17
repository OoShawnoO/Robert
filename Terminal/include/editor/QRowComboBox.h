/**
  ******************************************************************************
  * @file           : QRowComboBox.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/15
  ******************************************************************************
  */

#ifndef ROBERT_QROWCOMBOBOX_H
#define ROBERT_QROWCOMBOBOX_H

#include <QWidget>

namespace hzd {
    QT_BEGIN_NAMESPACE
    namespace Ui { class QRowComboBox; }
    QT_END_NAMESPACE

    class QRowComboBox : public QWidget {
    Q_OBJECT

    public:
        explicit QRowComboBox(
                const std::string& labelText,
                const std::vector<std::string>& comboItems,
                QWidget *parent = nullptr
        );

        ~QRowComboBox() override;

        std::string GetCurrentText() const;
        void SetCurrentText(QString text);
        void SetComboItems(const std::vector<std::string>& comboItems);
    signals:
        void currentTextChanged();
    private:
        Ui::QRowComboBox *ui;
    };
} // hzd

#endif //ROBERT_QROWCOMBOBOX_H
