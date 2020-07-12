#include "cnc_automation.h"

CNC_automation::CNC_automation(cnc_data *database,cnc_basefunctions *basefunctions)
{
    m_pause = false;
    m_database = database;
    m_basefunctions = basefunctions;
    m_F_max = m_database->m_max_speed;
    m_F_old = 0;
    m_X = 0;
    m_Y = 0;
    m_Z = 0;
    m_W = 0;
    m_S = 220;
    m_F = m_F_max*60;
}

CNC_automation::~CNC_automation()
{
    m_aboard = true;
}

void CNC_automation::nozzel_calib()
{
    m_basefunctions->settings_inQ(m_F_max,-1,-1,-1);
    m_basefunctions->move_inQ(m_database->m_act_X,m_database->m_act_Y,9999,m_database->m_act_W);
    m_basefunctions->move_inQ(-9999,-9999,9999,m_database->m_act_W);
    m_basefunctions->setPosition_inQ(0,0,m_database->m_Zmax_nozzel,0);
    m_basefunctions->move_inQ(m_database->m_calibplateX,m_database->m_calibplateY,20,m_database->m_act_W);
    m_basefunctions->move_inQ(m_database->m_calibplateX,m_database->m_calibplateY,0,m_database->m_act_W);
    m_basefunctions->setPosition_inQ(m_database->m_calibplateX,m_database->m_calibplateY,m_database->m_calibplateZ,0);
    m_basefunctions->move_inQ(m_database->m_calibplateX,m_database->m_calibplateY,m_database->m_calibplateZ+20,m_database->m_act_W);
    m_basefunctions->move_inQ(m_database->m_calibplateX,m_database->m_calibplateY-50,m_database->m_calibplateZ+20,m_database->m_act_W);
    //m_basefunctions->trigger_next_command();
    m_database->m_G_Code_State = 1;
}

void CNC_automation::move_home()
{
    m_basefunctions->settings_inQ(m_F_max,-1,-1,-1);
    m_basefunctions->move_inQ(m_database->m_act_X,m_database->m_act_Y,9999,m_database->m_act_W);
    m_basefunctions->move_inQ(-9999,-9999,9999,m_database->m_act_W);
    //m_basefunctions->setPosition_inQ(0,0,m_database->m_Zmax_nozzel,0);
    m_basefunctions->setPosition_inQ(m_database->m_X_inHome,m_database->m_Y_inHome,m_database->m_Zmax_nozzel,0);
    //m_basefunctions->trigger_next_command();
    m_database->m_G_Code_State = 1;
}

void CNC_automation::move_restposi()
{
    m_basefunctions->settings_inQ(m_F_max,-1,-1,-1);
    m_basefunctions->move_inQ(m_database->m_act_X,m_database->m_act_Y,9999,m_database->m_act_W);
    m_basefunctions->move_inQ(9999,-9999,9999,m_database->m_act_W);
    m_basefunctions->setPosition_inQ(m_database->m_size_X,0,m_database->m_Zmax_nozzel,0);
    //m_basefunctions->trigger_next_command();
    m_database->m_G_Code_State = 1;
}

void CNC_automation::calib_size()
{
    emit Log("calib size (do not use manuel move)");
    m_basefunctions->settings_inQ(m_F_max,-1,-1,-1);
    m_basefunctions->move_inQ(m_database->m_act_X,m_database->m_act_Y,9999,m_database->m_act_W);
    m_basefunctions->move_inQ(-9999,-9999,9999,m_database->m_act_W);
    m_basefunctions->setPosition_inQ(0,0,m_database->m_Zmax_nozzel,m_database->m_act_W);
    m_basefunctions->move_inQ(9999,9999,m_database->m_Zmax_nozzel,m_database->m_act_W);
    m_basefunctions->calib_size_safe();
    m_basefunctions->move_inQ(-9999,-9999,m_database->m_Zmax_nozzel,m_database->m_act_W);
    m_basefunctions->calib_size_results();
    //m_basefunctions->trigger_next_command();
    m_database->m_G_Code_State = 1;
}

