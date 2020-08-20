#include "cnc_basefunctions.h"

cnc_basefunctions::cnc_basefunctions(cnc_data *database)
{
    m_database = database;
    m_database->m_HW_status = 0;
}

void cnc_basefunctions::test()
{
    emit Log("basefunction is alive");
}

void cnc_basefunctions::send_acc(float acc,float speed_min)
{
    emit show_send_mes("G1 X1 Y1 Z1 E1");
}

void cnc_basefunctions::send_PID_Bed(float P,float I,float D,float PO)
{
    emit show_send_mes("G1 X1 Y1 Z1 E1");
}

void cnc_basefunctions::send_Temp_Setting_Bed(float R_vor,float B_Value,float R_nen)
{
    emit show_send_mes("G1 X1 Y1 Z1 E1");
}

void cnc_basefunctions::send_PID(float P,float I,float D,float PO)
{
    emit show_send_mes("G1 X1 Y1 Z1 E1");
}

void cnc_basefunctions::send_Temp_Setting(float R_vor,float B_Value,float R_nen)
{
    emit show_send_mes("G1 X1 Y1 Z1 E1");
}

void cnc_basefunctions::send_move(float X,float Y,float Z,float E)
{
    emit show_send_mes("G1 X"+QString::number(X)+" Y"+QString::number(Y)+
                       " Z"+QString::number(Z)+" E"+QString::number(E));
}

void cnc_basefunctions::send_settings(float speed,float temperatur,float filament,float bed_temp)
{
    if(speed > m_database->m_max_speed)
        speed = m_database->m_max_speed;
    //send_to_cnc('w',speed,temperatur,filament,bed_temp,1);
}

void cnc_basefunctions::send_init()
{
    //send_to_cnc('i',0,0,0,0,1);
}

void cnc_basefunctions::send_stop()
{

    emit show_send_mes("G9");
}

void cnc_basefunctions::send_getPosition()
{
    //send_to_cnc('t',0,0,0,0,1);
}

void cnc_basefunctions::send_setPosition(float X,float Y,float Z,float W)
{
    //send_to_cnc('p',X,Y,Z,W,1);
}

