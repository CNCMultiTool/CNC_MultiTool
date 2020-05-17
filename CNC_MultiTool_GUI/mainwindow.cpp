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
    ui->spinBoxAccSteps->setValue(m_database->m_soll_accSteps);


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
    connect(m_basefunctions,SIGNAL(show_status()),this,SLOT(show_status()));
    connect(m_basefunctions,SIGNAL(DataToGraph(float,float,float,float)),this,SLOT(addToGraph(float,float,float,float)));
    connect(m_basefunctions,SIGNAL(show_alive()),this,SLOT(show_alive()));

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

}

MainWindow::~MainWindow()
{
    m_database->saveSettings();
    delete ui;
}

void MainWindow::Log(const QString &s)
{
    QString text = QDateTime::currentDateTime().toString("hh:mm:ss.zzz ") + s;
    ui->textEditLog->append(text);
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
    ui->label_accSteps->setText(QString::number(m_database->m_soll_accSteps));
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
    float Acc = ui->spinBoxAccSteps->value();
    m_basefunctions->send_settings(Speed,Temperatur,Filament,Acc);
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
/*
    //auto level values
    QDoubleSpinBox *DSpinBox_X_max_Y_null = new QDoubleSpinBox(&dialog);
    DSpinBox_X_max_Y_null->setRange(-9999999,999999);
    DSpinBox_X_max_Y_null->setValue(m_database->m_error_X_max_Y_null);
    form.addRow("Z error at X:max Y:0", DSpinBox_X_max_Y_null);

    QDoubleSpinBox *DSpinBox_X_max_Y_max = new QDoubleSpinBox(&dialog);
    DSpinBox_X_max_Y_max->setRange(-9999999,999999);
    DSpinBox_X_max_Y_max->setValue(m_database->m_error_X_max_Y_max);
    form.addRow("Z error at X:max Y:max", DSpinBox_X_max_Y_max);

    QDoubleSpinBox *DSpinBox_X_null_Y_max = new QDoubleSpinBox(&dialog);
    DSpinBox_X_null_Y_max->setRange(-9999999,999999);
    DSpinBox_X_null_Y_max->setValue(m_database->m_error_X_null_Y_max);
    form.addRow("Z error at X:0 Y:max", DSpinBox_X_null_Y_max);

    QDoubleSpinBox *DSpinBox_X_null_Y_null = new QDoubleSpinBox(&dialog);
    DSpinBox_X_null_Y_null->setRange(-9999999,999999);
    DSpinBox_X_null_Y_null->setValue(m_database->m_error_X_null_Y_null);
    form.addRow("Z error at X:0 Y:0", DSpinBox_X_null_Y_null);

    QDoubleSpinBox *DSpinBox_angel_X = new QDoubleSpinBox(&dialog);
    DSpinBox_angel_X->setRange(-9999999,999999);
    DSpinBox_angel_X->setDecimals(6);
    DSpinBox_angel_X->setValue(m_database->m_X_angel);
    form.addRow("X Angle", DSpinBox_angel_X);

    QDoubleSpinBox *DSpinBox_angel_Y = new QDoubleSpinBox(&dialog);
    DSpinBox_angel_Y->setRange(-9999999,999999);
    DSpinBox_angel_Y->setDecimals(6);
    DSpinBox_angel_Y->setValue(m_database->m_Y_angel);
    form.addRow("Y Angle", DSpinBox_angel_Y);
*/
    //nozzel hight
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

        //kalibration results
        //m_database->m_error_X_max_Y_null = DSpinBox_X_max_Y_null->value();
        //m_database->m_error_X_max_Y_max = DSpinBox_X_max_Y_max->value();
        //m_database->m_error_X_null_Y_max = DSpinBox_X_null_Y_max->value();
        //m_database->m_error_X_null_Y_null = DSpinBox_X_null_Y_null->value();

        //correktion angel
        //m_database->m_X_angel = DSpinBox_angel_X->value();
        //m_database->m_Y_angel = DSpinBox_angel_Y->value();
        //TCP hight in home
        m_database->m_Zmax_nozzel = DSpinBox_TCP_higth->value();
        m_database->m_calibplateX = DSpinBox_calibplateX->value();
        m_database->m_calibplateY = DSpinBox_calibplateY->value();
        m_database->m_calibplateZ = DSpinBox_calibplateZ->value();
        //max values
        m_database->m_max_speed = DSpinBox_maxSpeed->value(); //mm per sec
        m_database->m_useCalibPlate = CheckBox_makeCalib->isChecked();


        m_database->saveSettings();
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
    ui->textEditLog->append("cycletime cheack");
    m_basefunctions->cycletimeTest();
    m_basefunctions->trigger_next_command();
}

void MainWindow::on_doubleSpinBoxZOffset_valueChanged(const QString &arg1)
{
    m_database->m_z_offset = ui->doubleSpinBoxZOffset->value();
}

void MainWindow::on_pushButton_nozzel_calib_clicked()
{
    m_automation->nozzel_calib();
}
