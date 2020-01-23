#ifndef SERIAL_H
#define SERIAL_H

#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include <QSerialPort>
#include <QTime>
#include <QTimer>
#include <QEventLoop>
#include <QCoreApplication>
#include "cnc_data.h"

class Serial : public QObject
{
    Q_OBJECT
public:
    Serial(cnc_data *database = nullptr);
    ~Serial();
    //void send(char command,float value1,float value2,float value3,float value4);

private:
    QSerialPort MySerial;
    cnc_data *m_database;
    QMutex m_mutex;
    QByteArray responseData;
    QList<QByteArray> m_SendData;
    QTimer *timer = new QTimer(this);

signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void recived(char command,float value1,float value2,float value3,float value4);

public slots:
    //void send(char command,float value1,float value2,float value3,float value4);
    void recive();
    void serial_start();
    void serial_close();
    void send(char command,float value1,float value2,float value3,float value4);

};

#endif // SERIAL_H
