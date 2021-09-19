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
    emit errorLog("error: no recive");
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
        m_recivedBytes += m_serial.readAll();
        m_serial.waitForReadyRead(m_recive_timeout);
        int command_end = m_recivedBytes.indexOf("\n");
        if(command_end>0){
            QByteArray helper = m_recivedBytes;
            QString Line = QString(helper.remove(command_end+1,helper.length()));
            m_recivedBytes.remove(0,command_end+1);

            if(Line.contains("rec")){
                sendAnswerTimeout->stop();
                m_lastsend = "";
                return;
            }else if(Line.contains("resend")){
                if(m_lastsend == "")
                    emit errorLog("resend request but empty m_lastsend");
                else
                    serial_send(m_lastsend);
                return;
            }
            emit Log("rec: "+Line);
            emit recLine(Line);
        }
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
    emit Log("send: "+mes.toHex());
    m_lastsend = mes;
    addChecksum(&mes);
    emit Log("send checksum: "+mes.toHex());
    QByteArray encodedData;
    CobsEncode(mes, encodedData);
    emit Log("send enc: "+encodedData.toHex());
    m_serial.write(encodedData);
    m_serial.waitForBytesWritten(m_send_timeout);
    sendAnswerTimeout->start(m_send_reciveTimeout);
}

void Serial::addChecksum(QByteArray *mes)
{
    char checksum = 0;
    for(int i=0;i< mes->length();i++)
        checksum += mes->at(i);
    mes->append(checksum);
    mes->prepend(mes->length()+1);
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
