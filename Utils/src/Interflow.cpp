/**
  ******************************************************************************
  * @file           : Interflow.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/18
  ******************************************************************************
  */


#include <cstring>                  /* memcpy */
#include <fcntl.h>                  /* fcntl */
#include <sys/sendfile.h>           /* sendfile */
#include <sys/mman.h>               /* shm_open */
#include <iostream>                 /* cerr */
#include <opencv2/imgcodecs.hpp>    /* imencode imdecode */

#include "Interflow.h"              /* Interflow Socket TcpSocket UdpSocket */


namespace hzd {


    ShareMemory::~ShareMemory() {
        if(shareKey.empty() || producerSemKey.empty() || consumerSemKey.empty()) return;

        sem_close(producerSem);
        sem_close(consumerSem);
        munmap(sharePtr,shareCapacity);
        sharePtr = nullptr;
        sem_unlink(producerSemKey.c_str());
        sem_unlink(consumerSemKey.c_str());
        shm_unlink(shareKey.c_str());
        close(shareId);
    }

    ShareMemory::ShareMemory(
            bool              _isProducer,
            const std::string &_shareKey,
            long              capacity,
            const std::string &_producerSemKey,
            const std::string &_consumerSemKey
    )
    {
        if(_shareKey.empty() || _producerSemKey.empty() || _consumerSemKey.empty()) return;

        isProducer = _isProducer;

        shareId = shm_open(_shareKey.c_str(),
                           O_CREAT | O_RDWR,
                           S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);
        if(shareId == -1) {
            std::cerr << "failed to create share memory" << std::endl;
            perror("shm_open");
            exit(-1);
        }
        shareKey = _shareKey;

        ftruncate(shareId,capacity);

        sharePtr = static_cast<unsigned char*>(
                mmap(
                        nullptr,
                        capacity,
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED,
                        shareId,
                        0
                )
        );
        if(sharePtr == MAP_FAILED) {
            std::cerr << "failed to map share memory" << std::endl;
            shm_unlink(shareKey.c_str());
            close(shareId);
            shareId = -1;
            exit(-1);
        }
        shareCapacity = capacity;

        producerSem = sem_open(_producerSemKey.c_str(),O_CREAT,0666,1);
        if(producerSem == SEM_FAILED) {
            std::cerr << "failed to create producer semaphore" << std::endl;
            perror("sem_open");

            munmap(sharePtr,shareCapacity);
            sharePtr = nullptr;
            shm_unlink(shareKey.c_str());
            close(shareId);
            shareId = -1;
            exit(-1);
        }
        producerSemKey = _producerSemKey;

        consumerSem = sem_open(_consumerSemKey.c_str(),O_CREAT,0666,0);
        if(consumerSem == SEM_FAILED) {
            std::cerr << "failed to create consumer semaphore" << std::endl;
            perror("sem_open");

            sem_unlink(producerSemKey.c_str());
            munmap(sharePtr,shareCapacity);
            sharePtr = nullptr;
            shm_unlink(shareKey.c_str());
            close(shareId);
            shareId = -1;
            exit(-1);
        }
        consumerSemKey = _consumerSemKey;

        if(!isProducer) {
            while(TryWaitConsumerSem());
            PostProducerSem();
        }
    }

    unsigned char *ShareMemory::GetShareMemory() {
        return sharePtr;
    }

    void ShareMemory::Clear() {
        bzero(sharePtr,shareCapacity);
    }

    const std::string InterflowChan = "Interflow";

