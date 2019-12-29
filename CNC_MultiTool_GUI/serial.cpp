#include "serial.h"


Serial::Serial()
{
    const QMutexLocker locker(&m_mutex);
    m_quit = true;
}

Serial::~Serial()
{
    m_mutex.lock();
    m_quit = true;
    m_cond.wakeOne();
    m_mutex.unlock();
    wait();
}

bool Serial::open(const QString &portName)
{
    m_portName = portName;
    if(m_quit == false)
    {
        m_quit = true;
        emit Log("close Serial");
        return(false);
    }

    m_quit = false;
    if (!isRunning())
        start();
    else
        m_cond.wakeOne();
    emit Log("open Serial");
    return(true);
}

void Serial::run()
{
    QSerialPort serial;
    m_serial = &serial;
    serial.setPortName(m_portName);
    serial.setParity(QSerialPort::Parity::NoParity);
    serial.setDataBits(QSerialPort::Data8);
    serial.setStopBits(QSerialPort::TwoStop);
    serial.setBaudRate(QSerialPort::Baud9600);
    m_mutex.lock();
    if (!serial.open(QIODevice::ReadWrite)) {
        m_quit = true;
        emit Log("can`t open Serial");
    }
    else
    {
        serial.clear();
    }
    m_mutex.unlock();

    QByteArray responseData;
    int StartIndex = 0;
    int TelegramSize = 19;
    char command = ' ';
    unsigned char checkSumm = 0;
    unsigned char newCheckSumm = 0;

    while(!m_quit)
    {
        //read byts from serial
        m_mutex.lock();
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
        //if startindex is not in first place clear fauld bytes
        if(StartIndex!=0)
        {
            responseData.remove(0,StartIndex);
            emit Log("delete "+QString::number(StartIndex));
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
           if(newCheckSumm!=checkSumm)
           {
               emit errorLog("check failt recive:"+QString::number(checkSumm)+" calc:"+QString::number(newCheckSumm));
           }
           else
           {
               //emit Log("check pass recive:"+QString::number(checkSumm)+" calc:"+QString::number(newCheckSumm));
               emit ProcesCommand(command,m_recive_telegram.Value[0],m_recive_telegram.Value[1],m_recive_telegram.Value[2],m_recive_telegram.Value[3]);
           }
            /*emit Log("recive("+QString(command)+"  "+
                               QString::number(m_recive_telegram.Value[0])+"  "+
                               QString::number(m_recive_telegram.Value[1])+"  "+
                               QString::number(m_recive_telegram.Value[2])+"  "+
                               QString::number(m_recive_telegram.Value[3])+"  "+
                               QString::number(checkSumm)+")");*/

            responseData.remove(0,TelegramSize);

        }
    }

    serial.close();
}

void Serial::send(char command,float value1,float value2,float value3,float value4)
{
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

    m_mutex.lock();
    m_serial->write(sendData);
    //m_serial->waitForBytesWritten(10);
    m_mutex.unlock();
    m_cond.wakeOne();

    emit Log("send("+QString(command)+"  "+
                       QString::number(m_send_telegram.Value[0])+"  "+
                       QString::number(m_send_telegram.Value[1])+"  "+
                       QString::number(m_send_telegram.Value[2])+"  "+
                       QString::number(m_send_telegram.Value[3])+"  "+
                       QString::number(newCheckSumm)+")");

}
