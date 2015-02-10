#include <QApplication>

#include "context_menu.h"

ContextMenu::ContextMenu(QObject *parent)
{
    m_Menu.reset(new QMenu("tray menu"));
}

ContextMenu::~ContextMenu()
{

}

void ContextMenu::addAction(QAction *action)
{
    if (action == NULL)
        throw std::runtime_error("bad action");

    m_Menu->addAction(action);
}
