#ifndef CONFIG_H
#define CONFIG_H

#include <QDialog>

class Config: public QDialog
{
    Q_OBJECT

public:

    Config(QObject *parent);
    virtual ~Config();

public slots:
    void show();

private:

};


#endif // CONFIG_H
