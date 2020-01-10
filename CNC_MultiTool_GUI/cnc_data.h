#ifndef CNC_DATA_H
#define CNC_DATA_H


#include <QObject>
class cnc_data : public QObject
{
    Q_OBJECT
public:
    cnc_data();

    //values to send
    //position
    float m_soll_X;
    float m_soll_Y;
    float m_soll_Z;
    float m_soll_W;
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

    void set_position(float X,float Y,float Z,float W);
    void set_settings(float speed,float temperatur,float filament);
    void set_endswitch(int X,int Y,int Z);
    void set_serial(bool isOpen);

    void test();

private:
    //status
    bool m_SerialIsOpen;


signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void show_position(float X,float Y,float Z,float W);
    void show_settings(float speed,float temperatur,float filament);
    void show_endswitch(float X,float Y,float Z);
    void show_serial(bool isOpen);

//private slots:

};

#endif // CNC_DATA_H
