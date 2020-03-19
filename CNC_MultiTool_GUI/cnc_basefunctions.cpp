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
    addComandToQueue('m',X,Y,Z,W,false);
    //emit send('m',X,Y,Z,W);
    m_database->set_HWisMoving(true);
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
        f = m_database->m_soll_filament;


    m_database->set_soll_settings(s,t,f);
    addComandToQueue('s',s,t,f,0,false);
    //emit send('s',s,t,f,0);
}

void cnc_basefunctions::send_stop()
{
    addComandToQueue('b',0,0,0,0,true);
    //emit send('b',0,0,0,0);
}

void cnc_basefunctions::send_getPosition()
{
    addComandToQueue('a',0,0,0,0,false);
    //emit send('t',0,0,0,0);
}

void cnc_basefunctions::send_setPosition(float X,float Y,float Z,float W)
{
    addComandToQueue('a',0,0,0,0,false);
    //emit send('p',X,Y,Z,W);
}

void cnc_basefunctions::send_ConfirmAnswer()
{
    addComandToQueue('a',0,0,0,0,false);
    //emit send('a',0,0,0,0);
}

void cnc_basefunctions::recived(char command,float value1,float value2,float value3,float value4)
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
        //m_loop.quit();
        m_database->FileLog("INFO recived set position and ready for next command: X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        send_ConfirmAnswer();
        sendNextComandFromQueue();
        break;
    case 's'://set the actual settings
        m_database->FileLog("INFO recived current setting: speed:"+QString::number(value1)+" temperatur:"+QString::number(value2)+" filament:"+QString::number(value3)+" PWM:"+QString::number(value4));
        if(5>abs(m_database->m_act_temperatur-m_database->m_soll_temperatur))
        {
            m_database->m_HWisAtHeat = true;
            m_database->m_HWisHeating = false;
            //_heating_loop.exit();
        }
        else
        {
            m_database->m_HWisAtHeat = false;
        }
        m_database->set_settings(value1,value2,value3,value4);
        sendNextComandFromQueue();
        break;
    case 'a':
        m_database->FileLog("INFO recived Answer");
        //emit Log("INFO recived Answer");
        break;
    case 'N':
        emit answer_repeatrequest();
        m_database->FileLog("INFO recived Repetrequest");
        emit Log("INFO recived Repetrequest");
        emit errorLog("INFO recived Repetrequest");
        break;
    case 'e':
        m_database->set_endswitch(value1,value2,value3);
        m_database->FileLog("INFO recived endswitches: X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        //emit Log("recive endswitch X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        send_ConfirmAnswer();
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
        emit errorLog("unknown kommand from Arduino");
        break;
    }
}


void cnc_basefunctions::addComandToQueue(char c,float v1,float v2,float v3,float v4,bool prio)
{
    cnc_command new_command;
    new_command.c = c;
    new_command.v1 = v1;
    new_command.v2 = v2;
    new_command.v3 = v3;
    new_command.v4 = v4;
    if(prio||commandQueue.size()==0)
    {
        emit send(c,v1,v2,v3,v4);
    }
    else
    {
        commandQueue.push_back(new_command);
    }
}
void cnc_basefunctions::sendNextComandFromQueue()
{
    if(commandQueue.size()!=0)
    {
        cnc_command next_command = commandQueue[0];
        emit send(next_command.c,next_command.v1,next_command.v2,next_command.v3,next_command.v4);
        commandQueue.pop_front();
    }
}


