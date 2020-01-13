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
}

void cnc_basefunctions::move_wait(float X,float Y,float Z,float W)
{
    send_move(X,Y,Z,W);
    m_loop.exec();
}

void cnc_basefunctions::wait_heat()
{
    if(!m_database->m_is_heated)
    {
        m_loop.exec();
    }
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
    case 'm'://set the actual position
        m_database->set_position(value1,value2,value3,value4);
        if(m_loop.isRunning())
        {
            m_loop.exit();
        }
        m_database->FileLog("INFO recived set position and ready for next move: X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'c'://set the actual position
        m_database->set_position(value1,value2,value3,value4);
        m_database->FileLog("INFO recived set position: X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'q'://set the actual settings
        m_database->set_settings(value1,value2,value3);
        m_database->FileLog("INFO recived current setting: speed:"+QString::number(value1)+" temperatur:"+QString::number(value2)+" filament:"+QString::number(value3)+" PWM:"+QString::number(value4));
        if(5>abs(m_database->m_act_temperatur-m_database->m_soll_temperatur))
        {
            m_database->m_is_heated = true;
            if(m_loop.isRunning())
            {
                m_loop.exit();
            }
        }
        else
        {
            m_database->m_is_heated = false;
        }
        break;
    case 'w':
        //emit Log("recive StepTimes X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'j':
        //emit Log("recive SollPosi X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'k':
        //emit Log("recive SollStep X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'b':
        //emit Log("recive actStep X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'e':
        m_database->set_endswitch(value1,value2,value3);
        m_database->FileLog("INFO recived endswitches: X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        //emit Log("recive endswitch X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    default:
        break;
    }
}
