/**
  ******************************************************************************
  * @file           : homepage.h
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/26
  ******************************************************************************
  */

#ifndef ROBERT_HOMEPAGE_H
#define ROBERT_HOMEPAGE_H

#include <QWidget>
#include "SolutionItem.h"
#include "Editor.h"
#include "LoginForm.h"

namespace hzd {

    QT_BEGIN_NAMESPACE
    namespace Ui { class HomePage; }
    QT_END_NAMESPACE

    class HomePage : public QWidget {
    Q_OBJECT
    public:
        explicit HomePage(QWidget *parent = nullptr);
        ~HomePage() override;
        // 添加新方案
        void AddSolution(long id,bool isExist = false);
        // 清空记录信息
        void ClearTableItem();
    signals:
        // 暂停信号
        void stop();
        // 运行信号
        void config(long solutionId,ConfigurePackage configurePackage,QJsonObject flowJson);
    public slots:
        // 添加一条新纪录
        void AddTableItem(QString time,QString procedure,bool isSuccess,QString reason = "");
    private:
        Ui::HomePage *ui;
        int successCount{0};
        int failedCount{0};
        long lastSolutionId{-1};
        std::unordered_map<long,SolutionItem*> solutionMap;
    };
} // hzd

#endif //ROBERT_HOMEPAGE_H
