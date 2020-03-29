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

    ui->spinBoxSpeed->setValue(50);
    ui->spinBoxFilament->setValue(35);

    m_database->m_soll_speed = ui->spinBoxSpeed->value();
    m_database->m_soll_temperatur = ui->spinBoxTemperatur->value();
    m_database->m_soll_filament = ui->spinBoxFilament->value();

    //chek availabal port add to the comboBox
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        ui->comboBoxComPortName->addItem(info.portName());
    //
    //conect manual moving buttons for stop action by releasing
    connect(ui->pushButtonMoveXPos,SIGNAL(released()),this,SLOT(sendStopMoving()));
    connect(ui->pushButtonMoveXNeg,SIGNAL(released()),this,SLOT(sendStopMoving()));
    connect(ui->pushButtonMoveYPos,SIGNAL(released()),this,SLOT(sendStopMoving()));
    connect(ui->pushButtonMoveYNeg,SIGNAL(released()),this,SLOT(sendStopMoving()));
    connect(ui->pushButtonMoveZPos,SIGNAL(released()),this,SLOT(sendStopMoving()));
    connect(ui->pushButtonMoveZNeg,SIGNAL(released()),this,SLOT(sendStopMoving()));
    connect(ui->pushButtonMoveWPos,SIGNAL(released()),this,SLOT(sendStopMoving()));
    connect(ui->pushButtonMoveWNeg,SIGNAL(released()),this,SLOT(sendStopMoving()));
    connect(ui->pushButtonMoveSTOP,SIGNAL(pressed()),this,SLOT(sendStopMoving()));

    connect(m_basefunctions,SIGNAL(Log(QString)),this,SLOT(Log(QString)));
    connect(m_basefunctions,SIGNAL(errorLog(QString)),this,SLOT(errorLog(QString)));
    connect(m_basefunctions,SIGNAL(trigger_send()),m_serial,SLOT(serial_send_command()));
    connect(m_basefunctions,SIGNAL(show_send_queue()),this,SLOT(show_send_queue()));

    connect(m_database,SIGNAL(Log(QString)),this,SLOT(Log(QString)));
    connect(m_database,SIGNAL(errorLog(QString)),this,SLOT(errorLog(QString)));
    connect(m_database,SIGNAL(show_position()),this,SLOT(show_position()));
    connect(m_database,SIGNAL(show_settings()),this,SLOT(show_settings()));
    connect(m_database,SIGNAL(show_endswitch(float,float,float)),this,SLOT(show_endswitch(float,float,float)));
    connect(m_database,SIGNAL(show_serial(bool)),this,SLOT(show_serial(bool)));
    //connect(m_database,SIGNAL(show_status()),this,SLOT(show_status()));
    connect(m_database,SIGNAL(recive_command()),m_basefunctions,SLOT(process_command()));

    connect(m_serial,SIGNAL(Log(QString)),this,SLOT(Log(QString)));
    connect(m_serial,SIGNAL(errorLog(QString)),this,SLOT(errorLog(QString)));
    connect(m_serial,SIGNAL(show_send_queue()),this,SLOT(show_send_queue()));

    connect(m_automation,SIGNAL(Log(QString)),this,SLOT(Log(QString)));
    connect(m_automation,SIGNAL(errorLog(QString)),this,SLOT(errorLog(QString)));
    connect(m_automation,SIGNAL(trigger_send()),m_serial,SLOT(serial_send_command()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Log(const QString &s)
{
    QString text = QDateTime::currentDateTime().toString("hh:mm:ss.zzz ") + s;
    ui->textEditLog->append(text);
}

void MainWindow::test()
{
    ui->textEditLog->append("trigger sending manuel");
    m_basefunctions->HW_is_working = false;
    m_basefunctions->trigger_next_command();
}

void MainWindow::errorLog(const QString &s)
{
    ui->textEditLog_error->append(s);
}


void MainWindow::show_send_queue()
{
    if(m_database->cnc_send_commands.size()==0)
    {
        ui->label_Queue->setStyleSheet("background-color: red");
    }else{
        ui->label_Queue->setStyleSheet("background-color: green");
    }
    ui->label_Queue->setText("Queue_count: "+QString::number(m_database->cnc_send_commands.size()));
}

void MainWindow::show_position()
{
    ui->label_XPos->setText(QString::number(m_database->m_act_X));
    ui->label_YPos->setText(QString::number(m_database->m_act_Y));
    ui->label_ZPos->setText(QString::number(m_database->m_act_Z));
    ui->label_WPos->setText(QString::number(m_database->m_act_W));
}

void MainWindow::show_settings()
{
    ui->label_actSpeed->setText(QString::number(m_database->m_act_speed)+"/"+QString::number(m_database->m_soll_speed));
    ui->label_actTemperatur->setText(QString::number(m_database->m_act_temperatur)+"/"+QString::number(m_database->m_soll_temperatur));
    ui->label_actFilament->setText(QString::number(m_database->m_act_filament)+"/"+QString::number(m_database->m_soll_filament));

    if(m_alive)
    {
        m_alive = false;
        ui->label_alive->setStyleSheet("background-color: blue");

    }
    else
    {
        m_alive = true;
        ui->label_alive->setStyleSheet("background-color: lightblue");
    }
}

void MainWindow::show_serial(bool isOpen)
{
    if(isOpen)
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

void MainWindow::show_endswitch(float X, float Y, float Z)
{
    endswitchButtonColor(X,ui->pushButtonMoveXPos,ui->pushButtonMoveXNeg);
    endswitchButtonColor(Y,ui->pushButtonMoveYPos,ui->pushButtonMoveYNeg);
    endswitchButtonColor(Z,ui->pushButtonMoveZPos,ui->pushButtonMoveZNeg);
}

void MainWindow::show_status()
{
    if(m_database->m_HWisMoving)
    {
        ui->label_Queue->setStyleSheet("background-color: green");
    }
    else
    {
        ui->label_Queue->setStyleSheet("background-color: red");
    }
    if(m_database->m_HWisHeating)
    {
        ui->label_Queue->setStyleSheet("background-color: green");
    }
    else
    {
        ui->label_Queue->setStyleSheet("background-color: red");
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

void MainWindow::sendStopMoving()
{
    m_basefunctions->send_stop();
}

void MainWindow::on_pushButtonSerialConnect_clicked()
{
    if(m_database->m_SerialIsOpen)
    {
        Log("close serial");
        m_serial->serial_close();
    }
    else
    {
        Log("open serial");
        m_basefunctions->HW_is_working = false;
        m_database->m_SerialPortName = ui->comboBoxComPortName->currentText();
        m_serial->serial_open();
        m_basefunctions->send_init();
    }
}

void MainWindow::on_pushButtonMoveXPos_pressed()
{
    float Y = m_database->m_act_Y;
    float Z = m_database->m_act_Z;
    float W = m_database->m_act_W;
    m_basefunctions->send_move(9999,Y,Z,W);
}

void MainWindow::on_pushButtonMoveXNeg_pressed()
{
    float Y = m_database->m_act_Y;
    float Z = m_database->m_act_Z;
    float W = m_database->m_act_W;
    m_basefunctions->send_move(-9999,Y,Z,W);
}

void MainWindow::on_pushButtonMoveYPos_pressed()
{
    float X = m_database->m_act_X;
    float Z = m_database->m_act_Z;
    float W = m_database->m_act_W;
    m_basefunctions->send_move(X,9999,Z,W);
}

void MainWindow::on_pushButtonMoveYNeg_pressed()
{
    float X = m_database->m_act_X;
    float Z = m_database->m_act_Z;
    float W = m_database->m_act_W;
    m_basefunctions->send_move(X,-9999,Z,W);
}

void MainWindow::on_pushButtonMoveZPos_pressed()
{
    float X = m_database->m_act_X;
    float Y = m_database->m_act_Y;
    float W = m_database->m_act_W;
    m_basefunctions->send_move(X,Y,9999,W);
}

void MainWindow::on_pushButtonMoveZNeg_pressed()
{
    float X = m_database->m_act_X;
    float Y = m_database->m_act_Y;
    float W = m_database->m_act_W;
    m_basefunctions->send_move(X,Y,-9999,W);
}

void MainWindow::on_pushButtonMoveWPos_pressed()
{
    float X = m_database->m_act_X;
    float Y = m_database->m_act_Y;
    float Z = m_database->m_act_Z;
    m_basefunctions->send_move(X,Y,Z,9999);
}

void MainWindow::on_pushButtonMoveWNeg_pressed()
{
    float X = m_database->m_act_X;
    float Y = m_database->m_act_Y;
    float Z = m_database->m_act_Z;
    m_basefunctions->send_move(X,Y,Z,-9999);
}

void MainWindow::on_pushButtonCopyToMoveTo_pressed()
{
    float X = m_database->m_act_X;
    float Y = m_database->m_act_Y;
    float Z = m_database->m_act_Z;
    float W = m_database->m_act_W;
    ui->doubleSpinBoxMoveX->setValue(X);
    ui->doubleSpinBoxMoveY->setValue(Y);
    ui->doubleSpinBoxMoveZ->setValue(Z);
    ui->doubleSpinBoxMoveW->setValue(W);
}

void MainWindow::on_pushButtonCopyToSetPosition_pressed()
{
    float X = m_database->m_act_X;
    float Y = m_database->m_act_Y;
    float Z = m_database->m_act_Z;
    float W = m_database->m_act_W;
    ui->doubleSpinBoxSetX->setValue(X);
    ui->doubleSpinBoxSetY->setValue(Y);
    ui->doubleSpinBoxSetZ->setValue(Z);
    ui->doubleSpinBoxSetW->setValue(W);
}

void MainWindow::on_pushButtonGetPosition_pressed()
{
    m_basefunctions->send_getPosition();
}

void MainWindow::on_pushButtonMoveTo_pressed()
{
    float X = ui->doubleSpinBoxMoveX->value();
    float Y = ui->doubleSpinBoxMoveY->value();
    float Z = ui->doubleSpinBoxMoveZ->value();
    float W = ui->doubleSpinBoxMoveW->value();
    m_basefunctions->send_move(X,Y,Z,W);
}

void MainWindow::on_pushButtonSetPosition_pressed()
{
    float X = ui->doubleSpinBoxSetX->value();
    float Y = ui->doubleSpinBoxSetY->value();
    float Z = ui->doubleSpinBoxSetZ->value();
    float W = ui->doubleSpinBoxSetW->value();
    m_basefunctions->send_setPosition(X,Y,Z,W);
}

void MainWindow::on_pushButtonSetSettings_pressed()
{
    float Speed = ui->spinBoxSpeed->value();
    float Temperatur = ui->spinBoxTemperatur->value();
    float Filament = ui->spinBoxFilament->value();
    m_basefunctions->send_settings(Speed,Temperatur,Filament);
}

void MainWindow::on_pushButton_home_pressed()
{
    m_automation->move_home();
}

void MainWindow::on_pushButton_sizecalib_pressed()
{
    m_automation->calib_size();
}

void MainWindow::on_pushButton_repeattest_pressed()
{
    m_automation->repeat_test();
}

void MainWindow::on_pushButton_Zcalib_pressed()
{
    m_automation->Z_calib();
}

void MainWindow::on_pushButton_browseGCode_pressed()
{
    QString directory = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Open File"),QDir::currentPath(),tr("G-Code (*.gcode)")));

    Log(directory);
    if (!directory.isEmpty())
    {
        ui->lineEdit_fileGCode->setText(directory);
    }
}

void MainWindow::on_pushButton_startGCode_pressed()
{
    m_automation->G_Code_Start(ui->lineEdit_fileGCode->text());
}

void MainWindow::on_pushButton_pauseGCode_pressed()
{
    m_automation->G_Code_Pause();
}

void MainWindow::on_pushButton_AboardGCode_pressed()
{
    m_automation->G_Code_Stop();
}

void MainWindow::on_pushButton_test_pressed()
{
    //m_database->test();
    test();
}

void MainWindow::on_pushButton_rest_pressed()
{
    m_automation->move_restposi();
}

void MainWindow::on_spinBoxSpeed_valueChanged(int arg1)
{
    m_database->m_soll_speed = arg1;
}

void MainWindow::on_spinBoxTemperatur_valueChanged(int arg1)
{
    m_database->m_soll_temperatur = arg1;
}

void MainWindow::on_spinBoxFilament_valueChanged(int arg1)
{
    m_database->m_soll_filament = arg1;
}

void MainWindow::on_pushButton_clear_queue_clicked()
{
    m_database->cnc_send_commands.clear();
    show_send_queue();
}

void MainWindow::on_pushButton_trigger_next_clicked()
{
    ui->textEditLog->append("trigger sending manuel");
    m_basefunctions->HW_is_working = false;
    m_basefunctions->trigger_next_command();
    show_send_queue();
}
