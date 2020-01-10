#ifndef AUTOFUNCTIONS_H
#define AUTOFUNCTIONS_H

#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QEventLoop>
#include <QList>

class AutoFunctions : public QThread
{
    Q_OBJECT
public:
    AutoFunctions();
    ~AutoFunctions();

    void G_Code_Start(QString fileName);
    void G_Code_Pause();
    void G_Code_Stop();
    void move_home();
    void calib_size();
    void repeat_test();
    void Z_calib();

    QEventLoop m_loop;
    float m_act_X;
    float m_act_Y;
    float m_act_Z;
    float m_act_W;
    float m_act_X_end;
    float m_act_Y_end;
    float m_act_Z_end;
    float m_size_X;
    float m_size_Y;

private:

    void run() override;
    float m_X;
    float m_Y;
    float m_Z;
    float m_W;
    float m_S;
    float m_F;
    float m_Fold;
    float m_Fmax;
    bool m_validCommand;
    bool m_pause;
    bool m_aboard;
    void getValue(const QString indent,const QString line,float *target);
    bool isCommand(const QString indent,const QString line);
    void wait_for_finish();
    void moveAndWait(float X,float Y,float Z,float W);
    void repeat_movement(float speed,float dist,int repeat);
    float probe_Z(float X,float Y);
    QString m_fileName;

    void g_code_process();
    void move_home_process();
    void calib_size_process();
    void repeat_test_process();
    void Z_calib_process();
    int m_programm;

signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void send_move(float X,float Y,float Z,float W);
    void send_settings(float speed,float temperatur,float filament);
    void send_stop();
    void send_getPosition();
    void send_setPosition(float X,float Y,float Z,float W);
    void calc_Zplane(QList<float> Z_errors);
};

#endif // AUTOFUNCTIONS_H
