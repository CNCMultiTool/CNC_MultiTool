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
    m_basefunctions->send_settings(50,-1,-1);
    m_basefunctions->move_wait(m_database->m_act_X,m_database->m_act_Y,9999,m_database->m_act_W);
    m_basefunctions->move_wait(-9999,-9999,m_database->m_act_Z,m_database->m_act_W);
}

void CNC_automation::move_restposi()
{
    m_basefunctions->send_settings(50,-1,-1);
    m_basefunctions->move_wait(m_database->m_act_X,m_database->m_act_Y,9999,m_database->m_act_W);
    m_basefunctions->move_wait(9999,-9999,m_database->m_act_Z,m_database->m_act_W);
}

void CNC_automation::calib_size()
{
    emit Log("calib size (do not use manuel move)");
    move_home();
    m_basefunctions->send_setPosition(0,0,0,0);
    m_basefunctions->move_wait(9999,9999,0,0);
    m_database->m_size_X = m_database->m_act_X;
    m_database->m_size_Y = m_database->m_act_Y;
    emit Log("result of Size calib is: X="+QString::number(m_database->m_size_X)+" Y="+QString::number(m_database->m_size_Y));
    move_home();
    emit Log("error of Size calib is: X="+QString::number(m_database->m_act_X)+" Y="+QString::number(m_database->m_act_Y));
}

void CNC_automation::repeat_test()
{
    emit Log("repeat test (do not use manuel move)");
    move_home();
    repeat_movement(50,1,10);
}

void CNC_automation::repeat_movement(float speed,float dist,int repeat)
{
    emit Log("settings speed: "+QString::number(speed)+"  trafel distans: "+QString::number(dist)+"  repeat`s: "+QString::number(repeat));
    move_home();
    m_basefunctions->send_setPosition(0,0,0,0);
    m_basefunctions->send_settings(speed,-1,-1);
    for(int i=0;i<repeat;i++)
    {
        m_basefunctions->move_wait(10,10,-10,0);
        m_basefunctions->move_wait(10+dist,10+dist,-1*(10+dist),0);
    }
    move_home();
    emit Log("result Xerror: "+QString::number(m_database->m_act_X)+"  Yerror: "+QString::number(m_database->m_act_Y)+"  Zerror: "+QString::number(m_database->m_act_Z));
}

void CNC_automation::Z_calib()
{
    emit Log("Z angel calib (do not use manuel move)");
    move_home();
    //messure origion Z
    m_basefunctions->move_wait(m_database->m_act_X,m_database->m_act_Y,-999,0);//move in home to z = 0
    m_basefunctions->send_setPosition(0,0,0,0);// set origon
    m_basefunctions->send_settings(20,-1,-1);//slower test speed
    m_basefunctions->move_wait(m_database->m_act_X,m_database->m_act_Y,10,0);//lift TCP
    m_database->m_error_X_max_Y_null = probe_Z(m_database->m_size_X,0);
    m_database->m_error_X_max_Y_max = probe_Z(m_database->m_size_X,m_database->m_size_Y);
    m_database->m_error_X_null_Y_max = probe_Z(0,m_database->m_size_Y);
    m_database->m_error_X_null_Y_null = probe_Z(0,0);
    move_home();
    emit Log("end of Z calib");
}

float CNC_automation::probe_Z(float X,float Y)
{
    m_basefunctions->send_settings(50,-1,-1);//fast traffel speed
    m_basefunctions->move_wait(X,Y,10,0);//move to test point
    m_basefunctions->send_settings(20,-1,-1);//slower test speed
    m_basefunctions->move_wait(X,Y,-999,0);//lower TCP
    float Zerror = m_database->m_act_Z;
    emit Log("Zerror: "+QString::number(m_database->m_act_Z)+" at X:"+QString::number(m_database->m_act_X)+" Y:"+QString::number(m_database->m_act_Y));
    m_basefunctions->move_wait(X,Y,10,0);//lift TCP
    return Zerror;
}


