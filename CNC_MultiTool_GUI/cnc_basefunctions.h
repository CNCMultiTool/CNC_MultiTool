#ifndef CNC_BASEFUNCTIONS_H
#define CNC_BASEFUNCTIONS_H

#include <cnc_data.h>
#include <serial.h>
#include <QTime>
#include <QString>
#include <QObject>
#include <QEventLoop>
#include <QThread>
typedef struct{
    float X;
    float Y;
    float Z;
}point;

class cnc_basefunctions: public QObject
{
    Q_OBJECT
public:
    cnc_basefunctions(cnc_data *database = nullptr);
    void test();
    void calib_size_safe();
    void calib_size_results();
    void repeattest_results();
    void z_calib_safePos();
    void z_calib_results();
    void wait_for_heating();
    void trigger_next_command();
    QList<point> m_pointList;

private:
    void send_to_cnc(char commmand,float v1,float v2,float v3,float v4,int action);
    cnc_data *m_database;
    void execute_command(char command,float value1,float value2,float value3,float value4);



signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void trigger_send();
    void show_send_queue();

public slots:
    void send_init();

    void send_move(float X,float Y,float Z,float W);
    void send_settings(float speed,float temperatur,float filament);
    void send_stop();
    void send_getPosition();
    void send_setPosition(float X,float Y,float Z,float W);
    void setPosition_inQ(float X,float Y,float Z,float W);
    void move_inQ(float X,float Y,float Z,float W);
    void settings_inQ(float speed,float temperatur,float filament);
    void process_command();
};

#endif // CNC_BASEFUNCTIONS_H
