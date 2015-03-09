#include <QDebug>
#include <QMessageBox>

#include <sstream>
#include <string>

#include "icon.h"

#define VERSION         "1.0.1"
#define ICON_TOOLTIP    "time to work, time to relax :)"

Icon::Icon(QObject *parent)
{
    m_SysTrayIcon.reset(new QSystemTrayIcon(parent));
    m_SysTrayIcon->setIcon(QIcon(":/icons/main_1.png"));
    m_SysTrayIcon->setToolTip(ICON_TOOLTIP);
    m_SysTrayIcon->show();

    std::string version = VERSION;
    std::stringstream s;
    s << "Running version: " << version;

    m_SysTrayIcon->showMessage(INFO_WINDOW_TITLE, s.str().c_str(),
                                     QSystemTrayIcon::NoIcon, /*msec*/1000);

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
/*
 * Функция вызывается по двойному клику в иконку приложения.
 * Для каждого таймера выводит время, оставшееся до срабатывания.
 *
 */
{
    if (reason == QSystemTrayIcon::DoubleClick)
    {
        std::stringstream s;
        s << "<b>Время до срабатывания:</b> <br />";
        s << "<table>";

        QDateTime cur_time = QDateTime::currentDateTime();

        std::vector<Event> &events = m_Config->events();
        for (unsigned i = 0; i < events.size(); ++i)
        {
            s << "<tr>";
            EventOptions opt = events[i].getOpts();
            s << "<td>" << opt.name.toStdString() << ":</td>";

            s << "<td>&nbsp;&nbsp;&nbsp;&nbsp;</td>";

            if (opt.event_type == EV_PERIODIC_TIMER)
            {
                if (0 == opt.last_activation.toTime_t())
                {
                    s << "<td>" << opt.timer_period_min << " минут</td>";
                }
                else
                {
                    qint64 msec_diff = cur_time.toMSecsSinceEpoch() - opt.last_activation.toMSecsSinceEpoch();
                    int seconds_elapsed = msec_diff/1000;
                    int minuts_elapsed = seconds_elapsed/60;
                    s << "<td>" << opt.timer_period_min - minuts_elapsed << " минут</td>";
                }
            }
            else if (opt.event_type == EV_SINGLE_TIMER)
            {
                // just in case
                if (cur_time.toMSecsSinceEpoch() - opt.timer_timeout_date.toMSecsSinceEpoch() < 0)
                {
                    int val = cur_time.secsTo(opt.timer_timeout_date)/60;
                    if (val == 0) val = 1;
                    s << "<td>" << val << " минут</td>";
                }
            }
            s << "</tr>";
        }

        s << "</table>";

        std::shared_ptr<QMessageBox> mbox (new QMessageBox(NULL));
        mbox->setWindowTitle(INFO_WINDOW_TITLE);
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
/*
 * Пока не используется.
 * Вызывается по клику в сообщение, выводимое
 * иконкой в системном трее.
 *
 */
{
    qDebug() << "message clicked";
}

void Icon::slotReset()
/*
 * "Сброс" всех периодических таймеров
 *
 */
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
/*
 * Настало время какого-то таймера -
 * время вывести сообщение на экран!
 *
 */
{
    std::stringstream s;
    s << opt.name.toStdString() << "\n";
    s << opt.message.toStdString() << "\n";

    if (opt.message_type == EV_MSG_SYS_TRAY_MESSAGE)
    {
        m_SysTrayIcon->showMessage(INFO_WINDOW_TITLE, s.str().c_str(),
                                   QSystemTrayIcon::NoIcon, /*msec*/10000);
    }
    else if (opt.message_type == EV_MSG_MESSAGE_BOX)
    {
        // TODO: non-blocking mbox!
        //       maybe better use qdialog

        std::shared_ptr<QMessageBox> mbox (new QMessageBox(NULL));

        // don't destroy object, if want to mbox->show() !
        //QMessageBox *mbox = new QMessageBox(NULL);
        mbox->setWindowTitle(INFO_WINDOW_TITLE);

        {
            // Добавим пробелов, чтоб окошко было покрасивше
            std::string spacer(mbox->width() / 10, ' ');
            s << spacer;
        }

        mbox->setText(s.str().c_str());
        mbox->setWindowFlags(Qt::WindowStaysOnTopHint);

        //mbox->setModal(false);
        //mbox->setWindowModality(Qt::NonModal);
        //mbox->show();
        mbox->exec();
    }
}

void Icon::slotTimerActivation()
/*
 * callback, повешенный на m_Timer.
 * Вызывается раз в 10 секунд, обсчитывает таймеры, вызывает
 * показ сообщений, если пришло время.
 *
 */
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
