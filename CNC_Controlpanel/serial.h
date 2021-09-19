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
    int m_send_reciveTimeout = 50;
    int m_recive_timeout = 10;

    QSerialPort m_serial;
    cnc_data *m_database;
    QTimer *timer = new QTimer(this);
    QTimer *sendAnswerTimeout = new QTimer(this);
    void addChecksum(QByteArray *mes);

    QByteArray m_recivedBytes;
    QByteArray m_lastsend = "";

    void CobsEncode(const QByteArray &rawData, QByteArray &encodedData);
signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void show_serial(bool b);
    void recLine(const QString &s);

public slots:
    void serial_send(QByteArray mes);
    void serial_sendTimeout();
    void serial_read(); //changed to slot
};

#endif // SERIAL_H
