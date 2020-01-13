#ifndef CNC_BASEFUNCTIONS_H
#define CNC_BASEFUNCTIONS_H

#include <cnc_data.h>
#include <serial.h>
#include <QTime>
#include <QString>
#include <QObject>
#include <QEventLoop>
#include <QThread>
class cnc_basefunctions: public QObject
{
    Q_OBJECT
public:
    cnc_basefunctions(cnc_data *database = nullptr);
    void move_wait(float X,float Y,float Z,float W);
    void wait_heat();
    void test();

private:
    cnc_data *m_database;
    QEventLoop m_loop;
    bool m_atPose;

signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void send(char command,float value1,float value2,float value3,float value4);
    void G_Code_Parser();

public slots:
    void send_move(float X,float Y,float Z,float W);
    void send_settings(float speed,float temperatur,float filament);
    void send_stop();
    void send_getPosition();
    void send_setPosition(float X,float Y,float Z,float W);
    void recived(char command,float value1,float value2,float value3,float value4);
};

#endif // CNC_BASEFUNCTIONS_H
