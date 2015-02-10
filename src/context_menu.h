#ifndef CONTEXT_MENU_H
#define CONTEXT_MENU_H

#include <QMenu>
#include <memory>

class ContextMenu: public QObject
{
    Q_OBJECT

public:
    ContextMenu(QObject *parent);
    virtual ~ContextMenu();

    void addAction(QAction *action);

// GETTERS:
    QMenu *menu()   { return m_Menu.get(); }

private:

    std::shared_ptr<QMenu> m_Menu;
};

#endif // CONTEXT_MENU_H
