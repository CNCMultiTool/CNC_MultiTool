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

    bool serial_open();
    void serial_close();

private:
    int m_send_timeout = 0;
    int m_recive_timeout = 0;

    QSerialPort m_serial;
    cnc_data *m_database;

    QByteArray m_recivedBytes;

signals:
    void Log(const QString &s);
    void errorLog(const QString &s);

public slots:
    void serial_read();
    void serial_send(QString mes);
};

#endif // SERIAL_H
