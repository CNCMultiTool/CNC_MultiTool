#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPortInfo>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QTime>
//#include <QRegularExpression.h>
#include <cnc_basefunctions.h>
#include <cnc_data.h>
#include <serial.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void serial_start();
    void serial_close();

private slots:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void show_serial(bool isOpen);
    void show_endswitch();
    void on_pushButtonSerialConnect_clicked();
    void on_pushButtonMoveXPos_pressed();
    void on_pushButtonMoveXNeg_pressed();
    void on_pushButtonMoveYPos_pressed();
    void on_pushButtonMoveYNeg_pressed();
    void on_pushButtonMoveZPos_pressed();
    void on_pushButtonMoveZNeg_pressed();
    void on_pushButtonMoveEPos_pressed();
    void on_pushButtonMoveENeg_pressed();
    void on_pushButtonMoveTo_pressed();
    void on_pushButtonSetPosition_pressed();
    void on_pushButtonMoveSTOP_pressed();
    void on_pushButtonMinSpeed_clicked();
    void on_pushButtonMaxSpeed_clicked();
    void on_pushButtonSpeed_clicked();
    void on_pushButtonBedTemp_clicked();
    void on_pushButtoHotendTemp_clicked();
    void on_pushButtonFilament_clicked();
    void on_pushButtonMaxAcc_clicked();

    void on_pushButton_browseGCode_clicked();
    void on_pushButton_startGCode_clicked();
    void on_pushButton_pauseGCode_clicked();
    void on_pushButton_continueGCode_clicked();
    void on_pushButton_AboardGCode_clicked();

    void calibratenValueBox();
    void PID_ValueBox();

    void show_position();
    void addToGraph();
    void show_acc_speed_fila();
    void show_speed();
    void show_act_temp();


    void on_pushButton_StartCreation_clicked();
    void on_pushButton_DeleteFile_clicked();
    void on_pushButton_ShowFiles_clicked();
    void on_pushButton_Test_clicked();
    void on_pushButton_NewHome_clicked();
    void on_pushButton_MoveHome_clicked();

    void on_pushButtonThreshAngle_clicked();

    void on_pushButtonGetPosition_clicked();

private:
    Ui::MainWindow *ui;

    cnc_data *m_database = new cnc_data();
    Serial *m_serial = new Serial(m_database);
    cnc_basefunctions *m_basefunctions = new cnc_basefunctions(m_database,m_serial);

    void endswitchButtonColor(float value,QPushButton *PosButton,QPushButton *NegButton);
    QTime time;
};
#endif // MAINWINDOW_H
