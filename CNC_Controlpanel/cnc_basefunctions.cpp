#include "cnc_basefunctions.h"

cnc_basefunctions::cnc_basefunctions(cnc_data *database,cnc_autofunctions *autofunc,Serial *mySerial)
{
    m_database = database;
    m_mySerial = mySerial;
    m_auto = autofunc;
}

cnc_basefunctions::~cnc_basefunctions()
{

}

void cnc_basefunctions::send_moveparam(float *acc,float *min_speed,float *max_speed,float *threshAngle,float *filament)
{
    QString Line = "Q10";
    if(acc != nullptr)
        Line += " X"+QString::number(*acc);
    if(min_speed != nullptr)
        Line += " Y"+QString::number(*min_speed);
    if(max_speed != nullptr)
        Line += " Z"+QString::number(*max_speed);
    if(threshAngle != nullptr)
        Line += " E"+QString::number(*threshAngle);
    if(filament != nullptr)
        Line += " S"+QString::number(*filament);

    emit serial_send(Line);
}

void cnc_basefunctions::send_NTC_values(float *bitwertNTC,float *widerstandNTC,float *widerstand1)
{
    QString Line = "Q20";
    if(bitwertNTC != nullptr)
        Line += " X"+QString::number(*bitwertNTC);
    if(widerstandNTC != nullptr)
        Line += " Y"+QString::number(*widerstandNTC);
    if(widerstand1 != nullptr)
        Line += " Z"+QString::number(*widerstand1);

    emit serial_send(Line);
}

void cnc_basefunctions::send_PID_values(float *P,float *I,float *D,bool *E)
{
    QString Line = "Q21";
    if(P != nullptr)
        Line += " X"+QString::number(*P);
    if(I != nullptr)
        Line += " Y"+QString::number(*I);
    if(D != nullptr)
        Line += " Z"+QString::number(*D);
    if(E != nullptr)
        Line += " E"+QString::number(*E);

    emit serial_send(Line);
}

void cnc_basefunctions::send_NTC_values_bed(float *bitwertNTC_bed,float *widerstandNTC_bed,float *widerstand1_bed)
{
    QString Line = "Q30";
    if(bitwertNTC_bed != nullptr)
        Line += " X"+QString::number(*bitwertNTC_bed);
    if(widerstandNTC_bed != nullptr)
        Line += " Y"+QString::number(*widerstandNTC_bed);
    if(widerstand1_bed != nullptr)
        Line += " Z"+QString::number(*widerstand1_bed);

    emit serial_send(Line);
}

void cnc_basefunctions::send_Speed(float F)
{
    emit serial_send("G1 F"+QString::number(F*60));
}

void cnc_basefunctions::send_Filament(float S)
{
    emit serial_send("Q10 S"+QString::number(S));
}

void cnc_basefunctions::send_BedTemp(float S)
{
    emit serial_send("M140 S"+QString::number(S));
}

void cnc_basefunctions::send_HotendTemp(float S)
{
    emit serial_send("M104 S"+QString::number(S));
}

void cnc_basefunctions::send_stop()
{
    emit serial_send("G9 ");
}
void cnc_basefunctions::send_GCodeStart(QString file)
{
    m_auto->GC_open(file);
    m_database->m_G_Code_State = 1;//run gcode
    QString newLine = m_auto->GC_getNextLine();
    emit Log("newLine: "+newLine);
    emit serial_send(newLine+" ");
}
void cnc_basefunctions::send_GCodePause()
{
    emit serial_send("Q103 ");
}
void cnc_basefunctions::send_GCodeContinue()
{
    emit serial_send("Q104 ");
}
void cnc_basefunctions::send_GCodeStop()
{
    emit serial_send("Q14 ");
}
void cnc_basefunctions::send_XXX()
{
    emit serial_send("XXX ");
}

void cnc_basefunctions::send_getPosition()
{
    emit serial_send("M114 ");
}

