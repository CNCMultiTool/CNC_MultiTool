#include "cnc_basefunctions.h"

cnc_basefunctions::cnc_basefunctions(cnc_data *database)
{
    m_database = database;
    m_database->m_HWisMoving = false;
}

void cnc_basefunctions::test()
{
    emit Log("basefunction is alive");
}

void cnc_basefunctions::send_move(float X,float Y,float Z,float W)
{
    //send_to_cnc('m',X,Y,Z,W,1);
    send_to_cnc('j',X,Y,Z,W,1);
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

void cnc_basefunctions::repeattest_results()
{
    send_to_cnc(' ',0,0,0,0,6);
}

void cnc_basefunctions::z_calib_safePos()
{
    send_to_cnc(' ',0,0,0,0,7);
}

void cnc_basefunctions::z_calib_results()
{
    send_to_cnc(' ',0,0,0,0,8);
}

void cnc_basefunctions::wait_for_heating()
{
    send_to_cnc(' ',0,0,0,0,9);
}

void cnc_basefunctions::cycletimeTest()
{
    send_to_cnc('l',0,0,0,0,2);
}

void cnc_basefunctions::send_settings(float speed,float temperatur,float filament,float acc)
{
    float s = speed;
    float t = temperatur;
    float f = filament;
    float a = acc;
    if(a<0)
        a = m_database->m_soll_accSteps;
    if(s<0)
        s = m_database->m_soll_speed;
    if(t<0)
        t = m_database->m_soll_temperatur;
    if(f<0)
        f = m_database->m_soll_filament;
    if(f<1)
        f = 1;

    m_database->set_soll_settings(s,t,f,a);
    //send_to_cnc('s',s,t,f,0,1);
    send_to_cnc('w',s,t,f,a,1);
}

void cnc_basefunctions::settings_inQ(float speed,float temperatur,float filament,float acc)
{
    float s = speed;
    float t = temperatur;
    float f = filament;
    float a = acc;
    if(a<0)
        a = m_database->m_soll_accSteps;
    if(s<0)
        s = m_database->m_soll_speed;
    if(t<0)
        t = m_database->m_soll_temperatur;
    if(f<0)
        f = m_database->m_soll_filament;
    if(f<1)
        f = 1;

    m_database->set_soll_settings(s,t,f,a);
    send_to_cnc('s',s,t,f,0,2);
}

void cnc_basefunctions::send_init()
{
    send_to_cnc('i',0,0,0,0,1);
}

void cnc_basefunctions::send_stop()
{
    send_to_cnc('b',0,0,0,0,4);
    m_database->m_HWisMoving = false;
}

void cnc_basefunctions::send_getPosition()
{
    send_to_cnc('t',0,0,0,0,1);
}

void cnc_basefunctions::send_setPosition(float X,float Y,float Z,float W)
{
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
    if(action == 1||action == 4)// an den anfang der queue stellen um es sofort auszuführen
    {
        m_database->cnc_send_commands.push_front(new_command);
        trigger_next_command();
        emit show_send_queue();
    }
    else// ans ende der queue stellen um später aus zu führen
    {
        m_database->cnc_send_commands.push_back(new_command);
        emit show_send_queue();
    }
}

void cnc_basefunctions::trigger_next_command()
{
        if(m_database->cnc_send_commands.size()==0)
        {
            //emit Log("trigger_next_command: nothing in queue");
            return;
        }
        int action = m_database->cnc_send_commands[0].action;

        emit Log("trigger_next_command (action: "+
                 QString::number(m_database->cnc_send_commands[0].action)+
                ", command: "+QString(m_database->cnc_send_commands[0].command)+")");

        if(action == 1||action == 2)
        {
            //if(m_database->m_HWisMoving)//did not send next command while HW is working
            //{
            //    emit Log("Hardware is Working");
            //    return;
            //}
            if(m_database->cnc_send_commands[0].command == 'm')
            {
                m_database->cnc_send_commands[0].value3 += m_database->m_z_offset;
            }

            emit trigger_send();
            speedOldPoint.X = m_database->m_act_X;
            speedOldPoint.Y = m_database->m_act_Y;
            speedOldPoint.Z = m_database->m_act_Z;
            speedTimer.restart();
            m_database->m_HWisMoving = true;
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
            m_database->m_HWisMoving = false;
        }
        if(action == 5)//show results of size calib
        {
            m_database->cnc_send_commands.pop_front();
            emit Log("Calib_Size size_x: "+QString::number(m_database->m_size_X)+
                     ", size_y: "+QString::number(m_database->m_size_Y));
            emit Log("Calib_Size error_x: "+QString::number(m_database->m_act_X)+
                     ", error_y: "+QString::number(m_database->m_act_Y));
            trigger_next_command();
        }
        if(action == 6)//repeattest_results
        {
            m_database->cnc_send_commands.pop_front();
            emit Log("result Xerror: "+QString::number(m_database->m_act_X)+"  Yerror: "+QString::number(m_database->m_act_Y)+"  Zerror: "+QString::number(m_database->m_act_Z));
            trigger_next_command();
        }
        if(action == 7)//z_calib_safePos
        {
            m_database->cnc_send_commands.pop_front();
            point temp_point = {m_database->m_act_X,m_database->m_act_Y,m_database->m_act_Z};
            m_pointList.append(temp_point);
            emit Log("calib_Z at X:"+QString::number(m_database->m_act_X)+
                     " Y:"+QString::number(m_database->m_act_Y)+
                     " Z:"+QString::number(m_database->m_act_Z));
            trigger_next_command();
        }
        if(action == 8)//z_calib_results
        {
            m_database->cnc_send_commands.pop_front();
            point temp_point;
            m_database->m_error_X_max_Y_null = m_pointList[1].Z;
            m_database->m_error_X_max_Y_max = m_pointList[2].Z;
            m_database->m_error_X_null_Y_max = m_pointList[3].Z;
            m_database->m_error_X_null_Y_null = m_pointList[4].Z;
            m_database->calc_correctionangel(m_pointList);
            for(int i=0;i<m_pointList.size();i++)
            {
                temp_point = m_pointList[i];
                emit Log("calib_Z_result X:"+QString::number(temp_point.X)+
                         " Y:"+QString::number(temp_point.Y)+
                         " Z:"+QString::number(temp_point.Z));
            }
            trigger_next_command();
        }
        if(action == 9)//wait for heating
        {
            emit Log("wait for heating");
            //m_database->cnc_send_commands.pop_front();
            m_database->m_HWisHeating = true;
        }
        emit show_send_queue();
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
    double tracDist;
    QString LogText;
    switch(command)
    {
    case 'm'://end of movemend
        m_database->set_position(value1,value2,value3,value4);
        m_database->FileLog("INFO recived set position: X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'c'://end of movemend and ready for next comand
        tracDist = sqrt(qPow(speedOldPoint.X-value1,2)+qPow(speedOldPoint.Y-value2,2)+qPow(speedOldPoint.Z-value3,2));
        emit Log("real traveled speed:"+QString::number((tracDist/speedTimer.elapsed())*1000)+" dist:"+QString::number(tracDist)+"mm time:"+QString::number(speedTimer.elapsed())+"ms soll_speed:"+QString::number(m_database->m_act_speed));

        m_database->set_position(value1,value2,value3,value4);
        m_database->set_HWisMoving(false);
        m_database->FileLog("INFO recived set position and ready for next command: X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        m_database->m_HWisMoving = false;
        trigger_next_command();
        break;
    case 's'://set the actual settings
        m_database->set_settings(value1,value2,value3,value4);
        m_database->FileLog("INFO recived current setting: speed:"+QString::number(value1)+" temperatur:"+QString::number(value2)+" filament:"+QString::number(value3)+" PWM:"+QString::number(value4));
        m_database->m_HWisMoving = false;
        trigger_next_command();
        break;
    case 'j'://set the actual settings
        m_database->set_settings(value1,value2,value3,value4);
        m_database->FileLog("INFO recived current setting: speed:"+QString::number(value1)+" temperatur:"+QString::number(value2)+" filament:"+QString::number(value3)+" PWM:"+QString::number(value4));
        if(m_database->m_HWisHeating == true)
        {
            float temp_dif = abs(m_database->m_act_temperatur - m_database->m_soll_temperatur);
            emit Log("temperature soll:"+QString::number(m_database->m_soll_temperatur)
                     +", ist:"+QString::number(m_database->m_act_temperatur)
                     +", dif:"+QString::number(temp_dif));
            if(3 > temp_dif)
            {
                emit Log("end of the heating");
                m_database->cnc_send_commands.pop_front();
                trigger_next_command();
                m_database->m_HWisHeating = false;
            }
        }
        break;
    case 'e':
        m_database->set_endswitch(value1,value2,value3);
        m_database->FileLog("INFO recived endswitches: X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        //emit Log("recive endswitch X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'l':
        LogText = "timecheck ";
        LogText += " value 1 :"+QString::number(value1);
        LogText += " value 2 :"+QString::number(value2);
        LogText += " value 3 :"+QString::number(value3);
        LogText += " value 4 :"+QString::number(value4);
        m_database->FileLog(LogText);
        emit Log(LogText);
        trigger_next_command();
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
        LogText = "unknown kommand from Arduino";
        LogText += " command :"+QString(command);
        LogText += " value 1 :"+QString::number(value1);
        LogText += " value 2 :"+QString::number(value2);
        LogText += " value 3 :"+QString::number(value3);
        LogText += " value 4 :"+QString::number(value4);
        m_database->FileLog(LogText);
        emit Log(LogText);
        break;
    }
}

