/**
  ******************************************************************************
  * @file           : VideoWidget.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/18
  ******************************************************************************
  */

// You may need to build the project (run Qt uic code generator) to get "ui_VideoWidget.h" resolved

#include <opencv2/imgproc.hpp>
#include <QPainter>
#include "VideoWidget.h"
#include "ui_VideoWidget.h"

namespace hzd {
    VideoWidget::VideoWidget(QWidget *parent) :
            QOpenGLWidget(parent), ui(new Ui::VideoWidget) {
        ui->setupUi(this);
        qRegisterMetaType<cv::Mat>("cv::Mat");
        connect(&VideoThread::Get(),&VideoThread::newFrame,this, &VideoWidget::paintFrame);
    }

    VideoWidget::~VideoWidget() {
        delete ui;
    }

    void VideoWidget::paintFrame(cv::Mat mat) {
        if(frame.isNull()) {
            auto* buffer = new uchar[height()*width()*3];
            frame = QImage(buffer,width(),height(),QImage::Format_RGB888);
        }
        cv::resize(mat,mat,cv::Size(frame.size().width(),frame.size().height()));
        cv::cvtColor(mat,mat,cv::COLOR_BGR2RGB);
        memcpy(frame.bits(),mat.data,mat.cols* mat.rows* mat.elemSize());
        update();
    }

    void VideoWidget::paintEvent(QPaintEvent *e) {
        p.begin(this);
        p.drawImage(QPoint(0,0),frame);
        p.end();
    }

    VideoThread::VideoThread() {
        isStop = false;
        start();
    }

    void VideoThread::run() {
        cv::Mat mat(1920,1080,CV_8UC3,cv::Scalar{0,0,0});
        emit newFrame(mat);
    }

    VideoThread &VideoThread::Get() {
        static VideoThread thread;
        return thread;
    }

    VideoThread::~VideoThread() {

    }

} // hzd
