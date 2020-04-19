#ifndef CNC_DATA_H
#define CNC_DATA_H


#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include <QSerialPort>
#include <QList>
#include <QSettings>
#include <QtMath>

union tTelegram
{
    float Value[4];
    char Bytes[16];
};

typedef struct{
    float X;
    float Y;
    float Z;
}point;

typedef struct
{
    char command;
    float value1;
    float value2;
    float value3;
    float value4;
    int action;
}cnc_command;

class cnc_data : public QObject
{
    Q_OBJECT
public:
    cnc_data();

    void FileLog(QString value);

    bool m_HWisHeating;
    bool m_HWisMoving;


    //values to send
    //settings
    float m_soll_speed;
    float m_soll_filament;
    float m_soll_temperatur;
    float m_soll_accSteps;

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
    float m_act_accSteps;

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

    float m_z_offset;

    //max values
    float m_max_speed; //mm per sec

    void set_position(float X,float Y,float Z,float W);
    void set_settings(float speed,float temperatur,float filament,float soll_temperatur);
    void set_soll_settings(float speed,float temperatur,float filament,float acc);
    void set_endswitch(int X,int Y,int Z);
    void set_serial(bool isOpen);

    void saveSettings();
    void loadSettings();

    void calc_correctionangel(QList<point> Z_errors);
    float calc_correction(float X,float Y);

    void test();

    void set_HWisHeating(bool status);
    void set_HWisMoving(bool status);
    
    void append_recive_command(cnc_command new_command);

    bool m_SerialIsOpen;
    QString m_SerialPortName;
    QList<cnc_command> cnc_send_commands;
    QList<cnc_command> cnc_recive_commands;

private:
    QString m_LogFileName = "CNC_Log.txt";
    QFile *m_LogFile;
    QMutex m_mutex;
    ~cnc_data();

signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void show_position();
    void show_settings();
    void show_endswitch(float X,float Y,float Z);
    void show_serial(bool isOpen);
    void show_status();
    void recive_command();


//private slots:

};

#endif // CNC_DATA_H
