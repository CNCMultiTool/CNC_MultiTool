#ifndef CNC_DATA_H
#define CNC_DATA_H


#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
class cnc_data : public QObject
{
    Q_OBJECT
public:
    cnc_data();

    void FileLog(QString value);

    bool m_is_heated;
    bool m_g_code_inProcess;
    bool m_wait_heating;

    //values to send
    //settings
    float m_soll_speed;
    float m_soll_filament;
    float m_soll_temperatur;

    //values recived
    //position
    float m_act_X;
    float m_act_Y;
    float m_act_Z;
    float m_act_W;
    //settings
    float m_act_speed;
    float m_act_filament;
    float m_act_temperatur;
    float m_output;

    //status
    float m_endswitch_X;
    float m_endswitch_Y;
    float m_endswitch_Z;

    //calibraten values
    //max size of CNC
    float m_size_X;
    float m_size_Y;
    //kalibration results
    float m_error_X_max_Y_null;
    float m_error_X_max_Y_max;
    float m_error_X_null_Y_max;
    float m_error_X_null_Y_null;
    //correktion angel
    float m_X_angel;
    float m_Y_angel;
    //TCP hight in home
    float m_Zmax_nozzel;

    void set_position(float X,float Y,float Z,float W);
    void set_settings(float speed,float temperatur,float filament,float output);
    void set_soll_settings(float speed,float temperatur,float filament);
    void set_endswitch(int X,int Y,int Z);
    void set_serial(bool isOpen);

    void test();

private:
    //status
    bool m_SerialIsOpen;
    QString m_LogFileName = "CNC_Log.txt";
    QFile *m_LogFile;
    QMutex m_file_mutex;
    ~cnc_data();

signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void show_position();
    void show_settings();
    void show_endswitch(float X,float Y,float Z);
    void show_serial(bool isOpen);

//private slots:

};

#endif // CNC_DATA_H
