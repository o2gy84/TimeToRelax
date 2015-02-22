#include <QDebug>
#include <QMessageBox>

#include <sstream>
#include <string>

#include "icon.h"

Icon::Icon(QObject *parent)
{
    m_SysTrayIcon.reset(new QSystemTrayIcon(parent));
    m_SysTrayIcon->setIcon(QIcon(":/icons/main_1.png"));
    m_SysTrayIcon->setToolTip("time to work, time to relax :)");
    m_SysTrayIcon->show();

    // TODO: to config!!!
    m_TresholdMin = 60;
    std::string version = "0.0.5";
    std::stringstream s;
    s << "program is running.";
    s << " version: " << version << ".";
    s << " treshold: " << m_TresholdMin << " min";

    m_SysTrayIcon->showMessage("Time To Relax", s.str().c_str());

    QObject::connect(m_SysTrayIcon.get(), SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                     this, SLOT(slotActivated(QSystemTrayIcon::ActivationReason)));

    QObject::connect(m_SysTrayIcon.get(), SIGNAL(messageClicked()),
                     this, SLOT(slotMessageClicked()));

    m_Timer.reset(new QTimer(this));
    QObject::connect(m_Timer.get(), SIGNAL(timeout()),
                     this, SLOT(slotTimerActivation()));


    // FIXME: дергаем таймер каждые 10 секунд
    //timer->setInterval();
    m_Timer->start(10000);
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
    if (reason == QSystemTrayIcon::DoubleClick)
    {
        int diff = m_LastTimeout.elapsed();
        int seconds_elapsed = diff/1000;
        int treshold_sec = m_TresholdMin * 60;
        std::stringstream s;
        if (seconds_elapsed < treshold_sec)
        {
            int sec = treshold_sec - seconds_elapsed;
            s << "До релакса: " << sec/60 << " минут";
        }

        std::shared_ptr<QMessageBox> mbox (new QMessageBox(NULL));
        mbox->setWindowTitle("Time To Relax");
        mbox->setText(s.str().c_str());
        mbox->setWindowFlags(Qt::WindowStaysOnTopHint);
        mbox->exec();
    }
    else
    {
        qDebug() << "activated. reason: " << reason;
    }
}

void Icon::slotMessageClicked()
{
    qDebug() << "message clicked";
}

void Icon::slotReset()
{
    qDebug() << "menu reset clicked";
    m_LastTimeout.restart();
}

void Icon::slotTimerActivation()
{
    int diff = m_LastTimeout.elapsed();
    int seconds_elapsed = diff/1000;

    //QTime cur = QTime::currentTime();
    qDebug() << "timer activation. seconds elapsed: " << seconds_elapsed;

    int treshold_sec = m_TresholdMin * 60;
    if (seconds_elapsed >= treshold_sec)
    {
        std::stringstream s;
        s << "Хватит работать! Иди отдыхай!\n";
        s << "Прошли очередные " << seconds_elapsed/60 << "минут";
        m_SysTrayIcon->showMessage("Time To Relax", s.str().c_str());
        m_LastTimeout.restart();
    }
}
