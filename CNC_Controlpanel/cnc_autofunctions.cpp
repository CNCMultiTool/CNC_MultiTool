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
    m_lineCount = 0;
    emit show_state(m_lineCount);
}

QByteArray cnc_autofunctions::GC_getNextLine()
{
    QByteArray newLine;
    //if line is commend read next line
    if(m_LineBuffer.length() == 0){
        //emit Log("read new line");
        GC_readNextLine();
        if(m_LineBuffer.length() > 0){
            //emit Log("use new line");
            newLine = m_LineBuffer.first();
            m_LineBuffer.removeFirst();
        }else{
            emit Log("no line after read");
        }
    }
    else
    {
        //send from linebuffer
        //emit Log("has line in buffer");
        newLine = m_LineBuffer.first();
        m_LineBuffer.removeFirst();
    }
    emit show_state(++m_lineCount);
    return newLine;
}

void cnc_autofunctions::GC_readNextLine(){
    QString newLine;
    QRegExp rx;
    rx.setPattern(";");
    if(true){//m_inputFile.isOpen()){

        //read new lines until it not starts with ; (is a commend)
        do{
            newLine = m_in.readLine();
        }while(rx.indexIn(newLine)==0);

        //cut new line at ; (after is commend)
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
    QByteArray lineInByteArray;

    emit Log("G_Code_Parser: "+newLine);

        rx.setPattern(";");
        if(rx.indexIn(newLine)==0)
        {
            //emit Log("Comment: "+newLine);

        }
        if(newLine=="")
        {
            //emit Log("emty line");

        }


        if(isCommand("G0",newLine))//fast move
        {
            lineInByteArray.append(10);//add command
            lineInByteArray.append(createValueArray(newLine));//add values
            lineInByteArray.append(createValueArray(QString("F%1").arg(m_database->m_max_speed)));//add values
        }
        if(isCommand("G1",newLine))//normal move
        {
            lineInByteArray.append(10);//add command
            lineInByteArray.append(createValueArray(newLine));//add values
        }
        if(isCommand("G9",newLine))//STOP
        {
            lineInByteArray.append(50);//add command
        }
        if(isCommand("G31",newLine))//move untile endswitch
        {
            return;
        }
        if(isCommand("G92",newLine))//set position
        {
            lineInByteArray.append(12);//add command
            lineInByteArray.append(createValueArray(newLine));//add values
        }
        if(isCommand("M104",newLine))//set temperatur
        {
            lineInByteArray.append(13);//add command
            lineInByteArray.append(createValueArray(newLine));//add values
        }
        if(isCommand("M109",newLine))//set Bed Temperatur
        {
            lineInByteArray.append(14);//add command
            lineInByteArray.append(createValueArray(newLine));//add values
        }
        if(isCommand("M140",newLine))//wait for reaching temperatur
        {
            lineInByteArray.append(15);//add command
            lineInByteArray.append(createValueArray(newLine));//add values
        }
        if(isCommand("M190",newLine))//set Bed Temperatur
        {
            lineInByteArray.append(16);//add command
            lineInByteArray.append(createValueArray(newLine));//add values
        }
        if(isCommand("Q10",newLine))//turn on/off ES
        {
            lineInByteArray.append(17);//add command
            lineInByteArray.append(createValueArray(newLine));//add values
        }
        if(isCommand("Q11",newLine))//turn on/off ES
        {
            lineInByteArray.append(18);//add command
            lineInByteArray.append(createValueArray(newLine));//add values
        }
        if(isCommand("Q12",newLine))//turn on/off motor
        {
            lineInByteArray.append(19);//add command
            lineInByteArray.append(createValueArray(newLine));//add values
        }
        if(isCommand("Q13",newLine))//reset wait for heat
        {
            lineInByteArray.append(51);//add command
        }
        if(isCommand("Q20",newLine))//reset wait for heat
        {
            lineInByteArray.append(21);//add command
            lineInByteArray.append(createValueArray(newLine));//add values
        }
        if(isCommand("Q21",newLine))//reset wait for heat
        {
            lineInByteArray.append(22);//add command
            lineInByteArray.append(createValueArray(newLine));//add values
        }
        if(isCommand("Q30",newLine))//reset wait for heat
        {
            lineInByteArray.append(23);//add command
            lineInByteArray.append(createValueArray(newLine));//add values
        }
        if(isCommand("M114",newLine))//reset wait for heat
        {
            lineInByteArray.append(24);//add command
        }
        if(isCommand("Q14",newLine))//reset wait for heat
        {
            lineInByteArray.append(52);//add command
        }
        if(isCommand("XXX",newLine))//this is just for testing stufff
        {
            lineInByteArray.append(60);//add command
        }

        if(isCommand("G28",newLine))//move home
        {
            G_Code_Parser("M114 ");
            G_Code_Parser("Q12 X1");//switch on motor
            G_Code_Parser("Q11 X1");//switch on endswitch
            G_Code_Parser("G1 Z9999");
            G_Code_Parser("G1 X-9999 Y-9999");
            G_Code_Parser(QString("G92 X%1 Y%2 Z%3 E0")
                                .arg(m_database->m_X_inHome)
                                .arg(m_database->m_Y_inHome)
                                .arg(m_database->m_Z_inHome));
            //G_Code_Parser("Q11 X0");//switch of endswitch
            //G_Code_Parser("M114 ");
        }
        if(lineInByteArray.isEmpty())
        {
            emit errorLog("no valid command find in G_Code_Parser");
        }else{
            m_LineBuffer.append(lineInByteArray);// add createt bytearray to the buffer
        }

        if(m_database->m_G_Code_State == 0)
        {
            QByteArray newBA = GC_getNextLine();
            emit serial_send(newBA);
        }
}

bool cnc_autofunctions::isCommand(const QString indent,const QString line)
{
//    QRegExp rx;
//    rx.setPattern(indent);
    if(line.contains(indent+" ")||line.compare(indent)==0)
    {
        //emit Log(indent+" X"+QString::number(m_X)+" Y"+QString::number(m_Y)+" Z"+QString::number(m_Z)+" W"+QString::number(m_W)+" S"+QString::number(m_S)+" F"+QString::number(m_F));
        return(true);
    }
    return(false);
}

/**
 * @brief cnc_autofunctions::getValue
 * @param indent Char (XYZESF) to find the right number
 * @param line String in with to search for the number
 * @param target pointer to float
 */
bool cnc_autofunctions::getValue(const QString indent,const QString line,float *target)
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
        return true;
    }
    return false;
}

QByteArray cnc_autofunctions::createValueArray(QString line)
{
    float newFloat;
    QByteArray ba;
    if(getValue("X",line,&newFloat)){
        ba.append(char(1));
        ba.append(reinterpret_cast<const char *>(&newFloat), sizeof (newFloat));
    }
    if(getValue("Y",line,&newFloat)){
        ba.append(char(2));
        ba.append(reinterpret_cast<const char *>(&newFloat), sizeof (newFloat));
    }
    if(getValue("Z",line,&newFloat)){
        ba.append(char(3));
        ba.append(reinterpret_cast<const char *>(&newFloat), sizeof (newFloat));
    }
    if(getValue("E",line,&newFloat)){
        ba.append(char(4));
        ba.append(reinterpret_cast<const char *>(&newFloat), sizeof (newFloat));
    }
    if(getValue("S",line,&newFloat)){
        ba.append(char(5));
        ba.append(reinterpret_cast<const char *>(&newFloat), sizeof (newFloat));
    }
    if(getValue("F",line,&newFloat)){
        newFloat = newFloat/60;
        ba.append(char(6));
        ba.append(reinterpret_cast<const char *>(&newFloat), sizeof (newFloat));
    }
    return ba;
}
