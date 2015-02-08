#include <QApplication>

#include "context_menu.h"

ContextMenu::ContextMenu(QObject *parent)
{
    m_Config.reset(new Config(parent));

    QAction *configAction = new QAction("settings", parent);
    QAction *quitAction = new QAction("quit", parent);

    QObject::connect(configAction, SIGNAL(triggered()), m_Config.get(), SLOT(show()));
    QObject::connect(quitAction, SIGNAL(triggered()), parent, SLOT(quit()));

    m_Menu.reset(new QMenu("tray menu"));
    m_Menu->addAction(configAction);
    m_Menu->addAction(quitAction);
}

ContextMenu::~ContextMenu()
{

}
