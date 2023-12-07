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

namespace hzd {
    QT_BEGIN_NAMESPACE
    namespace Ui { class HomePage; }
    QT_END_NAMESPACE

    class HomePage : public QWidget {
    Q_OBJECT
        enum Direction {
            NONE,BOTTOMRIGHT,TOPRIGHT,TOPLEFT,BOTTOMLEFT,RIGHT,DOWN,LEFT,UP
        };
    public:
        explicit HomePage(QWidget *parent = nullptr);

        ~HomePage() override;

        bool event(QEvent* event);
        void mousePressEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);
        void updateRegion(QMouseEvent* event);
        void resizeRegion(int marginTop,int marginBottom,
                          int marginLeft,int marginRight);
        void mouseReleaseEvent(QMouseEvent* event);
        void leaveEvent(QEvent* event);
        void createShadow();
        void maxmizeWidget();
        void restoreWidget();

    private:
        Ui::HomePage *ui;
        bool m_bIsPressed;
        QPoint m_pressPoint;
        QPoint m_movePoint;
        bool m_bIsDoublePressed;
        bool m_bIsResizing;
        Direction m_direction;
    };
} // hzd

#endif //ROBERT_HOMEPAGE_H
