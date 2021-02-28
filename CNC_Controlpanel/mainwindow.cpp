#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textEditLog->setReadOnly(true);
    //ui->textEditLog_error->setReadOnly(true);

    m_database->loadSettings();

    ui->doubleSpinBoxMinSpeed->setValue(m_database->m_min_speed);
    ui->doubleSpinBoxMaxSpeed->setValue(m_database->m_max_speed);
    ui->doubleSpinBoxMaxAcc->setValue(m_database->m_max_acc);
    ui->doubleSpinBoxFilament->setValue(m_database->m_soll_filament);
    ui->doubleSpinBoxSpeed->setValue(m_database->m_soll_speed);
    ui->doubleSpinBoxThreshAngle->setValue(m_database->m_threshAngle);

    //chek availabal port add to the comboBox
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        ui->comboBoxComPortName->addItem(info.portName());

    connect(ui->pushButtonMoveXPos,SIGNAL(released()),this,SLOT(on_pushButtonMoveSTOP_pressed()));
    connect(ui->pushButtonMoveXNeg,SIGNAL(released()),this,SLOT(on_pushButtonMoveSTOP_pressed()));
    connect(ui->pushButtonMoveYPos,SIGNAL(released()),this,SLOT(on_pushButtonMoveSTOP_pressed()));
    connect(ui->pushButtonMoveYNeg,SIGNAL(released()),this,SLOT(on_pushButtonMoveSTOP_pressed()));
    connect(ui->pushButtonMoveZPos,SIGNAL(released()),this,SLOT(on_pushButtonMoveSTOP_pressed()));
    connect(ui->pushButtonMoveZNeg,SIGNAL(released()),this,SLOT(on_pushButtonMoveSTOP_pressed()));
    connect(ui->pushButtonMoveEPos,SIGNAL(released()),this,SLOT(on_pushButtonMoveSTOP_pressed()));
    connect(ui->pushButtonMoveENeg,SIGNAL(released()),this,SLOT(on_pushButtonMoveSTOP_pressed()));

    connect(m_basefunctions,SIGNAL(Log(QString)),this,SLOT(Log(QString)));
    connect(m_basefunctions,SIGNAL(errorLog(QString)),this,SLOT(errorLog(QString)));
    connect(m_basefunctions,SIGNAL(serial_send(QString)),m_serial,SLOT(serial_send(QString)));
    connect(m_basefunctions,SIGNAL(show_position()),this,SLOT(show_position()));
    connect(m_basefunctions,SIGNAL(show_temp()),this,SLOT(addToGraph()));
    connect(m_basefunctions,SIGNAL(show_endswitch()),this,SLOT(show_endswitch()));
    connect(m_basefunctions,SIGNAL(show_speed()),this,SLOT(show_speed()));
    connect(m_basefunctions,SIGNAL(show_acc_speed_fila()),this,SLOT(show_acc_speed_fila()));
    connect(m_basefunctions,SIGNAL(show_act_temp()),this,SLOT(show_act_temp()));
    connect(m_basefunctions,SIGNAL(show_useES(float)),this,SLOT(show_useES(float)));
    connect(m_basefunctions,SIGNAL(show_state(float)),this,SLOT(show_state(float)));
    connect(m_basefunctions,SIGNAL(show_waitForHeat(float)),this,SLOT(show_waitForHeat(float)));

    connect(m_database,SIGNAL(Log(QString)),this,SLOT(Log(QString)));
    connect(m_database,SIGNAL(errorLog(QString)),this,SLOT(errorLog(QString)));

    connect(m_serial,SIGNAL(Log(QString)),this,SLOT(Log(QString)));
    connect(m_serial,SIGNAL(errorLog(QString)),this,SLOT(errorLog(QString)));
    connect(m_serial,SIGNAL(show_serial(bool)),this,SLOT(show_serial(bool)));
    connect(m_serial,SIGNAL(recLine(QString)),m_basefunctions,SLOT(processLine(QString)));

    connect(ui->actionValues,SIGNAL(triggered()),this,SLOT(calibratenValueBox()));
    connect(ui->actionPID,SIGNAL(triggered()),this,SLOT(PID_ValueBox()));

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
    ui->Plot_PID_Temp->yAxis->setRange(-10, 360);
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->Plot_PID_Temp->xAxis->setTicker(timeTicker);
    ui->Plot_PID_Temp->axisRect()->setupFullAxesBox();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Log(const QString &s)
{
    QString text = QDateTime::currentDateTime().toString("hh:mm:ss.zzz Log: ") + s;
    text.remove(QRegularExpression("\n"));
    ui->textEditLog->append(text);
}

