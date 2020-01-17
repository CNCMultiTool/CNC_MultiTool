#include "serial.h"


Serial::Serial(cnc_data *database)
{
    m_database = database;
    const QMutexLocker locker(&m_mutex);
    m_quit = true;
    m_dataReadyToSend = false;
}

Serial::~Serial()
{
    m_mutex.lock();
    m_quit = true;
    m_cond.wakeOne();
    m_mutex.unlock();
    wait();
}

void Serial::open_close(const QString &portName)
{
    m_portName = portName;
    //if open then close
    if(m_quit == false)
    {
        m_quit = true;
        return;
    }

    m_quit = false;

    if (!isRunning())
        start();
    else
        m_cond.wakeOne();
}

void Serial::run()
{
    QSerialPort serial;
    serial.setPortName(m_portName);
    serial.setParity(QSerialPort::Parity::NoParity);
    serial.setDataBits(QSerialPort::Data8);
    serial.setStopBits(QSerialPort::TwoStop);
    serial.setBaudRate(QSerialPort::Baud9600);
    m_mutex.lock();
    if (!serial.open(QIODevice::ReadWrite)) {
        m_quit = true;
        emit errorLog("can`t open Serial");
    }
    else
    {
        serial.clear();
        emit Log("open Serial");
        m_database->FileLog("INFO open Serial Port "+m_portName);
    }
    m_mutex.unlock();

    QByteArray responseData;
    int StartIndex = 0;
    int TelegramSize = 19;
    char command = ' ';
    unsigned char checkSumm = 0;
    unsigned char newCheckSumm = 0;
    tTelegram recive_telegram;

    emit show_serial(true);
    m_dataReadyToSend = false;
    //emit Log("serial open");
    send('i',0,0,0,0);
    m_database->FileLog("INFO recquest settings");
    while(!m_quit)
    {
        //send data if some available
        m_mutex.lock();
        if(m_dataReadyToSend == true)
        {
            //m_database->FileLog("DEBUG start sending");
            serial.write(m_SendData);
            if(!serial.waitForBytesWritten())
            {
                emit Log("timeout durring sending");
                m_database->FileLog("ERROR timeout durring sending");
            }
            m_dataReadyToSend = false;
            //m_wait_send_loop.exit();
            //m_database->FileLog("DEBUG finish sending");
        }
        m_mutex.unlock();

        //read byts from serial
        m_mutex.lock();
        //m_database->FileLog("DEBUG start reciving");
        while(serial.waitForReadyRead(10))
        {
            responseData += serial.readAll();
        }
        //m_database->FileLog("DEBUG end reciving");
        m_mutex.unlock();

        //check if Start codon is present
        for(int i = 0;i<responseData.size();i++)
        {
            if(responseData[i]== 'S')
            {
                StartIndex = i;
                break;
            }
        }
        //if startcodon index is not in first place clear fauld bytes
        if(StartIndex!=0)
        {
            responseData.remove(0,StartIndex);
            emit errorLog("delete "+QString::number(StartIndex));
            m_database->FileLog("ERROR delete "+QString::number(StartIndex));
        }

        //a complite telegram was recived and now get processd
        if(responseData.size()>=TelegramSize)
        {
            //m_database->FileLog("DEBUG enter processing telegram");
            //put bytes needet form (chars and floats)
            command = responseData[1];
            for(int i=0;i<16;i++)
            {
                recive_telegram.Bytes[i] = responseData[i+2];
            }
            checkSumm = responseData[18];
            //m_database->FileLog("DEBUG checksumm check");
            //check the checksumm
            newCheckSumm = 0;
            for(int i=0;i<TelegramSize-1;i++)
            {
                newCheckSumm += responseData[i];
            }
            if(newCheckSumm != checkSumm)
            {
                emit errorLog("check failt recive:"+QString::number(checkSumm)+" calc:"+QString::number(newCheckSumm));
                m_database->FileLog("ERROR checksum failt :"+QString::number(checkSumm)+" calc:"+QString::number(newCheckSumm));
            }
            else
            {
                emit recived(command,recive_telegram.Value[0],recive_telegram.Value[1],recive_telegram.Value[2],recive_telegram.Value[3]);
                m_database->FileLog("INFO recived:"+QString(command)+" A:"+QString::number(recive_telegram.Value[0])+" B:"+QString::number(recive_telegram.Value[1])+" C:"+QString::number(recive_telegram.Value[2])+" D:"+QString::number(recive_telegram.Value[3]));
            }
            //m_database->FileLog("DEBUG end Telegramm process");
            responseData.remove(0,TelegramSize);
        }
    }
    //emit Log("serial close");
    emit show_serial(false);
    serial.close();
    emit Log("close Serial");
    m_database->FileLog("INFO close Serial Port "+m_portName);

}

void Serial::send(char command,float value1,float value2,float value3,float value4)
{
    if(m_quit)
        return;

    unsigned char newCheckSumm = 0;
    QByteArray sendData = QString("S").toUtf8();
    sendData += QString(command).toUtf8();
    tTelegram send_telegram;
    send_telegram.Value[0] = value1;
    send_telegram.Value[1] = value2;
    send_telegram.Value[2] = value3;
    send_telegram.Value[3] = value4;
    for(int i=0;i<16;i++)
    {
        sendData += send_telegram.Bytes[i];
    }
    newCheckSumm = 0;
    for(int i=0;i<18;i++)
    {
        newCheckSumm += sendData[i];
    }
    sendData += newCheckSumm;
    m_database->FileLog("INFO send:"+QString(command)+" A:"+QString::number(value1)+" B:"+QString::number(value2)+" C:"+QString::number(value3)+" D:"+QString::number(value4));
    m_mutex.lock();
    m_SendData = sendData;
    m_dataReadyToSend = true;
    m_mutex.unlock();
}
