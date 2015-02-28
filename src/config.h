#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>
#include <QDialog>
#include <QDateTime>

#include <memory>

#define DEFAULT_MSG             "Hey, it's time to Relax :)"
#define DEFAULT_EVENT_NAME      "Обычный таймер"
#define DEFAULT_TIMER_PERIOD    60

typedef enum
{
    EV_PERIODIC_TIMER,
    EV_SINGLE_TIMER,
    EV_NONE
} event_t;

typedef enum
{
    EV_MSG_MESSAGE_BOX,
    EV_MSG_SYS_TRAY_MESSAGE,
    EV_MSG_NONE
} message_t;

struct EventOptions
{
public:
    explicit EventOptions()
    {
        event_type = EV_NONE;
        message_type = EV_MSG_NONE;
        message = DEFAULT_MSG;
        name = DEFAULT_EVENT_NAME;
        timer_period_min = DEFAULT_TIMER_PERIOD;
        timer_timeout_date = QDateTime();
    }

    event_t event_type;
    message_t message_type;
    QString message;
    QString name;
    int32_t timer_period_min;               // в минутах
    QDateTime timer_timeout_date;
};

class Event
{
public:
    explicit Event(const EventOptions &_opt):
        m_Options(_opt)
    {}
    EventOptions &getOpts() { return m_Options; }
    const EventOptions &getOpts() const { return m_Options; }
private:
    EventOptions m_Options;
};

class Config: public QObject
{
    Q_OBJECT

public:

    Config(QObject *parent);
    virtual ~Config();

public slots:
    void slotShowConfigDialog();
    void slotShowAddEventDialog();
    void slotAddEventToConfig();

private:
    int initEvents();
    void updateDialogEvents();

private:

    std::shared_ptr<QDialog> m_ConfigDialog;

    std::shared_ptr<QDialog> m_AddEventDialog;
    std::shared_ptr<Event> m_EventToAdd;

    std::shared_ptr<QSettings> m_Conf;
    std::vector<Event> m_Events;
};


#endif // CONFIG_H
