#include <QDebug>

#include <sstream>

#include "icon.h"

Icon::Icon(QObject *parent)
{
    m_SysTrayIcon.reset(new QSystemTrayIcon(parent));
    m_SysTrayIcon->setIcon(QIcon(":/icons/main_1.png"));
    m_SysTrayIcon->setToolTip("time to work, time to relax :)");
    m_SysTrayIcon->show();
    m_SysTrayIcon->showMessage("Time To Relax", "program is running. ver.0.0.1");

    QObject::connect(m_SysTrayIcon.get(), SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                     this, SLOT(slotActivated(QSystemTrayIcon::ActivationReason)));

    QObject::connect(m_SysTrayIcon.get(), SIGNAL(messageClicked()),
                     this, SLOT(slotMessageClicked()));

    m_Timer.reset(new QTimer(this));
    QObject::connect(m_Timer.get(), SIGNAL(timeout()),
                     this, SLOT(slotTimerActivation()));

    // FIXME: дергаем таймер каждые 10 секунд
    m_TimerIntervalMsec = 10000;

    //timer->setInterval();
    m_Timer->start(m_TimerIntervalMsec);
    m_LastTimeout.start();
}

Icon::~Icon()
{

}


void Icon::setMenu(std::shared_ptr<ContextMenu> menu)
{
    m_SysTrayIcon->setContextMenu(menu->menu());
}

void Icon::slotActivated(QSystemTrayIcon::ActivationReason reason)
{
    qDebug() << "activated. reason: " << reason;
}

void Icon::slotMessageClicked()
{
    qDebug() << "message clicked";
}

void Icon::slotTimerActivation()
{
    int diff = m_LastTimeout.elapsed();
    int seconds_elapsed = diff/1000;

    //QTime cur = QTime::currentTime();
    qDebug() << "timer activation. seconds elapsed: " << seconds_elapsed;

    // TODO: to config!!!
    int trashold_min = 1;
    int trashold_sec = trashold_min * 60;

    if (seconds_elapsed >= trashold_sec)
    {
        std::stringstream s;
        s << "Хватит работать! Иди отдыхай!\n";
        s << "Прошли очередные " << seconds_elapsed/60 << "минут";
        s << " (" << seconds_elapsed << " секунд)";
        m_SysTrayIcon->showMessage("Time To Relax", s.str().c_str());
        m_LastTimeout.restart();
    }
}