void CNC_automation::G_Code_Start(QString fileName)
{
    if(timer->isActive())
        timer->stop();
    emit Log("start g-code: "+fileName);
    m_database->FileLog("start g-code: "+fileName);
    m_fileName = fileName;
    m_aboard = false;
    m_pause = false;
    move_home();
    m_basefunctions->send_setPosition(0,0,m_database->m_Zmax_nozzel,0);
    //if(!isRunning())
    //{
    //    start();
    //}

    inputFile.setFileName(m_fileName);
    inputFile.open(QIODevice::ReadOnly);
    in.setDevice(&inputFile);
    in.setCodec("UTF-8");

    timer->start(1);
    connect(timer,SIGNAL(timeout()),this,SLOT(G_Code_Parser()));
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
        m_basefunctions->move_wait(m_X,m_Y,m_Z,m_W);
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
{
    //emit Log("g-code parser");
    QString newLine;
    QRegExp rx;
    if(m_database->m_HWisMoving)
        return;
    if(m_database->m_HWisHeating)
        return;
    newLine = in.readLine();
    m_database->FileLog(newLine);
    if(newLine.isNull())
    {
<<<<<<< HEAD
        emit Log("end of file");
        m_aboard = true;
        inputFile.close();
        if(timer->isActive())
            timer->stop();
        emit Log("G-Code is finishd");
        return;// /////////////////////////////////testing
    }
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
=======
        //if(m_pause)
        //{
        //  m_wait_loop.exec();
        //}
        newLine = in.readLine();
        m_database->FileLog(newLine);
        if(newLine.isNull())
        {
            emit Log("end of file");
            m_aboard = true;
            continue;
        }
        rx.setPattern(";");
        if(rx.indexIn(newLine)==0)
        {
            emit Log("Comment: "+newLine);
            continue;
        }
        if(newLine == "")
        {
            emit Log("emty line");
        }
        getValue("X",newLine,&m_X);
        getValue("Y",newLine,&m_Y);
        getValue("Z",newLine,&m_Z);
        getValue("E",newLine,&m_W);
        getValue("S",newLine,&m_S);
        getValue("F",newLine,&m_F);
>>>>>>> parent of 08a1822... functional fersion with bug

    if(isCommand("G0",newLine))//fast move
    {
        m_F = m_F_max*60;
        m_basefunctions->send_settings(m_F/60,m_S,-1);
        m_basefunctions->move_wait(m_X,m_Y,m_Z,m_W);
        m_validCommand = true;
    }
    if(isCommand("G1",newLine))//normal move
    {
        if(m_F>m_F_max*60)
        {
            m_F=m_F_max*60;
        }
        if(0.001>abs(m_F-m_F_old))
        {
            m_basefunctions->send_settings(m_F/60,m_S,-1);
        }
        m_basefunctions->move_wait(m_X,m_Y,m_Z,m_W);
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
/*
void CNC_automation::calc_correctionangel(QList<float> Z_errors)
{
    //m_X_angel = atan(Z_errors[0]/m_size_X);
    //m_Y_angel = atan(Z_errors[2]/m_size_Y);
    //a = math.tan(alpha)*b
    //float Xcorr = m_size_X * tan(m_X_angel);
    //float Ycorr = m_size_Y * tan(m_Y_angel);
    //emit Log("messuring error at end:"+QString::number(Z_errors[3]));
    //emit Log("X_angel:"+QString::number(m_X_angel)+" Y_angel:"+QString::number(m_Y_angel));
    //emit Log("Correction error:"+QString::number(Z_errors[1]-(Xcorr+Ycorr)));
}

//correction fallut
float CNC_automation::calc_correction(float X,float Y)
{
    //float Xcorr = X * tan(m_Y_angel);
    //float Ycorr = Y * tan(m_X_angel);
    //return(Xcorr+Ycorr);
}
*/
