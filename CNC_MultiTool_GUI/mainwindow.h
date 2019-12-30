#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QFileDialog>
#include <QTime>
#include "serial.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void Log(const QString &s);
    void errorLog(const QString &s);
    void show_position(float X,float Y,float Z,float W);
    void show_settings(float speed,float temperatur,float filament);
    void show_endswitch(float X,float Y,float Z);
    void show_serial(bool isOpen);

private slots:

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

signals:
    void send_move(float X,float Y,float Z,float W);
    void send_settings(float speed,float temperatur,float filament);
    void send_stop();
    void send_getPosition();
    void send_setPosition(float X,float Y,float Z,float W);
    void serial_open_close(QString portName);
    void G_Code_Start(QString fileName);
    void G_Code_Pause();
    void G_Code_Stop();

private:
    Ui::MainWindow *ui;

    void endswitchButtonColor(float value,QPushButton *PosButton,QPushButton *NegButton);


};
#endif // MAINWINDOW_H
