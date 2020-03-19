#ifndef CNC_BASEFUNCTIONS_H
#define CNC_BASEFUNCTIONS_H

#include <cnc_data.h>
#include <serial.h>
#include <QTime>
#include <QString>
#include <QObject>
#include <QEventLoop>
#include <QThread>
#include <QWaitCondition>
#include <QList>

typedef struct{
    char c;
    float v1;
    float v2;
    float v3;
    float v4;
}cnc_command;

class cnc_basefunctions: public QObject
{
    Q_OBJECT
public:
    cnc_basefunctions(cnc_data *database = nullptr);
    //~cnc_basefunctions();
    void test();

    void serial_send(char command,float value1,float value2,float value3,float value4);

private:
    cnc_data *m_database;
    void send_ConfirmAnswer();
    void addComandToQueue(char c,float v1,float v2,float v3,float v4,bool prio);
    void sendNextComandFromQueue();
    QList<cnc_command> commandQueue;

signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void send(char command,float value1,float value2,float value3,float value4);
    void answer_repeatrequest();
    void reach_pos();

public slots:
    void send_move(float X,float Y,float Z,float W);
    void send_settings(float speed,float temperatur,float filament);
    void send_stop();
    void send_getPosition();
    void send_setPosition(float X,float Y,float Z,float W);
    void recived(char command,float value1,float value2,float value3,float value4);
};

#endif // CNC_BASEFUNCTIONS_H
