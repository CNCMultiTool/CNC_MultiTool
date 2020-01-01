#include "autofunctions.h"

AutoFunctions::AutoFunctions()
{
    m_X = 0;
    m_Y = 0;
    m_Z = 0;
    m_W = 0;
    m_S = 0;
    m_F = 0;
    m_validCommand = false;
    m_pause = false;
    m_aboard = false;

    m_size_X = 269.89;
    m_size_Y = 231.68;

    m_Fmax = 50;
}

AutoFunctions::~AutoFunctions()
{
    m_aboard = true;
}

void AutoFunctions::G_Code_Stop()
{
    m_aboard = true;
    emit Log("g-code abbruch "+QString::number(m_aboard));
}

void AutoFunctions::G_Code_Pause()
{
    m_pause = !m_pause;
    emit Log("g-code pause "+QString::number(m_pause));
}

void AutoFunctions::moveAndWait(float X, float Y, float Z, float W)
{
    emit send_move(X,Y,Z,W);
    m_loop.exec();
}

void AutoFunctions::G_Code_Start(QString fileName)
{
    emit Log("pos in autofunction X:"+QString::number(m_act_X)+" Y:"+QString::number(m_act_Y)+" Z:"+QString::number(m_act_Z)+" W:"+QString::number(m_act_W));
    m_fileName = fileName;
    m_programm = 1;
    run();
}

void AutoFunctions::move_home()
{
    m_programm = 2;
    run();
}

void AutoFunctions::calib_size()
{
    m_programm = 3;
    run();
}

void AutoFunctions::repeat_test()
{
    m_programm = 4;
    run();
}

void AutoFunctions::Z_calib()
{
    m_programm = 5;
    run();
}

void AutoFunctions::run()
{
    switch(m_programm)
    {
        case 1:
            g_code_process();
            break;
        case 2:
            move_home_process();
            break;
        case 3:
            calib_size_process();
            break;
        case 4:
            repeat_test_process();
            break;
        case 5:
            Z_calib_process();
            break;
    }
}

void AutoFunctions::g_code_process()
{
    QFile inputFile(m_fileName);
    inputFile.open(QIODevice::ReadOnly);
    QTextStream in(&inputFile);
    in.setCodec("UTF-8");


    QString newLine;
    QRegExp rx;

    while(!m_aboard)
    {
        newLine = in.readLine();
        if(newLine.isNull())
        {
            emit Log("end of file");
            return;
        }
        rx.setPattern(";");
        if(rx.indexIn(newLine)==0)
        {
            emit Log("Comment: "+newLine);
            continue;
        }
        if(newLine == "")
        {
            emit Log("emti line");
        }
        getValue("X",newLine,&m_X);
        getValue("Y",newLine,&m_Y);
        getValue("Z",newLine,&m_Z);
        getValue("E",newLine,&m_W);
        getValue("S",newLine,&m_S);
        getValue("F",newLine,&m_F);

        if(isCommand("G0",newLine))//fast move
        {
            emit send_settings(m_S,m_Fmax,-1);
            moveAndWait(m_X,m_Y,m_Z,m_W);
            m_validCommand = true;
        }
        if(isCommand("G1",newLine))//normal move
        {
            if(0.001>abs(m_F-m_Fold))
                emit send_settings(m_S,m_F/60,-1);

            moveAndWait(m_X,m_Y,m_Z,m_W);
            m_validCommand = true;
        }
        if(isCommand("G21",newLine))//use mm
            m_validCommand = true;
        if(isCommand("G28",newLine))//move home
        {
            move_home();
            m_validCommand = true;
        }
        if(isCommand("G31",newLine))//move untile endswitch
        {
            calib_size();
            m_validCommand = true;
        }
        if(isCommand("G90",newLine))//absolute referenz
            m_validCommand = true;
        if(isCommand("G91",newLine))//incrementel movement
            m_validCommand = true;
        if(isCommand("G92",newLine))//set position
            emit send_setPosition(m_X,m_Y,m_Z,m_W);
            m_validCommand = true;
        if(isCommand("M104",newLine))//set temperatur
            emit send_settings(m_S,m_F/60,-1);
            m_validCommand = true;
        if(isCommand("M107",newLine))
            m_validCommand = true;
        if(isCommand("M109",newLine))//wait for reaching temperatur
            m_validCommand = true;

        if(!m_validCommand)
        {
            emit errorLog("Line not Known:"+newLine);
        }
        m_Fold = m_F;
    }
}

