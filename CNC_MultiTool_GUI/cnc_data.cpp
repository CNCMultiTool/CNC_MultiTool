#include "cnc_data.h"

cnc_data::cnc_data()
{
    m_size_X = 269.89;
    m_size_Y = 231.68;

    m_X_angel = -0.00155619;
    m_Y_angel = 0.0023308;

    m_soll_speed = 0;
    m_soll_filament = 0;
    m_soll_temperatur = 0;

    //values recived
    //position
    m_act_X = 0;
    m_act_Y = 0;
    m_act_Z = 0;
    m_act_W = 0;
    //settings
    m_act_speed = 0;
    m_act_filament = 0;
    m_act_temperatur = 0;

    //status
    m_endswitch_X = 0;
    m_endswitch_Y = 0;
    m_endswitch_Z = 0;

    //kalibration results
    m_error_X_max_Y_null = 0;
    m_error_X_max_Y_max = 0;
    m_error_X_null_Y_max = 0;
    m_error_X_null_Y_null = 0;

    m_Zmax_nozzel = 170.8;

    m_HWisHeating = false;
    m_HWisMoving = false;

    m_SerialIsOpen = false;
    m_Serial_quit = true;

    m_LogFile = new QFile;
    m_LogFile->setFileName(m_LogFileName);

    const QMutexLocker file_locker(&m_file_mutex);
}

cnc_data::~cnc_data()
{
    m_LogFile->close();
}

void cnc_data::FileLog(QString value)
{
    m_file_mutex.lock();
    m_LogFile->open(QIODevice::Append | QIODevice::Text);
    value = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.ms ") + value +"\n";
    QTextStream out(m_LogFile);
    out.setCodec("UTF-8");
    if (m_LogFile != 0) {
      out << value;
    }
    m_LogFile->close();
    m_file_mutex.unlock();
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

void cnc_data::set_settings(float speed,float temperatur,float filament,float output)
{
    m_act_speed = speed;
    m_act_temperatur = temperatur;
    m_act_filament = filament;
    m_output = output;
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
