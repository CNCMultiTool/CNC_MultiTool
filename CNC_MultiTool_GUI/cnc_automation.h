#ifndef CNC_AUTOMATION_H
#define CNC_AUTOMATION_H


#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include <QTime>
#include <QFile>
#include <QTextStream>
#include <QRegExp>

class CNC_automation : public QThread
{
    Q_OBJECT
private:
    QString m_FileName;
    QString loadTextFile();
    float m_X;
    float m_Y;
    float m_Z;
    float m_W;
    float m_F;
    float m_S;
    bool m_validCommand;

    void getValue(const QString indent,const QString line,float *target);
    bool isCommand(const QString indent,const QString line);


public:
    CNC_automation();
    ~CNC_automation();
    void GCode_Parser(QString fileName);

signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
};

#endif // CNC_AUTOMATION_H
