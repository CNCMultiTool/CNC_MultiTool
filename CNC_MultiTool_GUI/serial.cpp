#include "serial.h"


Serial::Serial(cnc_data *database)
{
    m_database = database;
}


Serial::~Serial()
{

}

void Serial::serial_open()
{
    m_serial.setPortName(m_database->m_SerialPortName);
    m_serial.setParity(QSerialPort::Parity::EvenParity);
    m_serial.setDataBits(QSerialPort::Data8);
    m_serial.setStopBits(QSerialPort::OneStop);
    m_serial.setBaudRate(QSerialPort::Baud115200);
    if (!m_serial.open(QIODevice::ReadWrite)) {
        emit errorLog("can`t start Serial");
        return;
    }
    m_serial.clear();
    m_database->set_serial(true);
    connect(&m_serial,SIGNAL(readyRead()),this,SLOT(serial_read_command()));
}

void Serial::serial_close()
{
    emit Log("serial_close");
    m_database->set_serial(false);
    m_serial.close();
}

/**
 * berechnet die checksumm aus dem gebenen bytarray
 */
int Serial::serial_calcCheckSumm(QByteArray bytes,unsigned char *Checksumm)
{
    if(bytes.length() < m_TelegramLength-1)
        return -1;

    *Checksumm = 0;
    for(int i=0;i<m_TelegramLength-1;i++)
    {
        *Checksumm += bytes[i];
    }
    return 0;
}

/**
 * prüft ob das empfangene telegramm correkt ist
 * länge
 * checksumm
 * startcodon
 */
int Serial::serial_CheckTelegram()
{
    //check if telegram is long enought
    if(m_recivedBytes.size()<m_TelegramLength){
        emit errorLog("serial: not enough bytes");
        return -1;
    }
    //check if start index is presend
    if(m_recivedBytes[0] != 'S'){
        emit errorLog("serial: not Startcodon");
        return -2;
    }
    //calculate Checksam and check if valid
    unsigned char checksumm;
    if(serial_calcCheckSumm(m_recivedBytes,&checksumm)<0){
        emit errorLog("serial: error in calcCheckSumm");
        return -3;
    }
    if(false)//checksumm != m_recivedBytes[m_TelegramLength-1]){
    {
        emit errorLog("serial: wrong checksumm (calc: "+
                      QString::number(checksumm)+
                      ", recive: "+m_recivedBytes[m_TelegramLength-1]+
                        ") "+"total length: "+
                        QString::number(m_recivedBytes.size()));
        QString debug_text = "";
        for(int i=0;i<m_recivedBytes.size();i++)
        {
            debug_text += m_recivedBytes[i];
        }
        emit Log(debug_text);
        m_recivedBytes.remove(0,m_recivedBytes.size());

        return -4;
    }
    return 0;
}

/**
 * liest daten vom serielen port
 * chekct ob ein telegramm empfangen
 * konvertiert das telegramm von bytes in comando und werte
 */
void Serial::serial_read_command()
{
    char command = ' ';
    unsigned char checkSumm = 0;

    m_recivedBytes += m_serial.readAll();
    m_serial.waitForReadyRead(10);

    if(serial_CheckTelegram()<0)
        return;

    command = char(m_recivedBytes[1]);
    for(int i=0;i<m_TelegramLength-2;i++)
    {
        m_recive_telegram.Bytes[i] = m_recivedBytes[i+2];
    }
    checkSumm = m_recivedBytes[m_TelegramLength-1];

    QString LogText = QString(char(m_recivedBytes[0]))+" ";
    LogText += QString(command)+" ";
    LogText += QString::number(m_recive_telegram.Value[0])+" ";
    LogText += QString::number(m_recive_telegram.Value[1])+" ";
    LogText += QString::number(m_recive_telegram.Value[2])+" ";
    LogText += QString::number(m_recive_telegram.Value[3])+" ";
    LogText += QString::number(checkSumm);
    m_database->FileLog("INFO recive:"+LogText);
    emit Log("INFO recive:"+LogText);

    //empfangene daten in cnc_command verpacken und an empfangs queue packen
    cnc_command new_command;
    new_command.command = command;
    new_command.value1 = m_recive_telegram.Value[0];
    new_command.value2 = m_recive_telegram.Value[1];
    new_command.value3 = m_recive_telegram.Value[2];
    new_command.value4 = m_recive_telegram.Value[3];
    m_database->append_command(new_command);
    //entvernen des gelesenene telegramms
    m_recivedBytes.remove(0,m_TelegramLength);
}

/**
 * checkt ob ein sende corgang gestartet werden kann
 */
int Serial::serial_send_ifValid()
{
    if(m_database->cnc_send_commands.length()<1)
        return -1;
    return 0;
}

/**
 * holt dsas nächste kommando aus der queue
 * check ob das command valide ist
 * verpackt es in ein bytearray mit startcodon und checsumm
 * sendet es mittels serielem port
 */
void Serial::serial_send_command()
{
    if(serial_send_ifValid()>0){
        emit errorLog("serial send: no command to send");
        return;
    }

    // read command and values to send
    cnc_command new_command = m_database->cnc_send_commands[0];
    m_database->cnc_send_commands.pop_front();

    unsigned char newCheckSumm = 0;
    m_sendBytes = QString("S").toUtf8();
    m_sendBytes += QString(new_command.command).toUtf8();
    tTelegram send_telegram;
    send_telegram.Value[0] = new_command.value1;
    send_telegram.Value[1] = new_command.value2;
    send_telegram.Value[2] = new_command.value3;
    send_telegram.Value[3] = new_command.value4;
    for(int i=0;i<16;i++)
    {
        m_sendBytes += send_telegram.Bytes[i];
    }
    newCheckSumm = 0;
    serial_calcCheckSumm(m_sendBytes,&newCheckSumm);
    m_sendBytes += newCheckSumm;

    m_serial.write(m_sendBytes);
    m_serial.waitForBytesWritten(10);

    QString LogText = QString(char(m_sendBytes[0]))+" ";
    LogText += QString(char(m_sendBytes[1]))+" ";
    LogText += QString::number(send_telegram.Value[0])+" ";
    LogText += QString::number(send_telegram.Value[1])+" ";
    LogText += QString::number(send_telegram.Value[2])+" ";
    LogText += QString::number(send_telegram.Value[3])+" ";
    LogText += QString::number(m_sendBytes[18]);
    emit Log("INFO send:"+LogText);
    m_database->FileLog("INFO send:"+LogText);
}
