/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *gridLayoutWidget;
    QGridLayout *MoveManuel;
    QPushButton *pushButtonMoveXNeg;
    QPushButton *pushButtonMoveYNeg;
    QPushButton *pushButtonMoveXPos;
    QPushButton *pushButtonMoveYPos;
    QPushButton *pushButtonMoveZPos;
    QPushButton *pushButtonMoveZNeg;
    QPushButton *pushButtonMoveWPos;
    QPushButton *pushButtonMoveWNeg;
    QPushButton *pushButtonMoveSTOP;
    QWidget *gridLayoutWidget_2;
    QGridLayout *MoveNunmeric;
    QPushButton *pushButtonCopyToSetPosition;
    QDoubleSpinBox *doubleSpinBoxMoveZ;
    QLabel *label_ZPos;
    QDoubleSpinBox *doubleSpinBoxMoveX;
    QDoubleSpinBox *doubleSpinBoxSetX;
    QLabel *label_WPos;
    QPushButton *pushButtonMoveTo;
    QDoubleSpinBox *doubleSpinBoxMoveW;
    QDoubleSpinBox *doubleSpinBoxMoveY;
    QLabel *label_YPos;
    QPushButton *pushButtonSetPosition;
    QLabel *label_XPos;
    QPushButton *pushButtonGetPosition;
    QDoubleSpinBox *doubleSpinBoxSetW;
    QDoubleSpinBox *doubleSpinBoxSetY;
    QDoubleSpinBox *doubleSpinBoxSetZ;
    QPushButton *pushButtonCopyToMoveTo;
    QLabel *label_X;
    QLabel *label_Y;
    QLabel *label_Z;
    QLabel *label_W;
    QWidget *gridLayoutWidget_3;
    QGridLayout *Settings;
    QLabel *label_actTemperatur;
    QPushButton *pushButtonSetSettings;
    QLabel *label_Speed;
    QLabel *label_actFilament;
    QSpinBox *spinBoxFilament;
    QLabel *label_actSpeed;
    QLabel *label_Filament;
    QSpinBox *spinBoxTemperatur;
    QSpinBox *spinBoxSpeed;
    QLabel *label_Temperatur;
    QLabel *label_Power;
    QLabel *label_actPower;
    QPushButton *pushButtonSerialConnect;
    QTextEdit *textEditLog;
    QComboBox *comboBoxComPortName;
    QWidget *gridLayoutWidget_4;
    QGridLayout *gridLayout;
    QPushButton *pushButton_browseGCode;
    QPushButton *pushButton_startGCode;
    QPushButton *pushButton_pauseGCode;
    QLineEdit *lineEdit_fileGCode;
    QPushButton *pushButton_AboardGCode;
    QTextEdit *textEditLog_error;
    QWidget *gridLayoutWidget_5;
    QGridLayout *gridLayout_2;
    QPushButton *pushButton_home;
    QPushButton *pushButton_repeattest;
    QPushButton *pushButton_sizecalib;
    QPushButton *pushButton_Zcalib;
    QPushButton *pushButton_test;
    QPushButton *pushButton_rest;
    QLabel *label_alive;
    QWidget *gridLayoutWidget_6;
    QGridLayout *gridLayout_3;
    QLabel *label_heating;
    QLabel *label_moveing;
    QMenuBar *menubar;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1028, 773);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayoutWidget = new QWidget(centralwidget);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(20, 60, 401, 171));
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

        pushButtonMoveWPos = new QPushButton(gridLayoutWidget);
        pushButtonMoveWPos->setObjectName(QString::fromUtf8("pushButtonMoveWPos"));

        MoveManuel->addWidget(pushButtonMoveWPos, 0, 4, 1, 1);

        pushButtonMoveWNeg = new QPushButton(gridLayoutWidget);
        pushButtonMoveWNeg->setObjectName(QString::fromUtf8("pushButtonMoveWNeg"));

        MoveManuel->addWidget(pushButtonMoveWNeg, 2, 4, 1, 1);

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

        gridLayoutWidget_2 = new QWidget(centralwidget);
        gridLayoutWidget_2->setObjectName(QString::fromUtf8("gridLayoutWidget_2"));
        gridLayoutWidget_2->setGeometry(QRect(20, 250, 481, 111));
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

        label_WPos = new QLabel(gridLayoutWidget_2);
        label_WPos->setObjectName(QString::fromUtf8("label_WPos"));

        MoveNunmeric->addWidget(label_WPos, 2, 4, 1, 1);

        pushButtonMoveTo = new QPushButton(gridLayoutWidget_2);
        pushButtonMoveTo->setObjectName(QString::fromUtf8("pushButtonMoveTo"));

        MoveNunmeric->addWidget(pushButtonMoveTo, 3, 0, 1, 1);

        doubleSpinBoxMoveW = new QDoubleSpinBox(gridLayoutWidget_2);
        doubleSpinBoxMoveW->setObjectName(QString::fromUtf8("doubleSpinBoxMoveW"));
        doubleSpinBoxMoveW->setMinimum(-9999.000000000000000);
        doubleSpinBoxMoveW->setMaximum(9999.000000000000000);

        MoveNunmeric->addWidget(doubleSpinBoxMoveW, 3, 4, 1, 1);

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

        doubleSpinBoxSetW = new QDoubleSpinBox(gridLayoutWidget_2);
        doubleSpinBoxSetW->setObjectName(QString::fromUtf8("doubleSpinBoxSetW"));
        doubleSpinBoxSetW->setMinimum(-9999.000000000000000);
        doubleSpinBoxSetW->setMaximum(9999.000000000000000);

        MoveNunmeric->addWidget(doubleSpinBoxSetW, 4, 4, 1, 1);

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

        label_W = new QLabel(gridLayoutWidget_2);
        label_W->setObjectName(QString::fromUtf8("label_W"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("MS Shell Dlg 2"));
        label_W->setFont(font1);
        label_W->setAlignment(Qt::AlignCenter);

        MoveNunmeric->addWidget(label_W, 1, 4, 1, 1);

        gridLayoutWidget_3 = new QWidget(centralwidget);
        gridLayoutWidget_3->setObjectName(QString::fromUtf8("gridLayoutWidget_3"));
        gridLayoutWidget_3->setGeometry(QRect(20, 380, 471, 80));
        Settings = new QGridLayout(gridLayoutWidget_3);
        Settings->setObjectName(QString::fromUtf8("Settings"));
        Settings->setContentsMargins(0, 0, 0, 0);
        label_actTemperatur = new QLabel(gridLayoutWidget_3);
        label_actTemperatur->setObjectName(QString::fromUtf8("label_actTemperatur"));

        Settings->addWidget(label_actTemperatur, 1, 2, 1, 1);

        pushButtonSetSettings = new QPushButton(gridLayoutWidget_3);
        pushButtonSetSettings->setObjectName(QString::fromUtf8("pushButtonSetSettings"));

        Settings->addWidget(pushButtonSetSettings, 2, 0, 1, 1);

        label_Speed = new QLabel(gridLayoutWidget_3);
        label_Speed->setObjectName(QString::fromUtf8("label_Speed"));

        Settings->addWidget(label_Speed, 0, 1, 1, 1);

        label_actFilament = new QLabel(gridLayoutWidget_3);
        label_actFilament->setObjectName(QString::fromUtf8("label_actFilament"));

        Settings->addWidget(label_actFilament, 1, 3, 1, 1);

        spinBoxFilament = new QSpinBox(gridLayoutWidget_3);
        spinBoxFilament->setObjectName(QString::fromUtf8("spinBoxFilament"));
        spinBoxFilament->setMaximum(999);
        spinBoxFilament->setValue(37);

        Settings->addWidget(spinBoxFilament, 2, 3, 1, 1);

        label_actSpeed = new QLabel(gridLayoutWidget_3);
        label_actSpeed->setObjectName(QString::fromUtf8("label_actSpeed"));

        Settings->addWidget(label_actSpeed, 1, 1, 1, 1);

        label_Filament = new QLabel(gridLayoutWidget_3);
        label_Filament->setObjectName(QString::fromUtf8("label_Filament"));

        Settings->addWidget(label_Filament, 0, 3, 1, 1);

        spinBoxTemperatur = new QSpinBox(gridLayoutWidget_3);
        spinBoxTemperatur->setObjectName(QString::fromUtf8("spinBoxTemperatur"));
        spinBoxTemperatur->setMaximum(999);

        Settings->addWidget(spinBoxTemperatur, 2, 2, 1, 1);

        spinBoxSpeed = new QSpinBox(gridLayoutWidget_3);
        spinBoxSpeed->setObjectName(QString::fromUtf8("spinBoxSpeed"));
        spinBoxSpeed->setMaximum(999);

        Settings->addWidget(spinBoxSpeed, 2, 1, 1, 1);

        label_Temperatur = new QLabel(gridLayoutWidget_3);
        label_Temperatur->setObjectName(QString::fromUtf8("label_Temperatur"));

        Settings->addWidget(label_Temperatur, 0, 2, 1, 1);

        label_Power = new QLabel(gridLayoutWidget_3);
        label_Power->setObjectName(QString::fromUtf8("label_Power"));

        Settings->addWidget(label_Power, 0, 4, 1, 1);

        label_actPower = new QLabel(gridLayoutWidget_3);
        label_actPower->setObjectName(QString::fromUtf8("label_actPower"));

        Settings->addWidget(label_actPower, 1, 4, 1, 1);

        pushButtonSerialConnect = new QPushButton(centralwidget);
        pushButtonSerialConnect->setObjectName(QString::fromUtf8("pushButtonSerialConnect"));
        pushButtonSerialConnect->setGeometry(QRect(20, 20, 139, 23));
        textEditLog = new QTextEdit(centralwidget);
        textEditLog->setObjectName(QString::fromUtf8("textEditLog"));
        textEditLog->setEnabled(true);
        textEditLog->setGeometry(QRect(510, 10, 511, 441));
        comboBoxComPortName = new QComboBox(centralwidget);
        comboBoxComPortName->setObjectName(QString::fromUtf8("comboBoxComPortName"));
        comboBoxComPortName->setGeometry(QRect(200, 20, 62, 22));
        gridLayoutWidget_4 = new QWidget(centralwidget);
        gridLayoutWidget_4->setObjectName(QString::fromUtf8("gridLayoutWidget_4"));
        gridLayoutWidget_4->setGeometry(QRect(19, 479, 471, 141));
        gridLayout = new QGridLayout(gridLayoutWidget_4);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        pushButton_browseGCode = new QPushButton(gridLayoutWidget_4);
        pushButton_browseGCode->setObjectName(QString::fromUtf8("pushButton_browseGCode"));

        gridLayout->addWidget(pushButton_browseGCode, 0, 0, 1, 1);

        pushButton_startGCode = new QPushButton(gridLayoutWidget_4);
        pushButton_startGCode->setObjectName(QString::fromUtf8("pushButton_startGCode"));

        gridLayout->addWidget(pushButton_startGCode, 1, 0, 1, 1);

        pushButton_pauseGCode = new QPushButton(gridLayoutWidget_4);
        pushButton_pauseGCode->setObjectName(QString::fromUtf8("pushButton_pauseGCode"));

        gridLayout->addWidget(pushButton_pauseGCode, 1, 1, 1, 1);

        lineEdit_fileGCode = new QLineEdit(gridLayoutWidget_4);
        lineEdit_fileGCode->setObjectName(QString::fromUtf8("lineEdit_fileGCode"));

        gridLayout->addWidget(lineEdit_fileGCode, 0, 1, 1, 2);

        pushButton_AboardGCode = new QPushButton(gridLayoutWidget_4);
        pushButton_AboardGCode->setObjectName(QString::fromUtf8("pushButton_AboardGCode"));

        gridLayout->addWidget(pushButton_AboardGCode, 1, 2, 1, 1);

        textEditLog_error = new QTextEdit(centralwidget);
        textEditLog_error->setObjectName(QString::fromUtf8("textEditLog_error"));
        textEditLog_error->setEnabled(true);
        textEditLog_error->setGeometry(QRect(520, 460, 511, 151));
        gridLayoutWidget_5 = new QWidget(centralwidget);
        gridLayoutWidget_5->setObjectName(QString::fromUtf8("gridLayoutWidget_5"));
        gridLayoutWidget_5->setGeometry(QRect(20, 630, 565, 81));
        gridLayout_2 = new QGridLayout(gridLayoutWidget_5);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        pushButton_home = new QPushButton(gridLayoutWidget_5);
        pushButton_home->setObjectName(QString::fromUtf8("pushButton_home"));

        gridLayout_2->addWidget(pushButton_home, 0, 0, 1, 1);

        pushButton_repeattest = new QPushButton(gridLayoutWidget_5);
        pushButton_repeattest->setObjectName(QString::fromUtf8("pushButton_repeattest"));

        gridLayout_2->addWidget(pushButton_repeattest, 0, 2, 1, 1);

        pushButton_sizecalib = new QPushButton(gridLayoutWidget_5);
        pushButton_sizecalib->setObjectName(QString::fromUtf8("pushButton_sizecalib"));

        gridLayout_2->addWidget(pushButton_sizecalib, 0, 1, 1, 1);

        pushButton_Zcalib = new QPushButton(gridLayoutWidget_5);
        pushButton_Zcalib->setObjectName(QString::fromUtf8("pushButton_Zcalib"));

        gridLayout_2->addWidget(pushButton_Zcalib, 0, 3, 1, 1);

        pushButton_test = new QPushButton(gridLayoutWidget_5);
        pushButton_test->setObjectName(QString::fromUtf8("pushButton_test"));

        gridLayout_2->addWidget(pushButton_test, 1, 3, 1, 1);

        pushButton_rest = new QPushButton(gridLayoutWidget_5);
        pushButton_rest->setObjectName(QString::fromUtf8("pushButton_rest"));

        gridLayout_2->addWidget(pushButton_rest, 1, 0, 1, 1);

        label_alive = new QLabel(centralwidget);
        label_alive->setObjectName(QString::fromUtf8("label_alive"));
        label_alive->setGeometry(QRect(440, 30, 47, 14));
        gridLayoutWidget_6 = new QWidget(centralwidget);
        gridLayoutWidget_6->setObjectName(QString::fromUtf8("gridLayoutWidget_6"));
        gridLayoutWidget_6->setGeometry(QRect(630, 630, 160, 80));
        gridLayout_3 = new QGridLayout(gridLayoutWidget_6);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        label_heating = new QLabel(gridLayoutWidget_6);
        label_heating->setObjectName(QString::fromUtf8("label_heating"));

        gridLayout_3->addWidget(label_heating, 1, 0, 1, 1);

        label_moveing = new QLabel(gridLayoutWidget_6);
        label_moveing->setObjectName(QString::fromUtf8("label_moveing"));

        gridLayout_3->addWidget(label_moveing, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1028, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        MainWindow->addToolBar(Qt::BottomToolBarArea, toolBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        pushButtonMoveXNeg->setText(QApplication::translate("MainWindow", "X-", nullptr));
        pushButtonMoveYNeg->setText(QApplication::translate("MainWindow", "Y-", nullptr));
        pushButtonMoveXPos->setText(QApplication::translate("MainWindow", "X+", nullptr));
        pushButtonMoveYPos->setText(QApplication::translate("MainWindow", "Y+", nullptr));
        pushButtonMoveZPos->setText(QApplication::translate("MainWindow", "Z+", nullptr));
        pushButtonMoveZNeg->setText(QApplication::translate("MainWindow", "Z-", nullptr));
        pushButtonMoveWPos->setText(QApplication::translate("MainWindow", "W+", nullptr));
        pushButtonMoveWNeg->setText(QApplication::translate("MainWindow", "W-", nullptr));
        pushButtonMoveSTOP->setText(QApplication::translate("MainWindow", "STOP", nullptr));
        pushButtonCopyToSetPosition->setText(QApplication::translate("MainWindow", "CopyPosition", nullptr));
        label_ZPos->setText(QApplication::translate("MainWindow", "0,00", nullptr));
        label_WPos->setText(QApplication::translate("MainWindow", "0,00", nullptr));
        pushButtonMoveTo->setText(QApplication::translate("MainWindow", "Move to", nullptr));
        label_YPos->setText(QApplication::translate("MainWindow", "0,00", nullptr));
        pushButtonSetPosition->setText(QApplication::translate("MainWindow", "SetPosition", nullptr));
        label_XPos->setText(QApplication::translate("MainWindow", "0,00", nullptr));
        pushButtonGetPosition->setText(QApplication::translate("MainWindow", "GetPosition", nullptr));
        pushButtonCopyToMoveTo->setText(QApplication::translate("MainWindow", "CopyPosition", nullptr));
        label_X->setText(QApplication::translate("MainWindow", "X", nullptr));
        label_Y->setText(QApplication::translate("MainWindow", "Y", nullptr));
        label_Z->setText(QApplication::translate("MainWindow", "Z", nullptr));
        label_W->setText(QApplication::translate("MainWindow", "W", nullptr));
        label_actTemperatur->setText(QApplication::translate("MainWindow", "--", nullptr));
        pushButtonSetSettings->setText(QApplication::translate("MainWindow", "SetSettings", nullptr));
        label_Speed->setText(QApplication::translate("MainWindow", "Speed (mm/sek)", nullptr));
        label_actFilament->setText(QApplication::translate("MainWindow", " --", nullptr));
        label_actSpeed->setText(QApplication::translate("MainWindow", "--", nullptr));
        label_Filament->setText(QApplication::translate("MainWindow", "Filament (Tick/mm)", nullptr));
        label_Temperatur->setText(QApplication::translate("MainWindow", "Temperatur (\302\260C)", nullptr));
        label_Power->setText(QApplication::translate("MainWindow", "PWM Output", nullptr));
        label_actPower->setText(QApplication::translate("MainWindow", "--", nullptr));
        pushButtonSerialConnect->setText(QApplication::translate("MainWindow", "Connect", nullptr));
        pushButton_browseGCode->setText(QApplication::translate("MainWindow", "Brows G-Code", nullptr));
        pushButton_startGCode->setText(QApplication::translate("MainWindow", "Start G-Code", nullptr));
        pushButton_pauseGCode->setText(QApplication::translate("MainWindow", "Pause G-Code", nullptr));
        pushButton_AboardGCode->setText(QApplication::translate("MainWindow", "Aboard G-Code", nullptr));
        pushButton_home->setText(QApplication::translate("MainWindow", "HOME", nullptr));
        pushButton_repeattest->setText(QApplication::translate("MainWindow", "repeat test", nullptr));
        pushButton_sizecalib->setText(QApplication::translate("MainWindow", "calib size", nullptr));
        pushButton_Zcalib->setText(QApplication::translate("MainWindow", "Z calib", nullptr));
        pushButton_test->setText(QApplication::translate("MainWindow", "TEST", nullptr));
        pushButton_rest->setText(QApplication::translate("MainWindow", "RestPosition", nullptr));
        label_alive->setText(QApplication::translate("MainWindow", "alive", nullptr));
        label_heating->setText(QApplication::translate("MainWindow", "heating", nullptr));
        label_moveing->setText(QApplication::translate("MainWindow", "moveing", nullptr));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
