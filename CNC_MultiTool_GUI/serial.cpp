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
    m_quit = true;
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
}

void Serial::run()
{
    QSerialPort serial;
    serial.setPortName(m_portName);
    serial.setParity(QSerialPort::Parity::EvenParity);
    serial.setDataBits(QSerialPort::Data8);
    serial.setStopBits(QSerialPort::OneStop);
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
        if(m_mutex.try_lock())
        {
            if(m_SendData.count()>0)
            {
                QByteArray sendByte = m_SendData[0];
                m_SendData.pop_front();
                int writtenBytes = serial.write(sendByte);
                serial.waitForBytesWritten(-1);
                if(serial.error()!=0&&serial.error()!=12)
                {
                    m_database->FileLog("ERROR Serial write : "+serial.errorString());
                    emit errorLog("ERROR Serial write : "+serial.errorString());
                    serial.clearError();
                    //serial.clear();
                }
                if(writtenBytes<19)
                {
                    m_database->FileLog("ERROR Serial no bytes written");
                    emit errorLog("ERROR Serial no bytes written");
                }
                //serial.flush();
            }
            m_mutex.unlock();
        }
        else
        {
            m_database->FileLog("DEBUG cant log send mutex");
        }
        //read byts from serial
        //m_database->FileLog("DEBUG start reciving");
        //if(serial.bytesAvailable())
        //{
        responseData += serial.readAll();
        serial.waitForReadyRead(10);
        //}

        //m_database->FileLog("DEBUG end reciving");
        if(responseData.length()>0)
        {
            m_database->FileLog("DEBUG len of recived byte"+QString::number(responseData.length()));
        }
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
            emit errorLog("ERROR delete "+QString::number(StartIndex));
            m_database->FileLog("ERROR delete "+QString::number(StartIndex));
        }

        //a complite telegram was recived and now get processd
        if(responseData.size()>=TelegramSize)
        {
            //put bytes needet form (chars and floats)
            command = responseData[1];
            for(int i=0;i<16;i++)
            {
                recive_telegram.Bytes[i] = responseData[i+2];
            }

            //check the checksumm
            checkSumm = responseData[18];
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
                m_database->FileLog("INFO recived:"+QString(command)+" A:"+QString::number(recive_telegram.Value[0])+" B:"+QString::number(recive_telegram.Value[1])+" C:"+QString::number(recive_telegram.Value[2])+" D:"+QString::number(recive_telegram.Value[3]));
                emit recived(command,recive_telegram.Value[0],recive_telegram.Value[1],recive_telegram.Value[2],recive_telegram.Value[3]);
                m_database->FileLog("INFO end reciveprocess");
            }
            responseData.remove(0,TelegramSize);
        }
        emit show_loops();
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
    m_database->FileLog("DEBUG try log to send");
    m_mutex.lock();
    m_SendData.push_back(sendData);
    m_dataReadyToSend = true;
    m_mutex.unlock();
    m_database->FileLog("INFO send:"+QString(command)+" A:"+QString::number(value1)+" B:"+QString::number(value2)+" C:"+QString::number(value3)+" D:"+QString::number(value4));
}
