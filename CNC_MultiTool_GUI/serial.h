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

private:
    QSerialPort m_serial;
    cnc_data *m_database;

    tTelegram m_recive_telegram;
    tTelegram m_send_telegram;

    QByteArray m_recivedBytes;
    QByteArray m_sendBytes;

    int m_TelegramLength = 19;

    int serial_CheckTelegram();
    int serial_calcCheckSumm(QByteArray bytes,unsigned char *Checksumm);

    int serial_send_ifValid();
signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void show_send_queue();

public slots:
    void serial_read_command();
    void serial_send_command();

};

#endif // SERIAL_H
