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

    void act_pose_recived();
    void G_Code_Start(QString fileName);
    void G_Code_Pause();
    void G_Code_Stop();
    QEventLoop m_loop;

private:
    void run() override;
    float m_X;
    float m_Y;
    float m_Z;
    float m_W;
    float m_S;
    float m_F;
    bool m_validCommand;
    bool m_pose_recived;
    bool m_pause;
    bool m_aboard;
    void getValue(const QString indent,const QString line,float *target);
    bool isCommand(const QString indent,const QString line);
    void wait_for_finish();
    void moveAndWait(float X,float Y,float Z,float W);
    QString m_fileName;

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
