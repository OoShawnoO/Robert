/**
  ******************************************************************************
  * @file           : Interflow.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/18
  ******************************************************************************
  */

#ifndef UTILS_INTERFLOW_H
#define UTILS_INTERFLOW_H

#include <unistd.h>         /* close */
#include <arpa/inet.h>      /* socket addr */
#include <sys/shm.h>        /* shm */
#include <sys/stat.h>       /* stat */
#include <semaphore.h>      /* sem_t */
#include <string>           /* string */
#include <vector>           /* vector */
#include <opencv2/core.hpp> /* Mat */

#include "AsyncLogger.h"    /* AsyncLogger */

namespace hzd {

#define SOCKET_BUF_SIZE 4096
#define BAD_FILE_DESCRIPTOR (-1)

    /**
     * @brief Socket接口要求 \n Socket interface requirement
     */
    class Socket {
    protected:
        // 是否为新的任务 / Is new task or not
        bool isNew                          {true};
        // socket类型 / Socket type
        int type                            {SOCK_STREAM};
        // 读写缓冲区 / Read-Write buffer
        char buffer[SOCKET_BUF_SIZE]    =   {0};
        // 读游标 / Read cursor
        size_t readCursor                   {0};
        // 写游标 / Write cursor
        size_t writeCursor                  {0};
        // 读总字节数 / Read total bytes count
        size_t readTotalBytes               {0};
        // 写总字节数 / Write total bytes count
        size_t writeTotalBytes              {0};
        // 文件资源文件描述符 / File resource file descriptor
        int file                            {BAD_FILE_DESCRIPTOR};
        // Socket / Socket
        int sock                            {BAD_FILE_DESCRIPTOR};
        // 本机地址 / Self address
        sockaddr_in myAddr                  {};
        // 本机端口 / Self port
        unsigned short myPort               {0};
        // 目的地址 / Destination address
        sockaddr_in destAddr                {};
        socklen_t   destAddrLen             {};
        // 发送实现 / Send implement
        virtual int sendImpl(const char* data) = 0;
        // 接受实现 / Recv implement
        virtual int recvImpl(std::string& data) = 0;

        struct stat stat                    {};
    public:
        virtual ~Socket();
        // 初始化 / Initialize
        virtual bool Init(const std::string& ip,unsigned short port) = 0;
        // 初始化 / Initialize
        virtual void Init(int _sock,sockaddr_in _myAddr,sockaddr_in _destAddr);
        // 绑定socket / Bind socket
        virtual inline bool Bind() = 0;
        // 使用自定义协议发送 / Use self-define protocol send
        virtual int SendWithHeader(const char* data,size_t size);
        // 使用自定义协议发送 / Use self-define protocol send
        virtual int SendWithHeader(const std::string& data);
        // 使用自定义协议发送 / Use self-define protocol send
        virtual int SendWithHeader(std::string&& data);
        // 发送所有数据 / Send all data
        virtual int SendAll(const std::string& data) = 0;
        // 发送固定大小数据 / Send fixed size data
        virtual int Send(const std::string& data,size_t size) = 0;
        // 发送固定大小数据 / Send fixed size data
        virtual int Send(std::string&& data,size_t size) = 0;
        // 发送固定大小数据 / Send fixed size data
        virtual int Send(const char* data,size_t size) = 0;
        // 使用自定义协议发送文件 / Use self-define protocol send file
        virtual int SendFileWithHeader(const std::string& filePath) = 0;
        // 发送文件 / Send file
        virtual int SendFile(const std::string& filePath) = 0;
        // 使用自定义协议接收 / Use self-define protocol recv
        virtual int RecvWithHeader(std::string& data,bool isAppend);
        // 接受固定大小数据 / Recv fixed size data
        virtual int Recv(std::string& data,size_t size,bool isAppend) = 0;
        // 一次性读取socket缓冲区中所有数据 / Recv all data in socket buffer
        virtual bool RecvAll(std::string& data,bool isAppend) = 0;
        // 使用自定义协议接收文件 / Use self-define protocol recv file
        virtual int RecvFileWithHeader(const std::string& filePath) = 0;
        // 接收文件 / Recv file
        virtual int RecvFile(const std::string& filePath,size_t size) = 0;
        // 获取Socket / Get socket
        int Sock() const;
        // 获取本机地址 / Get self-address
        sockaddr_in Addr() const;
        // 获取目的地址 / Get dest-address
        sockaddr_in DestAddr() const;
        virtual int Close();
    };

    /**
     * @brief TCP 套接字实现 / TCP Socket Implement
     */
    class TCPSocket : virtual public Socket {
#pragma pack(1)
#define TCP_HEADER_SIZE sizeof(header)
#pragma pack()
    protected:
        int sendImpl(const char* data) override;
        int recvImpl(std::string& data) override;