void MainWindow::errorLog(const QString &s)
{
    QString text = QDateTime::currentDateTime().toString("hh:mm:ss.zzz Error: ") + s;
    text.remove(QRegularExpression("\n"));
    //ui->textEditLog_error->append(text);
    ui->textEditLog->append(text);
}

void MainWindow::addToGraph()
{
    double sec = time.elapsed()/1000;
    ui->Plot_PID_Temp->graph(0)->addData(sec, m_database->m_soll_temperatur);
    ui->Plot_PID_Temp->graph(1)->addData(sec, m_database->m_act_temperatur);
    ui->Plot_PID_Temp->graph(2)->addData(sec, m_database->m_soll_bedTemp);
    ui->Plot_PID_Temp->graph(3)->addData(sec, m_database->m_act_bedTemp);
    ui->Plot_PID_Temp->xAxis->setRange(sec, m_database->m_plot_size, Qt::AlignRight);
    ui->Plot_PID_Temp->replot();
    show_act_temp();
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

void MainWindow::show_position()
{
    ui->label_XPos->setText(QString::number(m_database->m_act_X));
    ui->label_YPos->setText(QString::number(m_database->m_act_Y));
    ui->label_ZPos->setText(QString::number(m_database->m_act_Z));
    ui->label_EPos->setText(QString::number(m_database->m_act_E));
}

void MainWindow::show_acc_speed_fila()
{
    ui->label_max_acc->setText(QString::number(m_database->m_max_acc));
    ui->label_max_speed->setText(QString::number(m_database->m_max_speed));
    ui->label_min_speed->setText(QString::number(m_database->m_min_speed));
    ui->label_fila->setText(QString::number(m_database->m_act_filament));
    ui->label_ThreshAngle->setText(QString::number(m_database->m_threshAngle));
}
void MainWindow::show_speed()
{
    ui->label_speed->setText(QString::number(m_database->m_act_speed));
}

void MainWindow::show_act_temp()
{
    ui->label_temp->setText(QString::number(m_database->m_soll_temperatur)+" / "+QString::number(m_database->m_act_temperatur));
    ui->label_bed_temp->setText(QString::number(m_database->m_soll_bedTemp)+" / "+QString::number(m_database->m_act_bedTemp));
}

void MainWindow::show_endswitch()
{
    endswitchButtonColor(m_database->m_endswitch_X,ui->pushButtonMoveXPos,ui->pushButtonMoveXNeg);
    endswitchButtonColor(m_database->m_endswitch_Y,ui->pushButtonMoveYPos,ui->pushButtonMoveYNeg);
    endswitchButtonColor(m_database->m_endswitch_Z,ui->pushButtonMoveZPos,ui->pushButtonMoveZNeg);
}

void MainWindow::show_useES(float ES){
    if(ES)
        ui->pushButtonUseES->setStyleSheet("background-color: green");
    else
        ui->pushButtonUseES->setStyleSheet("background-color: red");
}

void MainWindow::show_state(float state){
    switch(int(state)){
    case 0:
        ui->label_state->setText("GCodeStart");
                break;
    case 1:
        ui->label_state->setText("GCodeRun");
                break;
    case 2:
        ui->label_state->setText("GCodePause");
                break;
    case 3:
        ui->label_state->setText("GCodeStop");
                break;
    case 4:
        ui->label_state->setText("GCodeCreate");
                break;
    case 5:
        ui->label_state->setText("GCodeStartHome");
                break;
    case 6:
        ui->label_state->setText("GCodeRunHome");
                break;
    case 7:
        ui->label_state->setText("GCodeStopHome");
                break;
    default:
        ui->label_state->setText("unknown state");
                break;
    }

}

void MainWindow::show_waitForHeat(float isWaiting){
    if(isWaiting)
        ui->label_waitForHeat->setStyleSheet("background-color: yellow");
    else
        ui->label_waitForHeat->setStyleSheet("background-color: lightblue");
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
        m_basefunctions->send_stop();

        m_basefunctions->send_NTC_values_bed(&m_database->m_bValue_Bed,&m_database->m_R_nen_Bed,&m_database->m_R_vor_Bed);
        m_basefunctions->send_NTC_values(&m_database->m_bValue,&m_database->m_R_nen,&m_database->m_R_vor);
        m_basefunctions->send_PID_values(&m_database->m_KP,&m_database->m_KI,&m_database->m_KD,&m_database->m_POn);
        m_basefunctions->send_moveparam(&m_database->m_max_acc,&m_database->m_min_speed,&m_database->m_max_speed,&m_database->m_threshAngle,&m_database->m_soll_filament);
        m_basefunctions->send_Speed(m_database->m_soll_speed);
        m_basefunctions->send_BedTemp(0);
        m_basefunctions->send_HotendTemp(0);
        m_basefunctions->send_stop();
    }
}

