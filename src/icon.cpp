#include <QDebug>
#include <QMessageBox>

#include <sstream>
#include <string>

#include "icon.h"

#define VERSION "1.0.0"

Icon::Icon(QObject *parent)
{
    m_SysTrayIcon.reset(new QSystemTrayIcon(parent));
    m_SysTrayIcon->setIcon(QIcon(":/icons/main_1.png"));
    m_SysTrayIcon->setToolTip("time to work, time to relax :)");
    m_SysTrayIcon->show();

    std::string version = VERSION;
    std::stringstream s;
    s << "program is running.";
    s << " version: " << version;

    m_SysTrayIcon->showMessage("Time To Relax", s.str().c_str());

    QObject::connect(m_SysTrayIcon.get(), SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                     this, SLOT(slotActivated(QSystemTrayIcon::ActivationReason)));

    QObject::connect(m_SysTrayIcon.get(), SIGNAL(messageClicked()),
                     this, SLOT(slotMessageClicked()));

    m_Timer.reset(new QTimer(this));
    QObject::connect(m_Timer.get(), SIGNAL(timeout()),
                     this, SLOT(slotTimerActivation()));


    // дергаем таймер каждые 10 секунд
    m_Timer->start(10000);
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
        std::stringstream s;
        s << "Время до срабатывания:\t\t\t\t\n";

        QDateTime cur_time = QDateTime::currentDateTime();

        std::vector<Event> &events = m_Config->events();
        for (unsigned i = 0; i < events.size(); ++i)
        {
            EventOptions opt = events[i].getOpts();
            s << opt.name.toStdString() << ":\t\t";

            if (opt.event_type == EV_PERIODIC_TIMER)
            {
                if (0 == opt.last_activation.toTime_t())
                {
                    s << opt.timer_period_min << " минут\n";
                }
                else
                {
                    qint64 msec_diff = cur_time.toMSecsSinceEpoch() - opt.last_activation.toMSecsSinceEpoch();
                    int seconds_elapsed = msec_diff/1000;
                    int minuts_elapsed = seconds_elapsed/60;
                    s << opt.timer_period_min - minuts_elapsed << " минут\n";
                }
            }
            else if (opt.event_type == EV_SINGLE_TIMER)
            {
                // just in case
                if (cur_time.toMSecsSinceEpoch() - opt.timer_timeout_date.toMSecsSinceEpoch() < 0)
                {
                    int val = cur_time.secsTo(opt.timer_timeout_date)/60;
                    if (val == 0) val = 1;
                    s << val << " минут\n";
                }
            }
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
    std::vector<Event> &events = m_Config->events();
    for (unsigned i = 0; i < events.size(); ++i)
    {
        EventOptions opt = events[i].getOpts();
        if (opt.event_type == EV_PERIODIC_TIMER)
        {
            events[i].updateLastActivation(QDateTime::currentDateTime());
        }
    }
}


void Icon::showUserMessage(const EventOptions& opt)
{
    std::stringstream s;
    s << opt.name.toStdString() << "\n";
    s << opt.message.toStdString() << "\n";

    if (opt.message_type == EV_MSG_SYS_TRAY_MESSAGE)
    {
        m_SysTrayIcon->showMessage("Time To Relax", s.str().c_str());
    }
    else if (opt.message_type == EV_MSG_MESSAGE_BOX)
    {
        // TODO: non-blocking mbox!
        //       maybe better use qdialog

        std::shared_ptr<QMessageBox> mbox (new QMessageBox(NULL));

        // don't destroy object, if want to mbox->show() !
        //QMessageBox *mbox = new QMessageBox(NULL);
        mbox->setWindowTitle("Time To Relax");
        mbox->setText(s.str().c_str());
        mbox->setWindowFlags(Qt::WindowStaysOnTopHint);

        //mbox->setModal(false);
        //mbox->setWindowModality(Qt::NonModal);
        //mbox->show();
        mbox->exec();
    }
}

void Icon::slotTimerActivation()
{
    QDateTime cur_time = QDateTime::currentDateTime();

    std::vector<Event> &events = m_Config->events();
    for (unsigned i = 0; i < events.size(); ++i)
    {
        EventOptions opt = events[i].getOpts();

        if (opt.event_type == EV_PERIODIC_TIMER)
        {
            if (0 == opt.last_activation.toTime_t())
            {
                // some new event
                events[i].updateLastActivation(QDateTime::currentDateTime());
            }
            else
            {
                qint64 msec_diff = cur_time.toMSecsSinceEpoch() - opt.last_activation.toMSecsSinceEpoch();
                int seconds_elapsed = msec_diff/1000;
                int seconds_trashold = opt.timer_period_min * 60;
                if (seconds_elapsed >= seconds_trashold)
                {
                    showUserMessage(opt);
                    events[i].updateLastActivation(QDateTime::currentDateTime());
                }
            }
        }
        else if (opt.event_type == EV_SINGLE_TIMER)
        {
            if (cur_time.toMSecsSinceEpoch() - opt.timer_timeout_date.toMSecsSinceEpoch() >= 0)
            {
                showUserMessage(opt);

                // TODO: сделать отложенное удаление в конце цикла!
                // need delete this event
                m_Config->deleteProcessedEvent(i);

                // После удаления итераторы становятся невалидны
                // поэтому данный цикл лучше не продолжать!
                break;
            }
        }
    }
}
