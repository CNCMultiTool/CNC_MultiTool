#include "serial.h"

#define STARTCOD 0x01
#define ENDCOD 0x02
#define SPEZIALCOD 0x03

Serial::Serial(cnc_data *database)
{
    m_database = database;
}

Serial::~Serial()
{
    //thread.exit();
}

bool Serial::serial_open()
{
    //disconnect(&m_serial,SIGNAL(readyRead()),this,SLOT(serial_read()));
    emit Log("serialOpen");
    m_serial.setPortName(m_database->m_SerialPortName);
    m_serial.setParity(QSerialPort::Parity::NoParity);
    m_serial.setDataBits(QSerialPort::Data8);
    m_serial.setStopBits(QSerialPort::TwoStop);
    m_serial.setBaudRate(QSerialPort::Baud115200);
    if (!m_serial.open(QIODevice::ReadWrite)) {
        emit errorLog("can`t start Serial");
        emit show_serial(false);
        serial_close();
        return false;
    }
    m_lastsend.clear();
    m_recivedBytes.clear();
    m_serial.clear();
    m_serial.flush();
    connect(timer, SIGNAL(timeout()), this, SLOT(serial_read()));
    connect(sendAnswerTimeout, SIGNAL(timeout()), this, SLOT(serial_sendTimeout()));
    timer->start(1);
    sendAnswerTimeout->stop();
    m_toSendBuffer.clear();
    //connect(&m_serial,SIGNAL(readyRead()),this,SLOT(serial_read()));
    emit show_serial(true);
    m_database->m_SerialIsOpen = true;
    return true;
}

void Serial::serial_close()
{
    m_recivedBytes.clear();
    m_serial.clear();
    //disconnect(&m_serial,SIGNAL(readyRead()),this,SLOT(serial_read()));
    disconnect(timer, SIGNAL(timeout()), this, SLOT(serial_read()));
    timer->stop();
    disconnect(sendAnswerTimeout, SIGNAL(timeout()), this, SLOT(serial_sendTimeout()));
    sendAnswerTimeout->stop();
    m_serial.close();
    emit show_serial(false);
    m_database->m_SerialIsOpen = false;
}

void Serial::serial_sendTimeout()
{
    emit errorLog("Timeout do to no recive");
    //serial_close();

    //serial_open();
}

/**
 * liest daten vom serielen port
 * chekct ob ein telegramm empfangen
 * konvertiert das telegramm von bytes in comando und werte
 */
void Serial::serial_read()
{
    if(m_serial.isOpen())
    {
        //check if stuff left to send
        if(!sendAnswerTimeout->isActive()&& !m_toSendBuffer.isEmpty()){
            serial_sendNext();
        }


        QByteArray ab;
        //read from serial
        m_recivedBytes += m_serial.readAll();
        m_serial.waitForReadyRead(m_recive_timeout);

        //chck if a startcodon is presend, remove leading nonsens bytes to the startcodon
        int command_start = m_recivedBytes.indexOf(char(STARTCOD));
        if(command_start < 0)return;//return if no start codon
        if(command_start > 0) m_recivedBytes.remove(0,command_start);
        command_start = 0;

        if(m_recivedBytes.length()<5)return;//return if minimum length is not given

        int command_end = m_recivedBytes.indexOf(char(ENDCOD));
        if(command_end < 0)return;//return if no end codon
        if(command_start < 0 || command_end < 0) return;

        int len = m_recivedBytes.at(command_start+1);

        if(len >= m_recivedBytes.length())return;//return if array is shorter then len info

        if(m_recivedBytes.at(len) != char(ENDCOD)){
            emit Log("PC end codon not at expectet pos");
            //request resend last
            ab.append(31);
            serial_addToSend(ab);
            m_recivedBytes.remove(0,1);//remove first startcodon to search for new start
            return;
        }



        command_end = len;
        QByteArray mes = m_recivedBytes.mid(0,command_end+1);
        m_recivedBytes.remove(0,command_end+1);

//        emit Log("start:"+QString::number(command_start)+" end:"+QString::number(command_end));
//        emit Log("lenInfo:"+QString::number(len));
//        emit Log("lenArray:"+QString::number(m_recivedBytes.length()));

        if(mes.at(1) != mes.length()-1)
        {
            //request resend last
            ab.append(31);
            serial_addToSend(ab);
            emit errorLog("PC length fail len "+QString::number(mes.length())+" rec "+QString::number(mes.at(1)));
            return;
        }

        unsigned char checksum = 0;
        for(int i=1;i < mes.length()-2;i++) {
            checksum += mes.at(i);
        }
        unsigned char new_cs = mes.at(mes.length()-2);
        if(checksum != new_cs)
        {
            //request resend last
            ab.append(31);
            serial_addToSend(ab);
            emit errorLog("PC chesumm fail cs "+QString::number(checksum)+" rec "+QString::number(new_cs));
            emit Log("len:"+QString::number(mes.length())+" mes:"+QString(mes.toHex()));
            return;
        }

        mes.chop(2);//cut checksum and endcodon
        QByteArray data = mes.mid(2);

        //recive an ack
        if(data.at(0) == 32){
            sendAnswerTimeout->stop();
            m_lastsend.clear();
            serial_sendNext();
            return;
        }
        //recive an resend request
        if(data.at(0) == 31){
            if(m_lastsend.isEmpty()){
                emit errorLog("resend request but empty m_lastsend");
                ab.append(32);
                serial_addToSend(ab);
                return;
            }else{
                sendAnswerTimeout->stop();
                emit errorLog("resend m_lastsend:"+QString(m_lastsend.toHex()));
                serial_send(m_lastsend);
                return;
            }
        }
        //send rec acknolage
        ab.append(32);
        serial_addToSend(ab);
        //process data

        emit recBytes(data);
    }
}


/**
 * verpackt und
 * sendet es mittels serielem port
 */
void Serial::serial_addToSend(QByteArray mes)
{
    if(!m_serial.isOpen())
    {
        emit errorLog("Serial not open");
        emit show_serial(false);
        return;
    }

    m_toSendBuffer.append(mes);
    if(!sendAnswerTimeout->isActive()){
        serial_sendNext();
    }
}

void Serial::serial_sendNext(){
    if(!m_toSendBuffer.isEmpty()){
        serial_send(m_toSendBuffer.first());
        m_toSendBuffer.pop_front();
    }
}

void Serial::serial_send(QByteArray mes){
    if(!m_serial.isOpen())
    {
        emit errorLog("Serial not open");
        emit show_serial(false);
        return;
    }

    //emit Log("send: "+mes.toHex());
    m_lastsend = mes;
    packMesage(&mes);
    //emit Log("send packet hex: "+mes.toHex());

    m_serial.write(mes);
    m_serial.waitForBytesWritten(m_send_timeout);
    //start timer
    if(!(m_lastsend.length()==1 && m_lastsend.at(0)==32)){
        sendAnswerTimeout->start(m_send_reciveTimeout);
    }
}

void Serial::packMesage(QByteArray *mes)
{
    char checksum = 0;
    unsigned char len = mes->length() + 3;
    mes->prepend(len);

    for(int i=0;i< mes->length();i++){
        checksum += mes->at(i);
    }
    mes->append(checksum);

//    for(int i=0;i< mes->length();i++){
//        if(mes->at(i) == STARTCOD || mes->at(i) == ENDCOD){
//            mes[i] = mes[i] + char(SPEZIALCOD);
//            mes->insert(i,char(SPEZIALCOD));
//        }
//    }

    mes->prepend(char(STARTCOD));
    mes->append(char(ENDCOD));

}