bool AutoFunctions::isCommand(const QString indent,const QString line)
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

void AutoFunctions::getValue(const QString indent,const QString line,float *target)
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

void AutoFunctions::move_home_process()
{
    emit send_settings(50,-1,-1);
    if(m_act_Z_end<0.5)
        moveAndWait(m_act_X,m_act_Y,9999,m_act_W);
    if(m_act_X_end>-0.5||m_act_Y_end>-0.5)
        moveAndWait(-9999,-9999,9999,m_act_W);
}

void AutoFunctions::calib_size_process()
{
    emit Log("calib_size");
    move_home();
    emit send_setPosition(0,0,0,0);
    moveAndWait(9999,9999,0,0);
    m_size_X = m_act_X;
    m_size_Y = m_act_Y;
    move_home();
    emit Log("Sizecalib X size: "+QString::number(m_size_X)+" Y size: "+QString::number(m_size_Y));
    emit Log("calib error X size: "+QString::number(m_act_X)+" Y size: "+QString::number(m_act_Y));
}

void AutoFunctions::repeat_test_process()
{
    emit Log("repeat test");
    //fast smal movements (speed,dist,repeat)
    emit Log("repeat test: fast smal movements");
    repeat_movement(50,10,10);
    //emit Log("repeat test: fast long movements");
    //repeat_movement(50,100,1);
    //emit Log("repeat test: fast long movements");
    //repeat_movement(40,10,10);

    emit Log("end repeattest");
}

void AutoFunctions::repeat_movement(float speed,float dist,int repeat)
{
    emit Log("settings speed: "+QString::number(speed)+"  trafel distans: "+QString::number(dist)+"  repeat`s: "+QString::number(repeat));
    move_home();
    emit send_setPosition(0,0,0,0);
    emit send_settings(speed,-1,-1);
    for(int i=0;i<repeat;i++)
    {
        moveAndWait(10,10,-10,0);
        moveAndWait(10+dist,10+dist,-1*(10+dist),0);
    }
    move_home();
    emit Log("result Xerror: "+QString::number(m_act_X)+"  Yerror: "+QString::number(m_act_Y)+"  Zerror: "+QString::number(m_act_Z));
}

void AutoFunctions::Z_calib_process()
{
    emit Log("Z calib");
    move_home();
    //messure origion Z
    moveAndWait(m_act_X,m_act_Y,-999,0);//move in home to z = 0
    emit send_setPosition(0,0,0,0);// set origon
    emit send_settings(20,-1,-1);//slower test speed
    moveAndWait(m_act_X,m_act_Y,10,0);//lift TCP

    probe_Z(m_size_X,0);
    probe_Z(m_size_X,m_size_Y);
    probe_Z(0,m_size_Y);
    probe_Z(0,0);
    emit Log("end of Z calib");
}

void AutoFunctions::probe_Z(float X,float Y)
{
    emit send_settings(50,-1,-1);//fast traffel speed
    moveAndWait(X,Y,10,0);//move to test point
    emit send_settings(20,-1,-1);//slower test speed
    moveAndWait(X,Y,-999,0);//lower TCP
    emit Log("Zerror: "+QString::number(m_act_Z)+" at X:"+QString::number(m_act_X)+" Y:"+QString::number(m_act_Y));
    moveAndWait(X,Y,10,0);//lift TCP
}
