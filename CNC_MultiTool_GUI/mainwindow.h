#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QFileDialog>
#include <QTime>
#include <QString>
#include <QThread>
#include <cnc_basefunctions.h>
#include <cnc_data.h>
#include <cnc_automation.h>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QCheckBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;

    cnc_data *m_database = new cnc_data();
    cnc_basefunctions *m_basefunctions = new cnc_basefunctions(m_database);
    Serial *m_serial = new Serial(m_database);
    CNC_automation *m_automation = new CNC_automation(m_database,m_basefunctions);

    QTime time;
    void endswitchButtonColor(float value,QPushButton *PosButton,QPushButton *NegButton);
    void send_move(float X,float Y,float Z,float W);
    void send_settings(float speed,float temperatur,float filament);
    void send_stop();
    void send_getPosition();
    void send_setPosition(float X,float Y,float Z,float W);
    void serial_open_close(QString portName);
    void G_Code_Start(QString fileName);
    void G_Code_Pause();
    void G_Code_Stop();

    void move_home();
    void calib_size();
    void repeat_test();
    void Z_calib();



    bool m_alive;

signals:
    void serial_start();
    void serial_close();

private slots:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void DebugLog(const QString &s);

    void show_position();
    void show_settings();
    void show_endswitch(float X,float Y,float Z);
    void show_serial(bool isOpen);
    void show_send_queue();
    void show_alive();
    void show_status();

    void sendStopMoving();

    void on_pushButtonSerialConnect_clicked();

    void on_pushButtonMoveXPos_pressed();

    void on_pushButtonMoveXNeg_pressed();

    void on_pushButtonMoveYPos_pressed();

    void on_pushButtonMoveYNeg_pressed();

    void on_pushButtonMoveZPos_pressed();

    void on_pushButtonMoveWNeg_pressed();

    void on_pushButtonMoveZNeg_pressed();

    void on_pushButtonMoveWPos_pressed();

    void on_pushButtonCopyToMoveTo_pressed();

    void on_pushButtonCopyToSetPosition_pressed();

    void on_pushButtonGetPosition_pressed();

    void on_pushButtonMoveTo_pressed();

    void on_pushButtonSetPosition_pressed();

    void on_pushButtonSetSettings_pressed();

    void on_pushButton_browseGCode_pressed();

    void on_pushButton_startGCode_pressed();

    void on_pushButton_Zcalib_pressed();

    void on_pushButton_home_pressed();

    void on_pushButton_sizecalib_pressed();

    void on_pushButton_repeattest_pressed();

    void on_pushButton_pauseGCode_pressed();
    void on_pushButton_AboardGCode_pressed();
    void on_pushButton_rest_pressed();
    void on_spinBoxSpeed_valueChanged(int arg1);
    void on_spinBoxTemperatur_valueChanged(int arg1);
    void on_spinBoxFilament_valueChanged(int arg1);
    void on_pushButton_clear_queue_clicked();
    void on_pushButton_trigger_next_clicked();
    void calibratenValueBox();
    void repeattestValueBox();
    void on_pushButton_test_clicked();
    void on_doubleSpinBoxZOffset_valueChanged(const QString &arg1);
    void on_pushButton_nozzel_calib_clicked();
    void PID_ValueBox();
    void PID_Bed_ValueBox();
    void bedCalibValueBox();
    void addToGraph(float T_100,float T_ntc,float PWM,float T_soll);
    void show_z_calib_result();
};
#endif // MAINWINDOW_H
