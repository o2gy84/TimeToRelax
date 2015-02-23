#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QFormLayout>
#include <QRadioButton>
#include <QDateTimeEdit>
#include <QButtonGroup>
#include <QStackedWidget>
#include <QMessageBox>

#include "config.h"

#define CONF_EVENT_TYPE "type"
#define CONF_MSG_TYPE "mtype"
#define CONF_MSG "message"
#define CONF_NAME "name"
#define CONF_TIMER_PERIOD "tperiod"
#define CONF_TIMER_TIMEOUT_DATE "tdate"

static
void save_events(std::shared_ptr<QSettings> conf, const std::vector<Event> &events)
{
    for (unsigned i = 0; i < events.size(); ++i)
    {
        EventOptions opt = events[i].getOpts();
        conf->beginWriteArray("events");
        conf->setArrayIndex(i);
        conf->setValue(CONF_EVENT_TYPE, opt.event_type);
        conf->setValue(CONF_MSG_TYPE, opt.message_type);
        conf->setValue(CONF_MSG, opt.message);
        conf->setValue(CONF_NAME, opt.name);
        conf->setValue(CONF_TIMER_PERIOD, opt.timer_period_min);
        conf->setValue(CONF_TIMER_TIMEOUT_DATE, opt.timer_timeout_date);
        conf->endArray();
    }
    conf->sync();
}

Config::Config(QObject *parent)
{
    if (parent)
    {
        // do nothing
    }

    int size = initEvents();
    if (size == 0)
    {
        // this is first run!
        // create default config with default event
        qDebug() << "creating default event";
        EventOptions opt;
        opt.event_type = EV_PERIODIC_TIMER;
        opt.message_type = EV_MSG_SYS_TRAY_MESSAGE;
        opt.timer_period_min = 60;
        opt.message = "default message";
        opt.name = "default event";
        Event event(opt);
        m_Events.push_back(event);
        save_events(m_Conf, m_Events);
    }
}

Config::~Config()
{

}

// private
int Config::initEvents()
{
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

    m_Events.clear();

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
        opt.event_type = (event_t)m_Conf->value(CONF_EVENT_TYPE).toInt();
        opt.message_type = (message_t)m_Conf->value(CONF_MSG_TYPE).toInt();
        opt.message = m_Conf->value(CONF_MSG).toString();
        opt.name = m_Conf->value(CONF_NAME).toString();
        opt.timer_period_min = m_Conf->value(CONF_TIMER_PERIOD).toInt();
        opt.timer_timeout_date = m_Conf->value(CONF_TIMER_TIMEOUT_DATE).toDateTime() ;
        Event event(opt);
        m_Events.push_back(event);
    }
    m_Conf->endArray();
    return size;
}

// private
void Config::updateDialogEvents()
/*
 * "Перерисовка" окна настроек приложения
 * на основе списка событий m_Events.
 */
{
    QSize size (400, 400);
    const QRect screen = QApplication::desktop()->screenGeometry();

    int left_x = screen.width() / 2 - size.width() / 2;
    int left_y = screen.height() / 2 - size.height() / 2;

    QPoint left_top(left_x, left_y);
    QRect dialog_geometry (left_top, size);

    m_ConfigDialog->setGeometry(dialog_geometry);

    QVBoxLayout *layout = new QVBoxLayout (m_ConfigDialog.get());
    layout->setAlignment(Qt::AlignTop);

    QPushButton *button_add = new QPushButton ("+");
    button_add->setMaximumWidth(30);
    QObject::connect(button_add, SIGNAL (clicked()), this, SLOT (slotShowAddEventDialog()));

    layout->addWidget(button_add);

    int i = 0;
    for (const auto &event : m_Events)
    {
        ++i;
        QLabel *label = new QLabel(event.getOpts().name);

        QHBoxLayout *group_box_layout = new QHBoxLayout();
        group_box_layout->addWidget(label);

        QGroupBox *group_box = new QGroupBox ("event: " + QString::number(i));
        group_box->setFlat(false);
        group_box->setMaximumHeight(100);

        group_box->setLayout(group_box_layout);

        layout->addWidget(group_box);
    }

    m_ConfigDialog->setLayout(layout);
}

void Config::slotShowConfigDialog()
/*
 * Юзер кликнул в контекстное меню иконки приложения:
 * "настройки"/"settings".
 * Надо показать настройки приложения.
 */
{
    // destroy prev dialog
    m_ConfigDialog.reset(new QDialog());
    updateDialogEvents();
    m_ConfigDialog->show();
}

