/**
  ******************************************************************************
  * @file           : LoginForm.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/19
  ******************************************************************************
  */

#ifndef ROBERT_LOGINFORM_H
#define ROBERT_LOGINFORM_H

#include <QMainWindow>
#include "Socket.h"
#include "Interflow.h"

namespace hzd {

    class Client : public TCPSocket {
    public:
        bool isConnected { false };
        Client() = default;
        ~Client() override = default;
        std::shared_ptr<Interflow>      pInterflow;
    };

    QT_BEGIN_NAMESPACE
    namespace Ui { class LoginForm; }
    QT_END_NAMESPACE

    class LoginForm : public QMainWindow {
    Q_OBJECT

    public:
        explicit LoginForm(Client& client,QWidget *parent = nullptr);

        ~LoginForm() override;

    private:
        Client& client;
        Ui::LoginForm *ui;
    };
} // hzd

#endif //ROBERT_LOGINFORM_H
