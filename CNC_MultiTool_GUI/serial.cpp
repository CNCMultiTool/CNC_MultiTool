#include "serial.h"


Serial::Serial(cnc_data *database)
{
    m_database = database;

}


Serial::~Serial()
{

}

bool Serial::serial_open()
{
    disconnect(&m_serial,SIGNAL(readyRead()),this,SLOT(serial_read()));
    m_database->SerialLog("serialOpen");
    m_serial.setPortName(m_database->m_SerialPortName);
    m_serial.setParity(QSerialPort::Parity::NoParity);
    m_serial.setDataBits(QSerialPort::Data8);
    m_serial.setStopBits(QSerialPort::TwoStop);
    m_serial.setBaudRate(QSerialPort::Baud115200);
    if (!m_serial.open(QIODevice::ReadWrite)) {
        emit errorLog("can`t start Serial");
        m_database->SerialLog("serialOpen can`t start Serial");
        return false;
    }
    m_recivedBytes.clear();
    m_serial.clear();
    m_database->set_serial(true);
    connect(&m_serial,SIGNAL(readyRead()),this,SLOT(serial_read()));
    return true;
}

void Serial::serial_close()
{
    disconnect(&m_serial,SIGNAL(readyRead()),this,SLOT(serial_read()));
    m_database->SerialLog("serialClose");
    m_database->set_serial(false);
    m_serial.close();
}




/**
 * liest daten vom serielen port
 * chekct ob ein telegramm empfangen
 * konvertiert das telegramm von bytes in comando und werte
 */
void Serial::serial_read()
{
    m_recivedBytes += m_serial.readAll();
    m_serial.waitForReadyRead(m_recive_timeout);

    int command_end = m_recivedBytes.indexOf("\n");
    if(command_end>0)
    {
        QByteArray helper = m_recivedBytes;
        emit Log(QString(helper.remove(command_end+1,helper.length())));
        m_recivedBytes.remove(0,command_end+1);
    }
}


/**
 * holt das nächste kommando aus der queue
 * check ob das command valide ist
 * verpackt es in ein bytearray mit startcodon und checsumm
 * sendet es mittels serielem port
 */
void Serial::serial_send(QString mes)
{
    if(!m_serial.isOpen())
    {
        emit Log("Serial not open");
        emit errorLog("Serial not open");
        m_database->SerialLog("send Serial not open");
        return;
    }
    emit Log(mes);
    mes += QString("\n").toUtf8();
    m_serial.write(mes.toUtf8());
    m_serial.waitForBytesWritten(m_send_timeout);

}
