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
    m_pose_recived = false;
    m_pause = false;
    m_aboard = false;
}

AutoFunctions::~AutoFunctions()
{
    m_aboard = true;
}


void AutoFunctions::act_pose_recived()
{
    m_pose_recived = true;
}

void AutoFunctions::wait_for_finish()
{
    m_pose_recived = false;
    while(m_pose_recived == false||m_pause == true)
    {

    }
}

void AutoFunctions::G_Code_Stop()
{
    m_aboard = true;
}

void AutoFunctions::G_Code_Pause()
{
    m_pause = !m_pause;
}

void AutoFunctions::moveAndWait(float X, float Y, float Z, float W)
{
    emit send_move(X,Y,Z,W);
    m_loop.exec();
}

void AutoFunctions::G_Code_Start(QString fileName)
{
    m_fileName = fileName;
    run();
}

void AutoFunctions::run()
{
    emit Log("start G-Code parsing");
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
            moveAndWait(m_X,m_Y,m_Z,m_W);
            m_validCommand = true;
        }
        if(isCommand("G1",newLine))//normal move
        {
            moveAndWait(m_X,m_Y,m_Z,m_W);
            m_validCommand = true;
        }
        if(isCommand("G21",newLine))//use mm
            m_validCommand = true;
        if(isCommand("G28",newLine))//move home
            m_validCommand = true;
        if(isCommand("G31",newLine))//move untile endswitch
            m_validCommand = true;
        if(isCommand("G90",newLine))//absolute referenz
            m_validCommand = true;
        if(isCommand("G91",newLine))//incrementel movement
            m_validCommand = true;
        if(isCommand("G92",newLine))//set position
            m_validCommand = true;
        if(isCommand("M104",newLine))//set temperatur
            m_validCommand = true;
        if(isCommand("M107",newLine))
            m_validCommand = true;
        if(isCommand("M109",newLine))//wait for reaching temperatur
            m_validCommand = true;

        if(!m_validCommand)
        {
            emit errorLog("Line not Known:"+newLine);
        }
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
