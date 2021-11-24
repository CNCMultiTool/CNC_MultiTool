/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionValues;
    QAction *actionPID;
    QWidget *centralwidget;
    QWidget *gridLayoutWidget;
    QGridLayout *MoveManuel;
    QPushButton *pushButtonMoveXNeg;
    QPushButton *pushButtonMoveYNeg;
    QPushButton *pushButtonMoveXPos;
    QPushButton *pushButtonMoveYPos;
    QPushButton *pushButtonMoveZPos;
    QPushButton *pushButtonMoveZNeg;
    QPushButton *pushButtonMoveEPos;
    QPushButton *pushButtonMoveENeg;
    QPushButton *pushButtonMoveSTOP;
    QDoubleSpinBox *doubleSpinBoxZOffset;
    QWidget *gridLayoutWidget_2;
    QGridLayout *MoveNunmeric;
    QPushButton *pushButtonCopyToSetPosition;
    QDoubleSpinBox *doubleSpinBoxMoveZ;
    QLabel *label_ZPos;
    QDoubleSpinBox *doubleSpinBoxMoveX;
    QDoubleSpinBox *doubleSpinBoxSetX;
    QLabel *label_EPos;
    QPushButton *pushButtonMoveTo;
    QDoubleSpinBox *doubleSpinBoxMoveE;
    QDoubleSpinBox *doubleSpinBoxMoveY;
    QLabel *label_YPos;
    QPushButton *pushButtonSetPosition;
    QLabel *label_XPos;
    QPushButton *pushButtonGetPosition;
    QDoubleSpinBox *doubleSpinBoxSetE;
    QDoubleSpinBox *doubleSpinBoxSetY;
    QDoubleSpinBox *doubleSpinBoxSetZ;
    QPushButton *pushButtonCopyToMoveTo;
    QLabel *label_X;
    QLabel *label_Y;
    QLabel *label_Z;
    QLabel *label_E;
    QWidget *gridLayoutWidget_4;
    QGridLayout *gridLayout;
    QPushButton *pushButton_pauseGCode;
    QPushButton *pushButton_MoveHome;
    QPushButton *pushButton_startGCode;
    QPushButton *pushButtonUseES;
    QPushButton *pushButton_continueGCode;
    QPushButton *pushButton_browseGCode;
    QLineEdit *lineEdit_fileGCode;
    QPushButton *pushButton_AboardGCode;
    QPushButton *pushButton_MotorState;
    QPushButton *pushButtonSerialConnect;
    QComboBox *comboBoxComPortName;
    QWidget *gridLayoutWidget_3;
    QGridLayout *gridLayout_2;
    QPushButton *pushButtonSpeed;
    QDoubleSpinBox *doubleSpinBoxMinSpeed;
    QLabel *label_speed;
    QLabel *label_bed_temp;
    QPushButton *pushButtonBedTemp;
    QLabel *label_max_speed;
    QLabel *label_fila;
    QPushButton *pushButtonFilament;
    QLabel *label_temp;
    QPushButton *pushButtonMaxSpeed;
    QPushButton *pushButtonMaxAcc;
    QDoubleSpinBox *doubleSpinBoxHotendTemp;
    QDoubleSpinBox *doubleSpinBoxMaxSpeed;
    QPushButton *pushButtonThreshAngle;
    QDoubleSpinBox *doubleSpinBoxBedTemp;
    QLabel *label_max_acc;
    QPushButton *pushButtoHotendTemp;
    QDoubleSpinBox *doubleSpinBoxFilament;
    QDoubleSpinBox *doubleSpinBoxSpeed;
    QLabel *label_min_speed;
    QDoubleSpinBox *doubleSpinBoxMaxAcc;
    QDoubleSpinBox *doubleSpinBoxThreshAngle;
    QLabel *label_ThreshAngle;
    QPushButton *pushButtonMinSpeed;
    QLabel *label_startTime;
    QPushButton *pushButton_waitForHeat;
    QLabel *label_lineCount;
    QPushButton *pushButton_Test;
    QPushButton *pushButtonInit;
    QTabWidget *tabWidget;
    QWidget *tab;
    QCustomPlot *Plot_PID_Temp;
    QTextEdit *textEditLog;
    QWidget *tab_2;
    QTextEdit *textEditLog_error;
    QMenuBar *menubar;
    QMenu *menuSettings;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1011, 771);
        actionValues = new QAction(MainWindow);
        actionValues->setObjectName(QString::fromUtf8("actionValues"));
        actionPID = new QAction(MainWindow);
        actionPID->setObjectName(QString::fromUtf8("actionPID"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayoutWidget = new QWidget(centralwidget);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(10, 50, 401, 171));
        MoveManuel = new QGridLayout(gridLayoutWidget);
        MoveManuel->setObjectName(QString::fromUtf8("MoveManuel"));
        MoveManuel->setContentsMargins(0, 0, 0, 0);
        pushButtonMoveXNeg = new QPushButton(gridLayoutWidget);
        pushButtonMoveXNeg->setObjectName(QString::fromUtf8("pushButtonMoveXNeg"));

        MoveManuel->addWidget(pushButtonMoveXNeg, 2, 1, 1, 1);

        pushButtonMoveYNeg = new QPushButton(gridLayoutWidget);
        pushButtonMoveYNeg->setObjectName(QString::fromUtf8("pushButtonMoveYNeg"));

        MoveManuel->addWidget(pushButtonMoveYNeg, 1, 2, 1, 1);

        pushButtonMoveXPos = new QPushButton(gridLayoutWidget);
        pushButtonMoveXPos->setObjectName(QString::fromUtf8("pushButtonMoveXPos"));

        MoveManuel->addWidget(pushButtonMoveXPos, 0, 1, 1, 1);

        pushButtonMoveYPos = new QPushButton(gridLayoutWidget);
        pushButtonMoveYPos->setObjectName(QString::fromUtf8("pushButtonMoveYPos"));

        MoveManuel->addWidget(pushButtonMoveYPos, 1, 0, 1, 1);

        pushButtonMoveZPos = new QPushButton(gridLayoutWidget);
        pushButtonMoveZPos->setObjectName(QString::fromUtf8("pushButtonMoveZPos"));

        MoveManuel->addWidget(pushButtonMoveZPos, 0, 3, 1, 1);

        pushButtonMoveZNeg = new QPushButton(gridLayoutWidget);
        pushButtonMoveZNeg->setObjectName(QString::fromUtf8("pushButtonMoveZNeg"));

        MoveManuel->addWidget(pushButtonMoveZNeg, 2, 3, 1, 1);

        pushButtonMoveEPos = new QPushButton(gridLayoutWidget);
        pushButtonMoveEPos->setObjectName(QString::fromUtf8("pushButtonMoveEPos"));

        MoveManuel->addWidget(pushButtonMoveEPos, 0, 4, 1, 1);

        pushButtonMoveENeg = new QPushButton(gridLayoutWidget);
        pushButtonMoveENeg->setObjectName(QString::fromUtf8("pushButtonMoveENeg"));

        MoveManuel->addWidget(pushButtonMoveENeg, 2, 4, 1, 1);

        pushButtonMoveSTOP = new QPushButton(gridLayoutWidget);
        pushButtonMoveSTOP->setObjectName(QString::fromUtf8("pushButtonMoveSTOP"));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(240, 240, 240, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        QBrush brush2(QColor(255, 255, 255, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Light, brush2);
        QBrush brush3(QColor(247, 247, 247, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Midlight, brush3);
        QBrush brush4(QColor(120, 120, 120, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush4);
        QBrush brush5(QColor(160, 160, 160, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        palette.setBrush(QPalette::Active, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush2);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush3);
        QBrush brush6(QColor(255, 255, 220, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ToolTipBase, brush6);
        palette.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        QBrush brush7(QColor(0, 0, 0, 128));
        brush7.setStyle(Qt::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Active, QPalette::PlaceholderText, brush7);
#endif
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush6);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Inactive, QPalette::PlaceholderText, brush7);
#endif
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush6);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Disabled, QPalette::PlaceholderText, brush7);
#endif
        pushButtonMoveSTOP->setPalette(palette);
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        font.setKerning(true);
        pushButtonMoveSTOP->setFont(font);

        MoveManuel->addWidget(pushButtonMoveSTOP, 1, 1, 1, 1);

        doubleSpinBoxZOffset = new QDoubleSpinBox(gridLayoutWidget);
        doubleSpinBoxZOffset->setObjectName(QString::fromUtf8("doubleSpinBoxZOffset"));
        doubleSpinBoxZOffset->setMinimum(-9999.000000000000000);
        doubleSpinBoxZOffset->setMaximum(9999.000000000000000);
        doubleSpinBoxZOffset->setSingleStep(0.010000000000000);

        MoveManuel->addWidget(doubleSpinBoxZOffset, 1, 3, 1, 1);

        gridLayoutWidget_2 = new QWidget(centralwidget);
        gridLayoutWidget_2->setObjectName(QString::fromUtf8("gridLayoutWidget_2"));
        gridLayoutWidget_2->setGeometry(QRect(10, 230, 481, 111));
        MoveNunmeric = new QGridLayout(gridLayoutWidget_2);
        MoveNunmeric->setObjectName(QString::fromUtf8("MoveNunmeric"));
        MoveNunmeric->setContentsMargins(0, 0, 0, 0);
        pushButtonCopyToSetPosition = new QPushButton(gridLayoutWidget_2);
        pushButtonCopyToSetPosition->setObjectName(QString::fromUtf8("pushButtonCopyToSetPosition"));

        MoveNunmeric->addWidget(pushButtonCopyToSetPosition, 4, 5, 1, 1);

        doubleSpinBoxMoveZ = new QDoubleSpinBox(gridLayoutWidget_2);
        doubleSpinBoxMoveZ->setObjectName(QString::fromUtf8("doubleSpinBoxMoveZ"));
        doubleSpinBoxMoveZ->setMinimum(-9999.000000000000000);
        doubleSpinBoxMoveZ->setMaximum(9999.000000000000000);

        MoveNunmeric->addWidget(doubleSpinBoxMoveZ, 3, 3, 1, 1);

        label_ZPos = new QLabel(gridLayoutWidget_2);
        label_ZPos->setObjectName(QString::fromUtf8("label_ZPos"));

        MoveNunmeric->addWidget(label_ZPos, 2, 3, 1, 1);

        doubleSpinBoxMoveX = new QDoubleSpinBox(gridLayoutWidget_2);
        doubleSpinBoxMoveX->setObjectName(QString::fromUtf8("doubleSpinBoxMoveX"));
        doubleSpinBoxMoveX->setMinimum(-9999.000000000000000);
        doubleSpinBoxMoveX->setMaximum(9999.000000000000000);

        MoveNunmeric->addWidget(doubleSpinBoxMoveX, 3, 1, 1, 1);

        doubleSpinBoxSetX = new QDoubleSpinBox(gridLayoutWidget_2);
        doubleSpinBoxSetX->setObjectName(QString::fromUtf8("doubleSpinBoxSetX"));
        doubleSpinBoxSetX->setMinimum(-9999.000000000000000);
        doubleSpinBoxSetX->setMaximum(9999.000000000000000);

        MoveNunmeric->addWidget(doubleSpinBoxSetX, 4, 1, 1, 1);

        label_EPos = new QLabel(gridLayoutWidget_2);
        label_EPos->setObjectName(QString::fromUtf8("label_EPos"));

        MoveNunmeric->addWidget(label_EPos, 2, 4, 1, 1);

        pushButtonMoveTo = new QPushButton(gridLayoutWidget_2);
        pushButtonMoveTo->setObjectName(QString::fromUtf8("pushButtonMoveTo"));

        MoveNunmeric->addWidget(pushButtonMoveTo, 3, 0, 1, 1);

        doubleSpinBoxMoveE = new QDoubleSpinBox(gridLayoutWidget_2);
        doubleSpinBoxMoveE->setObjectName(QString::fromUtf8("doubleSpinBoxMoveE"));
        doubleSpinBoxMoveE->setMinimum(-9999.000000000000000);
        doubleSpinBoxMoveE->setMaximum(9999.000000000000000);

        MoveNunmeric->addWidget(doubleSpinBoxMoveE, 3, 4, 1, 1);

        doubleSpinBoxMoveY = new QDoubleSpinBox(gridLayoutWidget_2);
        doubleSpinBoxMoveY->setObjectName(QString::fromUtf8("doubleSpinBoxMoveY"));
        doubleSpinBoxMoveY->setMinimum(-9999.000000000000000);
        doubleSpinBoxMoveY->setMaximum(9999.000000000000000);

        MoveNunmeric->addWidget(doubleSpinBoxMoveY, 3, 2, 1, 1);

        label_YPos = new QLabel(gridLayoutWidget_2);
        label_YPos->setObjectName(QString::fromUtf8("label_YPos"));

        MoveNunmeric->addWidget(label_YPos, 2, 2, 1, 1);

        pushButtonSetPosition = new QPushButton(gridLayoutWidget_2);
        pushButtonSetPosition->setObjectName(QString::fromUtf8("pushButtonSetPosition"));

        MoveNunmeric->addWidget(pushButtonSetPosition, 4, 0, 1, 1);

        label_XPos = new QLabel(gridLayoutWidget_2);
        label_XPos->setObjectName(QString::fromUtf8("label_XPos"));

        MoveNunmeric->addWidget(label_XPos, 2, 1, 1, 1);

        pushButtonGetPosition = new QPushButton(gridLayoutWidget_2);
        pushButtonGetPosition->setObjectName(QString::fromUtf8("pushButtonGetPosition"));

        MoveNunmeric->addWidget(pushButtonGetPosition, 2, 0, 1, 1);

        doubleSpinBoxSetE = new QDoubleSpinBox(gridLayoutWidget_2);
        doubleSpinBoxSetE->setObjectName(QString::fromUtf8("doubleSpinBoxSetE"));
        doubleSpinBoxSetE->setMinimum(-9999.000000000000000);
        doubleSpinBoxSetE->setMaximum(9999.000000000000000);

        MoveNunmeric->addWidget(doubleSpinBoxSetE, 4, 4, 1, 1);

        doubleSpinBoxSetY = new QDoubleSpinBox(gridLayoutWidget_2);
        doubleSpinBoxSetY->setObjectName(QString::fromUtf8("doubleSpinBoxSetY"));
        doubleSpinBoxSetY->setMinimum(-9999.000000000000000);
        doubleSpinBoxSetY->setMaximum(9999.000000000000000);

        MoveNunmeric->addWidget(doubleSpinBoxSetY, 4, 2, 1, 1);

        doubleSpinBoxSetZ = new QDoubleSpinBox(gridLayoutWidget_2);
        doubleSpinBoxSetZ->setObjectName(QString::fromUtf8("doubleSpinBoxSetZ"));
        doubleSpinBoxSetZ->setMinimum(-9999.000000000000000);
        doubleSpinBoxSetZ->setMaximum(9999.000000000000000);

        MoveNunmeric->addWidget(doubleSpinBoxSetZ, 4, 3, 1, 1);

        pushButtonCopyToMoveTo = new QPushButton(gridLayoutWidget_2);
        pushButtonCopyToMoveTo->setObjectName(QString::fromUtf8("pushButtonCopyToMoveTo"));

        MoveNunmeric->addWidget(pushButtonCopyToMoveTo, 3, 5, 1, 1);

        label_X = new QLabel(gridLayoutWidget_2);
        label_X->setObjectName(QString::fromUtf8("label_X"));
        label_X->setAlignment(Qt::AlignCenter);

        MoveNunmeric->addWidget(label_X, 1, 1, 1, 1);

        label_Y = new QLabel(gridLayoutWidget_2);
        label_Y->setObjectName(QString::fromUtf8("label_Y"));
        label_Y->setAlignment(Qt::AlignCenter);

        MoveNunmeric->addWidget(label_Y, 1, 2, 1, 1);

        label_Z = new QLabel(gridLayoutWidget_2);
        label_Z->setObjectName(QString::fromUtf8("label_Z"));
        label_Z->setAlignment(Qt::AlignCenter);

        MoveNunmeric->addWidget(label_Z, 1, 3, 1, 1);

        label_E = new QLabel(gridLayoutWidget_2);
        label_E->setObjectName(QString::fromUtf8("label_E"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("MS Shell Dlg 2"));
        label_E->setFont(font1);
        label_E->setAlignment(Qt::AlignCenter);

        MoveNunmeric->addWidget(label_E, 1, 4, 1, 1);

        gridLayoutWidget_4 = new QWidget(centralwidget);
        gridLayoutWidget_4->setObjectName(QString::fromUtf8("gridLayoutWidget_4"));
        gridLayoutWidget_4->setGeometry(QRect(10, 610, 711, 112));
        gridLayout = new QGridLayout(gridLayoutWidget_4);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        pushButton_pauseGCode = new QPushButton(gridLayoutWidget_4);
        pushButton_pauseGCode->setObjectName(QString::fromUtf8("pushButton_pauseGCode"));

        gridLayout->addWidget(pushButton_pauseGCode, 3, 2, 1, 1);

        pushButton_MoveHome = new QPushButton(gridLayoutWidget_4);
        pushButton_MoveHome->setObjectName(QString::fromUtf8("pushButton_MoveHome"));

        gridLayout->addWidget(pushButton_MoveHome, 4, 3, 1, 1);

        pushButton_startGCode = new QPushButton(gridLayoutWidget_4);
        pushButton_startGCode->setObjectName(QString::fromUtf8("pushButton_startGCode"));

        gridLayout->addWidget(pushButton_startGCode, 3, 0, 1, 1);

        pushButtonUseES = new QPushButton(gridLayoutWidget_4);
        pushButtonUseES->setObjectName(QString::fromUtf8("pushButtonUseES"));

        gridLayout->addWidget(pushButtonUseES, 4, 1, 1, 1);

        pushButton_continueGCode = new QPushButton(gridLayoutWidget_4);
        pushButton_continueGCode->setObjectName(QString::fromUtf8("pushButton_continueGCode"));

        gridLayout->addWidget(pushButton_continueGCode, 3, 1, 1, 1);

        pushButton_browseGCode = new QPushButton(gridLayoutWidget_4);
        pushButton_browseGCode->setObjectName(QString::fromUtf8("pushButton_browseGCode"));

        gridLayout->addWidget(pushButton_browseGCode, 2, 0, 1, 1);

        lineEdit_fileGCode = new QLineEdit(gridLayoutWidget_4);
        lineEdit_fileGCode->setObjectName(QString::fromUtf8("lineEdit_fileGCode"));

        gridLayout->addWidget(lineEdit_fileGCode, 2, 1, 1, 3);

        pushButton_AboardGCode = new QPushButton(gridLayoutWidget_4);
        pushButton_AboardGCode->setObjectName(QString::fromUtf8("pushButton_AboardGCode"));

        gridLayout->addWidget(pushButton_AboardGCode, 3, 3, 1, 1);

        pushButton_MotorState = new QPushButton(gridLayoutWidget_4);
        pushButton_MotorState->setObjectName(QString::fromUtf8("pushButton_MotorState"));

        gridLayout->addWidget(pushButton_MotorState, 4, 0, 1, 1);

        pushButtonSerialConnect = new QPushButton(centralwidget);
        pushButtonSerialConnect->setObjectName(QString::fromUtf8("pushButtonSerialConnect"));
        pushButtonSerialConnect->setGeometry(QRect(10, 10, 139, 23));
        comboBoxComPortName = new QComboBox(centralwidget);
        comboBoxComPortName->setObjectName(QString::fromUtf8("comboBoxComPortName"));
        comboBoxComPortName->setGeometry(QRect(160, 10, 62, 22));
        gridLayoutWidget_3 = new QWidget(centralwidget);
        gridLayoutWidget_3->setObjectName(QString::fromUtf8("gridLayoutWidget_3"));
        gridLayoutWidget_3->setGeometry(QRect(10, 350, 481, 251));
        gridLayout_2 = new QGridLayout(gridLayoutWidget_3);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        pushButtonSpeed = new QPushButton(gridLayoutWidget_3);
        pushButtonSpeed->setObjectName(QString::fromUtf8("pushButtonSpeed"));

        gridLayout_2->addWidget(pushButtonSpeed, 2, 0, 1, 1);

        doubleSpinBoxMinSpeed = new QDoubleSpinBox(gridLayoutWidget_3);
        doubleSpinBoxMinSpeed->setObjectName(QString::fromUtf8("doubleSpinBoxMinSpeed"));

        gridLayout_2->addWidget(doubleSpinBoxMinSpeed, 0, 1, 1, 1);

        label_speed = new QLabel(gridLayoutWidget_3);
        label_speed->setObjectName(QString::fromUtf8("label_speed"));

        gridLayout_2->addWidget(label_speed, 2, 2, 1, 1);

        label_bed_temp = new QLabel(gridLayoutWidget_3);
        label_bed_temp->setObjectName(QString::fromUtf8("label_bed_temp"));

        gridLayout_2->addWidget(label_bed_temp, 5, 2, 1, 1);

        pushButtonBedTemp = new QPushButton(gridLayoutWidget_3);
        pushButtonBedTemp->setObjectName(QString::fromUtf8("pushButtonBedTemp"));

        gridLayout_2->addWidget(pushButtonBedTemp, 5, 0, 1, 1);

        label_max_speed = new QLabel(gridLayoutWidget_3);
        label_max_speed->setObjectName(QString::fromUtf8("label_max_speed"));

        gridLayout_2->addWidget(label_max_speed, 1, 2, 1, 1);

        label_fila = new QLabel(gridLayoutWidget_3);
        label_fila->setObjectName(QString::fromUtf8("label_fila"));

        gridLayout_2->addWidget(label_fila, 7, 2, 1, 1);

        pushButtonFilament = new QPushButton(gridLayoutWidget_3);
        pushButtonFilament->setObjectName(QString::fromUtf8("pushButtonFilament"));

        gridLayout_2->addWidget(pushButtonFilament, 7, 0, 1, 1);

        label_temp = new QLabel(gridLayoutWidget_3);
        label_temp->setObjectName(QString::fromUtf8("label_temp"));

        gridLayout_2->addWidget(label_temp, 6, 2, 1, 1);

        pushButtonMaxSpeed = new QPushButton(gridLayoutWidget_3);
        pushButtonMaxSpeed->setObjectName(QString::fromUtf8("pushButtonMaxSpeed"));

        gridLayout_2->addWidget(pushButtonMaxSpeed, 1, 0, 1, 1);

        pushButtonMaxAcc = new QPushButton(gridLayoutWidget_3);
        pushButtonMaxAcc->setObjectName(QString::fromUtf8("pushButtonMaxAcc"));

        gridLayout_2->addWidget(pushButtonMaxAcc, 3, 0, 1, 1);

        doubleSpinBoxHotendTemp = new QDoubleSpinBox(gridLayoutWidget_3);
        doubleSpinBoxHotendTemp->setObjectName(QString::fromUtf8("doubleSpinBoxHotendTemp"));
        doubleSpinBoxHotendTemp->setMaximum(9999.989999999999782);

        gridLayout_2->addWidget(doubleSpinBoxHotendTemp, 6, 1, 1, 1);

        doubleSpinBoxMaxSpeed = new QDoubleSpinBox(gridLayoutWidget_3);
        doubleSpinBoxMaxSpeed->setObjectName(QString::fromUtf8("doubleSpinBoxMaxSpeed"));
        doubleSpinBoxMaxSpeed->setMaximum(999.990000000000009);

        gridLayout_2->addWidget(doubleSpinBoxMaxSpeed, 1, 1, 1, 1);

        pushButtonThreshAngle = new QPushButton(gridLayoutWidget_3);
        pushButtonThreshAngle->setObjectName(QString::fromUtf8("pushButtonThreshAngle"));

        gridLayout_2->addWidget(pushButtonThreshAngle, 4, 0, 1, 1);

        doubleSpinBoxBedTemp = new QDoubleSpinBox(gridLayoutWidget_3);
        doubleSpinBoxBedTemp->setObjectName(QString::fromUtf8("doubleSpinBoxBedTemp"));
        doubleSpinBoxBedTemp->setMaximum(999.990000000000009);

        gridLayout_2->addWidget(doubleSpinBoxBedTemp, 5, 1, 1, 1);

        label_max_acc = new QLabel(gridLayoutWidget_3);
        label_max_acc->setObjectName(QString::fromUtf8("label_max_acc"));

        gridLayout_2->addWidget(label_max_acc, 3, 2, 1, 1);

        pushButtoHotendTemp = new QPushButton(gridLayoutWidget_3);
        pushButtoHotendTemp->setObjectName(QString::fromUtf8("pushButtoHotendTemp"));

        gridLayout_2->addWidget(pushButtoHotendTemp, 6, 0, 1, 1);

        doubleSpinBoxFilament = new QDoubleSpinBox(gridLayoutWidget_3);
        doubleSpinBoxFilament->setObjectName(QString::fromUtf8("doubleSpinBoxFilament"));
        doubleSpinBoxFilament->setMinimum(-9999.000000000000000);
        doubleSpinBoxFilament->setMaximum(9999.989999999999782);

        gridLayout_2->addWidget(doubleSpinBoxFilament, 7, 1, 1, 1);

        doubleSpinBoxSpeed = new QDoubleSpinBox(gridLayoutWidget_3);
        doubleSpinBoxSpeed->setObjectName(QString::fromUtf8("doubleSpinBoxSpeed"));
        doubleSpinBoxSpeed->setMaximum(999.990000000000009);

        gridLayout_2->addWidget(doubleSpinBoxSpeed, 2, 1, 1, 1);

        label_min_speed = new QLabel(gridLayoutWidget_3);
        label_min_speed->setObjectName(QString::fromUtf8("label_min_speed"));

        gridLayout_2->addWidget(label_min_speed, 0, 2, 1, 1);

        doubleSpinBoxMaxAcc = new QDoubleSpinBox(gridLayoutWidget_3);
        doubleSpinBoxMaxAcc->setObjectName(QString::fromUtf8("doubleSpinBoxMaxAcc"));
        doubleSpinBoxMaxAcc->setMaximum(9999.989999999999782);

        gridLayout_2->addWidget(doubleSpinBoxMaxAcc, 3, 1, 1, 1);

        doubleSpinBoxThreshAngle = new QDoubleSpinBox(gridLayoutWidget_3);
        doubleSpinBoxThreshAngle->setObjectName(QString::fromUtf8("doubleSpinBoxThreshAngle"));
        doubleSpinBoxThreshAngle->setMaximum(9999.989999999999782);

        gridLayout_2->addWidget(doubleSpinBoxThreshAngle, 4, 1, 1, 1);

        label_ThreshAngle = new QLabel(gridLayoutWidget_3);
        label_ThreshAngle->setObjectName(QString::fromUtf8("label_ThreshAngle"));

        gridLayout_2->addWidget(label_ThreshAngle, 4, 2, 1, 1);

        pushButtonMinSpeed = new QPushButton(gridLayoutWidget_3);
        pushButtonMinSpeed->setObjectName(QString::fromUtf8("pushButtonMinSpeed"));

        gridLayout_2->addWidget(pushButtonMinSpeed, 0, 0, 1, 1);

        label_startTime = new QLabel(gridLayoutWidget_3);
        label_startTime->setObjectName(QString::fromUtf8("label_startTime"));

        gridLayout_2->addWidget(label_startTime, 0, 3, 1, 1);

        pushButton_waitForHeat = new QPushButton(gridLayoutWidget_3);
        pushButton_waitForHeat->setObjectName(QString::fromUtf8("pushButton_waitForHeat"));

        gridLayout_2->addWidget(pushButton_waitForHeat, 6, 3, 1, 1);

        label_lineCount = new QLabel(gridLayoutWidget_3);
        label_lineCount->setObjectName(QString::fromUtf8("label_lineCount"));

        gridLayout_2->addWidget(label_lineCount, 1, 3, 1, 1);

        pushButton_Test = new QPushButton(centralwidget);
        pushButton_Test->setObjectName(QString::fromUtf8("pushButton_Test"));
        pushButton_Test->setGeometry(QRect(730, 610, 115, 23));
        pushButtonInit = new QPushButton(centralwidget);
        pushButtonInit->setObjectName(QString::fromUtf8("pushButtonInit"));
        pushButtonInit->setGeometry(QRect(230, 10, 139, 23));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(500, 0, 501, 601));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        Plot_PID_Temp = new QCustomPlot(tab);
        Plot_PID_Temp->setObjectName(QString::fromUtf8("Plot_PID_Temp"));
        Plot_PID_Temp->setGeometry(QRect(0, 300, 491, 271));
        textEditLog = new QTextEdit(tab);
        textEditLog->setObjectName(QString::fromUtf8("textEditLog"));
        textEditLog->setEnabled(true);
        textEditLog->setGeometry(QRect(0, 0, 491, 291));
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        textEditLog_error = new QTextEdit(tab_2);
        textEditLog_error->setObjectName(QString::fromUtf8("textEditLog_error"));
        textEditLog_error->setEnabled(true);
        textEditLog_error->setGeometry(QRect(0, 0, 491, 291));
        tabWidget->addTab(tab_2, QString());
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1011, 22));
        menuSettings = new QMenu(menubar);
        menuSettings->setObjectName(QString::fromUtf8("menuSettings"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuSettings->menuAction());
        menuSettings->addAction(actionValues);
        menuSettings->addAction(actionPID);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionValues->setText(QCoreApplication::translate("MainWindow", "Values", nullptr));
        actionPID->setText(QCoreApplication::translate("MainWindow", "PID", nullptr));
        pushButtonMoveXNeg->setText(QCoreApplication::translate("MainWindow", "X-", nullptr));
        pushButtonMoveYNeg->setText(QCoreApplication::translate("MainWindow", "Y-", nullptr));
        pushButtonMoveXPos->setText(QCoreApplication::translate("MainWindow", "X+", nullptr));
        pushButtonMoveYPos->setText(QCoreApplication::translate("MainWindow", "Y+", nullptr));
        pushButtonMoveZPos->setText(QCoreApplication::translate("MainWindow", "Z+", nullptr));
        pushButtonMoveZNeg->setText(QCoreApplication::translate("MainWindow", "Z-", nullptr));
        pushButtonMoveEPos->setText(QCoreApplication::translate("MainWindow", "E+", nullptr));
        pushButtonMoveENeg->setText(QCoreApplication::translate("MainWindow", "E-", nullptr));
        pushButtonMoveSTOP->setText(QCoreApplication::translate("MainWindow", "STOP", nullptr));
        pushButtonCopyToSetPosition->setText(QCoreApplication::translate("MainWindow", "CopyPosition", nullptr));
        label_ZPos->setText(QCoreApplication::translate("MainWindow", "0,00", nullptr));
        label_EPos->setText(QCoreApplication::translate("MainWindow", "0,00", nullptr));
        pushButtonMoveTo->setText(QCoreApplication::translate("MainWindow", "Move to", nullptr));
        label_YPos->setText(QCoreApplication::translate("MainWindow", "0,00", nullptr));
        pushButtonSetPosition->setText(QCoreApplication::translate("MainWindow", "SetPosition", nullptr));
        label_XPos->setText(QCoreApplication::translate("MainWindow", "0,00", nullptr));
        pushButtonGetPosition->setText(QCoreApplication::translate("MainWindow", "GetPosition", nullptr));
        pushButtonCopyToMoveTo->setText(QCoreApplication::translate("MainWindow", "CopyPosition", nullptr));
        label_X->setText(QCoreApplication::translate("MainWindow", "X", nullptr));
        label_Y->setText(QCoreApplication::translate("MainWindow", "Y", nullptr));
        label_Z->setText(QCoreApplication::translate("MainWindow", "Z", nullptr));
        label_E->setText(QCoreApplication::translate("MainWindow", "E", nullptr));
        pushButton_pauseGCode->setText(QCoreApplication::translate("MainWindow", "Pause G-Code", nullptr));
        pushButton_MoveHome->setText(QCoreApplication::translate("MainWindow", "Move Home", nullptr));
        pushButton_startGCode->setText(QCoreApplication::translate("MainWindow", "Start G-Code", nullptr));
        pushButtonUseES->setText(QCoreApplication::translate("MainWindow", "use Endswitch", nullptr));
        pushButton_continueGCode->setText(QCoreApplication::translate("MainWindow", "Continue G-Code", nullptr));
        pushButton_browseGCode->setText(QCoreApplication::translate("MainWindow", "Brows G-Code", nullptr));
        pushButton_AboardGCode->setText(QCoreApplication::translate("MainWindow", "Aboard G-Code", nullptr));
        pushButton_MotorState->setText(QCoreApplication::translate("MainWindow", "Motors", nullptr));
        pushButtonSerialConnect->setText(QCoreApplication::translate("MainWindow", "Connect", nullptr));
        pushButtonSpeed->setText(QCoreApplication::translate("MainWindow", "Speed", nullptr));
        label_speed->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        label_bed_temp->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        pushButtonBedTemp->setText(QCoreApplication::translate("MainWindow", "Bed Temperatur", nullptr));
        label_max_speed->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        label_fila->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        pushButtonFilament->setText(QCoreApplication::translate("MainWindow", "Filament", nullptr));
        label_temp->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        pushButtonMaxSpeed->setText(QCoreApplication::translate("MainWindow", "max Speed", nullptr));
        pushButtonMaxAcc->setText(QCoreApplication::translate("MainWindow", "MaxAcc", nullptr));
        pushButtonThreshAngle->setText(QCoreApplication::translate("MainWindow", "ThreshAngle", nullptr));
        label_max_acc->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        pushButtoHotendTemp->setText(QCoreApplication::translate("MainWindow", "Hotend Temperatur", nullptr));
        label_min_speed->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        label_ThreshAngle->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        pushButtonMinSpeed->setText(QCoreApplication::translate("MainWindow", "min Speed", nullptr));
        label_startTime->setText(QCoreApplication::translate("MainWindow", "Start:", nullptr));
        pushButton_waitForHeat->setText(QCoreApplication::translate("MainWindow", "waitForHeat", nullptr));
        label_lineCount->setText(QCoreApplication::translate("MainWindow", "Count:", nullptr));
        pushButton_Test->setText(QCoreApplication::translate("MainWindow", "TEST XXX", nullptr));
        pushButtonInit->setText(QCoreApplication::translate("MainWindow", "Init", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("MainWindow", "Tab 1", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("MainWindow", "Tab 2", nullptr));
        menuSettings->setTitle(QCoreApplication::translate("MainWindow", "Settings", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
