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

    m_database->m_soll_speed = ui->spinBoxSpeed->value();
    m_database->m_soll_temperatur = ui->spinBoxTemperatur->value();
    m_database->m_soll_filament = ui->spinBoxFilament->value();

    m_database->loadSettings();
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
    connect(m_automation,SIGNAL(resend_last()),m_serial,SLOT(send_last()));

    connect(ui->actionValues,SIGNAL(triggered()),this,SLOT(calibratenValueBox()));
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
        m_database->m_SerialPortName = ui->comboBoxComPortName->currentText();
        m_database->m_HWisMoving = false;
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
    m_database->m_HWisMoving = false;
    m_basefunctions->trigger_next_command();
    show_send_queue();
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

    //nozzel hight
    QDoubleSpinBox *DSpinBox_TCP_higth = new QDoubleSpinBox(&dialog);
    DSpinBox_TCP_higth->setRange(-9999999,999999);
    DSpinBox_TCP_higth->setValue(m_database->m_Zmax_nozzel);
    form.addRow("X Size", DSpinBox_TCP_higth);

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
        m_database->m_error_X_max_Y_null = DSpinBox_X_max_Y_null->value();
        m_database->m_error_X_max_Y_max = DSpinBox_X_max_Y_max->value();
        m_database->m_error_X_null_Y_max = DSpinBox_X_null_Y_max->value();
        m_database->m_error_X_null_Y_null = DSpinBox_X_null_Y_null->value();
        //correktion angel
        m_database->m_X_angel = DSpinBox_angel_X->value();
        m_database->m_Y_angel = DSpinBox_angel_Y->value();
        //TCP hight in home
        m_database->m_Zmax_nozzel = DSpinBox_TCP_higth->value();
        //max values
        m_database->m_max_speed = DSpinBox_maxSpeed->value(); //mm per sec

        m_database->saveSettings();
    }
}

void MainWindow::on_pushButton_test_clicked()
{
    QList<point> pointList;
    point help;
    help.Z = m_database->m_error_X_null_Y_null;
    Log("P0:"+QString::number(m_database->m_error_X_null_Y_null));
    pointList.append(help);
    help.Z = m_database->m_error_X_max_Y_null;
    Log("P1:"+QString::number(m_database->m_error_X_max_Y_null));
    pointList.append(help);
    help.Z = m_database->m_error_X_max_Y_max;
    Log("P2:"+QString::number(m_database->m_error_X_max_Y_max));
    pointList.append(help);
    help.Z = m_database->m_error_X_null_Y_max;
    Log("P3:"+QString::number(m_database->m_error_X_null_Y_max));
    pointList.append(help);
    help.Z = m_database->m_error_X_null_Y_null;
    Log("P4:"+QString::number(m_database->m_error_X_null_Y_null));
    pointList.append(help);

    m_database->calc_correctionangel(pointList);
}

void MainWindow::on_doubleSpinBoxZOffset_valueChanged(const QString &arg1)
{
    m_database->m_z_offset = ui->doubleSpinBoxZOffset->value();
}
