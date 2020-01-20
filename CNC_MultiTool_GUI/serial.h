#ifndef SERIAL_H
#define SERIAL_H

#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include <QSerialPort>
#include <QTime>
#include <QEventLoop>
#include "cnc_data.h"

union tTelegram
{
    float Value[4];
    char Bytes[16];
};


class Serial : public QThread
{
    Q_OBJECT
public:
    explicit Serial(cnc_data *database = nullptr);
    ~Serial();
    void open_close(const QString &portName);
    //void send(char command,float value1,float value2,float value3,float value4);

private:

    cnc_data *m_database;
    void run() override;
    QString m_portName;
    bool m_quit;
    QMutex m_mutex;
    tTelegram m_recive_telegram;
    tTelegram m_send_telegram;

    QEventLoop m_wait_send_loop;
    QList<QByteArray> m_SendData;
    bool m_dataReadyToSend;

signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void recived(char command,float value1,float value2,float value3,float value4);
    void show_serial(bool isOpen);
    void show_loops();

public slots:
    void send(char command,float value1,float value2,float value3,float value4);
};

#endif // SERIAL_H
