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
    if(TCPSocket::Recv(temp,sizeof(ControlPacket),false)<0){
        LOG_ERROR(ServiceConnectionChan,"recv ControlPacket failed");
        return FAILED;
    }
    controlPacket = *(ControlPacket*)temp.c_str();
    return Connection::ReadCallback();
}

hzd::Connection::CallbackReturnType hzd::ServiceConnection::AfterReadCallback() {
    currentControl = controlPacket.control;
    switch(currentControl) {
        case Ack: {
            switch (controlPacket.mark) {
                case Right : {

                }
                case MatTimeout : {

                }
                case ResultTimeout : {
                    
                }
            }
            break;
        }
        case Change: {
            ptrInterflow->NotifyEnd();
            ptrInterflow.reset();
        }
        case Config : {
            std::string temp;
            if(!TCPSocket::RecvAll(temp, false)) {
                LOG_ERROR(ServiceConnectionChan,"recv config failed");
                return FAILED;
            }
            auto config = json::parse(temp);
            if(config.empty() || !Configure::Get().Reload(config)){
                LOG_ERROR(ServiceConnectionChan,"config file parse failed");
                return FAILED;
            }
            if(!interflowConfigure.Load(Configure::Get())){
                LOG_ERROR(ServiceConnectionChan,"config file have no interflow item");
                return FAILED;
            }

            ptrInterflow = std::make_shared<Interflow>(
                    interflowConfigure.isProducer,
                    interflowConfigure.isTcp,
                    interflowConfigure.myIpAddr,
                    interflowConfigure.myPort,
                    interflowConfigure.destIpAddr,
                    interflowConfigure.destPort,
                    interflowConfigure.shareKey,
                    interflowConfigure.producerSemKey,
                    interflowConfigure.consumerSemKey
            );

            break;
        }
        case Start: {
            break;
        }
        case Stop: {
            break;
        }
        case Wave: {
            ptrInterflow->NotifyEnd();
            ptrInterflow.reset();
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

