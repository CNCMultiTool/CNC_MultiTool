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

    ui->spinBoxSpeed->setValue(m_database->m_max_speed);
    ui->spinBoxFilament->setValue(m_database->m_soll_filament);

    m_database->loadSettings();
    ui->spinBoxSpeed->setValue(m_database->m_soll_speed);
    ui->spinBoxTemperatur->setValue(m_database->m_soll_temperatur);
    ui->spinBoxFilament->setValue(m_database->m_soll_filament);
    ui->spinBoxBedTemp->setValue(m_database->m_soll_bedTemp);


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
    connect(m_basefunctions,SIGNAL(DebugLog(QString)),this,SLOT(DebugLog(QString)));
    connect(m_basefunctions,SIGNAL(trigger_send()),m_serial,SLOT(serial_send_command()));
    connect(m_basefunctions,SIGNAL(show_send_queue()),this,SLOT(show_send_queue()));
    connect(m_basefunctions,SIGNAL(show_status()),this,SLOT(show_status()));
    connect(m_basefunctions,SIGNAL(DataToGraph(float,float,float,float)),this,SLOT(addToGraph(float,float,float,float)));
    connect(m_basefunctions,SIGNAL(show_alive()),this,SLOT(show_alive()));
    connect(m_basefunctions,SIGNAL(z_calib_resullt_finish()),this,SLOT(show_z_calib_result()));


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
    connect(m_serial,SIGNAL(show_alive()),this,SLOT(show_alive()));

    connect(m_automation,SIGNAL(Log(QString)),this,SLOT(Log(QString)));
    connect(m_automation,SIGNAL(errorLog(QString)),this,SLOT(errorLog(QString)));
    connect(m_automation,SIGNAL(trigger_send()),m_serial,SLOT(serial_send_command()));
    connect(m_automation,SIGNAL(resend_last()),m_serial,SLOT(send_last()));

    connect(ui->actionValues,SIGNAL(triggered()),this,SLOT(calibratenValueBox()));
    connect(ui->actionsettings_repeattest,SIGNAL(triggered()),this,SLOT(repeattestValueBox()));
    connect(ui->actionexecute,SIGNAL(triggered()),this,SLOT(on_pushButton_repeattest_pressed()));
    connect(ui->actionSet_PID,SIGNAL(triggered()),this,SLOT(PID_ValueBox()));
    connect(ui->actionSet_PID_for_Bed,SIGNAL(triggered()),this,SLOT(PID_Bed_ValueBox()));
    connect(ui->actionBedCalib,SIGNAL(triggered()),this,SLOT(bedCalibValueBox()));


    //SEtupPlot
    ui->Plot_PID_Temp->addGraph(); // blue line
    ui->Plot_PID_Temp->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    ui->Plot_PID_Temp->addGraph(); // red line
    ui->Plot_PID_Temp->graph(1)->setPen(QPen(QColor(255, 110, 40)));
    ui->Plot_PID_Temp->addGraph(); // red line
    ui->Plot_PID_Temp->graph(2)->setPen(QPen(QColor(60, 110, 40)));
    ui->Plot_PID_Temp->addGraph(); // red line
    ui->Plot_PID_Temp->graph(3)->setPen(QPen(QColor(255, 110, 200)));
    time.start();
    ui->Plot_PID_Temp->yAxis->setRange(-10, 260);
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->Plot_PID_Temp->xAxis->setTicker(timeTicker);
    ui->Plot_PID_Temp->axisRect()->setupFullAxesBox();

    m_database->SerialLog("new Start of The Programm");
}

MainWindow::~MainWindow()
{
    m_database->SerialLog("Close of The Programm");
    m_database->saveSettings();
    delete ui;
}

void MainWindow::Log(const QString &s)
{
    QString text = QDateTime::currentDateTime().toString("hh:mm:ss.zzz ") + s;
    ui->textEditLog->append(text);
}

void MainWindow::DebugLog(const QString &s)
{
    QString text = QDateTime::currentDateTime().toString("hh:mm:ss.zzz ") + s;
    ui->textEditDebugLog->append(text);
}

void MainWindow::errorLog(const QString &s)
{
    QString text = QDateTime::currentDateTime().toString("hh:mm:ss.zzz ") + s;
    ui->textEditLog_error->append(text);
}


