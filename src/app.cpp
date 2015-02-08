#include <QDebug>
#include <QAction>

#include "app.h"


App::App(int argc, char *argv[])
    :QApplication(argc, argv)
{
    qDebug() << "start app";

    m_Icon.reset(new Icon(this));
    m_Menu.reset(new ContextMenu(this));
    m_Icon->setMenu(m_Menu);

    setQuitOnLastWindowClosed(false);
}

App::~App()
{
    qDebug() << "close app";
}
