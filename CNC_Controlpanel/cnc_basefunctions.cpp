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

    m_auto->G_Code_Parser(Line);
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

    m_auto->G_Code_Parser(Line);
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

    m_auto->G_Code_Parser(Line);
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

    m_auto->G_Code_Parser(Line);
}

void cnc_basefunctions::send_Speed(float F)
{
    m_auto->G_Code_Parser("G1 F"+QString::number(F*60));
}

void cnc_basefunctions::send_Filament(float S)
{
    m_auto->G_Code_Parser("Q10 S"+QString::number(S));
}

void cnc_basefunctions::send_BedTemp(float S)
{
    m_auto->G_Code_Parser("M140 S"+QString::number(S));
}

void cnc_basefunctions::send_HotendTemp(float S)
{
    m_auto->G_Code_Parser("M104 S"+QString::number(S));
}

void cnc_basefunctions::send_stop()
{
    m_auto->G_Code_Parser("G9 ");
}
void cnc_basefunctions::send_GCodeStart(QString file)
{
    m_auto->GC_open(file);
    m_database->m_G_Code_State = 1;//run gcode
    QByteArray newBA = m_auto->GC_getNextLine();
    emit serial_send(newBA);
}
void cnc_basefunctions::send_GCodePause()
{
    //m_auto->G_Code_Parser("Q103 ");
}
void cnc_basefunctions::send_GCodeContinue()
{
    //m_auto->G_Code_Parser("Q104 ");
}
void cnc_basefunctions::send_GCodeStop()
{
    m_auto->G_Code_Parser("Q14 ");
}
void cnc_basefunctions::send_XXX()
{
    m_auto->G_Code_Parser("XXX ");
}

void cnc_basefunctions::send_getPosition()
{
    m_auto->G_Code_Parser("M114 ");
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

    m_auto->G_Code_Parser(Line);
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
    m_auto->G_Code_Parser(Line);
}
void cnc_basefunctions::send_moveHome(){
    m_auto->G_Code_Parser("G28 ");
}


void cnc_basefunctions::send_setESuse(bool state){
    m_auto->G_Code_Parser("Q11 X"+QString::number(state));
}

void cnc_basefunctions::send_setMotorUse(bool state){
    m_auto->G_Code_Parser("Q12 X"+QString::number(state));
}

void cnc_basefunctions::send_resetWaitForHeat()
{
    m_auto->G_Code_Parser("Q13 ");
}

