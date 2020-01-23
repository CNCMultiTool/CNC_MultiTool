#include "serial.h"


Serial::Serial(cnc_data *database)
{
    m_database = database;
    const QMutexLocker locker(&m_mutex);
    //connect(&MySerial,SIGNAL(readyRead()),this,SLOT(recive()));
}

Serial::~Serial()
{
    m_database->m_Serial_quit = true;
    timer->stop();
    MySerial.close();
}

void Serial::serial_start()
{
    MySerial.setPortName(m_database->m_SerialPortName);
    MySerial.setParity(QSerialPort::Parity::EvenParity);
    MySerial.setDataBits(QSerialPort::Data8);
    MySerial.setStopBits(QSerialPort::OneStop);
    MySerial.setBaudRate(QSerialPort::Baud115200);
    if (!MySerial.open(QIODevice::ReadWrite)) {
        emit errorLog("can`t start Serial");
        m_database->set_serial(false);
        m_database->m_Serial_quit = true;
        return;
    }
    m_database->m_Serial_quit = false;
    MySerial.clear();
    timer->start(1);
    connect(timer,SIGNAL(timeout()),this,SLOT(recive()));
    m_database->set_serial(true);
    send('i',0,0,0,0);
}

void Serial::serial_close()
{
    emit Log("serial_close");
    m_database->set_serial(false);
    m_database->m_Serial_quit = true;
    timer->stop();
    MySerial.close();
}

void Serial::recive()
{
    int TelegramSize = 19;
    char command = ' ';
    unsigned char checkSumm = 0;
    unsigned char newCheckSumm = 0;
    tTelegram recive_telegram;

        m_mutex.lock();
        if(m_SendData.count()>0)
        {
            //emit Log("send something");
            QByteArray sendByte = m_SendData[0];
            m_SendData.pop_front();
            MySerial.write(sendByte);
            MySerial.waitForBytesWritten(10);
        }
        m_mutex.unlock();

        responseData += MySerial.readAll();
        MySerial.waitForReadyRead(10);

        //a complite telegram was recived and now get processd
        if(responseData.size()>=TelegramSize)
        {
            //emit Log("telegram size :"+QString::number(responseData.size()));
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
            QString LogText = QString(char(responseData[0]))+" ";
            LogText += QString(command)+" ";
            LogText += QString::number(recive_telegram.Value[0])+" ";
            LogText += QString::number(recive_telegram.Value[1])+" ";
            LogText += QString::number(recive_telegram.Value[2])+" ";
            LogText += QString::number(recive_telegram.Value[3])+" ";
            LogText += QString::number(checkSumm);
            //emit Log(LogText);
            m_database->FileLog("INFO rexive:"+LogText);
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
}

void Serial::send(char command,float value1,float value2,float value3,float value4)
{
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
    m_mutex.lock();
    m_SendData.push_back(sendData);
    m_mutex.unlock();
    QString LogText = QString(char(responseData[0]))+" ";
    LogText += QString(command)+" ";
    LogText += QString::number(send_telegram.Value[0])+" ";
    LogText += QString::number(send_telegram.Value[1])+" ";
    LogText += QString::number(send_telegram.Value[2])+" ";
    LogText += QString::number(send_telegram.Value[3])+" ";
    LogText += QString::number(responseData[18]);
    //emit Log(LogText);
    m_database->FileLog("INFO send:"+LogText);
}
