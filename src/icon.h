#ifndef ICON_H
#define ICON_H

#include <QSystemTrayIcon>
#include <QObject>
#include <QTimer>
#include <QTime>

#include <memory>

#include "context_menu.h"
#include "config.h"

class Icon: public QObject
{
    Q_OBJECT

public:
    Icon(QObject *parent = 0);
    ~Icon();

    void setMenu(std::shared_ptr<ContextMenu> menu);
    void setConfig(std::shared_ptr<Config> conf) { m_Config = conf; }

public slots:
    void slotActivated(QSystemTrayIcon::ActivationReason reason);
    void slotMessageClicked();
    void slotTimerActivation();
    void slotReset();

protected:

private:
    void showUserMessage(const EventOptions& opt);

private:

    std::shared_ptr<QSystemTrayIcon> m_SysTrayIcon;
    std::shared_ptr<QTimer> m_Timer;
    std::shared_ptr<Config> m_Config;           // app config
};

#endif // ICON_H