        struct header{
            size_t size;
        };
    public:
        TCPSocket();
        ~TCPSocket() override;
        bool Init(const std::string& ip,unsigned short port) override;
        void Init();
        bool Bind() override;
        bool Listen();
        bool Accept(TCPSocket& tcpSocket);
        virtual bool Connect(const std::string& ip,unsigned short port);
        int SendWithHeader(const char* data,size_t size) override;
        int SendWithHeader(const std::string& data) override;
        int SendWithHeader(std::string&& data) override;
        int SendAll(const std::string& data) override;
        int Send(const std::string& data,size_t size) override;
        int Send(std::string&& data,size_t size) override;
        int Send(const char* data,size_t size) override;
        int SendFileWithHeader(const std::string& filePath) override;
        int SendFile(const std::string& filePath) override;

        int RecvWithHeader(std::string& data,bool isAppend) override;
        int Recv(std::string& data,size_t size,bool isAppend) override;
        bool RecvAll(std::string& data,bool isAppend) override;
        int RecvFileWithHeader(const std::string& filePath) override;
        int RecvFile(const std::string& filePath,size_t size) override;

        int Close() override;
    };

    /**
     * @brief UDP 套接字实现 / UDP Socket Implement
     */
    class UDPSocket : virtual public Socket {
    protected:
        int sendImpl(const char* data) override;
        int recvImpl(std::string& data) override;
    public:
        UDPSocket();
        ~UDPSocket() override;
        bool Init(const std::string& ip,unsigned short port) override;
        bool Init(const std::string& ip,unsigned short port,const std::string& destIP,short destPort);
        void SetDestAddr(const std::string& ip,unsigned short port);
        bool Bind() override;
        int SendWithHeader(const char* data,size_t size) override;
        int SendWithHeader(const std::string& data) override;
        int SendWithHeader(std::string&& data) override;
        int SendAll(const std::string& data) override;
        int Send(const std::string& data,size_t size) override;
        int Send(std::string&& data,size_t size) override;
        int Send(const char* data,size_t size) override;
        int SendFileWithHeader(const std::string& filePath) override;
        int SendFile(const std::string& filePath) override;

        int RecvWithHeader(std::string& data,bool isAppend) override;
        int Recv(std::string& data,size_t size,bool isAppend) override;
        bool RecvAll(std::string& data,bool isAppend) override;
        int RecvFileWithHeader(const std::string& filePath) override;
        int RecvFile(const std::string& filePath,size_t size) override;

        int Close() override;
    };

    /**
      * @brief 共享内存 / Share memory
      */
    class ShareMemory {
    public:
        ShareMemory(
                bool               isProducer,
                const std::string& shareKey,
                long               capacity,
                const std::string& producerSemKey,
                const std::string& consumerSemKey
        );
        ~ShareMemory();

        inline void WaitProducerSem() { sem_wait(producerSem); }
        inline void WaitConsumerSem() { sem_wait(consumerSem); }
        inline void PostProducerSem() { sem_post(producerSem); }
        inline void PostConsumerSem() { sem_post(consumerSem); }
        inline bool TryWaitProducerSem() {
            return producerSem && sem_trywait(producerSem) == 0;
        }
        inline bool TryWaitConsumerSem() {
            return consumerSem && sem_trywait(consumerSem) == 0;
        };
        unsigned char* GetShareMemory();
    private:
        bool            isProducer{false};
        int             shareId{-1};
        std::string     shareKey{};
        unsigned char*  sharePtr{nullptr};
        long            shareCapacity;
        std::string     producerSemKey{};
        sem_t*          producerSem{nullptr};
        std::string     consumerSemKey{};
        sem_t*          consumerSem{nullptr};
    };

    /**
      * @brief 集成交互工具 / Integrated Interflow tool
      */
    class Interflow {
    public:
        Interflow(
            bool               isProducer,
            const std::string& shareKey,
            long               capacity,
            const std::string& producerSemKey,
            const std::string& consumerSemKey,
            const std::string& ipAddr,
            unsigned short     port
        );

#define MAX_SHARE_MAT_SIZE (1920*1080*3 + 4)
        struct ShareMat {
            int rows;
            int cols;
            int channels;
            unsigned char data[MAX_SHARE_MAT_SIZE];
        };

        void PostConsumer();
        void PostProducer();
        bool SendMat(const cv::Mat& mat);
        bool ReceiveMat(cv::Mat& mat);
        bool SendJson(nlohmann::json& json);
        bool ReceiveJson(nlohmann::json& json);
    private:
        ShareMat*   shareMatPtr;
        ShareMemory shareMemory;
        UDPSocket   udp;
    };
} // hzd

#endif //UTILS_INTERFLOW_H