void MainWindow::on_pushButtonMoveXPos_pressed()
{
    float inv = 9999;
    m_basefunctions->send_move(&inv,nullptr,nullptr,nullptr,nullptr);
}

void MainWindow::on_pushButtonMoveXNeg_pressed()
{
    float inv = -9999;
    m_basefunctions->send_move(&inv,nullptr,nullptr,nullptr,nullptr);
}

void MainWindow::on_pushButtonMoveYPos_pressed()
{
    float inv = 9999;
    m_basefunctions->send_move(nullptr,&inv,nullptr,nullptr,nullptr);
}

void MainWindow::on_pushButtonMoveYNeg_pressed()
{
    float inv = -9999;
    m_basefunctions->send_move(nullptr,&inv,nullptr,nullptr,nullptr);
}

void MainWindow::on_pushButtonMoveZPos_pressed()
{
    float inv = 9999;
    m_basefunctions->send_move(nullptr,nullptr,&inv,nullptr,nullptr);
}

void MainWindow::on_pushButtonMoveZNeg_pressed()
{
    float inv = -9999;
    m_basefunctions->send_move(nullptr,nullptr,&inv,nullptr,nullptr);
}

void MainWindow::on_pushButtonMoveEPos_pressed()
{
    float inv = 9999;
    m_basefunctions->send_move(nullptr,nullptr,nullptr,&inv,nullptr);
}

void MainWindow::on_pushButtonMoveENeg_pressed()
{
    float inv = -9999;
    m_basefunctions->send_move(nullptr,nullptr,nullptr,&inv,nullptr);
}

void MainWindow::on_pushButtonGetPosition_clicked()
{
    m_basefunctions->send_getPosition();
}

void MainWindow::on_pushButtonMoveTo_pressed()
{
    float X = ui->doubleSpinBoxMoveX->value();
    float Y = ui->doubleSpinBoxMoveY->value();
    float Z = ui->doubleSpinBoxMoveZ->value();
    float E = ui->doubleSpinBoxMoveE->value();
    m_basefunctions->send_move(&X,&Y,&Z,&E,nullptr);
}

void MainWindow::on_pushButtonSetPosition_pressed()
{
    float X = ui->doubleSpinBoxSetX->value();
    float Y = ui->doubleSpinBoxSetY->value();
    float Z = ui->doubleSpinBoxSetZ->value();
    float E = ui->doubleSpinBoxSetE->value();
    m_basefunctions->send_setPosition(&X,&Y,&Z,&E);
}

void MainWindow::on_pushButtonMoveSTOP_pressed()
{
    m_basefunctions->send_stop();
}

void MainWindow::on_pushButtonMinSpeed_clicked()
{
    m_database->m_min_speed = ui->doubleSpinBoxMinSpeed->value();
    m_basefunctions->send_moveparam(nullptr,&m_database->m_min_speed,nullptr,nullptr,nullptr);
}

void MainWindow::on_pushButtonMaxSpeed_clicked()
{
    m_database->m_max_speed = ui->doubleSpinBoxMaxSpeed->value();
    m_basefunctions->send_moveparam(nullptr,nullptr,&m_database->m_max_speed,nullptr,nullptr);
}

void MainWindow::on_pushButtonSpeed_clicked()
{
    float S = ui->doubleSpinBoxSpeed->value();
    m_basefunctions->send_move(nullptr,nullptr,nullptr,nullptr,&S);
}

