#include "cnc_data.h"

cnc_data::cnc_data()
{
    m_size_X = 269.89;
    m_size_Y = 231.68;

    m_X_angel = -0.00155619;
    m_Y_angel = 0.0023308;
}

void cnc_data::test()
{
    emit Log("database is alive");
}

void cnc_data::set_position(float X,float Y,float Z,float W)
{
    m_act_X = X;
    m_act_Y = Y;
    m_act_Z = Z;
    m_act_W = W;
    emit show_position(X,Y,Z,W);
}

void cnc_data::set_settings(float speed,float temperatur,float filament)
{
    m_act_speed = speed;
    m_act_filament = temperatur;
    m_act_temperatur = filament;
    emit show_settings(speed,temperatur,filament);
}

void cnc_data::set_endswitch(int X,int Y,int Z)
{
    m_endswitch_X = X;
    m_endswitch_Y = Y;
    m_endswitch_Z = Z;
    emit show_endswitch(X,Y,Z);
}

void cnc_data::set_serial(bool isOpen)
{
    m_SerialIsOpen = isOpen;
    emit show_serial(isOpen);
}
