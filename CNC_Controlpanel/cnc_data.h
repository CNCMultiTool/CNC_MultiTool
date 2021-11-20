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

class cnc_data : public QObject
{
    Q_OBJECT
public:
    cnc_data();

    void FileLog(QString value);
    void SerialLog(QString value);

    int m_HW_status; //0 = idle, 1 = moveing, 2 = heating
    //int m_G_Code_State; //0 = idle , 1= running, 2 = pause


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
    float m_act_E;
    //settings
    float m_act_speed;
    float m_act_filament;
    float m_act_temperatur;
    float m_act_bedTemp;

    //status
    float m_endswitch_X;
    float m_endswitch_Y;
    float m_endswitch_Z;

    bool m_useES;
    bool m_motorUse;
    bool m_waitForHeat;


    //TCP hight in home
    float m_Z_inHome;
    float m_X_inHome,m_Y_inHome;

    float m_calibplateX;
    float m_calibplateY;
    float m_calibplateZ;
    bool m_useCalibPlate;

    float m_z_offset;

    //max values
    float m_max_speed; //mm per sec
    float m_min_speed;
    float m_max_acc; //max acc
    float m_threshAngle;


    //PID
    float m_KP,m_KI,m_KD;
    bool m_POn;
    float m_R_vor,m_R_nen, m_bValue;
    int m_plot_size;

    float m_R_vor_Bed,m_R_nen_Bed, m_bValue_Bed;
    int m_plot_size_Bed;

    void set_position(float X,float Y,float Z,float E);
    void set_settings(float speed,float temperatur,float filament,float soll_temperatur);
    void set_endswitch(int X,int Y,int Z);
    void setUseEs(bool state);

    void saveSettings();
    void loadSettings();

    QString m_SerialPortName;
    bool m_SerialIsOpen;


    bool getUseEs();
    void setMotorUse(bool state);
    bool getMotorUse();
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
    void show_useES(bool state);
    void show_motorUse(bool state);

};

#endif // CNC_DATA_H
