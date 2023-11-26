/**
  ******************************************************************************
  * @file           : main.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/26
  ******************************************************************************
  */

#include <QWidget>
#include <QApplication>
#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>
#include "SolutionItem.h"
#include "HomePage.h"

int main(int argc,char* argv[]) {
    QApplication application(argc,argv);
    hzd::HomePage homePage;
    homePage.show();
    return QApplication::exec();
}