void MainWindow::on_pushButtonMaxAcc_clicked()
{
    m_database->m_max_acc = ui->doubleSpinBoxMaxAcc->value();
    m_basefunctions->send_moveparam(&m_database->m_max_acc,nullptr,nullptr,nullptr,nullptr);
}

void MainWindow::on_pushButtonThreshAngle_clicked()
{
    m_database->m_threshAngle = ui->doubleSpinBoxThreshAngle->value();
    m_basefunctions->send_moveparam(nullptr,nullptr,nullptr,&m_database->m_threshAngle,nullptr);
}

void MainWindow::on_pushButtonBedTemp_clicked()
{
    float S = ui->doubleSpinBoxBedTemp->value();
    m_basefunctions->send_BedTemp(S);
}

void MainWindow::on_pushButtoHotendTemp_clicked()
{
    float S = ui->doubleSpinBoxHotendTemp->value();
    m_basefunctions->send_HotendTemp(S);
}

void MainWindow::on_pushButtonFilament_clicked()
{
    float S = ui->doubleSpinBoxFilament->value();
    m_basefunctions->send_Filament(S);
}

void MainWindow::calibratenValueBox()
{
    Log("show checkbox");
    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

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

    //min speed
    QDoubleSpinBox *DSpinBox_speed_min = new QDoubleSpinBox(&dialog);
    DSpinBox_speed_min->setRange(-9999999,999999);
    DSpinBox_speed_min->setValue(m_database->m_min_speed);
    form.addRow("Min Speed mm/s^2:", DSpinBox_speed_min);

    //max speed
    QDoubleSpinBox *DSpinBox_maxSpeed = new QDoubleSpinBox(&dialog);
    DSpinBox_maxSpeed->setRange(-9999999,999999);
    DSpinBox_maxSpeed->setValue(m_database->m_max_speed);
    form.addRow("Max Speed mm/s:", DSpinBox_maxSpeed);

    //default speed
    QDoubleSpinBox *DSpinBox_default_speed = new QDoubleSpinBox(&dialog);
    DSpinBox_default_speed->setRange(-9999999,999999);
    DSpinBox_default_speed->setValue(m_database->m_soll_speed);
    form.addRow("default Speed mm/s^2:", DSpinBox_default_speed);

    //acc
    QDoubleSpinBox *DSpinBox_max_acc = new QDoubleSpinBox(&dialog);
    DSpinBox_max_acc->setRange(-9999999,999999);
    DSpinBox_max_acc->setValue(m_database->m_max_acc);
    form.addRow("Max Acc mm/s^2:", DSpinBox_max_acc);

    //ThreshAngle
    QDoubleSpinBox *DSpinBox_ThreshAngle = new QDoubleSpinBox(&dialog);
    DSpinBox_ThreshAngle->setRange(-9999999,999999);
    DSpinBox_ThreshAngle->setValue(m_database->m_threshAngle);
    form.addRow("Angle Thresh:", DSpinBox_ThreshAngle);

    //default filament
    QDoubleSpinBox *DSpinBox_default_filament = new QDoubleSpinBox(&dialog);
    DSpinBox_default_filament->setRange(-9999999,999999);
    DSpinBox_default_filament->setValue(m_database->m_soll_filament);
    form.addRow("default filament:", DSpinBox_default_filament);

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted) {
        Log("save values");

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
        m_database->m_min_speed = DSpinBox_speed_min->value(); //mm per sec
        m_database->m_soll_speed = DSpinBox_default_speed->value();
        m_database->m_soll_filament = DSpinBox_default_filament->value();
        m_database->m_threshAngle = DSpinBox_ThreshAngle->value();

        m_database->m_useCalibPlate = CheckBox_makeCalib->isChecked();

        //m_basefunctions->send_acc(m_database->m_max_acc,m_database->m_speed_min);

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

    //Vorwiederstand
    QDoubleSpinBox *DSpinBox_m_R_vor_Bed = new QDoubleSpinBox(&dialog);
    DSpinBox_m_R_vor_Bed->setRange(-9999999,999999);
    DSpinBox_m_R_vor_Bed->setDecimals(2);
    DSpinBox_m_R_vor_Bed->setValue(m_database->m_R_vor_Bed);
    form.addRow("Vorwiederstand_Bed", DSpinBox_m_R_vor_Bed);

    //Vorwiederstand
    QDoubleSpinBox *DSpinBox_m_R_nen_Bed = new QDoubleSpinBox(&dialog);
    DSpinBox_m_R_nen_Bed->setRange(-9999999,999999);
    DSpinBox_m_R_nen_Bed->setDecimals(2);
    DSpinBox_m_R_nen_Bed->setValue(m_database->m_R_nen_Bed);
    form.addRow("Nennwiederstand_Bed", DSpinBox_m_R_nen_Bed);

    //B Value
    QDoubleSpinBox *DSpinBox_m_bValue_Bed = new QDoubleSpinBox(&dialog);
    DSpinBox_m_bValue_Bed->setRange(-9999999,999999);
    DSpinBox_m_bValue_Bed->setDecimals(2);
    DSpinBox_m_bValue_Bed->setValue(m_database->m_bValue_Bed);
    form.addRow("B-Wert_NTC_Bed", DSpinBox_m_bValue_Bed);

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

        m_database->m_R_vor_Bed = DSpinBox_m_R_vor_Bed->value();
        m_database->m_R_nen_Bed = DSpinBox_m_R_nen_Bed->value();
        m_database->m_bValue_Bed = DSpinBox_m_bValue_Bed->value();

        m_database->m_POn = CheckBox_POn->isChecked();

        m_basefunctions->send_NTC_values_bed(&m_database->m_bValue_Bed,&m_database->m_R_nen_Bed,&m_database->m_R_vor_Bed);
        m_basefunctions->send_NTC_values(&m_database->m_bValue,&m_database->m_R_nen,&m_database->m_R_vor);
        m_basefunctions->send_PID_values(&m_database->m_KP,&m_database->m_KI,&m_database->m_KD,&m_database->m_POn);

        m_database->saveSettings();

    }
}


