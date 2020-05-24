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

void cnc_basefunctions::send_PID_Bed(float P,float I,float D,float PO)
{
    send_to_cnc('k',P,I,D,PO,1);
}

void cnc_basefunctions::send_Temp_Setting_Bed(float R_vor,float B_Value,float R_nen)
{
    send_to_cnc('h',R_vor,B_Value,R_nen,0,1);
}

void cnc_basefunctions::send_PID(float P,float I,float D,float PO)
{
    send_to_cnc('o',P,I,D,PO,1);
}

void cnc_basefunctions::send_Temp_Setting(float R_vor,float B_Value,float R_nen)
{
    send_to_cnc('z',R_vor,B_Value,R_nen,0,1);
}

void cnc_basefunctions::send_move(float X,float Y,float Z,float W)
{
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

void cnc_basefunctions::send_settings(float speed,float temperatur,float filament,float bed_temp)
{
    send_to_cnc('w',speed,temperatur,filament,bed_temp,1);
}

void cnc_basefunctions::settings_inQ(float speed,float temperatur,float filament,float bed_temp)
{
    send_to_cnc('s',speed,temperatur,filament,bed_temp,2);
}

void cnc_basefunctions::send_init()
{
    send_to_cnc('i',0,0,0,0,1);
}

void cnc_basefunctions::send_stop()
{
    send_to_cnc('b',0,0,0,0,4);
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

        if(action == 1||action == 2)
        {
            char n_com = m_database->cnc_send_commands[0].command;
            if(n_com == 'm' || n_com == 'j' ||n_com == 'p')
            {
                m_database->cnc_send_commands[0].value3 += m_database->m_z_offset;
                //m_database->m_HWisMoving = true;
                if(n_com != 'p')
                    m_database->m_HW_status = 1;
                emit show_status();
            }
            if(n_com == 'w' || n_com == 's')
            {
                float s = m_database->cnc_send_commands[0].value1;
                float t = m_database->cnc_send_commands[0].value2;
                float f = m_database->cnc_send_commands[0].value3;
                float b = m_database->cnc_send_commands[0].value4;
                if(b<0)
                    b = m_database->m_soll_bedTemp;
                if(s<0)
                    s = m_database->m_soll_speed;
                if(s > m_database->m_max_speed)
                    s = m_database->m_max_speed;
                if(t<0)
                    t = m_database->m_soll_temperatur;
                if(f<0)
                    f = m_database->m_soll_filament;
                if(f<1)
                    f = 1;

                m_database->cnc_send_commands[0].value1 = s;
                m_database->cnc_send_commands[0].value2 = t;
                m_database->cnc_send_commands[0].value3 = f;
                m_database->cnc_send_commands[0].value4 = b;

                m_database->set_soll_settings(m_database->cnc_send_commands[0].value1,
                        m_database->cnc_send_commands[0].value2,
                        m_database->cnc_send_commands[0].value3,
                        m_database->cnc_send_commands[0].value4);
            }

            emit trigger_send();
            speedOldPoint.X = m_database->m_act_X;
            speedOldPoint.Y = m_database->m_act_Y;
            speedOldPoint.Z = m_database->m_act_Z;
            speedTimer.restart();
            //m_database->m_HWisMoving = true;
            //m_database->m_HW_status = 1;
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
            //m_database->m_HWisMoving = false;
            m_database->m_HW_status = 0;
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
            m_database->calc_correctionangel(m_pointList);
            for(int i=0;i<m_pointList.size();i++)
            {
                temp_point = m_pointList[i];
                emit Log("calib_Z_result X:"+QString::number(temp_point.X)+
                         " Y:"+QString::number(temp_point.Y)+
                         " Z:"+QString::number(temp_point.Z));
            }
            m_database->m_Z1 = m_pointList[0].Z;
            m_database->m_Z2 = m_pointList[1].Z;
            m_database->m_Z3 = m_pointList[2].Z;
            m_database->m_Z4 = m_pointList[3].Z;
            m_database->m_Z1_error = m_pointList[4].Z;
            emit z_calib_resullt_finish();

            trigger_next_command();
        }
        if(action == 9)//wait for heating
        {
            emit Log("wait for heating");
            //m_database->cnc_send_commands.pop_front();
            //m_database->m_HWisHeating = true;
            m_database->m_HW_status = 2;
            emit show_status();
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
    //double tracDist;
    QString LogText;
    switch(command)
    {
    case 'm'://end of movemend
        m_database->set_position(value1,value2,value3,value4);
        //m_database->FileLog("INFO recived set position: X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'c'://end of movemend and ready for next comand
        //tracDist = sqrt(qPow(speedOldPoint.X-value1,2)+qPow(speedOldPoint.Y-value2,2)+qPow(speedOldPoint.Z-value3,2));
        //emit Log("real traveled speed:"+QString::number((tracDist/speedTimer.elapsed())*1000)+" dist:"+QString::number(tracDist)+"mm time:"+QString::number(speedTimer.elapsed())+"ms soll_speed:"+QString::number(m_database->m_act_speed));

        m_database->set_position(value1,value2,value3,value4);
        m_database->m_HW_status = 0;
        emit show_status();
        trigger_next_command();
        break;
    case 's'://set the actual settings
        m_database->set_settings(value1,value2,value3,value4);
        //m_database->FileLog("INFO recived current setting: speed:"+QString::number(value1)+" temperatur:"+QString::number(value2)+" filament:"+QString::number(value3)+" ACCStep:"+QString::number(value4));
        trigger_next_command();
        break;
    case 'j'://set the actual settings
        m_database->set_settings(value1,value2,value3,value4);
        emit DataToGraph(m_database->m_act_bedTemp,m_database->m_act_temperatur,m_database->m_soll_bedTemp,m_database->m_soll_temperatur);
        emit show_alive();
        //m_database->FileLog("INFO recived current setting: speed:"+QString::number(value1)+" temperatur:"+QString::number(value2)+" filament:"+QString::number(value3)+" ACCStep:"+QString::number(value4));
        if(m_database->m_HW_status == 2)
        {
            float temp_dif = abs(m_database->m_act_temperatur - m_database->m_soll_temperatur);
            emit Log("temperature soll:"+QString::number(m_database->m_soll_temperatur)
                     +", ist:"+QString::number(m_database->m_act_temperatur)
                     +", dif:"+QString::number(temp_dif));
            if(m_database->m_act_temperatur > m_database->m_soll_temperatur-3)
            {
                emit Log("end of the heating");
                m_database->cnc_send_commands.pop_front();
                trigger_next_command();
                m_database->m_HW_status = 0;
                emit show_status();
            }
        }
        break;
    case 'e':
        m_database->set_endswitch(value1,value2,value3);
        //m_database->FileLog("INFO recived endswitches: X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        //emit Log("recive endswitch X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'l':
        LogText = "timecheck ";
        LogText += " value 1 :"+QString::number(value1);
        LogText += " value 2 :"+QString::number(value2);
        LogText += " value 3 :"+QString::number(value3);
        LogText += " value 4 :"+QString::number(value4);
        emit Log(LogText);
        m_database->FileLog(LogText);
        emit Log(LogText);
        break;
    case 'o'://set PID values response
        LogText = "new PID ";
        LogText += " P :"+QString::number(value1);
        LogText += " I :"+QString::number(value2);
        LogText += " D :"+QString::number(value3);
        LogText += " use POn :"+QString::number(value4);
        emit Log(LogText);
        m_database->FileLog(LogText);
        emit Log(LogText);
        break;
    case 'u'://PID debug infos
        LogText = "new PID ";
        LogText += " soll_T :"+QString::number(value1);
        LogText += " T_ntc :"+QString::number(value2);
        LogText += " PWM :"+QString::number(value3);
        LogText += " T_100 :"+QString::number(value4);
        //emit Log(LogText);
        //m_database->FileLog(LogText);
        emit DataToGraph(value4,value2,value3,value1);
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

