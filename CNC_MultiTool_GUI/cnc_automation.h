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
#include <math.h>

#include "cnc_data.h"
#include "cnc_basefunctions.h"

class CNC_automation : public QObject
{
    Q_OBJECT
public:
    CNC_automation(cnc_data *database = nullptr,cnc_basefunctions *basefunctions=nullptr);
    ~CNC_automation();
    void GCode_Parser(QString fileName);
    void act_pose_recived();

private:
    cnc_data *m_database;
    cnc_basefunctions *m_basefunctions;

    //position
    float m_X;
    float m_Y;
    float m_Z;
    float m_W;
    //settings
    float m_speed;
    float m_filament;
    float m_temperatur;
    //correktion angel
    float m_X_angel;
    float m_Y_angel;
    //max size of CNC
    float m_size_X;
    float m_size_Y;
    //kalibration results
    float m_result_X_max_Y_null;
    float m_result_X_max_Y_max;
    float m_result_X_null_Y_max;
    float m_result_X_null_Y_null;


    float calc_correction(float X,float Y);

    void getValue(const QString indent,const QString line,float *target);
    bool isCommand(const QString indent,const QString line);
    void wait_for_finish();

signals:
    void Log(const QString &s);
    void errorLog(const QString &s);

public slots:
    void G_Code_Start(QString fileName);
    void G_Code_Pause();
    void G_Code_Stop();

    void move_home();
    void calc_correctionangel(QList<float>);
    void repeat_test();
    void Z_calib();
    void calib_size();

};

#endif // CNC_AUTOMATION_H
