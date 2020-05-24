#include "serial.h"


Serial::Serial(cnc_data *database)
{
    m_database = database;
    connect(&serial_timeout, SIGNAL(timeout()), this, SLOT(serial_timeout_handler()));
    connect(&serial_fast_timeout, SIGNAL(timeout()), this, SLOT(serial_fasttimeout_handler()));
}


Serial::~Serial()
{
    serial_timeout.stop();
    serial_fast_timeout.stop();
}

void Serial::send_last()
{
    m_database->SerialLog("SendLast");
    if(!m_sendBytesLast.isEmpty()&&m_serial.isOpen())
    {
        m_database->SerialLog("SendLast send");
        emit Log("send last");
        //cnc_command dummy;
        //m_database->cnc_send_commands.append(dummy);
        m_serial.write(m_sendBytesLast);
        m_serial.waitForBytesWritten(m_send_timeout);
        debug_time.restart();
    }
}

void Serial::stop_timeouts()
{
    serial_timeout.stop();
    serial_fast_timeout.stop();
}

void Serial::serial_timeout_handler()
{
    stop_timeouts();
    m_database->SerialLog("Serial TimeoutHandler called after: " +QString::number(debug_time.elapsed())+" "+QString(m_recivedBytes));
    emit errorLog("Serial TimeoutHandler called after: " +QString::number(debug_time.elapsed())+" "+QString(m_recivedBytes));
    emit Log("Serial TimeoutHandler called after: " +QString::number(debug_time.elapsed()));
        //m_recivedBytes.clear();
        //m_sendBytes.clear();
    if(m_serial.isOpen())
    {
        m_serial.clearError();
        m_serial.flush();
        m_serial.clear();
        serial_close();
        serial_open();
        send_last();
        //serial_timeout.start(3*m_timeout);
    }else{
        m_database->SerialLog("Serial TimeoutHandler fasttimeout while closed");
    }
}

void Serial::serial_fasttimeout_handler()
{
    m_database->SerialLog("Serial FastTimeoutHandler called after: " +QString::number(fast_Timeout_time.elapsed())+" "+QString(m_recivedBytes));
    emit errorLog("Serial FastTimeoutHandler called after: " +QString::number(fast_Timeout_time.elapsed())+" "+QString(m_recivedBytes));
    emit Log("Serial FastTimeoutHandler called after: " +QString::number(fast_Timeout_time.elapsed()));
    if(m_serial.isOpen())
    {
        serial_close();
        serial_open();
        send_last();
        //serial_fast_timeout.start(3*m_fast_timeout);
    }else{
        emit Log("fasttimeout while closed");
        m_database->SerialLog("Serial FastTimeoutHandler fasttimeout while closed");
    }
    serial_fast_timeout.start(m_fast_timeout*3);
}

bool Serial::serial_open()
{
    disconnect(&m_serial,SIGNAL(readyRead()),this,SLOT(serial_read_command()));
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
    connect(&m_serial,SIGNAL(readyRead()),this,SLOT(serial_read_command()));
    //m_database->SerialLog("serialOpen start Serial");
    return true;
}

