#include "cnc_basefunctions.h"

cnc_basefunctions::cnc_basefunctions(cnc_data *database)
{
    m_database = database;
    HW_is_working = false;
}

void cnc_basefunctions::test()
{
    emit Log("basefunction is alive");
}

void cnc_basefunctions::send_move(float X,float Y,float Z,float W)
{
    send_to_cnc('m',X,Y,Z,W,1);
}

void cnc_basefunctions::move_inQ(float X,float Y,float Z,float W)
{
    send_to_cnc('m',X,Y,Z,W,2);
}

void cnc_basefunctions::calib_size_safe()
{
    send_to_cnc(' ',0,0,0,0,3);
}

void cnc_basefunctions::calib_size_results()
{
    send_to_cnc(' ',0,0,0,0,5);
}

void cnc_basefunctions::send_settings(float speed,float temperatur,float filament)
{
    float s = speed;
    float t = temperatur;
    float f = filament;
    if(s<0)
        s = m_database->m_soll_speed;
    if(t<0)
        t = m_database->m_soll_temperatur;
    if(f<1)
        f = 1;
    if(f<0)
        f = m_database->m_soll_filament;


    m_database->set_soll_settings(s,t,f);
    //emit send('s',s,t,f,0);
    send_to_cnc('s',s,t,f,0,1);
}

void cnc_basefunctions::settings_inQ(float speed,float temperatur,float filament)
{
    float s = speed;
    float t = temperatur;
    float f = filament;
    if(s<0)
        s = m_database->m_soll_speed;
    if(t<0)
        t = m_database->m_soll_temperatur;
    if(f<1)
        f = 1;
    if(f<0)
        f = m_database->m_soll_filament;

    m_database->set_soll_settings(s,t,f);

    send_to_cnc('s',s,t,f,0,2);
}

void cnc_basefunctions::send_init()
{
    send_to_cnc('i',0,0,0,0,1);
}

void cnc_basefunctions::send_stop()
{
    send_to_cnc('b',0,0,0,0,4);
    HW_is_working = false;
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

void cnc_basefunctions::setPosition_inQ(float X,float Y,float Z,float W)
{
    send_to_cnc('p',X,Y,Z,W,2);
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
    new_command.action = action;
    //send sofot
    if(action == 1||action == 4)// sofort ausführen
    {
        m_database->cnc_send_commands.push_front(new_command);
        emit show_send_queue();
        trigger_next_command();
    }
    else// if(action == 2||action == 3)// zur queue hinzufügen um es später aus zu führen
    {
        m_database->cnc_send_commands.push_back(new_command);
        emit show_send_queue();
    }
}

void cnc_basefunctions::trigger_next_command()
{

    if(m_database->cnc_send_commands.size()<1)
    {
        emit Log("trigger_next_command: nothing in queue");
        return;
    }
    int action = m_database->cnc_send_commands[0].action;
    emit Log("trigger_next_command (action: "+QString::number(m_database->cnc_send_commands[0].action)+")");
    if(action == 1||action == 2)
    {
        if(HW_is_working)
            return;
        emit trigger_send();
        HW_is_working = true;
    }
    if(action == 3)//calib size safe max posi
    {
        m_database->cnc_send_commands.pop_front();
        m_database->m_size_X = m_database->m_act_X;
        m_database->m_size_Y = m_database->m_act_Y;
        trigger_next_command();
    }
    if(action == 4)//ignore hw is working send sofort
    {
        emit trigger_send();
    }
    if(action == 5)//show results of size calib
    {
        emit Log("Calib_Size size_x: "+QString::number(m_database->m_size_X)+", size_y: "+QString::number(m_database->m_size_Y));
        emit Log("Calib_Size error_x: "+QString::number(m_database->m_size_X)+", error_y: "+QString::number(m_database->m_size_Y));
        trigger_next_command();
    }
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
        HW_is_working = false;
        trigger_next_command();
        break;
    case 's'://set the actual settings
        m_database->set_settings(value1,value2,value3);
        m_database->FileLog("INFO recived current setting: speed:"+QString::number(value1)+" temperatur:"+QString::number(value2)+" filament:"+QString::number(value3)+" PWM:"+QString::number(value4));
        HW_is_working = false;
        trigger_next_command();
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

