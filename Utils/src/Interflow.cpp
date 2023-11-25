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

    Interflow::CacheQueueItem& Interflow::CacheQueue::Adjust(size_t frameID) {
        while(queue.front().frameID < frameID) {
            queue.pop_front();
        }
        return queue.front();
    }

    void Interflow::CacheQueue::EmplaceBack(Interflow::CacheQueueItem &&item) {
        if(queue.size() > 64) queue.pop_front();
        queue.emplace_back(std::move(item));
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
        if(isTcp) return;
        shareMemory.Clear();
        shareMemory.PostConsumerSem();
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

    bool Interflow::UdpSendJson(const json &json) {
        std::string temp = to_string(json);
        size_t size = temp.size();

        return udp.Send((const char*)&size,sizeof(size_t)) == 1
            && udp.Send(temp,size);
    }

    bool Interflow::UdpReceiveJson(json &json) {
        std::string temp;
        if(!udp.Recv(temp,sizeof(size_t),false)){
            return false;
        }
        size_t size = *(size_t*)temp.data();
        if(!udp.Recv(temp,size,false)) {
            return false;
        }
        json = json::parse(temp);
        return !json.empty();
    }

    bool Interflow::SendItem(const cv::Mat &mat, const json &json) {
        if(isTcp) {
            return TcpSendItem(mat,json);
        }else{
            return ShareMemorySendMat(mat) && UdpSendJson(json);
        }
    }

    bool Interflow::ReceiveItem(cv::Mat &mat, json &json) {
        if(isTcp) {
            return TcpReceiveItem(mat,json);
        }else{
            return ShareMemoryReceiveMat(mat) && UdpReceiveJson(json);
        }
    }

    bool Interflow::TcpSendItem(const cv::Mat &mat, const json &json) {
        if(!isInit || !isTcp) {
            LOG_ERROR(InterflowChan,"未初始化或您正在使用共享内存+UDP方案，不支持TCP");
            return false;
        }

        cv::imencode(".jpg", mat, cacheQueueItem.matBuffer, encodeParams);

        cacheQueueItem.tcpMatProp.size = cacheQueueItem.matBuffer.size();

        if(!tcp.Send((const char*)&cacheQueueItem.tcpMatProp,sizeof(cacheQueueItem.tcpMatProp))
           || !tcp.Send((const char*)cacheQueueItem.matBuffer.data(),cacheQueueItem.matBuffer.size())){
            LOG_ERROR(InterflowChan, std::string{"send mat failed,reason:"} + strerror(errno));
            return false;
        }

        cacheQueueItem.result = to_string(json);
        cacheQueueItem.resultSize = cacheQueueItem.result.size();
        if(!tcp.Send((const char*)&cacheQueueItem.resultSize,sizeof(cacheQueueItem.resultSize))
           || !tcp.Send(cacheQueueItem.result.c_str(),cacheQueueItem.resultSize)) {
            LOG_ERROR(InterflowChan, std::string{"send result failed,reason:"} + strerror(errno));
            return false;
        }

        cacheQueue.EmplaceBack(std::move(cacheQueueItem));
        cacheQueueItem = CacheQueueItem{};
        return true;
    }

    bool Interflow::TcpReceiveItem(cv::Mat &mat, json &json) {
        if(!isInit || !isTcp) {
            LOG_ERROR(InterflowChan,"未初始化或您正在使用共享内存+UDP方案，不支持TCP");
            return false;
        }
        std::string temp;
        if(tcp.Recv(temp,sizeof(cacheQueueItem.tcpMatProp),false) <= 0){
            return false;
        }
        cacheQueueItem.tcpMatProp = *(TcpMatProp*)temp.data();
        if(tcp.Recv(temp,cacheQueueItem.tcpMatProp.size,false) <= 0) {
            return false;
        }
        cacheQueueItem.matBuffer.assign(temp.begin(),temp.end());
        mat = cv::imdecode(cacheQueueItem.matBuffer,cv::IMREAD_COLOR);
        if(mat.empty()) return false;

        if(tcp.Recv(temp,sizeof(size_t),false) <= 0 ) {
            return false;
        }
        cacheQueueItem.resultSize = *(size_t*)temp.data();
        if(tcp.Recv(temp,cacheQueueItem.resultSize,false) <= 0) {
            return false;
        }
        json = json::parse(temp);
        if(json.empty()) return false;
        return true;
    }

    Interflow::CacheQueueItem::CacheQueueItem(Interflow::CacheQueueItem && cq) noexcept {
        frameID = cq.frameID;
        tcpMatProp = cq.tcpMatProp;
        matBuffer = std::move(cq.matBuffer);
        resultSize = cq.resultSize;
        result = std::move(cq.result);
    }

    Interflow::CacheQueueItem& Interflow::CacheQueueItem::operator=(Interflow::CacheQueueItem && cq)  noexcept {
        frameID = cq.frameID;
        tcpMatProp = cq.tcpMatProp;
        matBuffer = std::move(cq.matBuffer);
        resultSize = cq.resultSize;
        result = std::move(cq.result);
        return *this;
    }
} // hzd