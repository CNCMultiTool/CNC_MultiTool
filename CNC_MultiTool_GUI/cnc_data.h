#ifndef CNC_DATA_H
#define CNC_DATA_H


#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include <QSerialPort>
#include <QEventLoop>

union tTelegram
{
    float Value[4];
    char Bytes[16];
};

class cnc_data : public QObject
{
    Q_OBJECT
public:
    cnc_data();

    void FileLog(QString value);

    QString m_SerialPortName;

    bool m_HWisHeating;
    bool m_HWisAtHeat;
    bool m_HWisMoving;


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

    void set_HWisHeating(bool status);
    void set_HWisMoving(bool status);

    bool m_SerialIsOpen;
    bool m_Serial_quit;
    void serial_send(char command,float value1,float value2,float value3,float value4);

private:
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
    void show_status();

//private slots:

};

#endif // CNC_DATA_H