void MainWindow::show_send_queue()
{
    if(m_database->cnc_send_commands.size()==0)
    {
        ui->label_Queue->setStyleSheet("background-color: red");
        ui->pushButton_startGCode->setStyleSheet("background-color: red");
        m_database->m_G_Code_State = 0;
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
    ui->label_bedTemp->setText(QString::number(m_database->m_act_bedTemp)+"/"+QString::number(m_database->m_soll_bedTemp));
}

void MainWindow::show_alive()
{
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

void MainWindow::show_z_calib_result()
{
    ui->label_bedCalibP1->setText("P1 X: "+QString::number(m_database->m_X1)+
                                  "  Y: "+QString::number(m_database->m_Y1)+
                                  "  Z: "+QString::number(m_database->m_Z1));
    ui->label_bedCalibP2->setText("P2 X: "+QString::number(m_database->m_X2)+
                                  "  Y: "+QString::number(m_database->m_Y2)+
                                  "  Z: "+QString::number(m_database->m_Z2));
    ui->label_bedCalibP3->setText("P3 X: "+QString::number(m_database->m_X3)+
                                  "  Y: "+QString::number(m_database->m_Y3)+
                                  "  Z: "+QString::number(m_database->m_Z3));
    ui->label_bedCalibP4->setText("P4 X: "+QString::number(m_database->m_X4)+
                                  "  Y: "+QString::number(m_database->m_Y4)+
                                  "  Z: "+QString::number(m_database->m_Z4));
    ui->label_bedCalibP5->setText("P5 Z error: "+QString::number(m_database->m_Z1_error - m_database->m_Z1));
}

void MainWindow::show_status()
{
    switch(m_database->m_HW_status)
    {
    case 0:
        ui->label_Queue_state->setStyleSheet("background-color: green");
        break;
    case 1:
        ui->label_Queue_state->setStyleSheet("background-color: blue");
        break;
    case 2:
        ui->label_Queue_state->setStyleSheet("background-color: yellow");
        break;
    default:
        ui->label_Queue_state->setStyleSheet("background-color: red");
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
        m_database->m_SerialPortName = ui->comboBoxComPortName->currentText();
        m_serial->serial_open();
        m_basefunctions->send_init();
        m_basefunctions->send_PID(m_database->m_KP,m_database->m_KI,m_database->m_KD,m_database->m_POn);
        m_basefunctions->send_Temp_Setting(m_database->m_R_vor,m_database->m_bValue,m_database->m_R_nen);
        m_basefunctions->send_PID_Bed(m_database->m_KP,m_database->m_KI,m_database->m_KD,m_database->m_POn);
        m_basefunctions->send_Temp_Setting_Bed(m_database->m_R_vor,m_database->m_bValue,m_database->m_R_nen);
        m_basefunctions->send_settings(m_database->m_soll_speed,-1,m_database->m_soll_filament,m_database->m_soll_bedTemp);
        m_basefunctions->send_acc(m_database->m_max_acc,m_database->m_speed_min);
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
    float BedTetmp = ui->spinBoxBedTemp->value();
    m_database->set_soll_settings(Speed,Temperatur,Filament,BedTetmp);
    m_basefunctions->send_settings(Speed,Temperatur,Filament,BedTetmp);
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
    m_serial->stop_timeouts();
    QString directory = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Open File"),QDir::currentPath(),tr("G-Code (*.gcode)")));

    Log(directory);
    if (!directory.isEmpty())
    {
        ui->lineEdit_fileGCode->setText(directory);
    }
}

void MainWindow::on_pushButton_startGCode_pressed()
{
    ui->pushButton_startGCode->setStyleSheet("background-color: green");
    m_automation->G_Code_Start(ui->lineEdit_fileGCode->text());
}

void MainWindow::on_pushButton_pauseGCode_pressed()
{
    ui->pushButton_startGCode->setStyleSheet("background-color: yellow");
    m_automation->G_Code_Pause();
}

void MainWindow::on_pushButton_AboardGCode_pressed()
{
    ui->pushButton_startGCode->setStyleSheet("background-color: red");
    m_automation->G_Code_Stop();
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
    m_basefunctions->trigger_next_command();
    show_send_queue();
}

void MainWindow::repeattestValueBox()
{
    Log("show checkbox");
    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

    //int m_repeat1,m_speed1;
    //float m_x11,m_y11,m_z11,m_x12,m_y12,m_z12;

    QSpinBox *SpinBox_repeat1 = new QSpinBox(&dialog);
    SpinBox_repeat1->setRange(0,999999);
    SpinBox_repeat1->setValue(m_database->m_repeat1);
    form.addRow("repeats ", SpinBox_repeat1);
    QSpinBox *SpinBox_speed1 = new QSpinBox(&dialog);
    SpinBox_speed1->setRange(0,999999);
    SpinBox_speed1->setValue(m_database->m_speed1);
    form.addRow("speed ",SpinBox_speed1);

    QDoubleSpinBox *DSpinBox_x11 = new QDoubleSpinBox(&dialog);
    DSpinBox_x11->setRange(-9999999,999999);
    DSpinBox_x11->setValue(m_database->m_x11);
    form.addRow("P1 X", DSpinBox_x11);
    QDoubleSpinBox *DSpinBox_y11 = new QDoubleSpinBox(&dialog);
    DSpinBox_y11->setRange(-9999999,999999);
    DSpinBox_y11->setValue(m_database->m_y11);
    form.addRow("P1 Y", DSpinBox_y11);
    QDoubleSpinBox *DSpinBox_z11 = new QDoubleSpinBox(&dialog);
    DSpinBox_z11->setRange(-9999999,999999);
    DSpinBox_z11->setValue(m_database->m_z11);
    form.addRow("P1 Z", DSpinBox_z11);

    QDoubleSpinBox *DSpinBox_x12 = new QDoubleSpinBox(&dialog);
    DSpinBox_x12->setRange(-9999999,999999);
    DSpinBox_x12->setValue(m_database->m_x12);
    form.addRow("P2 X", DSpinBox_x12);
    QDoubleSpinBox *DSpinBox_y12 = new QDoubleSpinBox(&dialog);
    DSpinBox_y12->setRange(-9999999,999999);
    DSpinBox_y12->setValue(m_database->m_y12);
    form.addRow("P2 Y", DSpinBox_y12);
    QDoubleSpinBox *DSpinBox_z12 = new QDoubleSpinBox(&dialog);
    DSpinBox_z12->setRange(-9999999,999999);
    DSpinBox_z12->setValue(m_database->m_z12);
    form.addRow("P2 Z", DSpinBox_z12);

    QDialogButtonBox buttonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted) {
        m_database->m_repeat1 = SpinBox_repeat1->value();
        m_database->m_speed1 = SpinBox_speed1->value();
        m_database->m_x11 = DSpinBox_x11->value();
        m_database->m_y11 = DSpinBox_y11->value();
        m_database->m_z11 = DSpinBox_z11->value();
        m_database->m_x12 = DSpinBox_x12->value();
        m_database->m_y12 = DSpinBox_y12->value();
        m_database->m_z12 = DSpinBox_z12->value();

        m_database->saveSettings();
    }

}

void MainWindow::bedCalibValueBox()
{
    Log("BedCalib");
    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

    //float m_X1,m_Y1,m_Z1;
    //float m_X2,m_Y2,m_Z2;
    //float m_X3,m_Y3,m_Z3;
    //float m_X4,m_Y4,m_Z4;

    QDoubleSpinBox *DSpinBox_x1 = new QDoubleSpinBox(&dialog);
    DSpinBox_x1->setRange(-9999999,999999);
    DSpinBox_x1->setValue(m_database->m_X1);
    form.addRow("P1 X", DSpinBox_x1);
    QDoubleSpinBox *DSpinBox_y1 = new QDoubleSpinBox(&dialog);
    DSpinBox_y1->setRange(-9999999,999999);
    DSpinBox_y1->setValue(m_database->m_Y1);
    form.addRow("P1 Y", DSpinBox_y1);

    QDoubleSpinBox *DSpinBox_x2 = new QDoubleSpinBox(&dialog);
    DSpinBox_x2->setRange(-9999999,999999);
    DSpinBox_x2->setValue(m_database->m_X2);
    form.addRow("P2 X", DSpinBox_x2);
    QDoubleSpinBox *DSpinBox_y2 = new QDoubleSpinBox(&dialog);
    DSpinBox_y2->setRange(-9999999,999999);
    DSpinBox_y2->setValue(m_database->m_Y2);
    form.addRow("P2 Y", DSpinBox_y2);

    QDoubleSpinBox *DSpinBox_x3 = new QDoubleSpinBox(&dialog);
    DSpinBox_x3->setRange(-9999999,999999);
    DSpinBox_x3->setValue(m_database->m_X3);
    form.addRow("P3 X", DSpinBox_x3);
    QDoubleSpinBox *DSpinBox_y3 = new QDoubleSpinBox(&dialog);
    DSpinBox_y3->setRange(-9999999,999999);
    DSpinBox_y3->setValue(m_database->m_Y3);
    form.addRow("P3 Y", DSpinBox_y3);

    QDoubleSpinBox *DSpinBox_x4 = new QDoubleSpinBox(&dialog);
    DSpinBox_x4->setRange(-9999999,999999);
    DSpinBox_x4->setValue(m_database->m_X4);
    form.addRow("P4 X", DSpinBox_x4);
    QDoubleSpinBox *DSpinBox_y4 = new QDoubleSpinBox(&dialog);
    DSpinBox_y4->setRange(-9999999,999999);
    DSpinBox_y4->setValue(m_database->m_Y4);
    form.addRow("P4 Y", DSpinBox_y4);

    QDialogButtonBox buttonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted) {
        m_database->m_X1 = DSpinBox_x1->value();
        m_database->m_Y1 = DSpinBox_y1->value();

        m_database->m_X2 = DSpinBox_x2->value();
        m_database->m_Y2 = DSpinBox_y2->value();

        m_database->m_X3 = DSpinBox_x3->value();
        m_database->m_Y3 = DSpinBox_y3->value();

        m_database->m_X4 = DSpinBox_x4->value();
        m_database->m_Y4 = DSpinBox_y4->value();

        m_database->saveSettings();
    }

}

void MainWindow::calibratenValueBox()
{
    Log("show checkbox");
    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

    // Add the lineEdits with their respective label
    QDoubleSpinBox *DSpinBox_size_X = new QDoubleSpinBox(&dialog);
    DSpinBox_size_X->setRange(-9999999,999999);
    DSpinBox_size_X->setValue(m_database->m_size_X);
    form.addRow("X Size", DSpinBox_size_X);

    QDoubleSpinBox *DSpinBox_size_Y = new QDoubleSpinBox(&dialog);
    DSpinBox_size_Y->setRange(-9999999,999999);
    DSpinBox_size_Y->setValue(m_database->m_size_Y);
    form.addRow("Y Size", DSpinBox_size_Y);

    //nozzel home high
    QDoubleSpinBox *DSpinBox_TCP_X_inHome = new QDoubleSpinBox(&dialog);
    DSpinBox_TCP_X_inHome->setRange(-9999999,999999);
    DSpinBox_TCP_X_inHome->setValue(m_database->m_X_inHome);
    form.addRow("Nozzel Home X", DSpinBox_TCP_X_inHome);

    //nozzel home high
    QDoubleSpinBox *DSpinBox_TCP_Y_inHome = new QDoubleSpinBox(&dialog);
    DSpinBox_TCP_Y_inHome->setRange(-9999999,999999);
    DSpinBox_TCP_Y_inHome->setValue(m_database->m_Y_inHome);
    form.addRow("Nozzel Home Y", DSpinBox_TCP_Y_inHome);

    //nozzel home high
    QDoubleSpinBox *DSpinBox_TCP_higth = new QDoubleSpinBox(&dialog);
    DSpinBox_TCP_higth->setRange(-9999999,999999);
    DSpinBox_TCP_higth->setValue(m_database->m_Zmax_nozzel);
    form.addRow("Nozzel Home Z", DSpinBox_TCP_higth);


    //make nozzel calib
    QCheckBox *CheckBox_makeCalib = new QCheckBox(&dialog);
    CheckBox_makeCalib->setCheckState(m_database->m_useCalibPlate == true ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    form.addRow("use Calibplate", CheckBox_makeCalib);

    //nozzel calib plate X
    QDoubleSpinBox *DSpinBox_calibplateX = new QDoubleSpinBox(&dialog);
    DSpinBox_calibplateX->setRange(-9999999,999999);
    DSpinBox_calibplateX->setDecimals(2);
    DSpinBox_calibplateX->setValue(m_database->m_calibplateX);
    form.addRow("X of Calibplate", DSpinBox_calibplateX);

    //nozzel calib plate Y
    QDoubleSpinBox *DSpinBox_calibplateY = new QDoubleSpinBox(&dialog);
    DSpinBox_calibplateY->setRange(-9999999,999999);
    DSpinBox_calibplateY->setDecimals(2);
    DSpinBox_calibplateY->setValue(m_database->m_calibplateY);
    form.addRow("Y of Calibplate", DSpinBox_calibplateY);

    //nozzel calib plate Z
    QDoubleSpinBox *DSpinBox_calibplateZ = new QDoubleSpinBox(&dialog);
    DSpinBox_calibplateZ->setRange(-9999999,999999);
    DSpinBox_calibplateZ->setDecimals(2);
    DSpinBox_calibplateZ->setValue(m_database->m_calibplateZ);
    form.addRow("Z to set after Calibplate", DSpinBox_calibplateZ);

    //max speed
    QDoubleSpinBox *DSpinBox_maxSpeed = new QDoubleSpinBox(&dialog);
    DSpinBox_maxSpeed->setRange(-9999999,999999);
    DSpinBox_maxSpeed->setValue(m_database->m_max_speed);
    form.addRow("Max Speed mm/s", DSpinBox_maxSpeed);

    //acc
    QDoubleSpinBox *DSpinBox_max_acc = new QDoubleSpinBox(&dialog);
    DSpinBox_max_acc->setRange(-9999999,999999);
    DSpinBox_max_acc->setValue(m_database->m_max_acc);
    form.addRow("Max Acc mm/s^2", DSpinBox_max_acc);

    //acc
    QDoubleSpinBox *DSpinBox_speed_min = new QDoubleSpinBox(&dialog);
    DSpinBox_speed_min->setRange(-9999999,999999);
    DSpinBox_speed_min->setValue(m_database->m_speed_min);
    form.addRow("Min Speed mm/s^2", DSpinBox_speed_min);

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted) {
        Log("save values");
        m_database->m_size_X = DSpinBox_size_X->value();
        m_database->m_size_Y = DSpinBox_size_Y->value();

        //TCP hight in home
        m_database->m_Zmax_nozzel = DSpinBox_TCP_higth->value();
        m_database->m_X_inHome = DSpinBox_TCP_X_inHome->value();
        m_database->m_Y_inHome = DSpinBox_TCP_Y_inHome->value();
        m_database->m_calibplateX = DSpinBox_calibplateX->value();
        m_database->m_calibplateY = DSpinBox_calibplateY->value();
        m_database->m_calibplateZ = DSpinBox_calibplateZ->value();
        //max values
        m_database->m_max_speed = DSpinBox_maxSpeed->value(); //mm per sec
        m_database->m_max_acc = DSpinBox_max_acc->value(); //mm per sec sec
        m_database->m_speed_min = DSpinBox_speed_min->value(); //mm per sec

        m_database->m_useCalibPlate = CheckBox_makeCalib->isChecked();

        m_basefunctions->send_acc(m_database->m_max_acc,m_database->m_speed_min);

        m_database->saveSettings();
    }
}

void MainWindow::PID_Bed_ValueBox()
{
    Log("show checkbox");
    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

    //KP
    QDoubleSpinBox *DSpinBox_KP_Bed = new QDoubleSpinBox(&dialog);
    DSpinBox_KP_Bed->setRange(-9999999,999999);
    DSpinBox_KP_Bed->setDecimals(2);
    DSpinBox_KP_Bed->setValue(m_database->m_KP_Bed);
    form.addRow("KP", DSpinBox_KP_Bed);

    //KI
    QDoubleSpinBox *DSpinBox_KI_Bed = new QDoubleSpinBox(&dialog);
    DSpinBox_KI_Bed->setRange(-9999999,999999);
    DSpinBox_KI_Bed->setDecimals(2);
    DSpinBox_KI_Bed->setValue(m_database->m_KI_Bed);
    form.addRow("KI", DSpinBox_KI_Bed);

    //KD
    QDoubleSpinBox *DSpinBox_KD_Bed = new QDoubleSpinBox(&dialog);
    DSpinBox_KD_Bed->setRange(-9999999,999999);
    DSpinBox_KD_Bed->setDecimals(2);
    DSpinBox_KD_Bed->setValue(m_database->m_KD_Bed);
    form.addRow("KD", DSpinBox_KD_Bed);

    //make nozzel calib
    QCheckBox *CheckBox_POn_Bed = new QCheckBox(&dialog);
    CheckBox_POn_Bed->setCheckState(m_database->m_POn_Bed == true ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    form.addRow("POn", CheckBox_POn_Bed);

    //Vorwiederstand
    QDoubleSpinBox *DSpinBox_m_R_vor_Bed = new QDoubleSpinBox(&dialog);
    DSpinBox_m_R_vor_Bed->setRange(-9999999,999999);
    DSpinBox_m_R_vor_Bed->setDecimals(2);
    DSpinBox_m_R_vor_Bed->setValue(m_database->m_R_vor_Bed);
    form.addRow("Vorwiederstand", DSpinBox_m_R_vor_Bed);

    //Vorwiederstand
    QDoubleSpinBox *DSpinBox_m_R_nen_Bed = new QDoubleSpinBox(&dialog);
    DSpinBox_m_R_nen_Bed->setRange(-9999999,999999);
    DSpinBox_m_R_nen_Bed->setDecimals(2);
    DSpinBox_m_R_nen_Bed->setValue(m_database->m_R_nen_Bed);
    form.addRow("Nennwiederstand", DSpinBox_m_R_nen_Bed);

    //B Value
    QDoubleSpinBox *DSpinBox_m_bValue_Bed = new QDoubleSpinBox(&dialog);
    DSpinBox_m_bValue_Bed->setRange(-9999999,999999);
    DSpinBox_m_bValue_Bed->setDecimals(2);
    DSpinBox_m_bValue_Bed->setValue(m_database->m_bValue_Bed);
    form.addRow("B-Wert_NTC", DSpinBox_m_bValue_Bed);


    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted) {
        Log("save PID");
        m_database->m_R_vor_Bed = DSpinBox_m_R_vor_Bed->value();
        m_database->m_R_nen_Bed = DSpinBox_m_R_nen_Bed->value();
        m_database->m_bValue_Bed = DSpinBox_m_bValue_Bed->value();

        m_database->m_KP_Bed = DSpinBox_KP_Bed->value();
        m_database->m_KI_Bed = DSpinBox_KI_Bed->value();
        m_database->m_KD_Bed = DSpinBox_KD_Bed->value();

        m_database->m_POn_Bed = CheckBox_POn_Bed->isChecked();

        m_database->saveSettings();

        m_basefunctions->send_PID_Bed(m_database->m_KP,m_database->m_KI,m_database->m_KD,m_database->m_POn);
        m_basefunctions->send_Temp_Setting_Bed(m_database->m_R_vor,m_database->m_bValue,m_database->m_R_nen);
    }
}

void MainWindow::PID_ValueBox()
{
    Log("show checkbox");
    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

    //KP
    QDoubleSpinBox *DSpinBox_KP = new QDoubleSpinBox(&dialog);
    DSpinBox_KP->setRange(-9999999,999999);
    DSpinBox_KP->setDecimals(2);
    DSpinBox_KP->setValue(m_database->m_KP);
    form.addRow("KP", DSpinBox_KP);

    //KI
    QDoubleSpinBox *DSpinBox_KI = new QDoubleSpinBox(&dialog);
    DSpinBox_KI->setRange(-9999999,999999);
    DSpinBox_KI->setDecimals(2);
    DSpinBox_KI->setValue(m_database->m_KI);
    form.addRow("KI", DSpinBox_KI);

    //KD
    QDoubleSpinBox *DSpinBox_KD = new QDoubleSpinBox(&dialog);
    DSpinBox_KD->setRange(-9999999,999999);
    DSpinBox_KD->setDecimals(2);
    DSpinBox_KD->setValue(m_database->m_KD);
    form.addRow("KD", DSpinBox_KD);

    //make nozzel calib
    QCheckBox *CheckBox_POn = new QCheckBox(&dialog);
    CheckBox_POn->setCheckState(m_database->m_POn == true ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    form.addRow("POn", CheckBox_POn);

    //Vorwiederstand
    QDoubleSpinBox *DSpinBox_m_R_vor = new QDoubleSpinBox(&dialog);
    DSpinBox_m_R_vor->setRange(-9999999,999999);
    DSpinBox_m_R_vor->setDecimals(2);
    DSpinBox_m_R_vor->setValue(m_database->m_R_vor);
    form.addRow("Vorwiederstand", DSpinBox_m_R_vor);

    //Vorwiederstand
    QDoubleSpinBox *DSpinBox_m_R_nen = new QDoubleSpinBox(&dialog);
    DSpinBox_m_R_nen->setRange(-9999999,999999);
    DSpinBox_m_R_nen->setDecimals(2);
    DSpinBox_m_R_nen->setValue(m_database->m_R_nen);
    form.addRow("Nennwiederstand", DSpinBox_m_R_nen);

    //B Value
    QDoubleSpinBox *DSpinBox_m_bValue = new QDoubleSpinBox(&dialog);
    DSpinBox_m_bValue->setRange(-9999999,999999);
    DSpinBox_m_bValue->setDecimals(2);
    DSpinBox_m_bValue->setValue(m_database->m_bValue);
    form.addRow("B-Wert_NTC", DSpinBox_m_bValue);

    //Plot site
    QDoubleSpinBox *DSpinBox_m_plot_size = new QDoubleSpinBox(&dialog);
    DSpinBox_m_plot_size->setRange(-9999999,999999);
    DSpinBox_m_plot_size->setDecimals(2);
    DSpinBox_m_plot_size->setValue(m_database->m_plot_size);
    form.addRow("PlotSize", DSpinBox_m_plot_size);


    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted) {
        Log("save PID");
        m_database->m_R_vor = DSpinBox_m_R_vor->value();
        m_database->m_R_nen = DSpinBox_m_R_nen->value();
        m_database->m_bValue = DSpinBox_m_bValue->value();
        m_database->m_plot_size = DSpinBox_m_plot_size->value();

        m_database->m_KP = DSpinBox_KP->value();
        m_database->m_KI = DSpinBox_KI->value();
        m_database->m_KD = DSpinBox_KD->value();

        m_database->m_POn = CheckBox_POn->isChecked();

        m_database->saveSettings();

        m_basefunctions->send_PID(m_database->m_KP,m_database->m_KI,m_database->m_KD,m_database->m_POn);
        m_basefunctions->send_Temp_Setting(m_database->m_R_vor,m_database->m_bValue,m_database->m_R_nen);
    }
}

void MainWindow::addToGraph(float T_100,float T_ntc,float PWM,float T_soll)
{
    double sec = time.elapsed()/1000;
    ui->Plot_PID_Temp->graph(0)->addData(sec, T_100);
    ui->Plot_PID_Temp->graph(1)->addData(sec, T_ntc);
    ui->Plot_PID_Temp->graph(2)->addData(sec, PWM);
    ui->Plot_PID_Temp->graph(3)->addData(sec, T_soll);
    ui->Plot_PID_Temp->xAxis->setRange(sec, m_database->m_plot_size, Qt::AlignRight);
    ui->Plot_PID_Temp->replot();
}


void MainWindow::on_pushButton_test_clicked()
{
    //ui->textEditLog->append("cycletime cheack");
    //m_basefunctions->cycletimeTest();
    //m_basefunctions->trigger_next_command();
    m_serial->serial_fasttimeout_handler();
}

void MainWindow::on_doubleSpinBoxZOffset_valueChanged(const QString &arg1)
{
    m_database->m_z_offset = ui->doubleSpinBoxZOffset->value();
}

void MainWindow::on_pushButton_nozzel_calib_clicked()
{
    m_automation->nozzel_calib();
}