void CNC_automation::repeat_test()
{
    emit Log("repeat test (do not use manuel move)");
    m_basefunctions->settings_inQ(50,-1,-1,-1);
    m_basefunctions->move_inQ(m_database->m_act_X,m_database->m_act_Y,9999,m_database->m_act_W);
    m_basefunctions->move_inQ(-9999,-9999,9999,m_database->m_act_W);
    m_basefunctions->setPosition_inQ(0,0,m_database->m_Zmax_nozzel,m_database->m_act_W);
    m_basefunctions->settings_inQ(m_database->m_speed1,-1,-1,-1);
    m_basefunctions->cycletimeTest();
    for(int i = 0;i<m_database->m_repeat1;i++)
    {
        m_basefunctions->move_inQ(m_database->m_x11,m_database->m_y11,m_database->m_z11,m_database->m_act_W);
        m_basefunctions->move_inQ(m_database->m_x12,m_database->m_y12,m_database->m_z12,m_database->m_act_W);
    }
    m_basefunctions->move_inQ(-9999,-9999,9999,m_database->m_act_W);
    m_basefunctions->repeattest_results();
    //m_basefunctions->trigger_next_command();
    m_database->m_G_Code_State = 1;
}

void CNC_automation::Z_calib()
{
    emit Log("Z angel calib (do not use manuel move)");
    m_basefunctions->m_pointList.clear();
    //first move home
    m_basefunctions->settings_inQ(m_F_max,-1,-1,-1);
    m_basefunctions->setPosition_inQ(0,0,0,0);
    m_basefunctions->move_inQ(-9999,-9999,9999,0);
    m_basefunctions->setPosition_inQ(m_database->m_X_inHome,m_database->m_Y_inHome,m_database->m_Zmax_nozzel,0);
    //messure origion Z
    m_basefunctions->move_inQ(m_database->m_X1,m_database->m_Y1,100,0);
    probe_Z(m_database->m_X1,m_database->m_Y1);

    probe_Z(m_database->m_X2,m_database->m_Y2);
    probe_Z(m_database->m_X3,m_database->m_Y3);
    probe_Z(m_database->m_X4,m_database->m_Y4);
    probe_Z(m_database->m_X1,m_database->m_Y1);

    // movbe home
    m_basefunctions->move_inQ(-1,-1,m_database->m_Zmax_nozzel+5,0);
    m_basefunctions->z_calib_results();
    //m_basefunctions->trigger_next_command();
    m_database->m_G_Code_State = 1;
}

void CNC_automation::probe_Z(float X,float Y)
{
    //move to test point
    m_basefunctions->settings_inQ(m_F_max,-1,-1,-1);
    m_basefunctions->move_inQ(X,Y,70,0);
    //make slow probe
    m_basefunctions->settings_inQ(70,-1,-1,-1);
    m_basefunctions->move_inQ(X,Y,-30,0);
    m_basefunctions->z_calib_safePos();
    m_basefunctions->move_inQ(X,Y,70,0);
    m_basefunctions->settings_inQ(m_F_max,-1,-1,-1);
}


void CNC_automation::G_Code_Start(QString fileName)
{
    m_database->m_G_Code_State = 2;
    emit Log("start g-code: "+fileName);
    m_database->FileLog("start g-code: "+fileName);
    m_database->SerialLog("start g-code: "+fileName);
    m_fileName = fileName;
    //m_basefunctions->send_setPosition(0,0,m_database->m_Zmax_nozzel,0);

    if(!inputFile.isOpen())
    {
        inputFile.setFileName(m_fileName);
        inputFile.open(QIODevice::ReadOnly);
        in.setDevice(&inputFile);
        in.setCodec("UTF-8");
    }else{
        emit errorLog("start g-code FAILD file is already open");
    }
    if(!isRunning())
    {
        this->start();
    }
}

void CNC_automation::run()
{
    G_Code_Parser();
}

