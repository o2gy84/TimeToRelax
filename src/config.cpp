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
#include <QScrollArea>

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
/*
 * Читаем настройки из конфига
 *
 */
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

        // just in case
        if (opt.event_type != EV_NONE && opt.message_type != EV_MSG_NONE)
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
    QVBoxLayout *layout = new QVBoxLayout ( /*m_ConfigDialog.get()*/ );
    layout->setAlignment(Qt::AlignTop);

    int i = 0;
    for (const auto &event : m_Events)
    {
        QLabel *label1 = new QLabel("Имя: " + event.getOpts().name);
        QLabel *label_msg = new QLabel("Сообщение: " + event.getOpts().message);

        QVBoxLayout *vbox_layout = new QVBoxLayout ();
        vbox_layout->addWidget(label1);
        vbox_layout->addWidget(label_msg);

        if (event.getOpts().message_type == EV_MSG_MESSAGE_BOX)
        {
            QLabel *msg_type_label = new QLabel("Тип сообщения: message box");
            vbox_layout->addWidget(msg_type_label);
        }
        else if (event.getOpts().message_type == EV_MSG_SYS_TRAY_MESSAGE)
        {
            QLabel *msg_type_label = new QLabel("Тип сообщения: область уведомлений");
            vbox_layout->addWidget(msg_type_label);
        }
        else
        {
            qDebug() << "config error";
            continue;
        }

        if (event.getOpts().event_type == EV_PERIODIC_TIMER)
        {
            QLabel *label2 = new QLabel("Тип: периодический");
            QString info = "Частота: раз в ";
            info += QString::number(event.getOpts().timer_period_min);
            info += " минут";
            QLabel *label3 = new QLabel(info);
            vbox_layout->addWidget(label2);
            vbox_layout->addWidget(label3);
        }
        else if (event.getOpts().event_type == EV_SINGLE_TIMER)
        {
            QLabel *label2 = new QLabel("Тип: однократный");
            QString info = "Срабатывание: " + event.getOpts().timer_timeout_date.toString();
            QLabel *label3 = new QLabel(info);
            vbox_layout->addWidget(label2);
            vbox_layout->addWidget(label3);
        }
        else
        {
            qDebug() << "config error";
            continue;
        }

        QPushButton *edit = new QPushButton ("edit");
        edit->setMaximumWidth(50);
        QPushButton *del = new QPushButton ("delete");
        del->setMaximumWidth(50);

        QGridLayout *group_box_layout = new QGridLayout();
        group_box_layout->addLayout(vbox_layout, i, 0);
        group_box_layout->addWidget(edit, i, 1);
        group_box_layout->addWidget(del, i, 2);

        QGroupBox *group_box = new QGroupBox ("event: " + QString::number(i+1));
        group_box->setFlat(false);
        group_box->setMaximumHeight(150);

        group_box->setLayout(group_box_layout);

        layout->addWidget(group_box);
        ++i;
    }

    QVBoxLayout *top_layout = new QVBoxLayout(m_ConfigDialog.get());
    {
        QPushButton *button_add = new QPushButton ("new event");
        button_add->setMaximumWidth(70);
        QObject::connect(button_add, SIGNAL (clicked()), this, SLOT (slotShowAddEventDialog()));

        QWidget *widget = new QWidget();
        widget->setLayout(layout);
        QScrollArea *scrollarea = new QScrollArea();
        scrollarea->setWidget(widget);

        top_layout->addWidget(button_add);
        top_layout->addWidget(scrollarea);
    }

    m_ConfigDialog->setLayout(top_layout);
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

    QSize size (400, 400);
    const QRect screen = QApplication::desktop()->screenGeometry();

    int left_x = screen.width() / 2 - size.width() / 2;
    int left_y = screen.height() / 2 - size.height() / 2;

    QPoint left_top(left_x, left_y);
    QRect dialog_geometry (left_top, size);

    m_ConfigDialog->setGeometry(dialog_geometry);

    updateDialogEvents();

    m_ConfigDialog->show();
    m_ConfigDialog->setFixedWidth(m_ConfigDialog->width());
}

void Config::slotShowAddEventDialog()
/*
 *  Показать диалог добавления нового ивента.
 */
{
    qDebug() << "add event dialog";

    EventOptions opts;

    // some default values
    {
        opts.timer_timeout_date = QDateTime::currentDateTime();
        opts.name = "таймер: " + QString::number(m_Events.size());
    }
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
    name_edit->setText(opts.name);
    QObject::connect(name_edit, &QLineEdit::editingFinished, [name_edit, this]()
    {
        m_EventToAdd->getOpts().name = name_edit->text();
    });

    QLineEdit *text_edit = new QLineEdit();
    text_edit->setText(opts.message);
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
            mins_edit->setText(QString::number(opts.timer_period_min));
            QObject::connect(mins_edit, &QLineEdit::editingFinished, [mins_edit, this]()
            {
                m_EventToAdd->getOpts().timer_period_min = mins_edit->text().toInt();
            });

            QDateTimeEdit *date_time_edit = new QDateTimeEdit();
            date_time_edit->setMaximumWidth(180);
            date_time_edit->setMinimumDateTime(opts.timer_timeout_date);
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

        EventOptions opts = ev->getOpts();
        qDebug() << "save event. name: " << opts.name
                    << "message: " << opts.message
                    << "datetime: " << opts.timer_timeout_date;
    }

    m_AddEventDialog.reset();
    if (need_update_config)
    {
        slotShowConfigDialog();
    }
}