void cnc_basefunctions::send_move(float *X,float *Y,float *Z,float *E,float *F)
{
    QString Line = "G1";
    if(X != nullptr)
        Line += " X"+QString::number(*X);
    if(Y != nullptr)
        Line += " Y"+QString::number(*Y);
    if(Z != nullptr)
        Line += " Z"+QString::number(*Z);
    if(E != nullptr)
        Line += " E"+QString::number(*E);
    if(F != nullptr)
        Line += " F"+QString::number(*F * 60);

    emit serial_send(Line);
}

void cnc_basefunctions::send_setPosition(float *X, float *Y, float *Z, float *E)
{
    QString Line = "G92";
    if(X != nullptr)
        Line += " X"+QString::number(*X);
    if(Y != nullptr)
        Line += " Y"+QString::number(*Y);
    if(Z != nullptr)
        Line += " Z"+QString::number(*Z);
    if(E != nullptr)
        Line += " E"+QString::number(*E);
    emit serial_send(Line);
}
void cnc_basefunctions::send_moveHome(){
    emit serial_send("M114 ");
    emit serial_send("Q12 X1");
    emit serial_send("Q11 X1");
    emit serial_send("G1 Z9999");
    emit serial_send("G1 X-9999 Y-9999");
    emit serial_send(QString("G92 X%1 Y%2 Z%3 E0").arg(m_database->m_X_inHome).arg(m_database->m_Y_inHome).arg(m_database->m_Z_inHome));
    emit serial_send("M114 ");
    emit serial_send("Q11 X0");
}


void cnc_basefunctions::send_setESuse(bool state){
    emit serial_send("Q11 X"+QString::number(state));
}

void cnc_basefunctions::send_setMotorUse(bool state){
    emit serial_send("Q12 X"+QString::number(state));
}

void cnc_basefunctions::send_resetWaitForHeat()
{
    emit serial_send("Q13 ");
}