void MainWindow::on_pushButton_browseGCode_clicked()
{
    //Log("hier soll ein G-Code File aus gesucht werden");
    //Log(" und auf der SD Carte des Arduino s angelegt werden");
    QString directory = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Open File"),QDir::currentPath(),tr("G-Code (*.gcode)")));

    Log(directory);
    if (!directory.isEmpty())
    {
        ui->lineEdit_fileGCode->setText(directory);
    }
}

void MainWindow::on_pushButton_startGCode_clicked()
{
    m_basefunctions->send_GCodeStart(ui->lineEdit_fileToCNC->text());
}

void MainWindow::on_pushButton_pauseGCode_clicked()
{
    m_basefunctions->send_GCodePause();
}

void MainWindow::on_pushButton_continueGCode_clicked()
{
    m_basefunctions->send_GCodeContinue();
}

void MainWindow::on_pushButton_AboardGCode_clicked()
{
    m_basefunctions->send_GCodeStop();
}

void MainWindow::on_pushButton_StartCreation_clicked()
{
    m_basefunctions->send_CreateFile(ui->lineEdit_fileGCode->text(),ui->lineEdit_fileToCNC->text());
}

void MainWindow::on_pushButton_DeleteFile_clicked()
{
    m_basefunctions->send_DeleteFile(ui->lineEdit_fileToCNC->text());
}

void MainWindow::on_pushButton_ShowFiles_clicked()
{
    m_basefunctions->send_GetFileList();
}

void MainWindow::on_pushButton_Test_clicked()
{
    m_basefunctions->send_XXX();
    //m_basefunctions->send_NTC_values_bed(&m_database->m_bValue_Bed,&m_database->m_R_nen_Bed,&m_database->m_R_vor_Bed);
    //m_basefunctions->send_NTC_values(&m_database->m_bValue,&m_database->m_R_nen,&m_database->m_R_vor);
    //m_basefunctions->send_PID_values(&m_database->m_KP,&m_database->m_KI,&m_database->m_KD,&m_database->m_POn);
}

void MainWindow::on_pushButton_NewHome_clicked()
{
    float X = m_database->m_X_inHome;
    float Y = m_database->m_Y_inHome;
    float Z = m_database->m_Zmax_nozzel;
    m_basefunctions->send_newHome(X,Y,Z);
}

void MainWindow::on_pushButton_MoveHome_clicked()
{
    m_basefunctions->send_moveHome();
}

void MainWindow::on_pushButtonUseES_clicked()
{
    m_basefunctions->send_toggleES();
}
