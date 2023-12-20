/**
  ******************************************************************************
  * @file           : LoginForm.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/12/19
  ******************************************************************************
  */

// You may need to build the project (run Qt uic code generator) to get "ui_LoginForm.h" resolved

#include <QMessageBox>
#include "LoginForm.h"
#include "ui_LoginForm.h"

namespace hzd {
    LoginForm::LoginForm(Client& _client,QWidget *parent) :
            QMainWindow(parent), ui(new Ui::LoginForm),client(_client) {
        ui->setupUi(this);

        connect(ui->connectButton,&QPushButton::clicked,this,[&]{
            if(!client.Connect(ui->ipEdit->text().toStdString(),ui->portEdit->text().toUInt())){
                QMessageBox::critical(this,"失败","连接失败...");
                return;
            }
            client.isConnected = true;
            QMessageBox::information(nullptr,"成功","连接成功!");
            close();
        });
    }

    LoginForm::~LoginForm() {
        delete ui;
    }

    Client::~Client() {
        Close();
    }

} // hzd
