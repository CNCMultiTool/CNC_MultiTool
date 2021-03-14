/****************************************************************************
** Meta object code from reading C++ file 'cnc_basefunctions.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../CNC_Controlpanel/cnc_basefunctions.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cnc_basefunctions.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_cnc_basefunctions_t {
    QByteArrayData data[17];
    char stringdata0[185];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_cnc_basefunctions_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_cnc_basefunctions_t qt_meta_stringdata_cnc_basefunctions = {
    {
QT_MOC_LITERAL(0, 0, 17), // "cnc_basefunctions"
QT_MOC_LITERAL(1, 18, 3), // "Log"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 1), // "s"
QT_MOC_LITERAL(4, 25, 8), // "errorLog"
QT_MOC_LITERAL(5, 34, 11), // "serial_send"
QT_MOC_LITERAL(6, 46, 3), // "mes"
QT_MOC_LITERAL(7, 50, 13), // "show_position"
QT_MOC_LITERAL(8, 64, 9), // "show_temp"
QT_MOC_LITERAL(9, 74, 14), // "show_endswitch"
QT_MOC_LITERAL(10, 89, 19), // "show_acc_speed_fila"
QT_MOC_LITERAL(11, 109, 10), // "show_speed"
QT_MOC_LITERAL(12, 120, 13), // "show_act_temp"
QT_MOC_LITERAL(13, 134, 10), // "show_useES"
QT_MOC_LITERAL(14, 145, 10), // "show_state"
QT_MOC_LITERAL(15, 156, 16), // "show_waitForHeat"
QT_MOC_LITERAL(16, 173, 11) // "processLine"

    },
    "cnc_basefunctions\0Log\0\0s\0errorLog\0"
    "serial_send\0mes\0show_position\0show_temp\0"
    "show_endswitch\0show_acc_speed_fila\0"
    "show_speed\0show_act_temp\0show_useES\0"
    "show_state\0show_waitForHeat\0processLine"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_cnc_basefunctions[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      12,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x06 /* Public */,
       4,    1,   82,    2, 0x06 /* Public */,
       5,    1,   85,    2, 0x06 /* Public */,
       7,    0,   88,    2, 0x06 /* Public */,
       8,    0,   89,    2, 0x06 /* Public */,
       9,    0,   90,    2, 0x06 /* Public */,
      10,    0,   91,    2, 0x06 /* Public */,
      11,    0,   92,    2, 0x06 /* Public */,
      12,    0,   93,    2, 0x06 /* Public */,
      13,    1,   94,    2, 0x06 /* Public */,
      14,    1,   97,    2, 0x06 /* Public */,
      15,    1,  100,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      16,    1,  103,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float,    2,
    QMetaType::Void, QMetaType::Float,    2,
    QMetaType::Void, QMetaType::Float,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void cnc_basefunctions::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<cnc_basefunctions *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Log((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->errorLog((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->serial_send((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->show_position(); break;
        case 4: _t->show_temp(); break;
        case 5: _t->show_endswitch(); break;
        case 6: _t->show_acc_speed_fila(); break;
        case 7: _t->show_speed(); break;
        case 8: _t->show_act_temp(); break;
        case 9: _t->show_useES((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 10: _t->show_state((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 11: _t->show_waitForHeat((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 12: _t->processLine((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (cnc_basefunctions::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cnc_basefunctions::Log)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (cnc_basefunctions::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cnc_basefunctions::errorLog)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (cnc_basefunctions::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cnc_basefunctions::serial_send)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (cnc_basefunctions::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cnc_basefunctions::show_position)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (cnc_basefunctions::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cnc_basefunctions::show_temp)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (cnc_basefunctions::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cnc_basefunctions::show_endswitch)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (cnc_basefunctions::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cnc_basefunctions::show_acc_speed_fila)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (cnc_basefunctions::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cnc_basefunctions::show_speed)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (cnc_basefunctions::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cnc_basefunctions::show_act_temp)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (cnc_basefunctions::*)(float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cnc_basefunctions::show_useES)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (cnc_basefunctions::*)(float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cnc_basefunctions::show_state)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (cnc_basefunctions::*)(float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cnc_basefunctions::show_waitForHeat)) {
                *result = 11;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject cnc_basefunctions::staticMetaObject = { {
    &QThread::staticMetaObject,
    qt_meta_stringdata_cnc_basefunctions.data,
    qt_meta_data_cnc_basefunctions,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *cnc_basefunctions::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *cnc_basefunctions::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_cnc_basefunctions.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int cnc_basefunctions::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void cnc_basefunctions::Log(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void cnc_basefunctions::errorLog(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void cnc_basefunctions::serial_send(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void cnc_basefunctions::show_position()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void cnc_basefunctions::show_temp()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void cnc_basefunctions::show_endswitch()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void cnc_basefunctions::show_acc_speed_fila()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void cnc_basefunctions::show_speed()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void cnc_basefunctions::show_act_temp()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void cnc_basefunctions::show_useES(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void cnc_basefunctions::show_state(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void cnc_basefunctions::show_waitForHeat(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
