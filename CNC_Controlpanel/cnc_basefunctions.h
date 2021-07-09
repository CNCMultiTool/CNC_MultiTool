#ifndef CNC_BASEFUNCTIONS_H
#define CNC_BASEFUNCTIONS_H

#include <cnc_data.h>
#include <serial.h>
#include <QString>
#include <QObject>
#include <QVector>
#include <serial.h>
#include "cnc_autofunctions.h"

class cnc_basefunctions: public QObject
{
    Q_OBJECT
public:
    cnc_basefunctions(cnc_data *database = nullptr,cnc_autofunctions *autofunc = nullptr  ,Serial *mySerial = nullptr);
    ~cnc_basefunctions();
    void test();

    void send_NTC_values_bed(float *bitwertNTC_bed,float *widerstandNTC_bed,float *widerstand1_bed);
    void send_NTC_values(float *bitwertNTC,float *widerstandNTC,float *widerstand1);
    void send_PID_values(float *P, float *I, float *D, bool *E);
    void send_moveparam(float *acc,float *min_speed,float *max_speed,float *threshAngle,float *filament);
    void send_Speed(float F);
    void send_Filament(float S);
    void send_BedTemp(float S);
    void send_HotendTemp(float S);
    void send_move(float *X, float *Y, float *Z, float *E, float *F);
    void send_setPosition(float *X,float *Y,float *Z,float *E);
    void send_stop();

    void send_GCodeStart(QString file);
    void send_GCodePause();
    void send_GCodeContinue();
    void send_GCodeStop();

    void send_XXX();

    void send_moveHome();
    void send_getPosition();
    void send_setESuse(bool state);
    void send_setMotorUse(bool state);
    void send_resetWaitForHeat();
private:
    QString m_fileName;
    QFile m_inputFile;
    QTextStream m_in;
    //void run() override;
    cnc_data *m_database;
    cnc_autofunctions *m_auto;
    Serial *m_mySerial;
    QVector<QString> m_ComandLines;
    int m_send_buffer_counter;
    int m_send_counter;
    int m_read_counter;

signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void serial_send(const QString &mes);
    void show_position();
    void show_temp();
    void show_endswitch();
    void show_acc_speed_fila();
    void show_speed();
    void show_act_temp();
    void show_waitForHeat(float);


private slots:
    void processLine(const QString &s);


};

#endif // CNC_BASEFUNCTIONS_H
