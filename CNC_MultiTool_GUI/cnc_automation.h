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
#include <QEventLoop>
#include <math.h>

#include "cnc_data.h"
#include "cnc_basefunctions.h"

class CNC_automation : public QObject
{
    Q_OBJECT
public:
    CNC_automation(cnc_data *database = nullptr,cnc_basefunctions *basefunctions=nullptr);
    ~CNC_automation();

    void move_home();
    void repeat_test();
    void Z_calib();
    void calib_size();

    void G_Code_Start(QString fileName);
    void G_Code_Pause();
    void G_Code_Stop();

private:
    cnc_data *m_database;
    cnc_basefunctions *m_basefunctions;

    void repeat_movement(float speed,float dist,int repeat);
    float probe_Z(float X,float Y);

    void getValue(const QString indent,const QString line,float *target);
    bool isCommand(const QString indent,const QString line);
    void G_Code_Parser();

    QString m_fileName;
    bool m_aboard;
    bool m_validCommand;
    bool m_pause;
    float m_X;
    float m_Y;
    float m_Z;
    float m_W;
    float m_S;
    float m_F;
    float m_F_max;
    float m_F_old;
    QEventLoop m_wait_loop;


signals:
    void Log(const QString &s);
    void errorLog(const QString &s);

public slots:
    float calc_correction(float X,float Y);
    void calc_correctionangel(QList<float>);
};

#endif // CNC_AUTOMATION_H
