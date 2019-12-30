#ifndef AUTOFUNCTIONS_H
#define AUTOFUNCTIONS_H

#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QEventLoop>
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

    QEventLoop m_loop;
    float m_act_X;
    float m_act_Y;
    float m_act_Z;
    float m_act_W;
    float m_act_X_end;
    float m_act_Y_end;
    float m_act_Z_end;

private:
    float m_size_X;
    float m_size_Y;

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
    QString m_fileName;

    void g_code_processor();
    int m_programm;

signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void send_move(float X,float Y,float Z,float W);
    void send_settings(float speed,float temperatur,float filament);
    void send_stop();
    void send_getPosition();
    void send_setPosition(float X,float Y,float Z,float W);
};

#endif // AUTOFUNCTIONS_H
