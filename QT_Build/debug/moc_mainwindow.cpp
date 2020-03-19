/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../CNC_MultiTool_GUI/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[47];
    char stringdata0[1037];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 12), // "serial_start"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 12), // "serial_close"
QT_MOC_LITERAL(4, 38, 3), // "Log"
QT_MOC_LITERAL(5, 42, 1), // "s"
QT_MOC_LITERAL(6, 44, 8), // "errorLog"
QT_MOC_LITERAL(7, 53, 4), // "test"
QT_MOC_LITERAL(8, 58, 13), // "show_position"
QT_MOC_LITERAL(9, 72, 13), // "show_settings"
QT_MOC_LITERAL(10, 86, 14), // "show_endswitch"
QT_MOC_LITERAL(11, 101, 1), // "X"
QT_MOC_LITERAL(12, 103, 1), // "Y"
QT_MOC_LITERAL(13, 105, 1), // "Z"
QT_MOC_LITERAL(14, 107, 11), // "show_serial"
QT_MOC_LITERAL(15, 119, 6), // "isOpen"
QT_MOC_LITERAL(16, 126, 11), // "show_status"
QT_MOC_LITERAL(17, 138, 14), // "sendStopMoving"
QT_MOC_LITERAL(18, 153, 34), // "on_pushButtonSerialConnect_cl..."
QT_MOC_LITERAL(19, 188, 29), // "on_pushButtonMoveXPos_pressed"
QT_MOC_LITERAL(20, 218, 29), // "on_pushButtonMoveXNeg_pressed"
QT_MOC_LITERAL(21, 248, 29), // "on_pushButtonMoveYPos_pressed"
QT_MOC_LITERAL(22, 278, 29), // "on_pushButtonMoveYNeg_pressed"
QT_MOC_LITERAL(23, 308, 29), // "on_pushButtonMoveZPos_pressed"
QT_MOC_LITERAL(24, 338, 29), // "on_pushButtonMoveWNeg_pressed"
QT_MOC_LITERAL(25, 368, 29), // "on_pushButtonMoveZNeg_pressed"
QT_MOC_LITERAL(26, 398, 29), // "on_pushButtonMoveWPos_pressed"
QT_MOC_LITERAL(27, 428, 33), // "on_pushButtonCopyToMoveTo_pre..."
QT_MOC_LITERAL(28, 462, 38), // "on_pushButtonCopyToSetPositio..."
QT_MOC_LITERAL(29, 501, 32), // "on_pushButtonGetPosition_pressed"
QT_MOC_LITERAL(30, 534, 27), // "on_pushButtonMoveTo_pressed"
QT_MOC_LITERAL(31, 562, 32), // "on_pushButtonSetPosition_pressed"
QT_MOC_LITERAL(32, 595, 32), // "on_pushButtonSetSettings_pressed"
QT_MOC_LITERAL(33, 628, 33), // "on_pushButton_browseGCode_pre..."
QT_MOC_LITERAL(34, 662, 32), // "on_pushButton_startGCode_pressed"
QT_MOC_LITERAL(35, 695, 28), // "on_pushButton_Zcalib_pressed"
QT_MOC_LITERAL(36, 724, 26), // "on_pushButton_home_pressed"
QT_MOC_LITERAL(37, 751, 31), // "on_pushButton_sizecalib_pressed"
QT_MOC_LITERAL(38, 783, 32), // "on_pushButton_repeattest_pressed"
QT_MOC_LITERAL(39, 816, 32), // "on_pushButton_pauseGCode_pressed"
QT_MOC_LITERAL(40, 849, 33), // "on_pushButton_AboardGCode_pre..."
QT_MOC_LITERAL(41, 883, 26), // "on_pushButton_test_pressed"
QT_MOC_LITERAL(42, 910, 26), // "on_pushButton_rest_pressed"
QT_MOC_LITERAL(43, 937, 28), // "on_spinBoxSpeed_valueChanged"
QT_MOC_LITERAL(44, 966, 4), // "arg1"
QT_MOC_LITERAL(45, 971, 33), // "on_spinBoxTemperatur_valueCha..."
QT_MOC_LITERAL(46, 1005, 31) // "on_spinBoxFilament_valueChanged"

    },
    "MainWindow\0serial_start\0\0serial_close\0"
    "Log\0s\0errorLog\0test\0show_position\0"
    "show_settings\0show_endswitch\0X\0Y\0Z\0"
    "show_serial\0isOpen\0show_status\0"
    "sendStopMoving\0on_pushButtonSerialConnect_clicked\0"
    "on_pushButtonMoveXPos_pressed\0"
    "on_pushButtonMoveXNeg_pressed\0"
    "on_pushButtonMoveYPos_pressed\0"
    "on_pushButtonMoveYNeg_pressed\0"
    "on_pushButtonMoveZPos_pressed\0"
    "on_pushButtonMoveWNeg_pressed\0"
    "on_pushButtonMoveZNeg_pressed\0"
    "on_pushButtonMoveWPos_pressed\0"
    "on_pushButtonCopyToMoveTo_pressed\0"
    "on_pushButtonCopyToSetPosition_pressed\0"
    "on_pushButtonGetPosition_pressed\0"
    "on_pushButtonMoveTo_pressed\0"
    "on_pushButtonSetPosition_pressed\0"
    "on_pushButtonSetSettings_pressed\0"
    "on_pushButton_browseGCode_pressed\0"
    "on_pushButton_startGCode_pressed\0"
    "on_pushButton_Zcalib_pressed\0"
    "on_pushButton_home_pressed\0"
    "on_pushButton_sizecalib_pressed\0"
    "on_pushButton_repeattest_pressed\0"
    "on_pushButton_pauseGCode_pressed\0"
    "on_pushButton_AboardGCode_pressed\0"
    "on_pushButton_test_pressed\0"
    "on_pushButton_rest_pressed\0"
    "on_spinBoxSpeed_valueChanged\0arg1\0"
    "on_spinBoxTemperatur_valueChanged\0"
    "on_spinBoxFilament_valueChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      39,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  209,    2, 0x06 /* Public */,
       3,    0,  210,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,  211,    2, 0x08 /* Private */,
       6,    1,  214,    2, 0x08 /* Private */,
       7,    0,  217,    2, 0x08 /* Private */,
       8,    0,  218,    2, 0x08 /* Private */,
       9,    0,  219,    2, 0x08 /* Private */,
      10,    3,  220,    2, 0x08 /* Private */,
      14,    1,  227,    2, 0x08 /* Private */,
      16,    0,  230,    2, 0x08 /* Private */,
      17,    0,  231,    2, 0x08 /* Private */,
      18,    0,  232,    2, 0x08 /* Private */,
      19,    0,  233,    2, 0x08 /* Private */,
      20,    0,  234,    2, 0x08 /* Private */,
      21,    0,  235,    2, 0x08 /* Private */,
      22,    0,  236,    2, 0x08 /* Private */,
      23,    0,  237,    2, 0x08 /* Private */,
      24,    0,  238,    2, 0x08 /* Private */,
      25,    0,  239,    2, 0x08 /* Private */,
      26,    0,  240,    2, 0x08 /* Private */,
      27,    0,  241,    2, 0x08 /* Private */,
      28,    0,  242,    2, 0x08 /* Private */,
      29,    0,  243,    2, 0x08 /* Private */,
      30,    0,  244,    2, 0x08 /* Private */,
      31,    0,  245,    2, 0x08 /* Private */,
      32,    0,  246,    2, 0x08 /* Private */,
      33,    0,  247,    2, 0x08 /* Private */,
      34,    0,  248,    2, 0x08 /* Private */,
      35,    0,  249,    2, 0x08 /* Private */,
      36,    0,  250,    2, 0x08 /* Private */,
      37,    0,  251,    2, 0x08 /* Private */,
      38,    0,  252,    2, 0x08 /* Private */,
      39,    0,  253,    2, 0x08 /* Private */,
      40,    0,  254,    2, 0x08 /* Private */,
      41,    0,  255,    2, 0x08 /* Private */,
      42,    0,  256,    2, 0x08 /* Private */,
      43,    1,  257,    2, 0x08 /* Private */,
      45,    1,  260,    2, 0x08 /* Private */,
      46,    1,  263,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float,   11,   12,   13,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   44,
    QMetaType::Void, QMetaType::Int,   44,
    QMetaType::Void, QMetaType::Int,   44,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->serial_start(); break;
        case 1: _t->serial_close(); break;
        case 2: _t->Log((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->errorLog((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->test(); break;
        case 5: _t->show_position(); break;
        case 6: _t->show_settings(); break;
        case 7: _t->show_endswitch((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3]))); break;
        case 8: _t->show_serial((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->show_status(); break;
        case 10: _t->sendStopMoving(); break;
        case 11: _t->on_pushButtonSerialConnect_clicked(); break;
        case 12: _t->on_pushButtonMoveXPos_pressed(); break;
        case 13: _t->on_pushButtonMoveXNeg_pressed(); break;
        case 14: _t->on_pushButtonMoveYPos_pressed(); break;
        case 15: _t->on_pushButtonMoveYNeg_pressed(); break;
        case 16: _t->on_pushButtonMoveZPos_pressed(); break;
        case 17: _t->on_pushButtonMoveWNeg_pressed(); break;
        case 18: _t->on_pushButtonMoveZNeg_pressed(); break;
        case 19: _t->on_pushButtonMoveWPos_pressed(); break;
        case 20: _t->on_pushButtonCopyToMoveTo_pressed(); break;
        case 21: _t->on_pushButtonCopyToSetPosition_pressed(); break;
        case 22: _t->on_pushButtonGetPosition_pressed(); break;
        case 23: _t->on_pushButtonMoveTo_pressed(); break;
        case 24: _t->on_pushButtonSetPosition_pressed(); break;
        case 25: _t->on_pushButtonSetSettings_pressed(); break;
        case 26: _t->on_pushButton_browseGCode_pressed(); break;
        case 27: _t->on_pushButton_startGCode_pressed(); break;
        case 28: _t->on_pushButton_Zcalib_pressed(); break;
        case 29: _t->on_pushButton_home_pressed(); break;
        case 30: _t->on_pushButton_sizecalib_pressed(); break;
        case 31: _t->on_pushButton_repeattest_pressed(); break;
        case 32: _t->on_pushButton_pauseGCode_pressed(); break;
        case 33: _t->on_pushButton_AboardGCode_pressed(); break;
        case 34: _t->on_pushButton_test_pressed(); break;
        case 35: _t->on_pushButton_rest_pressed(); break;
        case 36: _t->on_spinBoxSpeed_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 37: _t->on_spinBoxTemperatur_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 38: _t->on_spinBoxFilament_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MainWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::serial_start)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::serial_close)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 39)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 39;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 39)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 39;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::serial_start()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void MainWindow::serial_close()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
