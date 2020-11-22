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
    void test();

private:
    cnc_data *m_database;

signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void DebugLog(const QString &s);
    void show_send_mes(QString mes);



public slots:
    void send_init();
    void send_acc(float acc, float speed_min);
    void send_PID(float P,float I,float D,float PO);
    void send_Temp_Setting(float R_vor,float B_Value,float R_nen);
    void send_PID_Bed(float P,float I,float D,float PO);
    void send_Temp_Setting_Bed(float R_vor,float B_Value,float R_nen);
    void send_move(float X,float Y,float Z,float E);
    void send_settings(float speed,float temperatur,float filament,float bed_temp);
    void send_stop();
    void send_getPosition();
    void send_setPosition(float X,float Y,float Z,float W);
};

#endif // CNC_BASEFUNCTIONS_H
