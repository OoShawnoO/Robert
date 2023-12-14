/**
  ******************************************************************************
  * @file           : ConfigPage.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/6
  ******************************************************************************
  */

#ifndef ROBERT_CONFIGPAGE_H
#define ROBERT_CONFIGPAGE_H

#include <QWidget>
#include <Configure.h>

namespace hzd {
    QT_BEGIN_NAMESPACE
    namespace Ui { class ConfigPage; }
    QT_END_NAMESPACE

    class ConfigPage : public QWidget {
    Q_OBJECT

    public:
        explicit ConfigPage(
                QWidget *parent = nullptr,
                ConfigurePackage *configurePackage = nullptr
        );

        ~ConfigPage() override;
        signals:
        void finish();
    private:
        ConfigurePackage* configurePackage{nullptr};
        std::vector<YoloConfigure> tempYoloConfigures;
        Ui::ConfigPage *ui;
    };
} // hzd

#endif //ROBERT_CONFIGPAGE_H
