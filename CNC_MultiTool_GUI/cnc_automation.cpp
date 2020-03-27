#include "cnc_automation.h"

CNC_automation::CNC_automation(cnc_data *database,cnc_basefunctions *basefunctions)
{
    m_database = database;
    m_basefunctions = basefunctions;
    m_F_max = 50;
    m_F_old = 0;
    m_X = 0;
    m_Y = 0;
    m_Z = 0;
    m_W = 0;
    m_S = 0;
    m_F = 0;
}

CNC_automation::~CNC_automation()
{
    m_aboard = true;
}

void CNC_automation::move_home()
{
    m_basefunctions->settings_inQ(50,-1,35);
    m_basefunctions->move_inQ(m_database->m_act_X,m_database->m_act_Y,9999,m_database->m_act_W);
    m_basefunctions->move_inQ(-9999,-9999,9999,m_database->m_act_W);
    m_basefunctions->setPosition_inQ(0,0,m_database->m_Zmax_nozzel,0);
    m_basefunctions->trigger_next_command();
}

void CNC_automation::move_restposi()
{
    m_basefunctions->settings_inQ(50,-1,35);
    m_basefunctions->move_inQ(m_database->m_act_X,m_database->m_act_Y,9999,m_database->m_act_W);
    m_basefunctions->move_inQ(9999,-9999,9999,m_database->m_act_W);
    m_basefunctions->setPosition_inQ(m_database->m_size_X,0,m_database->m_Zmax_nozzel,0);
    m_basefunctions->trigger_next_command();
}

void CNC_automation::calib_size()
{
    emit Log("calib size (do not use manuel move)");
    m_basefunctions->settings_inQ(50,-1,35);
    m_basefunctions->move_inQ(m_database->m_act_X,m_database->m_act_Y,9999,m_database->m_act_W);
    m_basefunctions->move_inQ(-9999,-9999,9999,m_database->m_act_W);
    m_basefunctions->setPosition_inQ(0,0,m_database->m_Zmax_nozzel,m_database->m_act_W);
    m_basefunctions->move_inQ(9999,9999,m_database->m_Zmax_nozzel,m_database->m_act_W);
    m_basefunctions->calib_size_safe();
    m_basefunctions->move_inQ(-9999,-9999,m_database->m_Zmax_nozzel,m_database->m_act_W);
    m_basefunctions->calib_size_results();
    m_basefunctions->trigger_next_command();
}

void CNC_automation::repeat_test()
{
    emit Log("repeat test (do not use manuel move)");
    m_basefunctions->settings_inQ(50,-1,35);
    m_basefunctions->move_inQ(m_database->m_act_X,m_database->m_act_Y,9999,m_database->m_act_W);
    m_basefunctions->move_inQ(-9999,-9999,9999,m_database->m_act_W);
    m_basefunctions->setPosition_inQ(0,0,0,m_database->m_act_W);
    m_basefunctions->settings_inQ(30,-1,35);
    m_basefunctions->move_inQ(10,10,-10,m_database->m_act_W);
    for(int i = 0;i<2;i++)
    {
        m_basefunctions->move_inQ(20,20,-20,m_database->m_act_W);
        m_basefunctions->move_inQ(10,10,-10,m_database->m_act_W);
    }
    m_basefunctions->settings_inQ(50,-1,35);
    for(int i = 0;i<2;i++)
    {
        m_basefunctions->move_inQ(30,30,-30,m_database->m_act_W);
        m_basefunctions->move_inQ(10,10,-10,m_database->m_act_W);
    }
    m_basefunctions->move_inQ(-9999,-9999,9999,m_database->m_act_W);
    m_basefunctions->repeattest_results();
    m_basefunctions->trigger_next_command();
}

void CNC_automation::Z_calib()
{
    emit Log("Z angel calib (do not use manuel move)");
    m_basefunctions->m_pointList.clear();
    //first move home
    m_basefunctions->settings_inQ(50,-1,35);
    m_basefunctions->setPosition_inQ(0,0,0,0);
    m_basefunctions->move_inQ(-9999,-9999,9999,0);
    m_basefunctions->setPosition_inQ(0,0,0,0);
    //messure origion Z
    m_basefunctions->move_inQ(0,0,-9999,0);
    m_basefunctions->setPosition_inQ(0,0,0,0);
    probe_Z(0,0);
    probe_Z(m_database->m_size_X,0);
    probe_Z(m_database->m_size_X,m_database->m_size_X);
    probe_Z(0,m_database->m_size_X);
    probe_Z(0,0);
    // movbe home
    m_basefunctions->move_inQ(m_database->m_act_X,m_database->m_act_Y,9999,0);
    m_basefunctions->z_calib_results();
    m_basefunctions->trigger_next_command();
}

void CNC_automation::probe_Z(float X,float Y)
{
    //move to test point
    m_basefunctions->settings_inQ(50,-1,35);
    m_basefunctions->move_inQ(X,Y,10,0);
    //make slow probe
    m_basefunctions->settings_inQ(20,-1,35);
    m_basefunctions->move_inQ(X,Y,-10,0);
    m_basefunctions->z_calib_safePos();
    m_basefunctions->move_inQ(X,Y,10,0);
    m_basefunctions->settings_inQ(50,-1,35);
}


