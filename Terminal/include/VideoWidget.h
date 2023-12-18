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

namespace hzd {

    class VideoThread : public QThread {
    Q_OBJECT
        std::mutex mtx;
    protected:
        VideoThread();
    public:
        bool isStop{false};
        void run();
        static VideoThread& Get();
        VideoThread& operator=(VideoThread&) = delete;
        VideoThread(const VideoThread&) = delete;
        ~VideoThread();
    signals:
        void newFrame(cv::Mat mat);
    };

    QT_BEGIN_NAMESPACE
    namespace Ui { class VideoWidget; }
    QT_END_NAMESPACE

    class VideoWidget : public QOpenGLWidget {
    Q_OBJECT
    public:
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
