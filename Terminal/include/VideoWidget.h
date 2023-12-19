/**
  ******************************************************************************
  * @file           : VideoWidget.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/18
  ******************************************************************************
  */

#ifndef ROBERT_VIDEOWIDGET_H
#define ROBERT_VIDEOWIDGET_H

#include <QThread>
#include <QOpenGLWidget>
#include "LoginForm.h"

namespace hzd {

    class VideoThread : public QThread {
    Q_OBJECT
        std::mutex mtx;
        int currentSolutionId{-1};
        std::string configJsonStr;
        QJsonObject flowJson;
    public:
        Client client;
        bool isStop{false};
        bool isRun{true};
        bool isConfig{false};
        void run();
        void Wait();
        VideoThread();
        ~VideoThread();
    public slots:
        void stop();
        void config(int solutionId,std::string configJsonStr,QJsonObject flowJson);
    signals:
        void newFrame(cv::Mat mat);
    };

    QT_BEGIN_NAMESPACE
    namespace Ui { class VideoWidget; }
    QT_END_NAMESPACE

    class VideoWidget : public QOpenGLWidget {
    Q_OBJECT
    public:
        std::shared_ptr<VideoThread>     videoThread;

        explicit VideoWidget(QWidget *parent = nullptr);
        ~VideoWidget() override;
        void paintEvent(QPaintEvent* e);
    public slots:
        void paintFrame(cv::Mat mat);
    private:
        Ui::VideoWidget *ui;
        QImage frame;
        QPainter p;
    };
} // hzd

#endif //ROBERT_VIDEOWIDGET_H
