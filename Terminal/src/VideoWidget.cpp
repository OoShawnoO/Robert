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

    const std::string VideoThreadChan = "VideoThreadChan";

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

    void VideoThread::config(int solutionId, ConfigurePackage _configurePackage, QJsonObject _flowJson) {
        mtx.lock();
        if(currentSolutionId != solutionId){
            currentSolutionId = solutionId;
            configurePackage = std::move(_configurePackage);
            isSaveVideo = configurePackage.isSaveVideo;
            flowJson = std::move(_flowJson);
            isConfig = true;
        }
        isRun = true;
        mtx.unlock();
    }

    VideoThread::VideoThread() {
        isStop = false;
        ioThread = std::thread([&] {
            WriteProp writeProp;
            int fourcc = cv::VideoWriter::fourcc('a','v','c','1');
            while(!isStop) {
                if(!writeChan.pop(writeProp)) break;
                switch (writeProp.type) {
                    case WriteProp::Open: {
                        if(!writer.open(videoName,fourcc,25.0,writeProp.mat.size())){
                            LOG_ERROR(VideoThreadChan,"open video failed");
                        }
                    }
                    case WriteProp::Write: {
                        writer.write(writeProp.mat);
                        break;
                    }
                    case WriteProp::Release: {
                        writer.release();
                        break;
                    }
                }
            }
        });
    }

    void VideoThread::run() {
        cv::Mat mat;
        json json;
        std::clock_t clock;
        while(!isStop) {
            // 暂停
            while(!isRun) {
                msleep(20);
            }

            clock = std::clock();

            if(isConfig) {
                std::lock_guard<std::mutex> guard(mtx);
                std::string flag;
                if(!client.isConnected) {
                    error("未连接服务端...");
                    return;
                }
                makeFrame("send control packet...");
                if(!sendControlPacket(0,Config,Right)){
                    error("发送控制包失败...");
                    return;
                }
                makeFrame("send configure file...");
                auto configJsonStr = configurePackage.toJson();
                if(!client.SendWithHeader(configJsonStr.c_str(),configJsonStr.size())){
                    error("发送配置文件失败...");
                    return;
                }
                if(!acquireOk()) {
                    error("接收配置文件响应失败...");
                    return;
                }
                makeFrame("generate and send mission file...");
                Scene::Generate(flowJson,"__temp__");
                if(client.SendFileWithHeader("__temp__.mission") < 0){
                    error("发送任务文件失败...");
                    return;
                }
                if(!acquireOk()) {
                    error("接收传输文件响应失败...");
                    return;
                }
                makeFrame("link interflow...");
                client.pInterflow.reset();
                usleep(100);
                client.pInterflow = std::make_shared<Interflow>(
                    false,
                    configurePackage.interflowConfigure.isTcp,
                    "",
                    0,
                    configurePackage.interflowConfigure.myIpAddr,
                    configurePackage.interflowConfigure.myPort,
                    configurePackage.interflowConfigure.shareKey,
                    configurePackage.interflowConfigure.producerSemKey,
                    configurePackage.interflowConfigure.consumerSemKey
                );
                isConfig = false;
            }
            if(!sendControlPacket(frameId++,Work,Right)){
                error("发送控制包失败!");
                break;
            }
            if(!client.pInterflow->ReceiveItem(mat,json)){
                error("接收帧错误！");
                break;
            }
            emit newFrame(mat);

            if(isSaveVideo && writer.isOpened()) {
                writeChan.push({WriteProp::Write,mat.clone()});
            }

            if(json["code"] != 1) {
                QString time = std::string(json["time"]).c_str();
                QString procedure = std::string(json["procedure"]).c_str();
                int status = json["code"];
                switch(status) {
                    // Success
                    case 1 : {
                        if(isSaveVideo) writeChan.push({WriteProp::Release,{}});
                        emit addTableItem(time,procedure,true,"");
                    }
                        // NotSuccess
                    case 2 : {
                        if(isSaveVideo) writeChan.push({WriteProp::Release,{}});
                        for(const auto& reason : json["failedReason"]) {
                            emit addTableItem(time,procedure,false,std::string(reason).c_str());
                        }
                    }
                        // Start
                    case 3 : {
                        videoName = time.toStdString() + ".mp4";
                        if(isSaveVideo) writeChan.push({WriteProp::Open,mat.clone()});
                    }
                    default : {
                        break;
                    }
                }
            }

            clock = std::clock() - clock;
            clock = 34 - clock/1000;
            clock = clock > 0 ? clock : 0;
            msleep(clock);

        }
        writeChan.push();
    }

    void VideoThread::Wait() {
        isStop = true;
        isRun = true;
        wait();
        if(ioThread.joinable()) ioThread.join();
    }

    void VideoThread::stop() {
        mtx.lock();
        isRun = false;
        mtx.unlock();
    }

    bool VideoThread::sendControlPacket(size_t frameId, ControlType type, MarkType mark) {
        ControlPacket packet{frameId,type,mark};
        return client.Send((char*)&packet,sizeof(packet)) > 0;
    }

    bool VideoThread::acquireOk() {
        std::string flag;
        return !(client.Recv(flag,2,false) < 0 || flag != "ok");
    }

    void VideoThread::makeFrame(const std::string& text) {
        cv::Mat mat(1920,1080,CV_8UC3,cv::Scalar{26,26,26});
        cv::putText(mat,text,{20,800},cv::FONT_HERSHEY_DUPLEX,4,{255,255,255},4,cv::LINE_AA);
        emit newFrame(mat);
    }

    VideoThread::~VideoThread() = default;

} // hzd
