#include "cnc_data.h"

cnc_data::cnc_data()
{
    m_soll_speed = 0;
    m_soll_filament = 37;
    m_soll_temperatur = 0;
    m_soll_bedTemp = 0;
    //values recived
    //position
    m_act_X = 0;
    m_act_Y = 0;
    m_act_Z = 0;
    m_act_E = 0;
    //settings
    m_act_speed = 0;
    m_act_filament = m_soll_filament;
    m_act_temperatur = 0;
    m_act_bedTemp = 0;

    //status
    m_endswitch_X = 0;
    m_endswitch_Y = 0;
    m_endswitch_Z = 0;

    m_Z_inHome = 170.95;
    m_calibplateX = 0;
    m_calibplateY = 127.0;
    m_calibplateZ =  1;

    m_z_offset = 0;

    m_useES = true;
    m_motorUse = true;
    m_waitForHeat = false;

    m_max_speed = 25;
    m_max_acc = 25;
    m_min_speed = 5;

    m_plot_size = 250;

    m_LogFile = new QFile;
    m_LogFile->setFileName(m_LogFileName);

    const QMutexLocker locker(&m_mutex);
}

cnc_data::~cnc_data()
{
    m_LogFile->close();
}

void cnc_data::FileLog(QString value)
{
    m_mutex.lock();
    m_LogFile->open(QIODevice::Append | QIODevice::Text);
    value = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz ") + value +"\n";
    QTextStream out(m_LogFile);
    out.setCodec("UTF-8");
    if (m_LogFile != 0) {
      out << value;
    }
    m_LogFile->close();
    m_mutex.unlock();
}

void cnc_data::set_position(float X,float Y,float Z,float E)
{
    m_act_X = X;
    m_act_Y = Y;
    m_act_Z = Z;
    m_act_E = E;
    emit show_position();
}

void cnc_data::set_settings(float speed,float temperatur,float filament,float bed_temp)
{
    m_act_speed = speed;
    m_act_temperatur = temperatur;
    m_act_filament = filament;
    m_act_bedTemp = bed_temp;
    emit show_settings();
}

void cnc_data::set_endswitch(int X,int Y,int Z)
{
    m_endswitch_X = X;
    m_endswitch_Y = Y;
    m_endswitch_Z = Z;
    emit show_endswitch(X,Y,Z);
}

void cnc_data::setUseEs(bool state)
{
    m_useES = state;
    emit show_useES(state);

}
bool cnc_data::getUseEs()
{
    return m_useES;
}
void cnc_data::setMotorUse(bool state)
{
    m_motorUse = state;
    emit show_motorUse(state);

}
bool cnc_data::getMotorUse()
{
    return m_motorUse;
}

void cnc_data::loadSettings()
{
    QSettings settings("CNC_settings.ini", QSettings::IniFormat);

    m_Z_inHome = settings.value("m_Zmax_nozzel").toDouble();
    m_X_inHome = settings.value("m_X_inHome").toDouble();
    m_Y_inHome = settings.value("m_Y_inHome").toDouble();

    m_max_speed = settings.value("m_max_speed").toDouble();
    m_min_speed = settings.value("m_min_speed").toDouble();
    m_max_acc = settings.value("m_max_acc").toDouble();
    m_soll_speed = settings.value("m_default_speed").toDouble();
    m_soll_filament = settings.value("m_default_filament").toDouble();
    m_threshAngle = settings.value("m_threshAngle").toDouble();

    m_calibplateX = settings.value("m_calibplateX").toDouble();
    m_calibplateY = settings.value("m_calibplateY").toDouble();
    m_calibplateZ =  settings.value("m_calibplateZ").toDouble();
    m_useCalibPlate = settings.value("m_useCalibPlate").toBool();

    m_soll_speed = settings.value("m_soll_speed").toDouble();
    m_soll_temperatur = settings.value("m_soll_temperatur").toDouble();
    m_soll_filament = settings.value("m_soll_filament").toDouble();
    m_soll_bedTemp = settings.value("m_soll_bedTemp").toDouble();

    m_R_vor_Bed = settings.value("m_R_vor_Bed").toDouble();
    m_R_nen_Bed = settings.value("m_R_nen_Bed").toDouble();
    m_bValue_Bed = settings.value("m_bValue_Bed").toDouble();

    m_KP = settings.value("m_KP").toDouble();
    m_KI = settings.value("m_KI").toDouble();
    m_KD = settings.value("m_KD").toDouble();
    m_POn = settings.value("m_POn").toBool();

    m_R_vor = settings.value("m_R_vor").toDouble();
    m_R_nen = settings.value("m_R_nen").toDouble();
    m_bValue = settings.value("m_bValue").toDouble();
    m_plot_size = settings.value("m_plot_size").toDouble();
}

void cnc_data::saveSettings()
{
    QSettings settings("CNC_settings.ini", QSettings::IniFormat);

    settings.setValue("m_Zmax_nozzel",m_Z_inHome);
    settings.setValue("m_X_inHome",m_X_inHome);
    settings.setValue("m_Y_inHome",m_Y_inHome);

    settings.setValue("m_max_speed",m_max_speed);
    settings.setValue("m_min_speed",m_min_speed);
    settings.setValue("m_default_speed",m_soll_speed);
    settings.setValue("m_max_acc",m_max_acc);
    settings.setValue("m_default_filament",m_soll_filament);
    settings.setValue("m_threshAngle",m_threshAngle);


    settings.setValue("m_calibplateX",m_calibplateX);
    settings.setValue("m_calibplateY",m_calibplateY);
    settings.setValue("m_calibplateZ",m_calibplateZ);
    settings.setValue("m_useCalibPlate",m_useCalibPlate);

    settings.setValue("m_soll_speed",m_soll_speed);
    settings.setValue("m_soll_temperatur",m_soll_temperatur);
    settings.setValue("m_soll_filament",m_soll_filament);
    settings.setValue("m_soll_bedTemp",m_soll_bedTemp);

    settings.setValue("m_R_vor_Bed",m_R_vor_Bed);
    settings.setValue("m_R_nen_Bed",m_R_nen_Bed);
    settings.setValue("m_bValue_Bed",m_bValue_Bed);

    settings.setValue("m_KP",m_KP);
    settings.setValue("m_KI",m_KI);
    settings.setValue("m_KD",m_KD);
    settings.setValue("m_POn",m_POn);

    settings.setValue("m_R_vor",m_R_vor);
    settings.setValue("m_R_nen",m_R_nen);
    settings.setValue("m_bValue",m_bValue);
    settings.setValue("m_plot_size",m_plot_size);
}
