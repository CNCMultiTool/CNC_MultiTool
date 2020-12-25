#ifndef SERIAL_H
#define SERIAL_H

//#include <QMutex>
//#include <QThread>
//#include <QWaitCondition>
//#include <QSerialPort>
//#include <QTime>
#include <QTimer>
//#include <QEventLoop>
//include <QCoreApplication>
#include <QThread>
#include "cnc_data.h"


class Serial : public QObject
{
    Q_OBJECT
    QThread thread;



public:
    Serial(cnc_data *database = nullptr);
    ~Serial();

    bool serial_open();
    void serial_close();

private:
    int m_send_timeout = 10;
    int m_recive_timeout = 10;

    QSerialPort m_serial;
    cnc_data *m_database;
    QTimer *timer = new QTimer(this);
    QByteArray m_recivedBytes;

signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void show_serial(bool b);
    void recLine(const QString &s);

public slots:
    //void serial_read();
    void serial_send(QString mes);

    void serial_read(); //changed to slot
    void myslot()
    {
      //Here I want to put myfunction into a thread
      moveToThread(&thread);
      connect(&thread, SIGNAL(started()), this, SLOT(serial_read())); //cant have parameter sorry, when using connect
      thread.start();
    }





};

#endif // SERIAL_H