void  cnc_basefunctions::processLine(const QString &s)
{
    if(s.indexOf("request")!=-1 && m_database->m_G_Code_State == 1){
        //emit Log("get request");
        int start = s.indexOf("request ")+7;
        int end = s.length();
        int requests = s.mid(start,end-start).toFloat();
        QByteArray newBA;
        if(requests > 1){
            for(int i = 0;i < 1; i++){
                newBA = m_auto->GC_getNextLine();
                //emit Log("1 newLine: "+newLine);
                emit serial_send(newBA);
            }
        }
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

void cnc_basefunctions::requestNextLine(const QByteArray &in){
    emit Log("request");
    if(m_database->m_G_Code_State == 1){
        int requests;
        QByteArray newBA;
        for(int i=2;i<in.length();i+=5){
            if(in[i] == 1){
                requests = BtoF(in.mid(i+1,4));
            }
        }
        if(requests > 1){
            for(int i = 0;i < 1; i++){
                newBA = m_auto->GC_getNextLine();
                emit serial_send(newBA);
            }
        }
    }
}
void cnc_basefunctions::getCurrentPos(const QByteArray &in){
    emit Log("getPos");
    for(int i=2;i<in.length();i+=5){
        if(in[i] == 1){
            m_database->m_act_X = BtoF(in.mid(i+1,4));
        }
        if(in[i] == 2){
            m_database->m_act_Y = BtoF(in.mid(i+1,4));
        }
        if(in[i] == 3){
            m_database->m_act_Z = BtoF(in.mid(i+1,4));
        }
        if(in[i] == 4){
            m_database->m_act_E = BtoF(in.mid(i+1,4));
        }
        if(in[i] == 5){
            m_database->m_act_speed = BtoF(in.mid(i+1,4));
        }
    }
    emit show_speed();
    emit show_position();
}
void cnc_basefunctions::getTemperaturs(const QByteArray &in){
    emit Log("get temp");
    for(int i=2;i<in.length();i+=5){
        if(in[i] == 1){
            m_database->m_soll_temperatur = BtoF(in.mid(i+1,4));
        }
        if(in[i] == 2){
            m_database->m_act_temperatur = BtoF(in.mid(i+1,4));
        }
        if(in[i] == 3){
            m_database->m_soll_bedTemp = BtoF(in.mid(i+1,4));
        }
        if(in[i] == 4){
            m_database->m_act_bedTemp = BtoF(in.mid(i+1,4));
        }
    }
    emit show_temp();
}
void cnc_basefunctions::getEndswitches(const QByteArray &in){
    emit Log("getES");
    for(int i=2;i<in.length();i+=5){
        if(in[i] == 1){
            m_database->m_endswitch_X = BtoF(in.mid(i+1,4));
        }
        if(in[i] == 2){
            m_database->m_endswitch_Y = BtoF(in.mid(i+1,4));
        }
        if(in[i] == 3){
            m_database->m_endswitch_Z = BtoF(in.mid(i+1,4));
        }
    }
    emit show_endswitch();
}
void cnc_basefunctions::getHotendSoll(const QByteArray &in){
    for(int i=2;i<in.length();i+=5){
        if(in[i] == 5){
            m_database->m_soll_temperatur = BtoF(in.mid(i+1,4));
        }
    }
    emit show_act_temp();
}
void cnc_basefunctions::getHotendIst(const QByteArray &in){
    for(int i=2;i<in.length();i+=5){
        if(in[i] == 5){
            m_database->m_act_temperatur = BtoF(in.mid(i+1,4));
        }
    }
    emit show_act_temp();
}
void cnc_basefunctions::getBedSoll(const QByteArray &in){
    for(int i=2;i<in.length();i+=5){
        if(in[i] == 5){
            m_database->m_soll_bedTemp = BtoF(in.mid(i+1,4));
        }
    }
    emit show_act_temp();
}
void cnc_basefunctions::getBedIst(const QByteArray &in){
    for(int i=2;i<in.length();i+=5){
        if(in[i] == 5){
            m_database->m_act_bedTemp = BtoF(in.mid(i+1,4));
        }
    }
    emit show_act_temp();
}
void cnc_basefunctions::getMoveParams(const QByteArray &in){
    emit Log("getMoveParams");
    for(int i=2;i<in.length();i+=5){
        if(in[i] == 1){
            m_database->m_max_acc = BtoF(in.mid(i+1,4));
            emit Log("m_max_acc:"+QString::number(m_database->m_max_acc));
        }
        if(in[i] == 2){
            m_database->m_min_speed = BtoF(in.mid(i+1,4));
            emit Log("m_min_speed:"+QString::number(m_database->m_min_speed));
        }
        if(in[i] == 3){
            m_database->m_max_speed = BtoF(in.mid(i+1,4));
            emit Log("m_max_speed:"+QString::number(m_database->m_max_speed));
        }
        if(in[i] == 4){
            m_database->m_threshAngle = BtoF(in.mid(i+1,4));
            emit Log("m_threshAngle:"+QString::number(m_database->m_threshAngle));
        }
        if(in[i] == 5){
            m_database->m_act_filament = BtoF(in.mid(i+1,4));
            emit Log("m_act_filament:"+QString::number(m_database->m_act_filament));
        }
    }
    emit show_acc_speed_fila();
}
void cnc_basefunctions::getEsUse(const QByteArray &in){
    for(int i=2;i<in.length();i+=5){
        if(in[i] == 1){
            m_database->setUseEs(BtoF(in.mid(i+1,4)));
            emit Log("getEsUse:"+QString::number(BtoF(in.mid(i+1,4))));
        }
    }
}
void cnc_basefunctions::getMotorState(const QByteArray &in){
    emit Log("getMotorState");
    for(int i=2;i<in.length();i+=5){
        if(in[i] == 1){
            m_database->setMotorUse(BtoF(in.mid(i+1,4)));
        }
    }
}
void cnc_basefunctions::getHeaterState(const QByteArray &in){
    emit Log("getHeatingState");
    for(int i=2;i<in.length();i+=5){
        if(in[i] == 1){
            emit show_waitForHeat(BtoF(in.mid(i+1,4)));
        }
    }
}
void cnc_basefunctions::processBytes(const QByteArray &in){
    //emit Log("length "+QString::number(in[0])+" "+QString::number(in.length()));
    //char length = in[0];
    //    for(int i=0;i< in.length();i++) {
    //        emit Log("get:"+QString::number(i)+":"+QString::number(in.at(i))+":"+QString(in.at(i)));
    //    }
    union FB{
        float f;
        char b[4];
    }u;
    char command = in[1];
    switch(command){
    case 30:
        requestNextLine(in);
        break;
    case 11:
        getTemperaturs(in);
        break;
    case 12:
        getEndswitches(in);
        break;
    case 13://M104
        getHotendSoll(in);
        break;
    case 14://M109 wait to reach temp
        getHotendIst(in);
        break;
    case 15://M140
        getBedSoll(in);
        break;
    case 16://M190 wait to reach temp
        getBedIst(in);
        break;
    case 17://Q10 move params
        getMoveParams(in);
        break;
    case 18:
        getEsUse(in);
        break;
    case 19:
        getMotorState(in);
        break;
    case 20:
        getHeaterState(in);
        break;
    case 24:
        getCurrentPos(in);
        break;
    case 61://debug text
        emit Log("DebugMSG:"+in.mid(2));
        break;
    case 62://debug value
        u.b[0] = in[in.length()-4];
        u.b[1] = in[in.length()-3];
        u.b[2] = in[in.length()-2];
        u.b[3] = in[in.length()-1];
        emit Log("Value:"+in.mid(2,in.length()-6)+" = "+QString::number(u.f));
        break;
    case 63://error text
        emit errorLog("ErrorMSG:"+in.mid(2));
        break;
    case 64://error value
        u.b[0] = in[in.length()-4];
        u.b[1] = in[in.length()-3];
        u.b[2] = in[in.length()-2];
        u.b[3] = in[in.length()-1];
        emit errorLog("Value:"+in.mid(2,in.length()-6)+" = "+QString::number(u.f));
        break;
    default:
        emit errorLog("Find no matching command: "+ QString::number(command)+" len:"+QString::number(in[0]));
    }



}

float cnc_basefunctions::BtoF(QByteArray a){
    union FB{
      float f;
      char b[4];
    }u;
    u.b[0] = a[0];
    u.b[1] = a[1];
    u.b[2] = a[2];
    u.b[3] = a[3];
    return u.f;
}



