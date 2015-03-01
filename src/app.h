#ifndef APP_H
#define APP_H

#include <QApplication>

#include <memory>

#include "icon.h"
#include "context_menu.h"


class App: public QApplication
{
public:
    App(int argc, char *argv[]);
    ~App();

public slots:


private:

    std::shared_ptr<Icon> m_Icon;               // system tray icon
    std::shared_ptr<ContextMenu> m_Menu;        // context menu
};


#endif // APP_H
