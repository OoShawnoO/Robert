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

#include <opencv2/videoio.hpp>
#include <QThread>
#include <QOpenGLWidget>
#include "LoginForm.h"

namespace hzd {

    struct WriteProp {
        enum Type{
            Open,Write,Release
        }       type;
        cv::Mat mat;
    };

    class VideoThread : public QThread {
        Q_OBJECT
        std::mutex mtx;
        long currentSolutionId{-1};
        ConfigurePackage configurePackage;
        QJsonObject flowJson;
        int         frameId{0};
        std::thread ioThread{};
        std::string videoName;
        cv::VideoWriter writer;
        BlockChannel<WriteProp> writeChan{};
        bool sendControlPacket(size_t frameId,ControlType type,MarkType mark);
        bool acquireOk();
        void makeFrame(const std::string& text);
    public:
        Client client;
        bool isSaveVideo{false};
        bool isStop{false};
        bool isRun{false};
        bool isConfig{false};
        void run();
        void Wait();
        VideoThread();
        ~VideoThread();
    signals:
        void addTableItem(QString time,QString procedure,bool isSuccess,QString reason);
        void error(QString errorStr);
        void info(QString infoStr);
    public slots:
        void stop();
        void config(long solutionId,ConfigurePackage configurePackage,QJsonObject flowJson);
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
        void paintEvent(QPaintEvent* e) override;
    public slots:
        void paintFrame(cv::Mat mat);
    private:
        Ui::VideoWidget *ui;
        QPainter p;
        QImage frame;
    };
} // hzd

#endif //ROBERT_VIDEOWIDGET_H
