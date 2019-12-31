#include "cnc_automation.h"

CNC_automation::CNC_automation()
{
    m_X = 0;
    m_Y = 0;
    m_Z = 0;
    m_W = 0;

    m_MainWindow.setWindowTitle("Hildegart QT");
    m_MainWindow.show();

    connect(&m_MainWindow,SIGNAL(send_move(float,float,float,float))        ,this,SLOT(send_move(float,float,float,float)));
    connect(&m_MainWindow,SIGNAL(send_settings(float ,float ,float))        ,this,SLOT(send_settings(float ,float ,float)));
    connect(&m_MainWindow,SIGNAL(send_stop())                               ,this,SLOT(send_stop()));
    connect(&m_MainWindow,SIGNAL(send_getPosition())                        ,this,SLOT(send_getPosition()));
    connect(&m_MainWindow,SIGNAL(send_setPosition(float,float,float,float)) ,this,SLOT(send_setPosition(float,float,float,float)));
    connect(&m_MainWindow,SIGNAL(serial_open_close(QString))                ,this,SLOT(serial_open_close(QString)));
    connect(&m_MainWindow,SIGNAL(G_Code_Start(QString))                     ,this,SLOT(G_Code_Start(QString)));
    connect(&m_MainWindow,SIGNAL(G_Code_Pause())                            ,this,SLOT(G_Code_Pause()));
    connect(&m_MainWindow,SIGNAL(G_Code_Stop())                             ,this,SLOT(G_Code_Stop()));
    connect(&m_MainWindow,SIGNAL(move_home())                               ,this,SLOT(move_home()));
    connect(&m_MainWindow,SIGNAL(calib_size())                              ,this,SLOT(calib_size()));
    connect(&m_MainWindow,SIGNAL(repeat_test())                             ,this,SLOT(repeat_test()));
    connect(&m_MainWindow,SIGNAL(Z_calib())                                 ,this,SLOT(Z_calib()));

    connect(&m_Serial,SIGNAL(serial_show(bool))                             ,this,SLOT(serial_show(bool)));
    connect(&m_Serial,SIGNAL(process_recived(char,float,float,float,float)) ,this,SLOT(process_recived(char,float,float,float,float)));
    connect(&m_Serial,SIGNAL(Log(QString))                                  ,this,SLOT(Log(QString)));
    connect(&m_Serial,SIGNAL(errorLog(QString))                             ,this,SLOT(errorLog(QString)));

    connect(&m_AutoFunctions,SIGNAL(Log(QString))                           ,this,SLOT(Log(QString)));
    connect(&m_AutoFunctions,SIGNAL(errorLog(QString))                      ,this,SLOT(errorLog(QString)));
    connect(&m_AutoFunctions,SIGNAL(send_move(float,float,float,float))        ,this,SLOT(send_move(float,float,float,float)));
    connect(&m_AutoFunctions,SIGNAL(send_settings(float ,float ,float))        ,this,SLOT(send_settings(float ,float ,float)));
    connect(&m_AutoFunctions,SIGNAL(send_stop())                               ,this,SLOT(send_stop()));
    connect(&m_AutoFunctions,SIGNAL(send_getPosition())                        ,this,SLOT(send_getPosition()));
    connect(&m_AutoFunctions,SIGNAL(send_setPosition(float,float,float,float)) ,this,SLOT(send_setPosition(float,float,float,float)));

    connect(this,SIGNAL(reached_position()), &m_AutoFunctions.m_loop, SLOT(quit()));
}

CNC_automation::~CNC_automation()
{

}

void CNC_automation::Log(const QString &s)
{
    m_MainWindow.Log(s);
}

void CNC_automation::errorLog(const QString &s)
{
    m_MainWindow.errorLog(s);
}

void CNC_automation::G_Code_Start(QString fileName)
{
    Log("start g-code: "+fileName);
    m_AutoFunctions.G_Code_Start(fileName);
}

void CNC_automation::G_Code_Pause()
{
    m_AutoFunctions.G_Code_Pause();
}

void CNC_automation::G_Code_Stop()
{
    m_AutoFunctions.G_Code_Stop();
}

void CNC_automation::serial_show(bool isOpen)
{
    m_MainWindow.show_serial(isOpen);
}

void CNC_automation::send(char command,float value1,float value2,float value3,float value4)
{
    m_Serial.send(command,value1,value2,value3,value4);
}

void CNC_automation::send_move(float X,float Y,float Z,float W)
{
    m_Serial.send('m',X,Y,Z,W);
}

void CNC_automation::send_settings(float speed,float temperatur,float filament)
{
    float s = speed;
    float t = temperatur;
    float f = filament;
    if(s<0)
        s = m_speed;
    if(t<0)
        t = m_temperatur;
    if(f<0)
        f = m_filament;

    m_Serial.send('s',s,t,f,0);
}
void CNC_automation::send_stop()
{
    m_Serial.send('b',0,0,0,0);
}

void CNC_automation::send_getPosition()
{
    m_Serial.send('p',0,0,0,0);
}

void CNC_automation::send_setPosition(float X,float Y,float Z,float W)
{
    m_Serial.send('n',X,Y,Z,W);
}

void CNC_automation::serial_open_close(QString portName)
{
    m_Serial.open_close(portName);
}

void CNC_automation::process_recived(char command,float value1,float value2,float value3,float value4)
{
    switch(command)
    {
    case 'm'://set the actual position
        m_MainWindow.show_position(value1,value2,value3,value4);
        emit reached_position();
        m_X = value1;
        m_Y = value2;
        m_Z = value3;
        m_W = value4;
        m_AutoFunctions.m_act_X = value1;
        m_AutoFunctions.m_act_Y = value2;
        m_AutoFunctions.m_act_Z = value3;
        m_AutoFunctions.m_act_W = value4;
        //Log("recive act Pos X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'c'://set the actual position
        m_MainWindow.show_position(value1,value2,value3,value4);
        m_X = value1;
        m_Y = value2;
        m_Z = value3;
        m_W = value4;
        m_AutoFunctions.m_act_X = value1;
        m_AutoFunctions.m_act_Y = value2;
        m_AutoFunctions.m_act_Z = value3;
        m_AutoFunctions.m_act_W = value4;
        //Log("recive act Pos X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'q'://set the actual position
        m_MainWindow.show_settings(value1,value2,value3);
        m_speed = value1;
        m_filament = value2;
        m_temperatur = value3;
        //Log("recive Settings Speed:"+QString::number(value1)+" Temperatur:"+QString::number(value2)+" Filament:"+QString::number(value3));
        break;
    case 'w':
        Log("recive StepTimes X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'j':
        Log("recive SollPosi X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'k':
        Log("recive SollStep X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'b':
        Log("recive actStep X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'e':
        m_MainWindow.show_endswitch(value1,value2,value3);
        m_AutoFunctions.m_act_X_end = value1;
        m_AutoFunctions.m_act_Y_end = value2;
        m_AutoFunctions.m_act_Z_end = value3;
        //Log("recive endswitch X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    default:
        break;
    }
}

void CNC_automation::move_home()
{
    m_AutoFunctions.move_home();
}

void CNC_automation::calib_size()
{
    m_AutoFunctions.calib_size();
}

void CNC_automation::repeat_test()
{
    m_AutoFunctions.repeat_test();
}

void CNC_automation::Z_calib()
{
    m_AutoFunctions.Z_calib();
}
