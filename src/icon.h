#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSystemTrayIcon>
#include <QObject>
#include <QTimer>
#include <QTime>

#include <memory>

#include "context_menu.h"

class Icon: public QObject
{
    Q_OBJECT

public:
    Icon(QObject *parent = 0);
    ~Icon();

    void setMenu(std::shared_ptr<ContextMenu> menu);

public slots:
    void slotActivated(QSystemTrayIcon::ActivationReason reason);
    void slotMessageClicked();
    void slotTimerActivation();
    void slotReset();

protected:


private:

    std::shared_ptr<QSystemTrayIcon> m_SysTrayIcon;
    std::shared_ptr<QTimer> m_Timer;
    int m_TimerIntervalMsec;
    QTime m_LastTimeout;
    int m_TresholdMin;
};

#endif // MAINWINDOW_H
