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
<<<<<<< HEAD
    QSerialPort MySerial;
    cnc_data *m_database;
    QMutex m_mutex;
    QMutex m_serial_mutex;
    QByteArray responseData;
    QByteArray lastSendData;
    QList<QByteArray> m_SendData;
    QTimer *timer = new QTimer(this);
    QTimer *send_timeout = new QTimer(this);
    QTimer *setting_timer = new QTimer(this);
=======
    cnc_data *m_database;
    void run() override;
    QSerialPort *m_serial;
    QString m_portName;
    bool m_quit;
    QMutex m_mutex;
    QWaitCondition m_cond;
    tTelegram m_recive_telegram;
    tTelegram m_send_telegram;

    QEventLoop m_wait_send_loop;
    QByteArray m_SendData;
    bool m_dataReadyToSend;
>>>>>>> parent of 015e97d... still bugy

signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void recived(char command,float value1,float value2,float value3,float value4);
<<<<<<< HEAD
=======
    void show_serial(bool isOpen);
>>>>>>> parent of 015e97d... still bugy

public slots:
    //void send(char command,float value1,float value2,float value3,float value4);
    void recive();
    void serial_start();
    void serial_close();
    void send(char command,float value1,float value2,float value3,float value4);
    void error_handler();
    void request_settings();
    void answer_repeatrequest();

};

#endif // SERIAL_H