void Serial::serial_close()
{
    disconnect(&m_serial,SIGNAL(readyRead()),this,SLOT(serial_read_command()));
    stop_timeouts();
    //emit Log("serial_close");
    m_database->SerialLog("serialClose");
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
    if(m_recivedBytes.size()<1)
    {
        //m_database->SerialLog("recive telCheak no bytes to check");
        return -5;
    }
    if(m_recivedBytes[0] == 'Q')//std alive and idle signal
    {
        return 0;
    }
    if(m_recivedBytes[0] == 'W')//alive and working signal
    {
        return 0;
    }
    if(m_recivedBytes[0] == 'N')//repeat request
    {
        return 0;
    }
    if(m_recivedBytes[0] == 'T')//check kommand
    {
        return 0;
    }

    //check if start index is presend
    if(m_recivedBytes[0] != 'S'){
        emit errorLog("serial: not Startcodon");
        m_database->SerialLog("recive telCheak not Startcodon");
        m_recivedBytes.remove(0,1);
        return -2;
    }

    if(m_recivedBytes.size()<m_TelegramLength)
    {
        return -1;
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
    for(int i = 0;i<20;i++)
    {
        //emit Log("in recive loop");
        char command = ' ';
        unsigned char checkSumm = 0;

        m_recivedBytes += m_serial.readAll();
        m_serial.waitForReadyRead(m_recive_timeout);

        //emit errorLog(QString(m_recivedBytes));

        if(serial_CheckTelegram()<0)
            return;

        if(m_recivedBytes[0] == 'T'){
            m_sendBytesLast.clear();
            serial_timeout.stop();
            m_recivedBytes.remove(0,1);
            debug_time.elapsed();
            if(!m_database->cnc_send_commands.isEmpty())
            {
                m_database->cnc_send_commands.pop_front();
            }
            emit show_send_queue();
            continue;
        }

        if(m_recivedBytes[0] == 'Q'){
            //emit Log("HW IDLE HW_status:"+QString::number(m_database->m_HW_status)+
                     //" G-Code:"+QString::number(m_database->m_G_Code_State)+
                     //" Queue:"+QString::number(m_database->cnc_send_commands.size())+
                     //" LastSendEmpty:"+QString::number(!m_sendBytesLast.isEmpty()));
            serial_fast_timeout.stop();
            serial_fast_timeout.start(m_fast_timeout);
            m_recivedBytes.remove(0,1);
            fast_Timeout_time.restart();

            if(m_database->m_HW_status == 1 && m_database->m_G_Code_State == 1)
                m_Q_count--;
            //if(m_database->m_HW_status == 0 && m_database->cnc_send_commands.size()>0 && m_database->m_G_Code_State == 1)
                //m_Q_count--;

            if(m_Q_count<=0)
            {
                m_Q_count = 5;
                if(m_database->m_HW_status == 1 && m_database->m_G_Code_State == 1)
                {
                    emit Log("request last send from arduino");
                    m_database->SerialLog("request last send from arduino");
                    m_serial.write(QString("N").toUtf8());
                    m_serial.waitForBytesWritten(m_send_timeout);
                }
            }
            if(m_database->m_HW_status == 0 && m_database->cnc_send_commands.size()>0 && m_database->m_G_Code_State == 1)
            {
                emit Log("send new comand while idle");
                m_database->SerialLog("send new comand while idle");
                serial_send_command();
            }
            continue;
        }

        if(m_recivedBytes[0] == 'W'){
            m_Q_count = 5;
            //emit Log("HW WORKING HW_status:"+QString::number(m_database->m_HW_status)+
                     //" G-Code:"+QString::number(m_database->m_G_Code_State)+
                     //" Queue:"+QString::number(m_database->cnc_send_commands.size())+
                     //" LastSendEmpty:"+QString::number(!m_sendBytesLast.isEmpty()));
            serial_fast_timeout.stop();
            serial_fast_timeout.start(m_fast_timeout);
            m_recivedBytes.remove(0,1);
            fast_Timeout_time.restart();
            continue;
        }

        if(m_recivedBytes[0] == 'N'){
            m_database->SerialLog("recive N repeatrequest");
            emit errorLog("recive N repeatrequest");
            emit Log("recive N repeatrequest");
            serial_fast_timeout.stop();
            serial_fast_timeout.start(m_fast_timeout);
            m_recivedBytes.remove(0,1);
            //send_last();
            if(m_database->m_HW_status != 2)
            {
                m_serial.write(m_sendBytesLastSave);
                m_serial.waitForBytesWritten(m_send_timeout);
            }
            continue;
        }


        QByteArray respose = QString("T").toUtf8();
        m_serial.write(respose);
        m_serial.waitForBytesWritten(m_send_timeout);
        //m_database->SerialLog("recive send T");

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
        //m_database->SerialLog("recive S telegram "+LogText);
        emit Log("INFO recive:"+LogText);


        //empfangene daten in cnc_command verpacken und an empfangs queue packen
        cnc_command new_command;
        new_command.command = command;
        new_command.value1 = m_recive_telegram.Value[0];
        new_command.value2 = m_recive_telegram.Value[1];
        new_command.value3 = m_recive_telegram.Value[2];
        new_command.value4 = m_recive_telegram.Value[3];

        char NewCheckSumm = 'S';
        NewCheckSumm += command;
        for(int i=0;i<16;i++)
        {
            //m_database->SerialLog("calc "+QString::number(NewCheckSumm)+" to add "+QString::number(m_recivedBytes[i]));
            NewCheckSumm += m_recive_telegram.Bytes[i];
        }

        if(NewCheckSumm != m_recivedBytes[18])
        {
            m_database->SerialLog("recive telCheak wrong checksumm (calc: "+
                                  QString::number(NewCheckSumm)+
                                  ", recive: "+QString::number(m_recivedBytes[18])+")");
            m_recivedBytes.remove(0,m_TelegramLength);
            m_database->SerialLog("recive telCheak checksumm error");
            continue;
        }


        m_database->append_recive_command(new_command);
        //entvernen des gelesenene telegramms
        m_recivedBytes.remove(0,m_TelegramLength);
    }
    //emit Log("exit recive loop");
}


/**
 * holt das nächste kommando aus der queue
 * check ob das command valide ist
 * verpackt es in ein bytearray mit startcodon und checsumm
 * sendet es mittels serielem port
 */
void Serial::serial_send_command()
{
    if(!m_serial.isOpen())
    {
        emit Log("Serial not open");
        emit errorLog("Serial not open");
        m_database->SerialLog("send Serial not open");
        return;
    }
    if(!(m_database->cnc_send_commands.size()>0)){
        emit errorLog("serial send: no command to send");
        m_database->SerialLog("send no command to send");
        return;
    }

    // read command and values to send
    cnc_command new_command = m_database->cnc_send_commands[0];
    //m_database->cnc_send_commands.pop_front();
    //emit show_send_queue();

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
    if(new_command.command != 'b')
        m_sendBytesLast = m_sendBytes;//save last sended
        m_sendBytesLastSave = m_sendBytes;

    m_serial.write(m_sendBytes);
    m_serial.waitForBytesWritten(m_send_timeout);
    //emit Log("Serial timeout startet");
    serial_timeout.start(m_timeout);
    debug_time.restart();


    QString LogText = QString(char(m_sendBytes[0]))+" ";
    LogText += QString(char(m_sendBytes[1]))+" ";
    LogText += QString::number(send_telegram.Value[0])+" ";
    LogText += QString::number(send_telegram.Value[1])+" ";
    LogText += QString::number(send_telegram.Value[2])+" ";
    LogText += QString::number(send_telegram.Value[3])+" ";
    LogText += QString::number(m_sendBytes[18]);
    emit Log("INFO send:"+LogText);
    //m_database->SerialLog("send send telegram"+LogText);


}
