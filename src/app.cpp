#include <QDebug>
#include <QAction>

#include "app.h"
#include "config.h"


App::App(int argc, char *argv[])
    :QApplication(argc, argv)
{
    qDebug() << "start app";

    std::shared_ptr<Config> config(new Config(this));
    m_Icon.reset(new Icon(this));
    m_Menu.reset(new ContextMenu(this));

    QAction *configAction = new QAction("settings", this);
    QAction *resetAction = new QAction("reset", this);
    QAction *quitAction = new QAction("quit", this);

    QObject::connect(configAction, SIGNAL(triggered()), config.get(), SLOT(slotShowConfigDialog()));
    QObject::connect(resetAction, SIGNAL(triggered()), m_Icon.get(), SLOT(slotReset()));
    QObject::connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));

    m_Menu->addAction(configAction);
    m_Menu->addAction(resetAction);
    m_Menu->addAction(quitAction);

    m_Icon->setMenu(m_Menu);
    m_Icon->setConfig(config);

    setQuitOnLastWindowClosed(false);
}

App::~App()
{
    qDebug() << "close app";
}