void Config::slotShowAddEventDialog()
/*
 *  Показать диалог добавления нового ивента.
 */
{
    qDebug() << "add event dialog";

    EventOptions opts;
    m_EventToAdd.reset(new Event(opts));

    m_AddEventDialog.reset(new QDialog());
    m_AddEventDialog->setWindowTitle("add new event");

    QSize size (600, 200);
    const QRect screen = QApplication::desktop()->screenGeometry();

    int left_x = screen.width() / 2 - size.width() / 2;
    int left_y = screen.height() / 2 - size.height() / 2;

    QPoint left_top(left_x, left_y);
    QRect dialog_geometry (left_top, size);
    m_AddEventDialog->setGeometry(dialog_geometry);

    QLineEdit *name_edit = new QLineEdit();
    name_edit->setText("таймер: " + QString::number(m_Events.size()));
    QObject::connect(name_edit, &QLineEdit::editingFinished, [name_edit, this]()
    {
        m_EventToAdd->getOpts().name = name_edit->text();
    });

    QLineEdit *text_edit = new QLineEdit();
    text_edit->setText("time to relax, bro :)");
    QObject::connect(text_edit, &QLineEdit::editingFinished, [text_edit, this]()
    {
        m_EventToAdd->getOpts().message = text_edit->text();
    });

    QGroupBox *gbox1 = new QGroupBox();
    {
        QStackedWidget *stacked_widget = new QStackedWidget();
        {
            QLineEdit *mins_edit = new QLineEdit();
            mins_edit->setMaximumWidth(40);
            mins_edit->setText("60");
            QObject::connect(mins_edit, &QLineEdit::editingFinished, [mins_edit, this]()
            {
                m_EventToAdd->getOpts().timer_period_min = mins_edit->text().toInt();
            });

            QDateTimeEdit *date_time_edit = new QDateTimeEdit();
            date_time_edit->setMaximumWidth(180);
            date_time_edit->setMinimumDateTime(QDateTime::currentDateTime());
            date_time_edit->setCalendarPopup(true);
            QObject::connect(date_time_edit, &QDateTimeEdit::editingFinished, [date_time_edit, this]()
            {
                m_EventToAdd->getOpts().timer_timeout_date = date_time_edit->dateTime();
            });

            stacked_widget->addWidget(mins_edit);
            stacked_widget->addWidget(date_time_edit);
        }

        QRadioButton *rb1 = new QRadioButton("Периодический таймер (введите периодичность срабатывания в минутах)");
        QRadioButton *rb2 = new QRadioButton("Однократный таймер (введите дату срабатывания)");

        QObject::connect(rb1, &QRadioButton::clicked, [stacked_widget, this]() {
                stacked_widget->setCurrentIndex(0);
                m_EventToAdd->getOpts().event_type = EV_PERIODIC_TIMER;
            });

        QObject::connect(rb2, &QRadioButton::clicked, [stacked_widget, this]() {
                stacked_widget->setCurrentIndex(1);
                m_EventToAdd->getOpts().event_type = EV_SINGLE_TIMER;
            });

        QVBoxLayout *vbox = new QVBoxLayout();
        vbox->addWidget(rb1);
        vbox->addWidget(rb2);
        vbox->addWidget(stacked_widget);
        gbox1->setLayout(vbox);
    }

    QGroupBox *gbox2 = new QGroupBox();
    {
        QRadioButton *rb1 = new QRadioButton("System tray message");
        QRadioButton *rb2 = new QRadioButton("Windows message");

        QObject::connect(rb1, &QRadioButton::toggled, [this]() {
                m_EventToAdd->getOpts().message_type = EV_MSG_SYS_TRAY_MESSAGE;
            });
        QObject::connect(rb2, &QRadioButton::toggled, [this]() {
                m_EventToAdd->getOpts().message_type = EV_MSG_MESSAGE_BOX;
            });

        QVBoxLayout *vbox = new QVBoxLayout();
        vbox->addWidget(rb1);
        vbox->addWidget(rb2);
        gbox2->setLayout(vbox);
    }

    QPushButton *button_add = new QPushButton ("+");
    {
        button_add->setMaximumWidth(30);
        QObject::connect(button_add, SIGNAL (clicked()), this, SLOT (slotAddEventToConfig()));
    }

    QFormLayout *layout = new QFormLayout (m_AddEventDialog.get());
    layout->setAlignment(Qt::AlignTop);

    layout->addRow("имя события", name_edit);
    layout->addRow("текст сообщения", text_edit);
    layout->addRow("тип события", gbox1);
    layout->addRow("тип сообщения", gbox2);
    layout->addRow("готово!", button_add);

    m_AddEventDialog->show();
}

void Config::slotAddEventToConfig()
/*
 * Юзер создал новый ивент и кликнул "сохранить".
 * Необходимо дописать новый ивент в конфиг и перезагрузить
 * диалог со списком всех ивентов.
 */
{
    qDebug() << "need add event to config";
    bool need_update_config = false;

    if (m_EventToAdd->getOpts().event_type == EV_NONE
        || m_EventToAdd->getOpts().message_type == EV_MSG_NONE)
    {
        std::shared_ptr<QMessageBox> mbox (new QMessageBox(NULL));
        mbox->setWindowTitle("Time To Relax");
        mbox->setText("Не заполнены все необходимые поля!");
        mbox->setWindowFlags(Qt::WindowStaysOnTopHint);
        mbox->exec();
    }
    else
    {
        // save event to config
        Event *ev = m_EventToAdd.get();
        m_Events.push_back(*ev);        // copy
        save_events(m_Conf, m_Events);
        need_update_config = true;
    }

    m_AddEventDialog.reset();
    if (need_update_config)
    {
        slotShowConfigDialog();
    }
}
