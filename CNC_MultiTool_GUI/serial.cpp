#include "serial.h"


Serial::Serial(cnc_data *database)
{
    m_database = database;
    const QMutexLocker locker(&m_mutex);
    const QMutexLocker serial_locker(&m_serial_mutex);
    //connect(&MySerial,SIGNAL(readyRead()),this,SLOT(recive()));
    connect(send_timeout,SIGNAL(timeout()),this,SLOT(error_handler()));
    connect(setting_timer,SIGNAL(timeout()),this,SLOT(request_settings()));
    connect(timer,SIGNAL(timeout()),this,SLOT(recive()));
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
    timer->start(10);
    setting_timer->start(1000);
    m_database->set_serial(true);
    send('i',0,0,0,0);
}

void Serial::serial_close()
{
    emit Log("serial_close");
    m_database->set_serial(false);
    m_database->m_Serial_quit = true;
    timer->stop();
    setting_timer->stop();
    send_timeout->stop();
    MySerial.close();
}

void Serial::error_handler()
{
    try {
        timer->stop();
        setting_timer->stop();
        send_timeout->stop();
        //emit Log("ERROR SerialTimeout occured");
        emit errorLog("ERROR SerialTimeout occured");
        m_database->FileLog("ERROR SerialTimeout occured");
        //m_serial_mutex.lock();
        if(MySerial.isOpen())
        {
            //emit Log("timeout_handler: serial is still open");
            m_database->FileLog("ERROR timeout_handler: serial is still open");
            MySerial.waitForBytesWritten(10);
            MySerial.waitForReadyRead(10);
            MySerial.close();
            //emit Log("timeout_handler: close serial");
            m_database->FileLog("ERROR timeout_handler: close serial");
            if(MySerial.open(QIODevice::ReadWrite))
            {
                //emit Log("timeout_handler: open and clear serial");
                m_database->FileLog("ERROR timeout_handler: open and clear serial");
                MySerial.clear();
            }
            else
            {
                //emit Log("cant open serial in timeouthandling");
                m_database->FileLog("ERROR cant open serial in timeouthandling");
            }
        }
        else
        {
            //emit Log("timeout_handler: serial is already close");
            m_database->FileLog("ERROR timeout_handler: serial is already close");
            if(MySerial.open(QIODevice::ReadWrite))
            {
                //emit Log("timeout_handler: open and clear serial");
                m_database->FileLog("ERROR timeout_handler: open and clear serial");
                MySerial.clear();
            }
            else
            {
                //emit Log("cant open serial in timeouthandling");
                m_database->FileLog("ERROR cant open serial in timeouthandling");
            }
        }
        timer->start();
        setting_timer->start();
        QByteArray buffer = lastSendData;//speicere das zuletzt gesendete um es später erneut zu senden
        send('i',0,0,0,0);//init sequenz of arduino
        m_SendData.push_back(buffer);//resend last command
        m_database->FileLog("ERROR add to resend: "+QString(char(buffer[1])));
    }catch(...){
        emit errorLog("ERROR error_handler() throw an error");
        m_database->FileLog("ERROR error_handler() throw an error");
    }
}

void Serial::answer_repeatrequest()
{
    QString reciveTetxt;
    m_SendData.push_back(lastSendData);
    for(int i = 0;i<19;i++)
    {
        reciveTetxt += QString(char(lastSendData[i]));
    }
    emit Log("INFO resend last:"+reciveTetxt);
}

void Serial::request_settings()
{
    send('r',0,0,0,0);
}

void Serial::recive()
{
    try {
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
            m_serial_mutex.lock();
            if(!MySerial.isOpen())
            {
                emit Log(" open in send");
                m_database->FileLog("ERROR open in send");
                error_handler();
            }
            MySerial.write(sendByte);
            m_database->FileLog("INFO send command: "+QString(char(sendByte[1])));
            lastSendData = sendByte;
            MySerial.waitForBytesWritten(10);
            m_serial_mutex.unlock();
            if(char(sendByte[1]) != 'a')
            {
                send_timeout->start(100);
            }
        }
        m_mutex.unlock();

        m_serial_mutex.lock();
        if(!MySerial.isOpen())
        {
            emit Log("open in read");
            m_database->FileLog("ERROR open in read");
            error_handler();
        }
        responseData += MySerial.readAll();
        MySerial.waitForReadyRead(10);
        m_serial_mutex.unlock();

        //a complite telegram was recived and now get processd
        if(responseData.size()>=TelegramSize)
        {
            if(send_timeout->isActive())
            {
                send_timeout->stop();
            }
            //put bytes needet form (chars and floats)
            command = char(responseData[1]);
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
            //emit Log("INFO recive:"+LogText);
            //QString reciveTetxt;
            //for(int i = 0;i<19;i++)
            //{
            //    reciveTetxt += QString(char(responseData[i]));
            //}
            //emit Log("INFO recive:"+reciveTetxt);

            m_database->FileLog("INFO recive:"+LogText);
            if(newCheckSumm != checkSumm)
            {
                emit errorLog("check failt recive:"+QString::number(checkSumm)+" calc:"+QString::number(newCheckSumm));
                m_database->FileLog("ERROR checksum failt :"+QString::number(checkSumm)+" calc:"+QString::number(newCheckSumm));
                responseData.clear();
                error_handler();
            }
            else
            {
                emit recived(command,recive_telegram.Value[0],recive_telegram.Value[1],recive_telegram.Value[2],recive_telegram.Value[3]);
            }

            responseData.remove(0,TelegramSize);
        }
    } catch (...) {
        emit errorLog("recive() throw an error");
        m_database->FileLog("ERROR recive() throw an error");
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
    QString LogText = QString(char(sendData[0]))+" ";
    LogText += QString(char(sendData[1]))+" ";
    LogText += QString::number(send_telegram.Value[0])+" ";
    LogText += QString::number(send_telegram.Value[1])+" ";
    LogText += QString::number(send_telegram.Value[2])+" ";
    LogText += QString::number(send_telegram.Value[3])+" ";
    LogText += QString::number(sendData[18]);
    //emit Log("INFO send:"+LogText);
    m_database->FileLog("INFO send:"+LogText);
}
