#ifndef CNC_AUTOMATION_H
#define CNC_AUTOMATION_H


#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include <QTime>
#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QMainWindow>
#include <QObject>

#include <serial.h>
#include <mainwindow.h>
#include <autofunctions.h>

class CNC_automation : public QObject
{
    Q_OBJECT
private:
    //position
    float m_X;
    float m_Y;
    float m_Z;
    float m_W;
    //settings
    float m_speed;
    float m_filament;
    float m_temperatur;

    void getValue(const QString indent,const QString line,float *target);
    bool isCommand(const QString indent,const QString line);
    void wait_for_finish();

    //
    Serial m_Serial;
    MainWindow m_MainWindow;
    AutoFunctions m_AutoFunctions;

public:
    CNC_automation();
    ~CNC_automation();
    void GCode_Parser(QString fileName);
    void act_pose_recived();

public slots:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void send(char command,float value1,float value2,float value3,float value4);
    void send_move(float X,float Y,float Z,float W);
    void send_settings(float speed,float temperatur,float filament);
    void send_stop();
    void send_getPosition();
    void send_setPosition(float X,float Y,float Z,float W);
    void serial_open_close(QString portName);
    void serial_show(bool isOpen);
    void process_recived(char command,float value1,float value2,float value3,float value4);
    void G_Code_Start(QString fileName);
    void G_Code_Pause();
    void G_Code_Stop();

signals:
    void reached_position();
};

#endif // CNC_AUTOMATION_H
