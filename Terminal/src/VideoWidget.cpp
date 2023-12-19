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
#include <opencv2/videoio.hpp>
#include <QJsonObject>
#include <QMessageBox>
#include "VideoWidget.h"
#include "ui_VideoWidget.h"
#include "Scene.h"

namespace hzd {
    VideoWidget::VideoWidget(QWidget *parent) :
            QOpenGLWidget(parent), ui(new Ui::VideoWidget) {
        ui->setupUi(this);
        videoThread = std::make_shared<VideoThread>();
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

    void VideoThread::config(int solutionId, std::string _configJsonStr, QJsonObject _flowJson) {
        mtx.lock();
//        currentSolutionId = solutionId;
//        configJsonStr = std::move(_configJsonStr);
//        flowJson = std::move(_flowJson);
        isRun = true;
//        isConfig = true;
        mtx.unlock();
    }

    VideoThread::VideoThread() {
        isStop = false;
    }

    void VideoThread::run() {
        cv::VideoCapture videoCapture("/mnt/c/Users/84083/Desktop/毕设/3.mp4");
        cv::Mat mat;
        while(!isStop) {
            // 暂停
            while(!isRun) {
//                std::lock_guard<std::mutex> guard(mtx);
                msleep(10);
            }
            mtx.lock();
            if(!videoCapture.read(mat)) {
                videoCapture.open("/mnt/c/Users/84083/Desktop/毕设/3.mp4");
                videoCapture.read(mat);
            }
            mtx.unlock();
            emit newFrame(mat);
            msleep(40);

            /*
            if(isConfig) {
                if(!client.isConnected) {
                    QMessageBox::critical(nullptr,"错误","未连接服务端...");
                    return;
                }
                if(!client.SendWithHeader(configJsonStr.c_str(),configJsonStr.size())){
                    QMessageBox::critical(nullptr,"错误","发送配置文件失败.");
                    return;
                }
                std::string flag;
                if(client.Recv(flag,2,false) < 0) {
                    QMessageBox::critical(nullptr,"错误","接收配置文件响应失败.");
                    return;
                }
                if(flag != "ok") {
                    QMessageBox::critical(nullptr,"错误","配置文件响应失败.");
                    return;
                }
                Scene::Generate(flowJson,"__temp__.mission");
                if(client.SendFile("__temp__.mission") < 0){
                    QMessageBox::critical(nullptr,"错误","发送任务文件失败.");
                    return;
                }
                isConfig = false;
            }
            */
        }
    }

    void VideoThread::Wait() {
        isStop = true;
        isRun = true;
        wait();
    }

    void VideoThread::stop() {
        mtx.lock();
        isRun = false;
        mtx.unlock();
    }

    VideoThread::~VideoThread() = default;

} // hzd
