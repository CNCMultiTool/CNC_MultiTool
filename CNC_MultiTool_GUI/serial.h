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

    void serial_open();
    void serial_close();
    void stop_timeouts();


private:
    int m_send_timeout = 0;
    int m_recive_timeout = 0;
    int m_fast_timeout = 300;

    QSerialPort m_serial;
    cnc_data *m_database;

    tTelegram m_recive_telegram;
    tTelegram m_send_telegram;

    QByteArray m_recivedBytes;
    QByteArray m_sendBytes;
    QByteArray m_sendBytesLast;

    bool m_serial_running;

    int m_TelegramLength = 19;

    QTimer serial_timeout;
    QTimer serial_fast_timeout;
    QTime debug_time;
    int serial_CheckTelegram();
    int serial_calcCheckSumm(QByteArray bytes,unsigned char *Checksumm);

signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void show_send_queue();

public slots:
    void serial_read_command();
    void serial_send_command();
    void serial_timeout_handler();
    void serial_fast_timeout_handler();
    void send_last();

};

#endif // SERIAL_H
