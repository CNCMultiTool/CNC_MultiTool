#include "cnc_automation.h"

CNC_automation::CNC_automation(cnc_data *database,cnc_basefunctions *basefunctions)
{
    m_database = database;
    m_basefunctions = basefunctions;
}

CNC_automation::~CNC_automation()
{

}

void CNC_automation::move_home()
{
    m_basefunctions->send_settings(50,m_database->m_soll_temperatur,m_database->m_soll_filament);
    m_basefunctions->move_wait(m_database->m_act_X,m_database->m_act_Y,9999,m_database->m_act_W);
    m_basefunctions->move_wait(-9999,-9999,m_database->m_act_Z,m_database->m_act_W);
}

void CNC_automation::calib_size()
{
    //m_AutoFunctions.calib_size();
}

void CNC_automation::repeat_test()
{
    //m_AutoFunctions.repeat_test();
}

void CNC_automation::Z_calib()
{
    //m_AutoFunctions.Z_calib();
}


void CNC_automation::G_Code_Start(QString fileName)
{
    Log("start g-code: "+fileName);
    //m_AutoFunctions.G_Code_Start(fileName);
}

void CNC_automation::G_Code_Pause()
{
    //m_AutoFunctions.G_Code_Pause();
}

void CNC_automation::G_Code_Stop()
{
    //m_AutoFunctions.G_Code_Stop();
}


void CNC_automation::calc_correctionangel(QList<float> Z_errors)
{
    m_X_angel = atan(Z_errors[0]/m_size_X);
    m_Y_angel = atan(Z_errors[2]/m_size_Y);
    //a = math.tan(alpha)*b
    float Xcorr = m_size_X * tan(m_X_angel);
    float Ycorr = m_size_Y * tan(m_Y_angel);
    emit Log("messuring error at end:"+QString::number(Z_errors[3]));
    emit Log("X_angel:"+QString::number(m_X_angel)+" Y_angel:"+QString::number(m_Y_angel));
    emit Log("Correction error:"+QString::number(Z_errors[1]-(Xcorr+Ycorr)));
}

//correction fallut
float CNC_automation::calc_correction(float X,float Y)
{
    float Xcorr = X * tan(m_Y_angel);
    float Ycorr = Y * tan(m_X_angel);
    return(Xcorr+Ycorr);
}

