#ifndef CONTEXT_MENU_H
#define CONTEXT_MENU_H


#include <QMenu>

#include <memory>

#include <config.h>

class ContextMenu: public QObject
{
    Q_OBJECT

public:
    ContextMenu(QObject *parent);
    virtual ~ContextMenu();

// GETTERS:
    QMenu *menu()   { return m_Menu.get(); }

private:

    std::shared_ptr<QMenu> m_Menu;
    std::shared_ptr<Config> m_Config;

};

#endif // CONTEXT_MENU_H
