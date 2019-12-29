#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QFileDialog>
#include "serial.h"
#include "cnc_automation.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void ProcesCommand(const char command,const float value1,const float value2,const float value3,const float value4);

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

private:
    Ui::MainWindow *ui;
    Serial m_Serial;
    CNC_automation m_CNC_auto;
    void endswitchButtonColor(float value,QPushButton *PosButton,QPushButton *NegButton);


};
#endif // MAINWINDOW_H
