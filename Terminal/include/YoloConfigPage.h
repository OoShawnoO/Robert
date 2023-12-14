/**
  ******************************************************************************
  * @file           : YoloConfigPage.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/7
  ******************************************************************************
  */

#ifndef ROBERT_YOLOCONFIGPAGE_H
#define ROBERT_YOLOCONFIGPAGE_H

#include <QWidget>
#include <Configure.h>

namespace hzd {
    QT_BEGIN_NAMESPACE
    namespace Ui { class YoloConfigPage; }
    QT_END_NAMESPACE

    class YoloConfigPage : public QWidget {
    Q_OBJECT

    public:
        explicit YoloConfigPage(
                QWidget *parent = nullptr,
                YoloConfigure* yoloConfigPage = nullptr
        );

        ~YoloConfigPage() override;
    signals:
        void finished();

    private:
        Ui::YoloConfigPage *ui;
        YoloConfigure* yoloConfigure;
    };
} // hzd

#endif //ROBERT_YOLOCONFIGPAGE_H