void  cnc_basefunctions::processLine(const QString &s)
{
    if(s.indexOf("GC_NL")!=-1)
    {
        m_send_buffer_counter--;
        while(m_ComandLines.length()!=0 && m_send_buffer_counter<5)
        {
            m_send_buffer_counter++;
            emit serial_send(m_ComandLines[0]);
            m_ComandLines.pop_front();
            m_send_counter++;
            emit Log("Process "+QString::number(m_send_counter)+" of "+QString::number(m_read_counter));
        }
    }

    if(s.indexOf("request")!=-1 && m_database->m_G_Code_State == 1){
        //emit Log("get request");
        int start = s.indexOf("request ")+7;
        int end = s.length();
        int requests = s.mid(start,end-start).toFloat();
        QString newLine;
        if(requests > 1){
            for(int i = 0;i < 1; i++){
                newLine = m_auto->GC_getNextLine();
                //emit Log("1 newLine: "+newLine);
                emit serial_send(newLine+" ");
            }
        }
    }

    if(s.indexOf("errorFileOpen")!=-1)
    {
        m_ComandLines.clear();
        emit Log("create file failed");
    }

    if(s.indexOf("M114 X")!=-1)
    {
        int start = s.indexOf("M114 X")+6;
        int end = s.indexOf(" Y",start);
        m_database->m_act_X = s.mid(start,end-start).toFloat();
        start = s.indexOf(" Y",end)+2;
        end = s.indexOf(" Z",start);
        m_database->m_act_Y = s.mid(start,end-start).toFloat();
        start = s.indexOf(" Z",end)+2;
        end = s.indexOf(" E",start);
        m_database->m_act_Z = s.mid(start,end-start).toFloat();
        start = s.indexOf(" E",end)+2;
        end = s.length();
        m_database->m_act_E = s.mid(start,end-start).toFloat();
        emit show_position();
    }
    if(s.indexOf("Temp THsoll")!=-1)
    {
        int start = s.indexOf("Temp THsoll")+11;
        int end = s.indexOf(" THist",start);
        m_database->m_soll_temperatur = s.mid(start,end-start).toFloat();
        start = s.indexOf(" THist",end)+6;
        end = s.indexOf(" TBsoll",start);
        m_database->m_act_temperatur = s.mid(start,end-start).toFloat();
        start = s.indexOf(" TBsoll",end)+7;
        end = s.indexOf(" TBist",start);
        m_database->m_soll_bedTemp = s.mid(start,end-start).toFloat();
        start = s.indexOf(" TBist",end)+6;
        end = s.length();
        m_database->m_act_bedTemp = s.mid(start,end-start).toFloat();
        emit show_temp();
    }
    if(s.indexOf("ES ")!=-1)
    {
        int start;
        int end = end = s.length();
        if(s.contains("X")){
            start = s.indexOf("X")+1;
            m_database->m_endswitch_X = s.mid(start,end-start).toFloat();
        }else if(s.contains("Y")){
            start = s.indexOf("Y")+1;
            m_database->m_endswitch_Y = s.mid(start,end-start).toFloat();
        }else if(s.contains("Z")){
            start = s.indexOf("Z")+1;
            m_database->m_endswitch_Z = s.mid(start,end-start).toFloat();
        }
        emit show_endswitch();
    }
    if(s.indexOf("G1 F")!=-1)
    {
        int start = s.indexOf("G1 F")+4;
        int end = s.length();
        m_database->m_act_speed = s.mid(start,end-start).toFloat();
        emit show_speed();
    }
    if(s.indexOf("Q10 X")!=-1)
    {
        int start = s.indexOf("Q10 X")+5;
        int end = s.indexOf(" Y",start);
        m_database->m_max_acc = s.mid(start,end-start).toFloat();
        start = s.indexOf(" Y",end)+2;
        end = s.indexOf(" Z",start);
        m_database->m_min_speed = s.mid(start,end-start).toFloat();
        start = s.indexOf(" Z",end)+2;
        end = s.indexOf(" E",start);
        m_database->m_max_speed = s.mid(start,end-start).toFloat();
        start = s.indexOf(" E",end)+2;
        end = s.indexOf(" S",start);
        m_database->m_threshAngle = s.mid(start,end-start).toFloat();
        start = s.indexOf(" S",end)+2;
        end = s.length();
        m_database->m_act_filament = s.mid(start,end-start).toFloat();
        emit show_acc_speed_fila();
    }
    if(s.indexOf("M104 S")!=-1)
    {
        int start = s.indexOf("M104 S")+6;
        int end = s.length();
        m_database->m_soll_temperatur = s.mid(start,end-start).toFloat();
        emit show_act_temp();
    }
    if(s.indexOf("M109 S")!=-1)
    {
        int start = s.indexOf("M109 S")+6;
        int end = s.length();
        m_database->m_soll_temperatur = s.mid(start,end-start).toFloat();
        emit show_act_temp();
    }
    if(s.indexOf("M140 S")!=-1)
    {
        int start = s.indexOf("M140 S")+6;
        int end = s.length();
        m_database->m_soll_bedTemp = s.mid(start,end-start).toFloat();
        emit show_act_temp();
    }
    if(s.indexOf("useES ")!=-1)
    {
        int start = s.indexOf("useES ")+6;
        int end = s.length();
        m_database->setUseEs(s.mid(start,end-start).toFloat());
    }
    if(s.indexOf("motorState ")!=-1)
    {
        int start = s.indexOf("motorState ")+11;
        int end = s.length();
        m_database->setMotorUse(s.mid(start,end-start).toFloat());
    }
    if(s.indexOf("waitForHeat ")!=-1)
    {
        int start = s.indexOf("waitForHeat ")+12;
        int end = s.length();
        emit show_waitForHeat(s.mid(start,end-start).toFloat());
    }

}


