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
    unsigned char Bytes[16];
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
    void SerialLog(QString value);

    int m_HW_status; //0 = idle, 1 = moveing, 2 = heating
    int m_G_Code_State; //0 = idle , 1= running, 2 = pause


    //values to send
    //settings
    float m_soll_speed;
    float m_soll_filament;
    float m_soll_temperatur;
    float m_soll_bedTemp;

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
    float m_act_bedTemp;

    //status
    float m_endswitch_X;
    float m_endswitch_Y;
    float m_endswitch_Z;

    //calibraten values
    //max size of CNC
    float m_size_X;
    float m_size_Y;
    //correktion angel
    float m_X_angel;
    float m_Y_angel;
    //kalib
    float m_X1,m_Y1,m_Z1,m_Z1_error;
    float m_X2,m_Y2,m_Z2;
    float m_X3,m_Y3,m_Z3;
    float m_X4,m_Y4,m_Z4;

    //TCP hight in home
    float m_Zmax_nozzel;
    float m_X_inHome,m_Y_inHome;

    float m_calibplateX;
    float m_calibplateY;
    float m_calibplateZ;
    bool m_useCalibPlate;

    float m_z_offset;

    //max values
    float m_max_speed; //mm per sec
    float m_max_acc; //max acc

    //PID
    float m_KP,m_KI,m_KD;
    bool m_POn;
    float m_R_vor,m_R_nen, m_bValue;
    int m_plot_size;

    //PID
    float m_KP_Bed,m_KI_Bed,m_KD_Bed;
    bool m_POn_Bed;
    float m_R_vor_Bed,m_R_nen_Bed, m_bValue_Bed;
    int m_plot_size_Bed;


    //repeattest values
    int m_repeat1,m_speed1;
    float m_x11,m_y11,m_z11,m_x12,m_y12,m_z12;

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
    
    void append_recive_command(cnc_command new_command);

    bool m_SerialIsOpen;
    QString m_SerialPortName;
    QList<cnc_command> cnc_send_commands;
    QList<cnc_command> cnc_recive_commands;

private:
    QString m_LogFileName = "CNC_Log.txt";
    QFile *m_LogFile;
    QMutex m_mutex;
    QString m_SerialLogFileName = "CNC_SerialLog.txt";
    QFile *m_SerialLogFile;
    QMutex m_Serialmutex;
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
