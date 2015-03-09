#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>
#include <QDialog>
#include <QDateTime>

#include <memory>

#define DEFAULT_MSG             "Hey, it's time to Relax :)"
#define DEFAULT_EVENT_NAME      "Обычный таймер"
#define DEFAULT_TIMER_PERIOD    60

#define INFO_WINDOW_TITLE       "Time To Relax"

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

        // service field, not for user
        last_activation = QDateTime::fromTime_t(0);
    }

    event_t event_type;
    message_t message_type;
    QString message;
    QString name;
    int32_t timer_period_min;               // в минутах
    QDateTime timer_timeout_date;

    // service field, not for user
    QDateTime last_activation;
};

class Event
{
public:
    explicit Event(const EventOptions &_opt):
        m_Options(_opt)
    {}
    EventOptions &getOpts() { return m_Options; }
    const EventOptions &getOpts() const { return m_Options; }

    void updateLastActivation(const QDateTime &d) { m_Options.last_activation = d; }

private:
    EventOptions m_Options;
};

class Config: public QObject
{
    Q_OBJECT

public:
    typedef void (Config::*ev_manager_cb_t)(int);

public:

    Config(QObject *parent);
    virtual ~Config();

    std::vector<Event> &events() { return m_Events; }
    void deleteProcessedEvent(int num);

public slots:
    void slotShowConfigDialog();                // click context_menu->settings
    void slotShowAddEventDialog();              // click menu->settings->add
    void slotShowEditEventDialog(int num);      // click menu->settings->edit

private:
    int initEvents();
    void updateDialogEvents();
    void showEventManagerDialog(const QString &title, std::shared_ptr<Event> ev,
                                ev_manager_cb_t cb, int cb_param);
    void deleteEvent(int num);
    void addEvent(int num);
    void editEvent(int num);

private:

    std::shared_ptr<QDialog> m_ConfigDialog;

    std::shared_ptr<QDialog> m_EventManagerDialog;
    std::shared_ptr<Event> m_EventToAdd;

    std::shared_ptr<QSettings> m_Conf;
    std::vector<Event> m_Events;
};

#endif // CONFIG_H
