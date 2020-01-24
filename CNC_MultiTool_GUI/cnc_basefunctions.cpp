#include "cnc_basefunctions.h"

cnc_basefunctions::cnc_basefunctions(cnc_data *database)
{
    m_database = database;
}

void cnc_basefunctions::test()
{
    emit Log("basefunction is alive");
}

void cnc_basefunctions::send_move(float X,float Y,float Z,float W)
{
    emit send('m',X,Y,Z,W);
    m_database->set_HWisMoving(true);
}

void cnc_basefunctions::move_wait(float X,float Y,float Z,float W)
{
    send_move(X,Y,Z,W);
    m_loop.exec();
}

void cnc_basefunctions::send_settings(float speed,float temperatur,float filament)
{
    float s = speed;
    float t = temperatur;
    float f = filament;
    if(s<0)
        s = m_database->m_act_speed;
    if(t<0)
        t = m_database->m_act_temperatur;
    if(f<0)
        f = m_database->m_act_filament;
    if(f<1)
        f = 1;

    m_database->set_soll_settings(s,t,f);
    emit send('s',s,t,f,0);
}

void cnc_basefunctions::send_stop()
{
    emit send('b',0,0,0,0);
}

void cnc_basefunctions::send_getPosition()
{
    emit send('p',0,0,0,0);
}

void cnc_basefunctions::send_setPosition(float X,float Y,float Z,float W)
{
    emit send('n',X,Y,Z,W);
}

void cnc_basefunctions::recived(char command,float value1,float value2,float value3,float value4)
{
    switch(command)
    {
    case 'm'://end of movemend
        m_database->set_position(value1,value2,value3,value4);
        m_database->FileLog("INFO recived set position: X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'c'://end of movemend and ready for next comand
        m_database->set_position(value1,value2,value3,value4);
        m_database->set_HWisMoving(false);
        m_loop.exit();
        m_database->FileLog("INFO recived set position and ready for next command: X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'q'://set the actual settings
        m_database->FileLog("INFO recived current setting: speed:"+QString::number(value1)+" temperatur:"+QString::number(value2)+" filament:"+QString::number(value3)+" PWM:"+QString::number(value4));
        if(5>abs(m_database->m_act_temperatur-m_database->m_soll_temperatur))
        {
            m_database->m_HWisAtHeat = true;
            m_database->m_HWisHeating = false;
        }
        else
        {
            m_database->m_HWisAtHeat = false;
        }
        m_database->set_settings(value1,value2,value3,value4);
        //emit send('j',0,0,0,0);
        break;
    case 'a':
        m_database->set_settings(value1,value2,value3,value4);
        m_database->FileLog("INFO recived current setting and ready for next command: speed:"+QString::number(value1)+" temperatur:"+QString::number(value2)+" filament:"+QString::number(value3)+" PWM:"+QString::number(value4));
        break;
    case 'e':
        m_database->set_endswitch(value1,value2,value3);
        m_database->FileLog("INFO recived endswitches: X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        //emit Log("recive endswitch X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'I':
        m_database->FileLog("INFO recived Answer");
        //emit Log("INFO recived Answer");
        break;
    default:
        break;
    }
}