    Interflow::Interflow(
            bool                isProducer,
            bool                _isTcp,
            const std::string   &myIpAddr,
            unsigned short      myPort,
            const std::string   &destIpAddr,
            unsigned short      destPort,
            const std::string   &shareKey,
            const std::string   &producerSemKey,
            const std::string   &consumerSemKey
    ): isTcp(_isTcp),shareMemory(isProducer,shareKey,MAX_SHARE_MAT_SIZE,producerSemKey,consumerSemKey)
    {
        isInit = true;
        shareMatPtr = reinterpret_cast<ShareMat*>(shareMemory.GetShareMemory());

        if(isProducer) {
            if(isTcp) {

                if(!tcp.Init(myIpAddr,myPort) || !tcp.Bind()){
                    exit(-1);
                }
                TCPSocket tempSocket;
                if (!tcp.Listen() || !tcp.Accept(tempSocket)) {
                    exit(-1);
                }
                tempSocket.Moveto(tcp);

                encodeParams.emplace_back(cv::IMWRITE_JPEG_QUALITY);
                encodeParams.emplace_back(80);
            }
            else {
                if(!udp.Init(myIpAddr,myPort,destIpAddr,destPort) || !udp.Bind()) {
                    exit(-1);
                }
            }
        }
        else {
            if(isTcp) {
                tcp.Init();
                if(!tcp.Connect(destIpAddr,destPort)){
                    exit(-1);
                }
            }
            else {
                if(!udp.Init(myIpAddr,myPort,destIpAddr,destPort) || !udp.Bind()) {
                    exit(-1);
                }
                struct timeval timeout{};
                timeout.tv_sec = 1;  // 设置超时时间为1秒
                timeout.tv_usec = 0;

                if (setsockopt(udp.Sock(), SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
                    std::cerr << "Set socket timeout error" << std::endl;
                    udp.Close();
                    exit(-1);
                }

                int newBufferSize = 1024 * 1024;
                if (setsockopt(udp.Sock(), SOL_SOCKET, SO_RCVBUF, &newBufferSize, sizeof(newBufferSize)) == -1) {
                    std::cerr << "Set socket recv buffer error" << std::endl;
                    udp.Close();
                    exit(-1);
                }
            }
        }

    }

    void Interflow::NotifyEnd() {
        if(!isInit) return;
        shareMemory.Clear();
        shareMemory.PostConsumerSem();
    }

    bool Interflow::TcpSendMat(const cv::Mat &mat) {
        if(!isTcp) return false;

        cv::imencode(".jpg", mat, buffer, encodeParams);

        tcpMatProp.size = buffer.size();

        if(!tcp.Send((const char*)&tcpMatProp,sizeof(tcpMatProp))
        || !tcp.Send((const char*)buffer.data(),buffer.size())){
            LOG_ERROR(InterflowChan, strerror(errno));
            return false;
        }

        return true;
    }

    bool Interflow::TcpReceiveMat(cv::Mat &mat) {
        if(!isTcp) return false;

        std::string temp;
        if(tcp.Recv(temp,sizeof(tcpMatProp),false) <= 0){
            return false;
        }
        tcpMatProp = *(TcpMatProp*)temp.data();
        if(tcp.Recv(temp,tcpMatProp.size,false) <= 0) {
            return false;
        }
        buffer.assign(temp.begin(),temp.end());
        mat = cv::imdecode(buffer,cv::IMREAD_COLOR);
        return !mat.empty();
    }

    bool Interflow::ShareMemorySendMat(const cv::Mat &mat) {
        shareMemory.WaitProducerSem();
        shareMatPtr->cols = mat.cols;
        shareMatPtr->rows = mat.rows;
        shareMatPtr->channels = mat.channels();
        memcpy(shareMatPtr->data,mat.data,
               mat.cols*mat.rows*mat.channels());
        shareMemory.PostConsumerSem();
        return true;
    }

    bool Interflow::ShareMemoryReceiveMat(cv::Mat &mat) {
        shareMemory.WaitConsumerSem();
        if(shareMatPtr->channels != 3){
            while(shareMemory.TryWaitConsumerSem());
            shareMemory.PostProducerSem();
            return false;
        }
        mat = cv::Mat(shareMatPtr->rows,shareMatPtr->cols,CV_8UC3);
        memcpy(mat.data,shareMatPtr->data,
               shareMatPtr->rows*shareMatPtr->cols*shareMatPtr->channels);
        shareMemory.PostProducerSem();
        return true;
    }

    bool Interflow::UdpSendJson(json &json) {
        return 1 == udp.SendAll(to_string(json));
    }

    bool Interflow::UdpReceiveJson(json &json) {
        std::string temp;
        if(1 != udp.RecvAll(temp,false)) return false;
        json = hzd::json::parse(temp);
        return !json.empty();
    }

    bool Interflow::TcpSendJson(json &json) {
        return 1 == tcp.SendAll(to_string(json));
    }

    bool Interflow::TcpReceiveJson(json &json) {
        std::string temp;
        if(1 != tcp.RecvAll(temp,false)) return false;
        json = hzd::json::parse(temp);
        return !json.empty();
    }

    bool Interflow::SendJson(json &json) {
        if(!isInit) return false;

        return isTcp ? TcpSendJson(json) : UdpSendJson(json);
    }

    bool Interflow::ReceiveJson(json &json) {
        if(!isInit) return false;

        return isTcp ? TcpReceiveJson(json) : UdpReceiveJson(json);
    }

    bool Interflow::SendMat(const cv::Mat &mat) {
        if(!isInit) return false;

        return isTcp ? TcpSendMat(mat) : ShareMemorySendMat(mat);
    }

    bool Interflow::ReceiveMat(cv::Mat &mat) {
        if(!isInit) return false;

        return isTcp ? TcpReceiveMat(mat) : ShareMemoryReceiveMat(mat);
    }

} // hzd