#include "cnc_automation.h"

CNC_automation::CNC_automation()
{
    m_X = 0;
    m_Y = 0;
    m_Z = 0;
    m_W = 0;
    m_F = 0;
    m_S = 0;
    m_validCommand = false;
}

CNC_automation::~CNC_automation()
{

}


void CNC_automation::GCode_Parser(QString fileName)
{
    m_FileName = fileName;
    emit Log("start G-Code parsing");
    QFile inputFile(m_FileName);
    inputFile.open(QIODevice::ReadOnly);
    QTextStream in(&inputFile);
    in.setCodec("UTF-8");


    QString newLine;
    QRegExp rx;

    while(true)
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
        getValue("X",newLine,&m_X);
        getValue("Y",newLine,&m_Y);
        getValue("Z",newLine,&m_Z);
        getValue("E",newLine,&m_W);
        getValue("S",newLine,&m_S);
        getValue("F",newLine,&m_F);

        if(isCommand("G0",newLine))
            m_validCommand = true;
        if(isCommand("G1",newLine))
            m_validCommand = true;
        if(isCommand("G21",newLine))
            m_validCommand = true;
        if(isCommand("G28",newLine))
            m_validCommand = true;
        if(isCommand("G90",newLine))
            m_validCommand = true;
        if(isCommand("G91",newLine))
            m_validCommand = true;
        if(isCommand("G92",newLine))
            m_validCommand = true;
        if(isCommand("M104",newLine))
            m_validCommand = true;
        if(isCommand("M107",newLine))
            m_validCommand = true;
        if(isCommand("M109",newLine))
            m_validCommand = true;

        if(!m_validCommand)
        {
            emit errorLog("Line not Known:"+newLine);
        }


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

QString CNC_automation::loadTextFile()
{
    QFile inputFile(m_FileName);
    inputFile.open(QIODevice::ReadOnly);
    QTextStream in(&inputFile);
    in.setCodec("UTF-8");
    return in.readAll();
}
