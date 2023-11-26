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
#include "SolutionItem.h"

int main(int argc,char* argv[]) {
    QApplication application(argc,argv);
    hzd::SolutionItem item;
    item.show();
    return QApplication::exec();
}