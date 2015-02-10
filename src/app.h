#ifndef APP_H
#define APP_H

#include <QApplication>

#include <memory>

#include "icon.h"
#include "context_menu.h"
#include "config.h"


class App: public QApplication
{
public:
    App(int argc, char *argv[]);
    ~App();

public slots:


private:

    std::shared_ptr<Icon> m_Icon;               // system tray icon
    std::shared_ptr<ContextMenu> m_Menu;        // context menu
    std::shared_ptr<Config> m_Config;           // app config

};


#endif // APP_H
