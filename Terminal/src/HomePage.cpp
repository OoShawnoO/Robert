/**
  ******************************************************************************
  * @file           : homepage.cpp
  * @author         : huzhida
  * @brief          : None
  * @date           : 2023/11/26
  ******************************************************************************
  */

// You may need to build the project (run Qt uic code generator) to get "ui_HomePage.h" resolved

#include <QScrollBar>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QGraphicsDropShadowEffect>
#include "HomePage.h"
#include "ui_HomePage.h"
#include "SolutionItem.h"
#include "ConfigPage.h"
#include "YoloConfigPage.h"

namespace hzd {
#define MARGIN_MIN_SIZE 0
#define MARGIN_MAX_SIZE 20


    void AddTableItem(QTableWidget* table,const QString& time,const QString& procedure,bool isSuccess,const QString& reason = "") {
        table->insertRow(0);
        table->setItem(0,0,new QTableWidgetItem(time));
        table->setItem(0,1,new QTableWidgetItem(procedure));
        table->setItem(0,2,new QTableWidgetItem(isSuccess ? "success" : "failed"));
        table->setItem(0,3,new QTableWidgetItem(reason));
        for(int i=0;i<4;i++) {
            table->item(0,i)->setTextAlignment(Qt::AlignCenter);
        }
        table->resizeColumnsToContents();
    }

    HomePage::HomePage(QWidget *parent) :
            QWidget(parent), ui(new Ui::HomePage) {
        ui->setupUi(this);
        /* region scroll area style */
        ui->resultTable
        ->verticalScrollBar()
        ->setStyleSheet("QScrollBar:vertical {\n"
                        "          border-width: 0px;\n"
                        "          border: none;\n"
                        "          background:rgba(64, 65, 79, 0);\n"
                        "          width:12px;\n"
                        "          margin: 0px 0px 0px 0px;\n"
                        "      }\n"
                        "      QScrollBar::handle:vertical {\n"
                        "          background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
                        "          stop: 0 #aaaaff, stop: 0.5 #aaaaff, stop:1 #aaaaff);\n"
                        "          min-height: 20px;\n"
                        "          max-height: 20px;\n"
                        "          margin: 0 0px 0 0px;\n"
                        "          border-radius: 6px;\n"
                        "      }\n"
                        "      QScrollBar::add-line:vertical {\n"
                        "          background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
                        "          stop: 0 rgba(64, 65, 79, 0), stop: 0.5 rgba(64, 65, 79, 0),  stop:1 rgba(64, 65, 79, 0));\n"
                        "          height: 0px;\n"
                        "          border: none;\n"
                        "          subcontrol-position: bottom;\n"
                        "          subcontrol-origin: margin;\n"
                        "      }\n"
                        "      QScrollBar::sub-line:vertical {\n"
                        "          background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
                        "          stop: 0  rgba(64, 65, 79, 0), stop: 0.5 rgba(64, 65, 79, 0),  stop:1 rgba(64, 65, 79, 0));\n"
                        "          height: 0 px;\n"
                        "          border: none;\n"
                        "          subcontrol-position: top;\n"
                        "          subcontrol-origin: margin;\n"
                        "      }\n"
                        "      QScrollBar::sub-page:vertical {\n"
                        "      background: rgba(64, 65, 79, 0);\n"
                        "      }\n"
                        "\n"
                        "      QScrollBar::add-page:vertical {\n"
                        "      background: rgba(64, 65, 79, 0);\n"
                        "      }");
        ui->scrollArea
        ->verticalScrollBar()
        ->setStyleSheet("QScrollBar:vertical {\n"
               "          border-width: 0px;\n"
               "          border: none;\n"
               "          background:rgba(64, 65, 79, 0);\n"
               "          width:12px;\n"
               "          margin: 0px 0px 0px 0px;\n"
               "      }\n"
               "      QScrollBar::handle:vertical {\n"
               "          background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
               "          stop: 0 #aaaaff, stop: 0.5 #aaaaff, stop:1 #aaaaff);\n"
               "          min-height: 20px;\n"
               "          max-height: 20px;\n"
               "          margin: 0 0px 0 0px;\n"
               "          border-radius: 6px;\n"
               "      }\n"
               "      QScrollBar::add-line:vertical {\n"
               "          background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
               "          stop: 0 rgba(64, 65, 79, 0), stop: 0.5 rgba(64, 65, 79, 0),  stop:1 rgba(64, 65, 79, 0));\n"
               "          height: 0px;\n"
               "          border: none;\n"
               "          subcontrol-position: bottom;\n"
               "          subcontrol-origin: margin;\n"
               "      }\n"
               "      QScrollBar::sub-line:vertical {\n"
               "          background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
               "          stop: 0  rgba(64, 65, 79, 0), stop: 0.5 rgba(64, 65, 79, 0),  stop:1 rgba(64, 65, 79, 0));\n"
               "          height: 0 px;\n"
               "          border: none;\n"
               "          subcontrol-position: top;\n"
               "          subcontrol-origin: margin;\n"
               "      }\n"
               "      QScrollBar::sub-page:vertical {\n"
               "      background: rgba(64, 65, 79, 0);\n"
               "      }\n"
               "\n"
               "      QScrollBar::add-page:vertical {\n"
               "      background: rgba(64, 65, 79, 0);\n"
               "      }");
    /* endregion */
        setWindowFlags(Qt::FramelessWindowHint);    //隐藏标题栏（无边框）
        setAttribute(Qt::WA_Hover);

        AddTableItem(ui->resultTable,"2023-12-5 18:25","步骤",true);
        AddTableItem(ui->resultTable,"2023-12-5 18:25","步骤",true);
        AddTableItem(ui->resultTable,"2023-12-5 18:25","步骤",false);
        AddTableItem(ui->resultTable,"2023-12-5 18:25","步骤",true);

        ui->solutions->addWidget(new SolutionItem);
        ui->solutions->addWidget(new SolutionItem);
        ui->solutions->addWidget(new SolutionItem);
        ui->solutions->addWidget(new SolutionItem);
        ui->solutions->addWidget(new SolutionItem);
        ui->solutions->addWidget(new SolutionItem);
        ui->solutions->addWidget(new SolutionItem);
        ui->solutions->addWidget(new SolutionItem);
        ui->solutions->addWidget(new SolutionItem);
        ui->solutions->addWidget(new SolutionItem);
        ui->solutions->addWidget(new SolutionItem);
        ui->solutions->addWidget(new SolutionItem);

        auto con = new ConfigPage(this);
        QPoint globalPos = this->mapToGlobal(QPoint(0, 0));
        con->move(globalPos.x() + 200, globalPos.y() + 100);
        con->show();

        auto yolo = new YoloConfigPage(this);
        yolo->move(globalPos.x() + 200, globalPos.y() + 100);
        yolo->show();
        yolo->show();

    }

