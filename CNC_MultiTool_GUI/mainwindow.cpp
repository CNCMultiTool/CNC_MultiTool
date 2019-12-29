#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textEditLog->setReadOnly(true);
    ui->textEditLog_error->setReadOnly(true);

    //chek availabal port add add to the comboBox
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        ui->comboBoxComPortName->addItem(info.portName());
    //

    connect(&m_CNC_auto,&CNC_automation::Log,this,&MainWindow::Log);
    connect(&m_CNC_auto,&CNC_automation::errorLog,this,&MainWindow::errorLog);

    connect(&m_Serial,&Serial::Log,this,&MainWindow::Log);
    connect(&m_Serial,&Serial::errorLog,this,&MainWindow::errorLog);
    connect(&m_Serial,&Serial::ProcesCommand,this,&MainWindow::ProcesCommand);

    connect(ui->pushButtonMoveXPos,SIGNAL(released()),this,SLOT(sendStopMoving()));
    connect(ui->pushButtonMoveXNeg,SIGNAL(released()),this,SLOT(sendStopMoving()));
    connect(ui->pushButtonMoveYPos,SIGNAL(released()),this,SLOT(sendStopMoving()));
    connect(ui->pushButtonMoveYNeg,SIGNAL(released()),this,SLOT(sendStopMoving()));
    connect(ui->pushButtonMoveZPos,SIGNAL(released()),this,SLOT(sendStopMoving()));
    connect(ui->pushButtonMoveZNeg,SIGNAL(released()),this,SLOT(sendStopMoving()));
    connect(ui->pushButtonMoveWPos,SIGNAL(released()),this,SLOT(sendStopMoving()));
    connect(ui->pushButtonMoveWNeg,SIGNAL(released()),this,SLOT(sendStopMoving()));
    connect(ui->pushButtonMoveSTOP,SIGNAL(pressed()),this,SLOT(sendStopMoving()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Log(const QString &s)
{
    ui->textEditLog->append(s);
}

void MainWindow::errorLog(const QString &s)
{
    ui->textEditLog_error->append(s);
}

void MainWindow::ProcesCommand(const char command,const float value1,const float value2,const float value3,const float value4)
{
    switch(command)
    {
    case 'm'://set the actual position
        ui->label_XPos->setNum(value1);
        ui->label_YPos->setNum(value2);
        ui->label_ZPos->setNum(value3);
        ui->label_WPos->setNum(value4);
        Log("recive act Pos X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'q'://set the actual position
        ui->label_actSpeed->setNum(value1);
        ui->label_actTemperatur->setNum(value2);
        ui->label_actFilament->setNum(value3);
        ui->label_actPower->setNum(value4);
        Log("recive Settings Speed:"+QString::number(value1)+" Temperatur:"+QString::number(value2)+" Filament:"+QString::number(value3));
        break;
    case 'w':
        Log("recive StepTimes X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'j':
        Log("recive SollPosi X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'k':
        Log("recive SollStep X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'b':
        Log("recive actStep X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    case 'e':
        endswitchButtonColor(value1,ui->pushButtonMoveXPos,ui->pushButtonMoveXNeg);
        endswitchButtonColor(value2,ui->pushButtonMoveYPos,ui->pushButtonMoveYNeg);
        endswitchButtonColor(value3,ui->pushButtonMoveZPos,ui->pushButtonMoveZNeg);
        Log("recive endswitch X:"+QString::number(value1)+" Y:"+QString::number(value2)+" Z:"+QString::number(value3)+" W:"+QString::number(value4));
        break;
    default:
        break;
    }
}

//paints the movebuttons red or green depending of the endswitches
void MainWindow::endswitchButtonColor(float value,QPushButton *PosButton,QPushButton *NegButton)
{
    if(value>0.5){
        PosButton->setStyleSheet("background-color: red");
        NegButton->setStyleSheet("background-color: green");
    }else if (value< -0.5){
        PosButton->setStyleSheet("background-color: green");
        NegButton->setStyleSheet("background-color: red");
    }else if (abs(value) < 0.4){
        PosButton->setStyleSheet("background-color: green");
        NegButton->setStyleSheet("background-color: green");
    }
}


void MainWindow::on_pushButtonSerialConnect_clicked()
{
    if(m_Serial.open(ui->comboBoxComPortName->currentText()))
    {
        ui->comboBoxComPortName->setEnabled(false);
        ui->pushButtonSerialConnect->setStyleSheet("background-color: green");
    }
    else
    {
        ui->comboBoxComPortName->setEnabled(true);
        ui->pushButtonSerialConnect->setStyleSheet("background-color: red");
    }

}

void MainWindow::sendStopMoving()
{
    m_Serial.send('b',0,0,0,0);
}

void MainWindow::on_pushButtonMoveXPos_pressed()
{
    float Y =ui->label_YPos->text().toFloat();
    float Z =ui->label_ZPos->text().toFloat();
    float W =ui->label_WPos->text().toFloat();
    m_Serial.send('m',9999,Y,Z,W);
}

void MainWindow::on_pushButtonMoveXNeg_pressed()
{
    float Y =ui->label_YPos->text().toFloat();
    float Z =ui->label_ZPos->text().toFloat();
    float W =ui->label_WPos->text().toFloat();
    m_Serial.send('m',-9999,Y,Z,W);
}

void MainWindow::on_pushButtonMoveYPos_pressed()
{
    float X =ui->label_XPos->text().toFloat();
    float Z =ui->label_ZPos->text().toFloat();
    float W =ui->label_WPos->text().toFloat();
    m_Serial.send('m',X,9999,Z,W);
}

void MainWindow::on_pushButtonMoveYNeg_pressed()
{
    float X =ui->label_XPos->text().toFloat();
    float Z =ui->label_ZPos->text().toFloat();
    float W =ui->label_WPos->text().toFloat();
    m_Serial.send('m',X,-9999,Z,W);
}

void MainWindow::on_pushButtonMoveZPos_pressed()
{
    float X =ui->label_XPos->text().toFloat();
    float Y =ui->label_YPos->text().toFloat();
    float W =ui->label_WPos->text().toFloat();
    m_Serial.send('m',X,Y,9999,W);
}

void MainWindow::on_pushButtonMoveZNeg_pressed()
{
    float X =ui->label_XPos->text().toFloat();
    float Y =ui->label_YPos->text().toFloat();
    float W =ui->label_WPos->text().toFloat();
    m_Serial.send('m',X,Y,-9999,W);
}

void MainWindow::on_pushButtonMoveWPos_pressed()
{
    float X =ui->label_XPos->text().toFloat();
    float Y =ui->label_YPos->text().toFloat();
    float Z =ui->label_ZPos->text().toFloat();
    m_Serial.send('m',X,Y,Z,9999);
}

void MainWindow::on_pushButtonMoveWNeg_pressed()
{
    float X =ui->label_XPos->text().toFloat();
    float Y =ui->label_YPos->text().toFloat();
    float Z =ui->label_ZPos->text().toFloat();
    m_Serial.send('m',X,Y,Z,-9999);
}

void MainWindow::on_pushButtonCopyToMoveTo_pressed()
{
    float X =ui->label_XPos->text().toDouble();
    float Y =ui->label_YPos->text().toDouble();
    float Z =ui->label_ZPos->text().toDouble();
    float W =ui->label_WPos->text().toDouble();
    ui->doubleSpinBoxMoveX->setValue(X);
    ui->doubleSpinBoxMoveY->setValue(Y);
    ui->doubleSpinBoxMoveZ->setValue(Z);
    ui->doubleSpinBoxMoveW->setValue(W);
}

void MainWindow::on_pushButtonCopyToSetPosition_pressed()
{
    float X =ui->label_XPos->text().toDouble();
    float Y =ui->label_YPos->text().toDouble();
    float Z =ui->label_ZPos->text().toDouble();
    float W =ui->label_WPos->text().toDouble();
    ui->doubleSpinBoxSetX->setValue(X);
    ui->doubleSpinBoxSetY->setValue(Y);
    ui->doubleSpinBoxSetZ->setValue(Z);
    ui->doubleSpinBoxSetW->setValue(W);
}

void MainWindow::on_pushButtonGetPosition_pressed()
{
    m_Serial.send('p',0,0,0,0);
}

void MainWindow::on_pushButtonMoveTo_pressed()
{
    float X = ui->doubleSpinBoxMoveX->value();
    float Y = ui->doubleSpinBoxMoveY->value();
    float Z = ui->doubleSpinBoxMoveZ->value();
    float W = ui->doubleSpinBoxMoveW->value();
    m_Serial.send('m',X,Y,Z,W);
}

void MainWindow::on_pushButtonSetPosition_pressed()
{
    float X = ui->doubleSpinBoxSetX->value();
    float Y = ui->doubleSpinBoxSetY->value();
    float Z = ui->doubleSpinBoxSetZ->value();
    float W = ui->doubleSpinBoxSetW->value();
    m_Serial.send('n',X,Y,Z,W);
}

void MainWindow::on_pushButtonSetSettings_pressed()
{
    float Speed = ui->spinBoxSpeed->value();
    float Temperatur = ui->spinBoxTemperatur->value();
    float Filament = ui->spinBoxFilament->value();
    m_Serial.send('s',Speed,Temperatur,Filament,0);
}

void MainWindow::on_pushButton_browseGCode_pressed()
{
    QString directory = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Open File"),QDir::currentPath(),tr("G-Code (*.gcode)")));
    //QString directory = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Open File"),QString("C:\Users\yanni\Documents\G-Code"),tr("G-Code (*.gcode)")));

    Log(directory);
    if (!directory.isEmpty())
    {
        ui->lineEdit_fileGCode->setText(directory);
    }
}

void MainWindow::on_pushButton_startGCode_pressed()
{
    m_CNC_auto.GCode_Parser(ui->lineEdit_fileGCode->text());
}
