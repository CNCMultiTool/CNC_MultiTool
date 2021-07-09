#include "cnc_autofunctions.h"

cnc_autofunctions::cnc_autofunctions(cnc_data *database)
{
    m_database = database;
}

cnc_autofunctions::~cnc_autofunctions()
{

}

void cnc_autofunctions::GC_open(QString fileName)
{
    if(m_inputFile.isOpen())
    {
        m_inputFile.close();
    }
    m_inputFile.setFileName(fileName);
    m_inputFile.open(QIODevice::ReadOnly);
    m_in.setDevice(&m_inputFile);
    m_in.setCodec("UTF-8");
    if(!m_inputFile.isOpen())
    {
        emit Log("file not open");
    }
}
QString cnc_autofunctions::GC_getNextLine()
{
    QString newLine = "";
    //if line is commend read next line
    if(m_LineBuffer.length() == 0){
        emit Log("read new line");
        GC_readNextLine();
        if(m_LineBuffer.length() > 0){
            emit Log("use new line");
            newLine = m_LineBuffer.first();
            m_LineBuffer.removeFirst();
        }else{
            emit Log("no line after read");
        }
    }
    else
    {
        //send from linebuffer
        emit Log("has line in buffer");
        newLine = m_LineBuffer.first();
        m_LineBuffer.removeFirst();
    }
    return newLine;
}

void cnc_autofunctions::GC_readNextLine(){
    QString newLine;
    QRegExp rx;
    rx.setPattern(";");
    if(true){//m_inputFile.isOpen()){
        do{
            newLine = m_in.readLine();
        }while(rx.indexIn(newLine)==0);

        if(rx.indexIn(newLine)!=-1)
        {
            newLine = newLine.mid(0,rx.indexIn(newLine));
        }

        if(newLine.isNull()){
            emit Log("end of gCode");
            m_database->m_G_Code_State = 0;
            GC_close();
        }else{
            G_Code_Parser(newLine);//add command to a buffer
        }


    }else{
        emit Log("no G File open");
    }
}

void cnc_autofunctions::GC_close()
{
    if(!m_inputFile.isOpen())
    {
        m_inputFile.close();
    }
}

void cnc_autofunctions::G_Code_Parser(QString newLine)
{
    QRegExp rx;

            rx.setPattern(";");
        if(rx.indexIn(newLine)==0)
        {
            //emit Log("Comment: "+newLine);

        }
        if(newLine=="")
        {
            //emit Log("emty line");

        }
//        getValue("X",newLine,&m_X);
//        getValue("Y",newLine,&m_Y);
//        getValue("Z",newLine,&m_Z);
//        getValue("E",newLine,&m_W);
//        getValue("S",newLine,&m_S);
//        getValue("F",newLine,&m_F);

        if(isCommand("G0",newLine))//fast move
        {
            m_LineBuffer.append(newLine);
        }
        if(isCommand("G1",newLine))//normal move
        {
            m_LineBuffer.append(newLine);
        }
        if(isCommand("G21",newLine))//use mm
        {

        }
        if(isCommand("G28",newLine))//move home
        {
            m_LineBuffer.append("M114 ");
            m_LineBuffer.append("Q12 X1");//switch on motor
            m_LineBuffer.append("Q11 X1");//switch on endswitch
            m_LineBuffer.append("G1 Z9999");
            m_LineBuffer.append("G1 X-9999 Y-9999");
            m_LineBuffer.append("G92 X%1 Y%2 Z%3 E0");
            m_LineBuffer.append("Q11 X0");//switch of endswitch
            m_LineBuffer.append("M114 ");
        }
        if(isCommand("G31",newLine))//move untile endswitch
        {

        }
        if(isCommand("G90",newLine))//absolute referenz
        {

        }
        if(isCommand("G91",newLine))//incrementel movement
        {

        }
        if(isCommand("G92",newLine))//set position
        {
            m_LineBuffer.append(newLine);
        }
        if(isCommand("M104",newLine))//set temperatur
        {
            m_LineBuffer.append(newLine);
        }
        if(isCommand("M106",newLine))
        {

        }
        if(isCommand("M107",newLine))
        {

        }
        if(isCommand("M109",newLine))//wait for reaching temperatur
        {
            m_LineBuffer.append(newLine);
        }
        if(isCommand("M190",newLine))//set Bed Temperatur
        {
            m_LineBuffer.append(newLine);
        }
        if(isCommand("M140",newLine))//set Bed Temperatur
        {
            m_LineBuffer.append(newLine);
        }
}

bool cnc_autofunctions::isCommand(const QString indent,const QString line)
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

void cnc_autofunctions::getValue(const QString indent,const QString line,float *target)
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