    HomePage::~HomePage() {
        delete ui;
    }

    bool HomePage::event(QEvent *event) {
        if (event->type() == QEvent::HoverMove) {
            auto *hoverEvent = dynamic_cast<QHoverEvent *>(event);
            QMouseEvent mouseEvent(QEvent::MouseMove, hoverEvent->pos(),
                                   Qt::NoButton, Qt::NoButton, Qt::NoModifier);
            mouseMoveEvent(&mouseEvent);
        }

        return QWidget::event(event);
    }

    void HomePage::mousePressEvent(QMouseEvent *event) {
        if (event->button() == Qt::LeftButton) {
            m_bIsPressed = true;
            m_pressPoint = event->globalPos();
        }

        return QWidget::mousePressEvent(event);
    }

    void HomePage::mouseMoveEvent(QMouseEvent *event) {
        if (m_bIsPressed) {
            if (m_bIsResizing) {
                m_movePoint = event->globalPos() - m_pressPoint;
                m_pressPoint += m_movePoint;
            } else {
                if (!m_bIsDoublePressed && windowState() == Qt::WindowMaximized) {
                    restoreWidget();
                    QPointF point(width() * ((double)(event->globalPos().x())/QApplication::desktop()->width()),
                                  height() * ((double)(event->globalPos().y())/QApplication::desktop()->height()));

                    move(event->globalPos() - point.toPoint());

                    m_pressPoint = event->globalPos();
                }

                QPoint point = event->globalPos() - m_pressPoint;
                move(pos() + point);

                m_pressPoint = event->globalPos();
            }
        }

        if (windowState() != Qt::WindowMaximized) {
            updateRegion(event);
        }

        QWidget::mouseMoveEvent(event);
    }