void CNC_automation::G_Code_Pause()
{
    if(m_database->m_G_Code_State == 2)
    {
        emit Log("G-Code end pause");
        emit resend_last();
        //m_pause = false;
        m_database->m_G_Code_State = 1;
    }
    else
    {
        emit Log("G-Code pause");
        m_basefunctions->send_stop();
        //m_pause = true;
        m_database->m_G_Code_State = 2;
    }
}

void CNC_automation::G_Code_Stop()
{
    m_basefunctions->send_stop();
    m_database->cnc_send_commands.clear();
    m_database->m_G_Code_State = 0;
}

void CNC_automation::G_Code_Parser()
{
    QString newLine;
    QRegExp rx;

    //clear Queue
    m_database->cnc_send_commands.clear();

    //move home and set position
    m_basefunctions->settings_inQ(50,-1,-1,-1);
    m_basefunctions->setPosition_inQ(0,0,0,0);
    m_basefunctions->move_inQ(0, 0,9999,0);
    m_basefunctions->setPosition_inQ(0,0,0,0);
    m_basefunctions->move_inQ(-9999, -9999, 0, 0);
    //m_basefunctions->setPosition_inQ(0,0,m_database->m_Zmax_nozzel,0);
    m_basefunctions->setPosition_inQ(m_database->m_X_inHome,m_database->m_Y_inHome,m_database->m_Zmax_nozzel,0);
    m_X = 10;
    m_Y = 10;
    m_Z = m_database->m_Zmax_nozzel;
    m_W = 0;
    m_F = m_F_max*60;
    m_S = 220;

    //calib Z on calib plate
    if(m_database->m_useCalibPlate)
    {
        m_basefunctions->move_inQ(m_database->m_calibplateX,m_database->m_calibplateY,20,m_database->m_act_W);
        m_basefunctions->move_inQ(m_database->m_calibplateX,m_database->m_calibplateY,0,m_database->m_act_W);
        m_basefunctions->setPosition_inQ(m_database->m_calibplateX,m_database->m_calibplateY,m_database->m_calibplateZ,0);
        m_basefunctions->move_inQ(m_database->m_calibplateX,m_database->m_calibplateY,m_database->m_calibplateZ+20,m_database->m_act_W);
        m_basefunctions->move_inQ(m_database->m_calibplateX,m_database->m_calibplateY-50,m_database->m_calibplateZ+20,m_database->m_act_W);
        m_X = m_database->m_calibplateX;
        m_Y = m_database->m_calibplateY-50;
        m_Z = m_database->m_calibplateZ+20;
        m_W = m_database->m_act_W;
    }

    do{
        newLine = in.readLine();
        //m_database->FileLog(newLine);

        rx.setPattern(";");
        if(rx.indexIn(newLine)==0)
        {
            //emit Log("Comment: "+newLine);
            continue;
        }
        if(newLine=="")
        {
            //emit Log("emty line");
            continue;
        }
        getValue("X",newLine,&m_X);
        getValue("Y",newLine,&m_Y);
        getValue("Z",newLine,&m_Z);
        getValue("E",newLine,&m_W);
        getValue("S",newLine,&m_S);
        getValue("F",newLine,&m_F);

        if(isCommand("G0",newLine))//fast move
        {
            m_F = m_F_max*60;
            m_basefunctions->settings_inQ(m_F/60,m_S,-1,-1);
            m_basefunctions->move_inQ(m_X,m_Y,m_Z,m_W);
            m_validCommand = true;
        }
        if(isCommand("G1",newLine))//normal move
        {
            if(0.001>abs(m_F-m_F_old))
            {
                m_basefunctions->settings_inQ(m_F/60,m_S,-1,-1);
            }
            m_basefunctions->move_inQ(m_X,m_Y,m_Z,m_W);
            m_validCommand = true;
        }
        if(isCommand("G21",newLine))//use mm
        {
            m_validCommand = true;
        }
        if(isCommand("G28",newLine))//move home
        {
            m_basefunctions->settings_inQ(50,-1,-1,-1);
            m_basefunctions->setPosition_inQ(0,0,0,0);
            m_basefunctions->move_inQ(0, 0,9999,0);
            m_basefunctions->setPosition_inQ(0,0,0,0);
            m_basefunctions->move_inQ(-9999, -9999, 0, 0);
            m_basefunctions->setPosition_inQ(0,0,m_database->m_Zmax_nozzel,0);
            m_validCommand = true;
        }
        if(isCommand("G31",newLine))//move untile endswitch
        {
            m_validCommand = true;
        }
        if(isCommand("G90",newLine))//absolute referenz
        {
            m_validCommand = true;
        }
        if(isCommand("G91",newLine))//incrementel movement
        {
            m_database->FileLog("ERROR find G91 (useless comand)");
            emit errorLog("use incrementel movement");
            m_validCommand = true;
        }
        if(isCommand("G92",newLine))//set position
        {
           m_basefunctions->setPosition_inQ(m_X,m_Y,m_Z,m_W);
           m_validCommand = true;
        }
        if(isCommand("M104",newLine))//set temperatur
        {
            m_basefunctions->settings_inQ(m_F/60,m_S,-1,-1);
            m_validCommand = true;
        }
        if(isCommand("M106",newLine))
        {
            //m_database->FileLog("find M106 (useless comand)");
            m_validCommand = true;
        }
        if(isCommand("M107",newLine))
        {
            //m_database->FileLog("find M107 (useless comand)");
            m_validCommand = true;
        }
        if(isCommand("M109",newLine))//wait for reaching temperatur
        {
            m_basefunctions->settings_inQ(m_F/60,m_S,-1,-1);
            m_basefunctions->wait_for_heating();
            m_validCommand = true;
        }
        if(isCommand("M190",newLine))//set Bed Temperatur
        {
            m_basefunctions->settings_inQ(m_F/60,-1,-1,m_S);
            m_validCommand = true;
        }
        if(isCommand("M140",newLine))//set Bed Temperatur
        {
            m_basefunctions->settings_inQ(m_F/60,-1,-1,m_S);
            m_validCommand = true;
        }
        if(!m_validCommand)
        {
            emit errorLog("Line not Known:"+newLine);
            emit Log("Line not Known:"+newLine);
            m_database->FileLog("Line not Known:"+newLine);
        }else{
            emit Log("Command: "+newLine);
        }
    }while(!newLine.isNull());

    //move home and set position
    m_basefunctions->settings_inQ(50,0,-1,-1);
    m_basefunctions->setPosition_inQ(0,0,0,0);
    m_basefunctions->move_inQ(0, 0,9999,0);
    m_basefunctions->setPosition_inQ(0,0,0,0);
    m_basefunctions->move_inQ(-9999, -9999, 0, 0);

    emit Log("end of file");
    inputFile.close();
    emit Log("read G-Code is finishd");
    //m_basefunctions->trigger_next_command();
    m_database->m_G_Code_State = 1;
}

bool CNC_automation::isCommand(const QString indent,const QString line)
{
    QRegExp rx;
    rx.setPattern(indent);
    if(rx.indexIn(line)!=-1)
    {
        //emit Log(indent+" X"+QString::number(m_X)+" Y"+QString::number(m_Y)+" Z"+QString::number(m_Z)+" W"+QString::number(m_W)+" S"+QString::number(m_S)+" F"+QString::number(m_F));
        return(true);
    }
    return(false);
}

void CNC_automation::getValue(const QString indent,const QString line,float *target)
{
    QRegExp rx;
    QString resultStr;
    //set the pattern for an indent and the folloing number
    rx.setPattern(indent+"-?\\d*.\\d*");
    //if indent and number there
    if(rx.indexIn(line)!=-1)
    {
        //remove first letter and convert to float
        resultStr = rx.cap(0);
        resultStr.remove(0, 1);
        *target = resultStr.toFloat();
    }
}



