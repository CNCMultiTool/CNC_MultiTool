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

class CNC_automation : public QThread
{
    Q_OBJECT
public:
    CNC_automation(cnc_data *database = nullptr,cnc_basefunctions *basefunctions=nullptr);
    ~CNC_automation();

    void move_home();
    void move_restposi();
    void repeat_test();
    void Z_calib();
    void calib_size();

    void G_Code_Start(QString fileName);


private:
    QFile m_inputFile;
    QTextStream m_in;
    void run() override;
    cnc_data *m_database;
    cnc_basefunctions *m_basefunctions;

    QTimer *timer = new QTimer(this);
    QFile inputFile;
    QTextStream in;
    void probe_Z(float X,float Y);
    void getValue(const QString indent,const QString line,float *target);
    bool isCommand(const QString indent,const QString line);

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

signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void trigger_send();
    void resend_last();

public slots:
    void G_Code_Parser();
    void G_Code_Pause();
    void G_Code_Stop();
    //float calc_correction(float X,float Y);
    //void calc_correctionangel(QList<float>);
};

#endif // CNC_AUTOMATION_H