    void HomePage::updateRegion(QMouseEvent *event) {
        QRect mainRect = geometry();

        int marginTop = event->globalY() - mainRect.y();
        int marginBottom = mainRect.y() + mainRect.height() - event->globalY();
        int marginLeft = event->globalX() - mainRect.x();
        int marginRight = mainRect.x() + mainRect.width() - event->globalX();

        if (!m_bIsResizing) {
            if ( (marginRight >= MARGIN_MIN_SIZE && marginRight <= MARGIN_MAX_SIZE)
                 && ((marginBottom <= MARGIN_MAX_SIZE) && marginBottom >= MARGIN_MIN_SIZE) ) {
                m_direction = BOTTOMRIGHT;
                setCursor(Qt::SizeFDiagCursor);
            } else if ( (marginTop >= MARGIN_MIN_SIZE && marginTop <= MARGIN_MAX_SIZE)
                        && (marginRight >= MARGIN_MIN_SIZE && marginRight <= MARGIN_MAX_SIZE)) {
                m_direction = TOPRIGHT;
                setCursor(Qt::SizeBDiagCursor);
            } else if ( (marginLeft >= MARGIN_MIN_SIZE && marginLeft <= MARGIN_MAX_SIZE)
                        && (marginTop >= MARGIN_MIN_SIZE && marginTop <= MARGIN_MAX_SIZE) ) {
                m_direction = TOPLEFT;
                setCursor(Qt::SizeFDiagCursor);
            } else if ( (marginLeft >= MARGIN_MIN_SIZE && marginLeft <= MARGIN_MAX_SIZE)
                        && (marginBottom >= MARGIN_MIN_SIZE && marginBottom <= MARGIN_MAX_SIZE)) {
                m_direction = BOTTOMLEFT;
                setCursor(Qt::SizeBDiagCursor);
            } else if (marginBottom <= MARGIN_MAX_SIZE && marginBottom >= MARGIN_MIN_SIZE) {
                m_direction = DOWN;
                setCursor(Qt::SizeVerCursor);
            } else if (marginLeft <= MARGIN_MAX_SIZE - 1 && marginLeft >= MARGIN_MIN_SIZE - 1) {
                m_direction = LEFT;
                setCursor(Qt::SizeHorCursor);
            } else if (marginRight <= MARGIN_MAX_SIZE && marginRight >= MARGIN_MIN_SIZE) {
                m_direction = RIGHT;
                setCursor(Qt::SizeHorCursor);
            } else if (marginTop <= MARGIN_MAX_SIZE && marginTop >= MARGIN_MIN_SIZE) {
                m_direction = UP;
                setCursor(Qt::SizeVerCursor);
            } else {
                if (!m_bIsPressed) {
                    setCursor(Qt::ArrowCursor);
                }
            }
        }

        if (NONE != m_direction) {
            m_bIsResizing = true;
            resizeRegion(marginTop, marginBottom, marginLeft, marginRight);
        }
    }

    void HomePage::resizeRegion(int marginTop, int marginBottom, int marginLeft, int marginRight) {
        if (m_bIsPressed) {
            switch (m_direction) {
                case BOTTOMRIGHT:
                {
                    QRect rect = geometry();
                    rect.setBottomRight(rect.bottomRight() + m_movePoint);
                    setGeometry(rect);
                }
                    break;
                case TOPRIGHT:
                {
                    if (marginLeft > minimumWidth() && marginBottom > minimumHeight()) {
                        QRect rect = geometry();
                        rect.setTopRight(rect.topRight() + m_movePoint);
                        setGeometry(rect);
                    }
                }
                    break;
                case TOPLEFT:
                {
                    if (marginRight > minimumWidth() && marginBottom > minimumHeight()) {
                        QRect rect = geometry();
                        rect.setTopLeft(rect.topLeft() + m_movePoint);
                        setGeometry(rect);
                    }
                }
                    break;
                case BOTTOMLEFT:
                {
                    if (marginRight > minimumWidth() && marginTop> minimumHeight()) {
                        QRect rect = geometry();
                        rect.setBottomLeft(rect.bottomLeft() + m_movePoint);
                        setGeometry(rect);
                    }
                }
                    break;
                case RIGHT:
                {
                    QRect rect = geometry();
                    rect.setWidth(rect.width() + m_movePoint.x());
                    setGeometry(rect);
                }
                    break;
                case DOWN:
                {
                    QRect rect = geometry();
                    rect.setHeight(rect.height() + m_movePoint.y());
                    setGeometry(rect);
                }
                    break;
                case LEFT:
                {
                    if (marginRight > minimumWidth()) {
                        QRect rect = geometry();
                        rect.setLeft(rect.x() + m_movePoint.x());
                        setGeometry(rect);
                    }
                }
                    break;
                case UP:
                {
                    if (marginBottom > minimumHeight()) {
                        QRect rect = geometry();
                        rect.setTop(rect.y() + m_movePoint.y());
                        setGeometry(rect);
                    }
                }
                    break;
                default:
                {
                }
                    break;
            }
        } else {
            m_bIsResizing = false;
            m_direction = NONE;
        }
    }

    void HomePage::mouseReleaseEvent(QMouseEvent *event) {
        if (event->button() == Qt::LeftButton) {
            m_bIsPressed = false;
            m_bIsResizing = false;
            m_bIsDoublePressed = false;
        }

        QWidget::mouseReleaseEvent(event);
    }

    void HomePage::leaveEvent(QEvent *event) {
        m_bIsPressed = false;
        m_bIsDoublePressed = false;
        m_bIsResizing = false;

        QWidget::leaveEvent(event);
    }

    void HomePage::createShadow() {
        QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
        shadowEffect->setColor(Qt::black);
        shadowEffect->setOffset(0, 0);
        shadowEffect->setBlurRadius(13);
        ui->widgetContent->setGraphicsEffect(shadowEffect);
    }

    void HomePage::maxmizeWidget() {
        ui->pushButtonRestore->show();
        ui->pushButtonMax->hide();
        showMaximized();
    }

    void HomePage::restoreWidget() {
        ui->pushButtonRestore->hide();
        ui->pushButtonMax->show();
        showNormal();
    }
} // hzd
