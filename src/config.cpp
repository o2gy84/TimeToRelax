#include <QDebug>
#include "config.h"

Config::Config(QObject *parent) :
    QDialog(NULL)
{
    if (parent)
    {
        // do nothing
    }
}


Config::~Config()
{

}


void Config::slotShow()
{
    qDebug() << "menu config clicked";
    QDialog::show();
}
