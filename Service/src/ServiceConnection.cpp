/**
  ******************************************************************************
  * @file           : ServiceConnection.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/24
  ******************************************************************************
  */

#include "ServiceConnection.h"      /* ServiceConnection */

const std::string ServiceConnectionChan = "ServiceConnection";

int hzd::ServiceConnection::Init(
        const hzd::ConnectionInitParameter &connectionInitParameter
)
{
    return Connection::Init(connectionInitParameter);
}

int hzd::ServiceConnection::Close() {
    return Connection::Close();
}

hzd::Connection::CallbackReturnType hzd::ServiceConnection::ReadCallback() {
    std::string temp;
    if(!TCPSocket::Recv(temp,sizeof(ControlPacket),false)){
        LOG_ERROR(ServiceConnectionChan,"recv ControlPacket failed");
        return FAILED;
    }
    controlPacket = *(ControlPacket*)temp.c_str();
    return SUCCESS;
}

hzd::Connection::CallbackReturnType hzd::ServiceConnection::AfterReadCallback() {
    switch(controlPacket.control) {
        case Ack: {
            break;
        }
        case Handshake: {
            break;
        }
        case Start: {
            break;
        }
        case Stop: {
            break;
        }
        case Change: {
            break;
        }
        case Wave: {
            break;
        }
    }
    return Connection::AfterReadCallback();
}

hzd::Connection::CallbackReturnType hzd::ServiceConnection::WriteCallback() {
    return Connection::WriteCallback();
}

hzd::Connection::CallbackReturnType hzd::ServiceConnection::AfterWriteCallback() {
    return Connection::AfterWriteCallback();
}

