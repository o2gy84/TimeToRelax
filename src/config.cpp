#include <QDebug>
#include "config.h"

Config::Config(QObject *parent) :
    QDialog(NULL)
{
    if (parent)
    {
        // do nothing
    }

#if defined Q_OS_WIN
    m_Conf.reset(new QSettings(QSettings::IniFormat, QSettings::UserScope, "o2gy", "TimeToRelax"));
    qDebug() << "conf: " << m_Conf->fileName();
#elif defined Q_OS_MAC
    throw std::runtime_error("not implmented");
#elif defined Q_OS_UNIX
    throw std::runtime_error("not implmented");
#else
    throw std::runtime_error("what a fuck?");
#endif

    m_Conf->beginGroup("common");
    m_Conf->setValue("message", "just message");
    m_Conf->setValue("sound", 0);
    m_Conf->endGroup();

    int size = m_Conf->beginReadArray("events");
    for (int i = 0; i < size; ++i)
    {
        qDebug() << "read: " << i+1 << "event";
        m_Conf->setArrayIndex(i);
        EventOptions opt;
        opt.setEvType((event_t)m_Conf->value("event_type").toInt());
        opt.setMsgType((message_t)m_Conf->value("message_type").toInt());
        opt.setMessage(m_Conf->value("message").toString());
        Event event(m_Conf->value("name").toString(), opt);
        m_Events.push_back(event);
    }
    m_Conf->endArray();

    if (size == 0)
    {
        // this is first run!
        // create default event
        qDebug() << "need to create default event";
        EventOptions opt;
        opt.setEvType(EV_PERIODIC_TIMER);
        opt.setMsgType(EV_SYS_TRAY_MESSAGE);
        opt.setMessage("default message");
        Event event("default_event", opt);
        m_Events.push_back(event);

        m_Conf->beginWriteArray("events", 1);
        m_Conf->setValue("event_type", opt.getEvType());
        m_Conf->setValue("message_type", opt.getMsgType());
        m_Conf->setValue("message", opt.getMessage());
        m_Conf->setValue("name", event.getName());
        m_Conf->endArray();
    }


    m_Conf->sync();
}


Config::~Config()
{

}


void Config::slotShow()
{
    qDebug() << "menu config clicked";







    QDialog::show();
}
