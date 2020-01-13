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
    m_cond.wakeAll();
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

    emit show_serial(true);
    m_dataReadyToSend = false;
    //emit Log("serial open");
    send('i',0,0,0,0);
    m_database->FileLog("INFO recquest settings");
    while(!m_quit)
    {

        //send data if some available
        m_mutex.lock();
        m_cond.wakeAll();
        if(m_dataReadyToSend == true)
        {
            m_database->FileLog("INFO send:"+QString(char(m_SendData[1]))+" A:"+QString::number(m_send_telegram.Value[0])+" B:"+QString::number(m_send_telegram.Value[1])+" C:"+QString::number(m_send_telegram.Value[2])+" D:"+QString::number(m_send_telegram.Value[3]));
            serial.write(m_SendData);
            m_dataReadyToSend = false;
        }
        m_mutex.unlock();

        //read byts from serial
        m_mutex.lock();
        m_cond.wakeAll();
        responseData += serial.readAll();
        serial.waitForReadyRead(10);
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
            //put bytes needet form (chars and floats)
            command = responseData[1];
            for(int i=0;i<16;i++)
            {
                m_recive_telegram.Bytes[i] = responseData[i+2];
            }
            checkSumm = responseData[18];

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
                emit recived(command,m_recive_telegram.Value[0],m_recive_telegram.Value[1],m_recive_telegram.Value[2],m_recive_telegram.Value[3]);
                m_database->FileLog("INFO recived:"+QString(command)+" A:"+QString::number(m_recive_telegram.Value[0])+" B:"+QString::number(m_recive_telegram.Value[1])+" C:"+QString::number(m_recive_telegram.Value[2])+" D:"+QString::number(m_recive_telegram.Value[3]));
            }

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
    //emit Log("send: "+QString(command));
    if(m_quit)
        return;
    unsigned char newCheckSumm = 0;
    QByteArray sendData = QString("S").toUtf8();
    sendData += QString(command).toUtf8();
    m_send_telegram.Value[0] = value1;
    m_send_telegram.Value[1] = value2;
    m_send_telegram.Value[2] = value3;
    m_send_telegram.Value[3] = value4;
    for(int i=0;i<16;i++)
    {
        sendData += m_send_telegram.Bytes[i];
    }
    newCheckSumm = 0;
    for(int i=0;i<18;i++)
    {
        newCheckSumm += sendData[i];
    }
    sendData += newCheckSumm;

    while(m_dataReadyToSend)
    {

    }

    m_mutex.lock();
    m_cond.wakeAll();
    m_SendData = sendData;
    m_dataReadyToSend = true;
    m_mutex.unlock();
}
