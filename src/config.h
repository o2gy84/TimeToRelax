#ifndef CONFIG_H
#define CONFIG_H

#include <QDialog>
#include <QSettings>

#include <memory>

typedef enum
{
    EV_PERIODIC_TIMER,
    EV_SINGLE_TIMER
} event_t;

typedef enum
{
    EV_MESSAGE_BOX,
    EV_SYS_TRAY_MESSAGE,
    EV_NONE
} message_t;

class EventOptions
{
public:
    explicit EventOptions() {}
    ~EventOptions() {}

    // GETERS
    event_t getEvType() const { return m_EventType; }
    message_t getMsgType() const { return m_MessageType; }
    QString getMessage() const { return m_Message; }

    // SETTERS
    void setEvType(event_t ev) {m_EventType = ev;}
    void setMsgType(message_t msg) {m_MessageType = msg;}
    void setMessage(const QString &msg) {m_Message = msg;}

private:
    event_t m_EventType;
    message_t m_MessageType;
    QString m_Message;
};

class Event
{
public:
    explicit Event(const QString &_name, const EventOptions &_opt):
        m_Name(_name),
        m_Options(_opt)
    {}

    QString getName() const { return m_Name; }

private:
    QString m_Name;
    EventOptions m_Options;
};

class Config: public QDialog
{
    Q_OBJECT

public:

    Config(QObject *parent);
    virtual ~Config();

public slots:
    void slotShow();

private:

    std::shared_ptr<QSettings> m_Conf;
    std::vector<Event> m_Events;
};


#endif // CONFIG_H