void CNC_automation::G_Code_Start(QString fileName)
{
    emit Log("start g-code: "+fileName);
    m_database->FileLog("start g-code: "+fileName);
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
    if(m_pause)
    {
        m_pause = false;
        //m_basefunctions->move_wait(m_X,m_Y,m_Z,m_W);
        if(!timer->isActive())
            timer->start();
    }
    else
    {
        m_pause = true;
        if(timer->isActive())
            timer->stop();
        m_basefunctions->send_stop();
    }
}

void CNC_automation::G_Code_Stop()
{
    m_aboard = true;
    if(timer->isActive())
        timer->stop();
    m_basefunctions->send_stop();
}

void CNC_automation::G_Code_Parser()
{/*
    QString newLine;
    QRegExp rx;

    do{
        newLine = in.readLine();
        m_database->FileLog(newLine);

        rx.setPattern(";");
        if(rx.indexIn(newLine)==0)
        {
            emit Log("Comment: "+newLine);
            return;
        }
        if(newLine=="")
        {
            emit Log("emty line");
            return;
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
            m_basefunctions->send_settings(m_F/60,m_S,-1);
            //m_basefunctions->move_wait(m_X,m_Y,m_Z,m_W);
            m_validCommand = true;
        }
        if(isCommand("G1",newLine))//normal move
        {
            if(0.001<abs(m_F-m_F_old))
            {
                //m_basefunctions->settings_wait(m_F/60,m_S,-1);
            }
            //m_basefunctions->move_wait(m_X,m_Y,m_Z,m_W);
            m_validCommand = true;
        }
        if(isCommand("G21",newLine))//use mm
        {
            emit Log("use mm");
            m_database->FileLog("use mm");
            m_validCommand = true;
        }
        if(isCommand("G28",newLine))//move home
        {
            move_home();
            m_validCommand = true;
        }
        if(isCommand("G31",newLine))//move untile endswitch
        {
            //calib_size();
            m_validCommand = true;
        }
        if(isCommand("G90",newLine))//absolute referenz
        {
            m_database->FileLog("find G90 (useless comand)");
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
           m_validCommand = true;
        }
        if(isCommand("M104",newLine))//set temperatur
        {
            //m_basefunctions->settings_wait(m_F/60,m_S,-1);
            m_validCommand = true;
        }
        if(isCommand("M106",newLine))
        {
            m_database->FileLog("find M106 (useless comand)");
            m_validCommand = true;
        }
        if(isCommand("M107",newLine))
        {
            m_F=m_F_max*60;
        }
        if(0.001>abs(m_F-m_F_old))
        {

            emit Log("Wait for Nozzel to heat");

            emit Log("Nozzel is heated");
            m_validCommand = true;
        }
        //m_basefunctions->move_wait(m_X,m_Y,m_Z,m_W);
        m_validCommand = true;
    }
    if(isCommand("G21",newLine))//use mm
    {
        emit Log("use mm");
        m_database->FileLog("use mm");
        m_validCommand = true;
    }
    if(isCommand("G28",newLine))//move home
    {
        move_home();
        m_validCommand = true;
    }
    if(isCommand("G31",newLine))//move untile endswitch
    {
        //calib_size();
        m_validCommand = true;
    }
    if(isCommand("G90",newLine))//absolute referenz
    {
        m_database->FileLog("find G90 (useless comand)");
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
        m_basefunctions->send_setPosition(m_X,m_Y,m_Z,m_W);
        m_validCommand = true;
    }
    if(isCommand("M104",newLine))//set temperatur
    {
        m_basefunctions->send_settings(m_F/60,m_S,-1);
        m_validCommand = true;
    }
    if(isCommand("M106",newLine))
    {
        m_database->FileLog("find M106 (useless comand)");
        m_validCommand = true;
    }
    if(isCommand("M107",newLine))
    {
        m_database->FileLog("find M107 (useless comand)");
        m_validCommand = true;
    }
    if(isCommand("M109",newLine))//wait for reaching temperatur
    {
        m_basefunctions->send_settings(m_F/60,m_S,-1);
        emit Log("Wait for Nozzel to heat");
        m_database->set_HWisHeating(true);
        m_validCommand = true;
    }
    if(!m_validCommand)
    {
        emit errorLog("Line not Known:"+newLine);
        emit Log("Line not Known:"+newLine);
        m_database->FileLog("Line not Known:"+newLine);
    }



    }
    emit Log("end of file");
    inputFile.close();
    emit Log("G-Code is finishd");*/
}

bool CNC_automation::isCommand(const QString indent,const QString line)
{
    QRegExp rx;
    rx.setPattern(indent);
    if(rx.indexIn(line)!=-1)
    {
        emit Log(indent+" X"+QString::number(m_X)+" Y"+QString::number(m_Y)+" Z"+QString::number(m_Z)+" W"+QString::number(m_W)+" S"+QString::number(m_S)+" F"+QString::number(m_F));
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

