/**
  ******************************************************************************
  * @file           : main.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/24
  ******************************************************************************
  */

#include "Acceptor.h"
#include "ServiceConnection.h"

int main() {
    bool stop = false;
    hzd::Acceptor<hzd::Epoller<hzd::ServiceConnection>> acceptor(
            stop,
            hzd::Configure::Get("../etc/config.json")
    );
    acceptor.Run();
}