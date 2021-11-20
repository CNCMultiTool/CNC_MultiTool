#include "serial.h"


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
    m_recivedBytes.clear();
    m_serial.clear();
    m_serial.flush();
    connect(timer, SIGNAL(timeout()), this, SLOT(serial_read()));
    connect(sendAnswerTimeout, SIGNAL(timeout()), this, SLOT(serial_sendTimeout()));
    timer->start(1);
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
    emit errorLog("error: Timeout do to no recive");
    serial_close();

    serial_open();
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
        QByteArray ab;
        m_recivedBytes += m_serial.readAll();
        m_serial.waitForReadyRead(m_recive_timeout);
        int  command_end = m_recivedBytes.indexOf(char(0x00));
        if(command_end<0){
            return;
        }
        QByteArray helper = m_recivedBytes;
        QByteArray mes;
        CobsDecode(helper.remove(command_end+1,helper.length()), mes);
        m_recivedBytes.remove(0,command_end+1);

        unsigned char checksum = 0;
        for(int i=0;i< mes.length()-1;i++) {
            checksum += mes.at(i);
        }
        unsigned char new_cs = mes.at(mes.length()-1);
        if(checksum != new_cs)
        {
            //request resend last
            sendAnswerTimeout->stop();
            ab.append(31);
            serial_send(ab);
            emit errorLog("PC chesumm fail cs "+QString::number(checksum)+" rec "+QString::number(new_cs));
            emit errorLog("data "+mes);
            return;
        }
        if(mes.at(0) != mes.length()-2)
        {
            //request resend last
            sendAnswerTimeout->stop();
            ab.append(31);
            serial_send(ab);
            emit errorLog("PC length fail len "+QString::number(mes.length()-2)+" rec "+QString::number(mes.at(0)));
            QString s;
            foreach(auto c,mes)
                s.append(c);
            emit errorLog("data "+s);
            return;
        }

        if(mes.at(1) == 32){
            sendAnswerTimeout->stop();
            m_lastsend = "";
            return;
        }
        if(mes.at(1) == 31){
            if(m_lastsend == ""){
                emit errorLog("resend request but empty m_lastsend");
            }else{
                emit errorLog("resend m_lastsend");
                serial_send(m_lastsend);
                return;
            }
        }
        //send rec acknolage
        ab.append(32);
        serial_send(ab);
        //process data
        QByteArray data = mes.remove(mes.length()-1,1);
        emit recBytes(data);
    }
}


/**
 * verpackt es mittels COBS
 * sendet es mittels serielem port
 */
void Serial::serial_send(QByteArray mes)
{
    if(!m_serial.isOpen())
    {
        emit errorLog("Serial not open");
        emit show_serial(false);
        return;
    }
    //emit Log("send: "+mes.toHex());
    m_lastsend = mes;
    addChecksum(&mes);
    //emit Log("send checksum: "+mes.toHex());
    QByteArray encodedData;
    CobsEncode(mes, encodedData);
    //emit Log("send enc: "+encodedData.toHex());
    m_serial.write(encodedData);
    m_serial.waitForBytesWritten(m_send_timeout);
    if(!(m_lastsend.length()==1 && m_lastsend.at(0)==32)){
        sendAnswerTimeout->start(m_send_reciveTimeout);
    }
}

void Serial::addChecksum(QByteArray *mes)
{
    char checksum = 0;
    //emit Log("length "+QString::number(mes->length()));
    unsigned char len = mes->length();
    mes->prepend(len);
    for(int i=0;i< mes->length();i++){
        checksum += mes->at(i);
    }
    //emit Log("cs "+QString::number(checksum));
    mes->append(checksum);

}

void Serial::CobsEncode(const QByteArray &rawData, QByteArray &encodedData)
{
    int startOfCurrBlock = 0;
    uint8_t numElementsInCurrBlock = 0;

    auto it = rawData.begin();

    // Create space for first (this will be
    // overwritten once count to next 0x00 is known)
    encodedData.append('\0');

    while (it != rawData.end()) {
        if (*it == 0x00) {
            // Save the number of elements before the next 0x00 into
            // the output
            encodedData[startOfCurrBlock] = (uint8_t)(numElementsInCurrBlock + 1);

            // Add placeholder at start of next block
            encodedData.append('\0');

            startOfCurrBlock = encodedData.size() - 1;

            // Reset count of num. elements in current block
            numElementsInCurrBlock = 0;

        } else {
            encodedData.push_back(*it);
            numElementsInCurrBlock++;

            if (numElementsInCurrBlock == 254) {
                encodedData[startOfCurrBlock] = (uint8_t)(numElementsInCurrBlock + 1);

                // Add placeholder at start of next block
                encodedData.append('\0');

                startOfCurrBlock = encodedData.size() - 1;

                // Reset count of num. elements in current block
                numElementsInCurrBlock = 0;
            }
        }
        it++;
    }

    // Finish the last block
    // Insert pointer to the terminating 0x00 character
    encodedData[startOfCurrBlock] = numElementsInCurrBlock + 1;
    encodedData.append('\0');
}

/**
 * @brief MainWindow::CobsDecode
 * decodes incomming messages with OCBS
 * !!! need 0x00 at last byte of the bytearray !!!
 * @param encodedData
 * @param decodedData
 */
void Serial::CobsDecode(const QByteArray &encodedData, QByteArray &decodedData)
{
    decodedData.clear();
    //    Log(QString(__func__) + " decoded " + encodedData.toHex()
    //        + " len: " + QString::number(encodedData.size()),DEBUG_LOW);

    int encodedDataPos = 0;

    while (encodedDataPos < encodedData.size()) {
        int numElementsInBlock = encodedData[encodedDataPos] - 1;
        encodedDataPos++;

        // Copy across all bytes within block
        for (int i = 0; i < numElementsInBlock; i++) {
            uint8_t byteOfData = encodedData[encodedDataPos];
            if (byteOfData == 0x00) {
                //decodedData.clear();
                //throw CobsDecodingFailed(encodedData);
                //Log(QString(__func__) + "ERROR decoding failed",ERROR_LOW);
                //return;
                break;
            }

            decodedData.push_back(encodedData[encodedDataPos]);
            encodedDataPos++;
        }

        if (encodedData[encodedDataPos] == '\0') {
            // End of packet found!
            break;
        }

        // We only add a 0x00 byte to the decoded data
        // IF the num. of elements in block was less than 254.
        // If num. elements in block is max (254), then we know that
        // the block was created due to it reaching maximum size, not because
        // a 0x00 was found
        if (numElementsInBlock < 0xFE) {
            decodedData.append('\0');
        }
    }
}
