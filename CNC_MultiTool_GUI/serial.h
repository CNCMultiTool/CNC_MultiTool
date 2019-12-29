#ifndef SERIAL_H
#define SERIAL_H

#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include <QSerialPort>
#include <QTime>

union tTelegram
{
    float Value[4];
    char Bytes[16];
};


class Serial : public QThread
{
    Q_OBJECT
public:
    explicit Serial();
    ~Serial();
    bool open(const QString &portName);
    void send(char command,float value1,float value2,float value3,float value4);

private:
    void run() override;
    QSerialPort *m_serial;
    QString m_portName;
    bool m_quit;
    QMutex m_mutex;
    QWaitCondition m_cond;
    tTelegram m_recive_telegram;
    tTelegram m_send_telegram;


signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void ProcesCommand(const char command,float value1,float value2,float value3,float value4);

};

#endif // SERIAL_H
