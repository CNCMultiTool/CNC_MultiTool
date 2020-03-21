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
    send_to_cnc('m',X,Y,Z,W,1);
}

void cnc_basefunctions::move_wait(float X,float Y,float Z,float W)
{
    send_move(X,Y,Z,W);
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
    if(f<1)
        f = 1;
    if(f<0)
        f = m_database->m_act_filament;


    m_database->set_soll_settings(s,t,f);
    //emit send('s',s,t,f,0);
    send_to_cnc('s',s,t,f,0,1);
}

void cnc_basefunctions::send_stop()
{
    send_to_cnc('b',0,0,0,0,1);
}

void cnc_basefunctions::send_getPosition()
{
    //emit send('t',0,0,0,0);
    send_to_cnc('t',0,0,0,0,1);
}

void cnc_basefunctions::send_setPosition(float X,float Y,float Z,float W)
{
    //emit send('p',X,Y,Z,W);
    send_to_cnc('p',X,Y,Z,W,1);
}

void cnc_basefunctions::send_to_cnc(char command,float v1,float v2,float v3,float v4,int action)
{
    //verpacke daten in comando structur
    cnc_command new_command;
    new_command.command = command;
    new_command.value1 = v1;
    new_command.value2 = v2;
    new_command.value3 = v3;
    new_command.value4 = v4;
    //send sofot
    if(action == 1)
        m_database->cnc_send_commands.push_front(new_command);
        emit trigger_send();
}

void cnc_basefunctions::process_command()
{
    cnc_command new_command = m_database->cnc_recive_commands[0];
    m_database->cnc_recive_commands.pop_front();
    execute_command(new_command.command,
                    new_command.value1,
                    new_command.value2,
                    new_command.value3,
                    new_command.value4);
}

void cnc_basefunctions::execute_command(char command,float value1,float value2,float value3,float value4)
{
    QString LogText;
    switch(command)
    {
    case 'm'://end of movemend
        m_database->set_position(value1,value2,value3,value4);
        m_database->FileLog("INFO recived set position: X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'c'://end of movemend and ready for next comand
        m_database->set_position(value1,value2,value3,value4);
        m_database->set_HWisMoving(false);
        m_database->FileLog("INFO recived set position and ready for next command: X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 's'://set the actual settings
        m_database->set_settings(value1,value2,value3);
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
        break;
    case 'a':
        m_database->set_settings(value1,value2,value3);
        m_database->FileLog("INFO recived current setting and ready for next command: speed:"+QString::number(value1)+" temperatur:"+QString::number(value2)+" filament:"+QString::number(value3)+" PWM:"+QString::number(value4));
        break;
    case 'w':
        //emit Log("recive StepTimes X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'j':
        //emit Log("recive SollPosi X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        m_database->FileLog("INFO recived Answer");
        //emit Log("INFO recived Answer");
        break;
    case 'N':
        m_database->FileLog("INFO recived Repetrequest");
        emit Log("INFO recived Repetrequest");
        break;
    case 'e':
        m_database->set_endswitch(value1,value2,value3);
        m_database->FileLog("INFO recived endswitches: X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        //emit Log("recive endswitch X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'd':
        LogText = "DEBUG recived Debug";
        LogText += " value 1 :"+QString::number(value1);
        LogText += " value 2 :"+QString::number(value2);
        LogText += " value 3 :"+QString::number(value3);
        LogText += " value 4 :"+QString::number(value4);
        m_database->FileLog(LogText);
        emit Log(LogText);
        break;
    default:
        m_database->FileLog("unknown kommand from Arduino");
        emit Log("unknown kommand from Arduino");
        break;
    }
}

