#include "cnc_data.h"

cnc_data::cnc_data()
{


    m_soll_speed = 0;
    m_soll_filament = 30;
    m_soll_temperatur = 0;

    //values recived
    //position
    m_act_X = 0;
    m_act_Y = 0;
    m_act_Z = 0;
    m_act_W = 0;
    //settings
    m_act_speed = 0;
    m_act_filament = m_soll_filament;
    m_act_temperatur = 0;

    //status
    m_endswitch_X = 0;
    m_endswitch_Y = 0;
    m_endswitch_Z = 0;

    m_size_X = 269.89;
    m_size_Y = 231.68;
    m_Zmax_nozzel = 170.95;

    m_X_angel = -0.00155619;
    m_Y_angel = 0.0023308;

    //kalibration results
    m_error_X_max_Y_null = 0;
    m_error_X_max_Y_max = 0;
    m_error_X_null_Y_max = 0;
    m_error_X_null_Y_null = 0;

    m_max_speed = 50;

    m_HWisHeating = false;
    m_HWisMoving = false;

    m_SerialIsOpen = false;

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
    value = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.ms ") + value +"\n";
    QTextStream out(m_LogFile);
    out.setCodec("UTF-8");
    if (m_LogFile != 0) {
      out << value;
    }
    m_LogFile->close();
    m_mutex.unlock();
}

void cnc_data::test()
{
    FileLog("test Log File in QT");
    emit Log("database is alive");
}

void cnc_data::set_position(float X,float Y,float Z,float W)
{
    m_act_X = X;
    m_act_Y = Y;
    m_act_Z = Z;
    m_act_W = W;
    emit show_position();
}

void cnc_data::set_settings(float speed,float temperatur,float filament,float soll_temperatur)
{
    m_act_speed = speed;
    m_act_temperatur = temperatur;
    m_act_filament = filament;
    m_soll_temperatur = soll_temperatur;
    emit show_settings();
}

void cnc_data::set_soll_settings(float speed,float temperatur,float filament)
{
    m_soll_speed = speed;
    m_soll_temperatur = temperatur;
    m_soll_filament = filament;
    emit show_settings();
}

void cnc_data::set_endswitch(int X,int Y,int Z)
{
    m_endswitch_X = X;
    m_endswitch_Y = Y;
    m_endswitch_Z = Z;
    emit show_endswitch(X,Y,Z);
}

void cnc_data::set_serial(bool isOpen)
{
    m_SerialIsOpen = isOpen;
    emit show_serial(isOpen);
}

void cnc_data::set_HWisHeating(bool status)
{
    m_HWisHeating = status;
    emit show_status();
}

void cnc_data::set_HWisMoving(bool status)
{
    m_HWisMoving = status;
    emit show_status();
}

void cnc_data::append_recive_command(cnc_command new_command)
{
    cnc_recive_commands.append(new_command);
    emit recive_command();
}

void cnc_data::loadSettings()
{
    QSettings settings("CNC_settings.ini", QSettings::IniFormat);
    m_size_X = settings.value("m_size_X").toDouble();
    m_size_Y = settings.value("m_size_Y").toDouble();
    m_Zmax_nozzel = settings.value("m_Zmax_nozzel").toDouble();

    m_X_angel = settings.value("m_X_angel").toDouble();
    m_Y_angel = settings.value("m_Y_angel").toDouble();

    //kalibration results
    m_error_X_max_Y_null = settings.value("m_error_X_max_Y_null").toDouble();
    m_error_X_max_Y_max = settings.value("m_error_X_max_Y_max").toDouble();
    m_error_X_null_Y_max = settings.value("m_error_X_null_Y_max").toDouble();
    m_error_X_null_Y_null = settings.value("m_error_X_null_Y_null").toDouble();

    m_max_speed = settings.value("m_max_speed").toDouble();

}

void cnc_data::saveSettings()
{
    QSettings settings("CNC_settings.ini", QSettings::IniFormat);
    settings.setValue("m_size_X",m_size_X);
    settings.setValue("m_size_Y",m_size_Y);
    settings.setValue("m_Zmax_nozzel",m_Zmax_nozzel);

    settings.setValue("m_X_angel",m_X_angel);
    settings.setValue("m_Y_angel",m_Y_angel);

    //kalibration results
    settings.setValue("m_error_X_max_Y_null",m_error_X_max_Y_null);
    settings.setValue("m_error_X_max_Y_max",m_error_X_max_Y_max);
    settings.setValue("m_error_X_null_Y_max", m_error_X_null_Y_max);
    settings.setValue("m_error_X_null_Y_null",m_error_X_null_Y_null);

    settings.setValue("m_max_speed",m_max_speed);
}
