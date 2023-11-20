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
    static const std::string SocketChan = "Socket";

    int Socket::SendWithHeader(const char *data,size_t size) {
        return 1;
    }

    int Socket::SendWithHeader(const std::string &data) {
        return 1;
    }

    int Socket::SendWithHeader(std::string &&data) {
        return 1;
    }

    int Socket::RecvWithHeader(std::string &data,bool isAppend) {
        return 1;
    }

    int Socket::Close() {
        if(sock != BAD_FILE_DESCRIPTOR){ close(sock); sock = BAD_FILE_DESCRIPTOR;}
        if(file != BAD_FILE_DESCRIPTOR){ close(file); file = BAD_FILE_DESCRIPTOR;}
        return -1;
    }

    Socket::~Socket() {
        Socket::Close();
    }

    int Socket::Sock() const {
        return sock;
    }

    sockaddr_in Socket::DestAddr() const {
        return destAddr;
    }

    sockaddr_in Socket::Addr() const{
        return myAddr;
    }

    void Socket::Init(int _sock, sockaddr_in _myAddr, sockaddr_in _destAddr) {
        sock = _sock;
        myAddr = _myAddr;
        myPort = ntohs(_myAddr.sin_port);
        destAddr = _destAddr;
    }

    void Socket::Moveto(Socket &s) {
        s = *this;
        sock = BAD_FILE_DESCRIPTOR;
        file = BAD_FILE_DESCRIPTOR;
    }

    /* TCP Impl */
    TCPSocket::TCPSocket() {
        type = SOCK_STREAM;
    }
    /**
     * @return 0 again
     * @return 1 true
     * @return -1 false
     */
    int TCPSocket::sendImpl(const char *data) {
        size_t needSend;
        ssize_t hadSend;
        while(writeCursor < writeTotalBytes){
            needSend = writeTotalBytes - writeCursor;
            if((hadSend = ::send(sock,data + writeCursor,needSend,MSG_NOSIGNAL)) <= 0){
                if(errno == EAGAIN || errno == EWOULDBLOCK){
                    isNew = true;
                    return 0;
                }
                isNew = true;
                return -1;
            }
            writeCursor += hadSend;
        }
        writeCursor = 0;
        writeTotalBytes = 0;
        isNew = true;
        return 1;
    }
    /**
     * @return 0 again
     * @return 1 true
     * @return -1 false
     */
    int TCPSocket::recvImpl(std::string &data) {
        if(readTotalBytes <= 0) return false;
        ssize_t hadRecv;
        size_t needRecv;
        if(readTotalBytes != SIZE_MAX && data.size() < readTotalBytes){
            data.reserve(readTotalBytes);
        }
        int flag = readTotalBytes == SIZE_MAX ? MSG_DONTWAIT : 0;
        while(readCursor < readTotalBytes){
            bzero(buffer,SOCKET_BUF_SIZE);
            needRecv = (readTotalBytes - readCursor) > SOCKET_BUF_SIZE ? SOCKET_BUF_SIZE : (readTotalBytes - readCursor);
            if((hadRecv = ::recv(sock,buffer,needRecv,flag)) <= 0){
                if(errno == EAGAIN || errno == EWOULDBLOCK){
                    if(data.empty()) continue;
                    isNew = true;
                    return 0;
                }
                if(hadRecv == 0 && readTotalBytes == SIZE_MAX) {
                    shutdown(sock,SHUT_RDWR);
                    isNew = true;
                    return 0;
                }else if(hadRecv == 0) {
                    LOG_INFO(SocketChan,"对端关闭");
                    Close();
                    return -1;
                }
                LOG_ERROR(SocketChan, strerror(errno));
                return -1;
            }
            data.append(buffer,hadRecv);
            readCursor += hadRecv;
        }
        isNew = true;
        return 1;
    }

    TCPSocket::~TCPSocket() {
        TCPSocket::Close();
    }

    bool TCPSocket::Init(const std::string &ip, unsigned short port) {
        if(sock != -1) {
            close(sock);
            sock = -1;
        }
        if((sock = socket(AF_INET,type,0)) < 0){
            LOG_ERROR(SocketChan,strerror(errno));
            return false;
        }
        int reuse = 1;
        if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse)) < 0){
            LOG_ERROR(SocketChan, strerror(errno));
            return false;
        }
        if(setsockopt(sock,SOL_SOCKET,SO_REUSEPORT,&reuse,sizeof(reuse)) < 0){
            LOG_ERROR(SocketChan,strerror(errno));
            return false;
        }

        myAddr.sin_family = AF_INET;
        myAddr.sin_addr.s_addr = inet_addr(ip.c_str());
        myAddr.sin_port = htons(port);
        return true;
    }

    bool TCPSocket::Bind() {
        if(sock == -1){
            LOG_ERROR(SocketChan,"please bind after create socket.");
            return false;
        }
        if(bind(sock,(sockaddr*)&myAddr,sizeof(myAddr)) < 0){
            LOG_ERROR(SocketChan,strerror(errno));
            return false;
        }
        return true;
    }

    bool TCPSocket::Listen() {
        if(sock == -1){
            LOG_ERROR(SocketChan,"please listen after create socket.");
            return false;
        }
        if(listen(sock,1024) < 0){
            LOG_ERROR(SocketChan,strerror(errno));
            return false;
        }
        return true;
    }

    bool TCPSocket::Accept(TCPSocket& tcpSocket) {
        tcpSocket.sock = BAD_FILE_DESCRIPTOR;
        bzero(&tcpSocket.destAddr,sizeof(tcpSocket.destAddr));
        tcpSocket.destAddrLen = sizeof(tcpSocket.destAddr);
        if((tcpSocket.sock = accept(sock,(sockaddr*)&tcpSocket.destAddr,&tcpSocket.destAddrLen)) < 0){
            LOG_ERROR(SocketChan, strerror(errno));
            return false;
        }

        tcpSocket.myAddr = myAddr;
        return true;
    }

    bool TCPSocket::Connect(const std::string& ip, unsigned short port) {
        if(sock != -1) {
            close(sock);
            sock = -1;
        }
        if((sock = socket(AF_INET,type,0)) < 0){
            LOG_ERROR(SocketChan,strerror(errno));
            return false;
        }
        sockaddr_in destAddr{};
        destAddr.sin_port = htons(port);
        destAddr.sin_family = AF_INET;
        destAddr.sin_addr.s_addr = inet_addr(ip.c_str());
        if(connect(sock,(sockaddr*)&destAddr,sizeof(destAddr)) < 0){
            LOG_ERROR(SocketChan,strerror(errno));
            return false;
        }
        return true;
    }

    void TCPSocket::Init() {
        if(sock != -1) {
            close(sock);
            sock = -1;
        }
        if((sock = socket(AF_INET,type,0)) < 0 ){
            LOG_ERROR(SocketChan, strerror(errno));
            return;
        }
    }

    int TCPSocket::SendWithHeader(const char *data,size_t size) {
        if(isNew){
            writeTotalBytes = size;
            writeCursor = 0;
            if(writeTotalBytes <= 1) return -1;
            header h{writeTotalBytes};
            if(::send(sock,&h,TCP_HEADER_SIZE,0) <= 0) return -1;
            isNew = false;
        }
        return TCPSocket::sendImpl(data);
    }

    int TCPSocket::SendWithHeader(const std::string &data) {
        return SendWithHeader(data.c_str(),data.size());
    }

    int TCPSocket::SendWithHeader(std::string &&data) {
        return SendWithHeader(data.c_str(),data.size());
    }

    int TCPSocket::SendAll(const std::string &data) {
        if(isNew){
            writeTotalBytes = data.size();
            writeCursor = 0;
            if(writeTotalBytes <= 1) return -1;
            isNew = false;
        }
        return TCPSocket::sendImpl(data.c_str());
    }

    int TCPSocket::Send(const char *data, size_t size) {
        if(isNew){
            writeTotalBytes = size;
            writeCursor = 0;
            if(writeTotalBytes <= 1) return -1;
            isNew = false;
        }
        return TCPSocket::sendImpl(data);
    }

    int TCPSocket::Send(const std::string &data, size_t size) {
        return Send(data.c_str(),size);
    }

    int TCPSocket::Send(std::string &&data, size_t size) {
        return Send(data.c_str(),size);
    }

    int TCPSocket::SendFileWithHeader(const std::string &filePath) {
        if(isNew){
            file = open(filePath.c_str(),O_RDONLY);
            if(file < 0){
                LOG_ERROR(SocketChan,strerror(errno));
                return -1;
            }
            memset(&stat,0,sizeof(stat));
            fstat(file,&stat);
            writeTotalBytes = stat.st_size;
            writeCursor = 0;
            if(send(sock,&writeTotalBytes,sizeof(writeTotalBytes),0) < 0){
                LOG_ERROR(SocketChan, strerror(errno));
                return -1;
            }
        }
        ssize_t hadSend;
        while(writeCursor < writeTotalBytes){
            auto offset = (off_t)writeCursor;
            if((hadSend = sendfile(sock,file,&offset,writeTotalBytes - writeCursor)) < 0){
                if(errno == EAGAIN || errno == EWOULDBLOCK){
                    isNew = false;
                    return 0;
                }
                LOG_ERROR(SocketChan, strerror(errno));
                close(file);
                file = -1;
                return -1;
            }
            writeCursor += hadSend;
        }
        close(file);
        file = -1;
        isNew = true;
        return 1;
    }

    int TCPSocket::SendFile(const std::string &filePath) {
        if(isNew){
            file = open(filePath.c_str(),O_RDONLY);
            if(file < 0){
                LOG_ERROR(SocketChan,strerror(errno));
                return -1;
            }
            memset(&stat,0,sizeof(stat));
            fstat(file,&stat);
            writeTotalBytes = stat.st_size;
            writeCursor = 0;
        }
        ssize_t hadSend;
        while(writeCursor < writeTotalBytes){
            auto offset = (off_t)writeCursor;
            if((hadSend = sendfile(sock,file,&offset,writeTotalBytes - writeCursor)) < 0){
                if(errno == EAGAIN || errno == EWOULDBLOCK){
                    isNew = false;
                    return 0;
                }
                LOG_ERROR(SocketChan, strerror(errno));
                close(file);
                file = -1;
                return -1;
            }
            writeCursor += hadSend;
        }
        close(file);
        file = -1;
        isNew = true;
        return 1;
    }

    int TCPSocket::RecvWithHeader(std::string &data,bool isAppend) {
        if(isNew){
            if(!isAppend) data.clear();
            header h{};
            if(::recv(sock,&h,TCP_HEADER_SIZE,0) <= 0){
                return -1;
            }
            readTotalBytes = h.size;
            readCursor = 0;
            isNew = false;
        }
        return TCPSocket::recvImpl(data);
    }

    int TCPSocket::Recv(std::string &data, size_t size,bool isAppend) {
        if(isNew){
            if(!isAppend) data.clear();
            readTotalBytes = size;
            readCursor = 0;
            isNew = false;
        }
        return TCPSocket::recvImpl(data);
    }

    bool TCPSocket::RecvAll(std::string &data,bool isAppend) {
        if(!isAppend) data.clear();
        readTotalBytes = SIZE_MAX;
        readCursor = 0;

        return TCPSocket::recvImpl(data) == 0;
    }

    int TCPSocket::RecvFileWithHeader(const std::string &filePath) {
        if(isNew){
            size_t size;
            if(recv(sock,&size,sizeof(size),0) < 0){
                LOG_ERROR(SocketChan, strerror(errno));
                return -1;
            }
            readCursor = 0;
            readTotalBytes = size;
            file = open(filePath.c_str(),O_CREAT | O_WRONLY);
            if(file < 0){
                LOG_ERROR(SocketChan, strerror(errno));
                return -1;
            }
            isNew = false;
        }
        ssize_t hadRecv;
        size_t needRecv;
        while(readCursor < readTotalBytes){
            bzero(buffer,SOCKET_BUF_SIZE);
            needRecv = (readTotalBytes - readCursor) > SOCKET_BUF_SIZE ? SOCKET_BUF_SIZE : (readTotalBytes - readCursor);
            if((hadRecv = ::recv(sock,buffer,needRecv,0)) < 0){
                if(errno == EAGAIN || errno == EWOULDBLOCK){
                    isNew = false;
                    return 0;
                }
                close(file);
                file = -1;
                return -1;
            }
            if(write(file,buffer,hadRecv) != hadRecv){
                LOG_ERROR(SocketChan,"failed to write");
                return -1;
            }
            readCursor += hadRecv;
        }
        isNew = true;
        close(file);
        file = -1;
        return 1;
    }

    int TCPSocket::RecvFile(const std::string &filePath,size_t size) {
        if(isNew){
            readCursor = 0;
            readTotalBytes = size;
            isNew = false;
            file = open(filePath.c_str(),O_CREAT | O_WRONLY);
        }
        ssize_t hadRecv;
        size_t needRecv;
        while(readCursor < readTotalBytes){
            bzero(buffer,SOCKET_BUF_SIZE);
            needRecv = (readTotalBytes - readCursor) > SOCKET_BUF_SIZE ? SOCKET_BUF_SIZE : (readTotalBytes - readCursor);
            if((hadRecv = ::recv(sock,buffer,needRecv,0)) < 0){
                if(errno == EAGAIN || errno == EWOULDBLOCK){
                    isNew = false;
                    return 0;
                }
                close(file);
                file = -1;
                return -1;
            }
            write(file,buffer,hadRecv);
            readCursor += hadRecv;
        }
        isNew = true;
        close(file);
        file = -1;
        return 1;
    }

    int TCPSocket::Close() {
        return Socket::Close();
    }

    /* udp Impl*/
    UDPSocket::UDPSocket() {
        type = SOCK_DGRAM;
    }

    int UDPSocket::sendImpl(const char *data) {
        size_t needSend;
        ssize_t hadSend;
        while(writeCursor < writeTotalBytes){
            needSend = (writeTotalBytes - writeCursor) > SOCKET_BUF_SIZE ? SOCKET_BUF_SIZE : writeTotalBytes - writeCursor;
            memcpy(buffer,data + writeCursor,needSend);
            if((hadSend = sendto(sock,buffer,needSend,
                                 MSG_NOSIGNAL,(sockaddr*)&destAddr,sizeof(destAddr))) < 0){
                if(errno == EWOULDBLOCK || errno == EAGAIN){
                    isNew = false;
                    return 0;
                }
                return -1;
            }
            writeCursor += hadSend;
        }
        isNew = true;
        return 1;
    }

    int UDPSocket::recvImpl(std::string &data) {
        if(readTotalBytes <= 0) return false;
        ssize_t hadRecv;
        size_t needRecv;
        if(readTotalBytes != SIZE_MAX && data.size() < readTotalBytes){
            data.reserve(readTotalBytes);
        }
        while(readCursor < readTotalBytes){
            bzero(buffer,SOCKET_BUF_SIZE);
            memset(&destAddr,0,sizeof(destAddr));
            needRecv = (readTotalBytes - readCursor) > SOCKET_BUF_SIZE ? SOCKET_BUF_SIZE : (readTotalBytes - readCursor);
            if((hadRecv = ::recvfrom(sock,buffer,needRecv,0,(struct sockaddr*)&destAddr,&destAddrLen)) <= 0){
                if(errno == EAGAIN || errno == EWOULDBLOCK){
                    isNew = false;
                    return 0;
                }
                LOG_ERROR(SocketChan, strerror(errno));
                return -1;
            }
            data.append(buffer,hadRecv);
            readCursor += hadRecv;
        }
        isNew = true;
        return 1;
    }

    UDPSocket::~UDPSocket() {
        UDPSocket::Close();
    }

    bool UDPSocket::Init(const std::string &ip, unsigned short port) {
        if((sock = socket(AF_INET,type,0)) < 0){
            LOG_ERROR(SocketChan,strerror(errno));
            return false;
        }
        myAddr.sin_addr.s_addr = inet_addr(ip.c_str());
        myAddr.sin_port = htons(port);
        myAddr.sin_family = AF_INET;
        return true;
    }

    bool UDPSocket::Bind() {
        if(bind(sock,(sockaddr*)&myAddr,sizeof(myAddr)) < 0){
            LOG_ERROR(SocketChan,strerror(errno));
            return false;
        }
        return true;
    }

    void UDPSocket::SetDestAddr(const std::string &ip, unsigned short port){
        destAddr.sin_addr.s_addr = inet_addr(ip.c_str());
        destAddr.sin_port = htons(port);
        destAddr.sin_family = type;
        destAddrLen = sizeof(destAddr);
    }

    bool UDPSocket::Init(const std::string &ip, unsigned short port, const std::string &destIP, unsigned short destPort) {
        SetDestAddr(destIP,destPort);
        return Init(ip,port);
    }

    int UDPSocket::SendWithHeader(const char *data,size_t size) {
        if(sendto(sock,&size,sizeof(size),0,(sockaddr*)&destAddr,destAddrLen) < 0){
            LOG_ERROR(SocketChan, strerror(errno));
            return -1;
        }
        return Send(data,size);
    }

    int UDPSocket::SendWithHeader(const std::string &data) {
        return SendWithHeader(data.c_str(),data.size());
    }

    int UDPSocket::SendWithHeader(std::string &&data) {
        return SendWithHeader(data.c_str(),data.size());
    }

    int UDPSocket::SendAll(const std::string &data) {
        if(isNew){
            writeTotalBytes = data.size();
            writeCursor = 0;
            if(writeTotalBytes <= 1) return -1;
            isNew = false;
        }
        return UDPSocket::sendImpl(data.c_str());
    }

    int UDPSocket::Send(const std::string &data, size_t size) {
        return Send(data.c_str(),size);
    }

    int UDPSocket::Send(std::string &&data, size_t size) {
        return Send(data.c_str(),size);
    }

    int UDPSocket::Send(const char *data, size_t size) {
        if(isNew){
            writeTotalBytes = size;
            writeCursor = 0;
            if(writeTotalBytes < 1) return -1;
            isNew = false;
        }
        return UDPSocket::sendImpl(data);
    }

    int UDPSocket::SendFileWithHeader(const std::string &filePath) {
        if(isNew){
            file = open(filePath.c_str(),O_RDONLY);
            fstat(file,&stat);
            writeTotalBytes = stat.st_size;
            writeCursor = 0;
            if(sendto(sock,&writeTotalBytes,SOCKET_BUF_SIZE,0,(sockaddr*)&destAddr,destAddrLen) < 0){
                LOG_ERROR(SocketChan, strerror(errno));
                return -1;
            }
            isNew = false;
        }
        ssize_t hadSend;
        while(writeCursor < writeTotalBytes){
            bzero(buffer,SOCKET_BUF_SIZE);
            ssize_t hadRead = read(file,buffer,SOCKET_BUF_SIZE);
            if(hadRead <= 0){
                return -1;
            }
            if((hadSend = sendto(sock,buffer,hadRead,0,(sockaddr*)&destAddr,sizeof(destAddr))) < 0){
                if(errno == EAGAIN || errno == EWOULDBLOCK) return 0;
                return -1;
            }
            writeCursor += hadSend;
        }
        close(file);
        file = -1;
        isNew = true;
        return 1;
    }

    int UDPSocket::SendFile(const std::string &filePath) {
        if(isNew){
            file = open(filePath.c_str(),O_RDONLY);
            fstat(file,&stat);
            writeTotalBytes = stat.st_size;
            writeCursor = 0;
            isNew = false;
        }
        ssize_t hadSend;
        while(writeCursor < writeTotalBytes){
            bzero(buffer,SOCKET_BUF_SIZE);
            ssize_t hadRead = read(file,buffer,SOCKET_BUF_SIZE);
            if(hadRead <= 0){
                return -1;
            }
            if((hadSend = sendto(sock,buffer,hadRead,0,(sockaddr*)&destAddr,sizeof(destAddr))) < 0){
                if(errno == EAGAIN || errno == EWOULDBLOCK) return 0;
                return -1;
            }
            writeCursor += hadSend;
        }
        close(file);
        file = -1;
        isNew = true;
        return 1;
    }


    int UDPSocket::RecvWithHeader(std::string &data,bool isAppend) {
        size_t size;
        if(recvfrom(sock,&size,sizeof(size_t),0,(sockaddr*)&destAddr,&destAddrLen) < 0){
            LOG_ERROR(SocketChan, strerror(errno));
            return -1;
        }
        return Recv(data,size,isAppend);
    }

    int UDPSocket::Recv(std::string &data, size_t size,bool isAppend) {
        if(isNew){
            if(!isAppend) data.clear();
            readCursor = 0;
            readTotalBytes = size;
            if(readTotalBytes < 1) return -1;
            isNew = false;
        }
        return UDPSocket::recvImpl(data);
    }

    bool UDPSocket::RecvAll(std::string &data,bool isAppend) {
        if(isNew){
            if(!isAppend) data.clear();
            readCursor = 0;
            readTotalBytes = SIZE_MAX;
            if(readTotalBytes < 1) return -1;
            isNew = false;
        }
        return UDPSocket::recvImpl(data) == 0;
    }

    int UDPSocket::RecvFileWithHeader(const std::string &filePath) {
        if(isNew){
            size_t size;
            if(recvfrom(sock,&size,sizeof(size),0,(sockaddr*)&destAddr,&destAddrLen) < 0){
                LOG_ERROR(SocketChan, strerror(errno));
                return -1;
            }
            readCursor = 0;
            readTotalBytes = size;
            file = open(filePath.c_str(),O_CREAT | O_WRONLY);
            if(file < 0){
                LOG_ERROR(SocketChan, strerror(errno));
                return -1;
            }
            isNew = false;
        }
        ssize_t hadRecv;
        size_t needRecv;
        while(readCursor < readTotalBytes){
            bzero(buffer,SOCKET_BUF_SIZE);
            needRecv = readTotalBytes - readCursor > SOCKET_BUF_SIZE ? SOCKET_BUF_SIZE : readTotalBytes - readCursor;
            if((hadRecv = ::recvfrom(sock,buffer,needRecv,0,(sockaddr*)&destAddr,&destAddrLen)) < 0){
                if(errno == EAGAIN || errno == EWOULDBLOCK){
                    isNew = false;
                    return 0;
                }
                close(file);
                file = -1;
                return -1;
            }
            if(write(file,buffer,hadRecv) != hadRecv){
                LOG_ERROR(SocketChan,"failed to write");
                return -1;
            }
            readCursor += hadRecv;
        }
        isNew = true;
        close(file);
        file = -1;
        return 1;
    }

    int UDPSocket::RecvFile(const std::string &filePath,size_t size) {
        if(isNew){
            readCursor = 0;
            readTotalBytes = size;
            file = open(filePath.c_str(),O_CREAT | O_WRONLY);
            isNew = false;
        }
        ssize_t hadRecv;
        size_t needRecv;
        while(readCursor < readTotalBytes){
            bzero(buffer,SOCKET_BUF_SIZE);
            needRecv = readTotalBytes - readCursor > SOCKET_BUF_SIZE ? SOCKET_BUF_SIZE : readTotalBytes - readCursor;
            if((hadRecv = ::recvfrom(sock,buffer,needRecv,0,(sockaddr*)&destAddr,&destAddrLen)) < 0){
                if(errno == EAGAIN || errno == EWOULDBLOCK){
                    isNew = false;
                    return 0;
                }
                close(file);
                file = -1;
                return -1;
            }
            write(file,buffer,hadRecv);
            readCursor += hadRecv;
        }
        isNew = true;
        close(file);
        file = -1;
        return 1;
    }

    int UDPSocket::Close() {
        return Socket::Close();
    }

    ShareMemory::~ShareMemory() {
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

    const std::string InterflowChan = "Interflow";

    Interflow::Interflow(
            bool                isProducer,
            bool                _isTcp,
            const std::string   &shareKey,
            const std::string   &producerSemKey,
            const std::string   &consumerSemKey,
            const std::string   &myIpAddr,
            unsigned short      myPort,
            const std::string   &destIpAddr,
            unsigned short      destPort
    ): isTcp(_isTcp),shareMemory(isProducer,shareKey,MAX_SHARE_MAT_SIZE,producerSemKey,consumerSemKey)
    {
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

    void Interflow::PostConsumer() {
        shareMemory.PostConsumerSem();
    }

    void Interflow::PostProducer() {
        shareMemory.PostProducerSem();
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
        return isTcp ? TcpSendJson(json) : UdpSendJson(json);
    }

    bool Interflow::ReceiveJson(json &json) {
        return isTcp ? TcpReceiveJson(json) : UdpReceiveJson(json);
    }

} // hzd