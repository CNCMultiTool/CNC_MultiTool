/****************************************************************************
** Meta object code from reading C++ file 'cnc_basefunctions.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../CNC_MultiTool_GUI/cnc_basefunctions.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cnc_basefunctions.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_cnc_basefunctions_t {
    QByteArrayData data[26];
    char stringdata0[216];
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
QT_MOC_LITERAL(5, 34, 4), // "send"
QT_MOC_LITERAL(6, 39, 7), // "command"
QT_MOC_LITERAL(7, 47, 6), // "value1"
QT_MOC_LITERAL(8, 54, 6), // "value2"
QT_MOC_LITERAL(9, 61, 6), // "value3"
QT_MOC_LITERAL(10, 68, 6), // "value4"
QT_MOC_LITERAL(11, 75, 20), // "answer_repeatrequest"
QT_MOC_LITERAL(12, 96, 9), // "move_wait"
QT_MOC_LITERAL(13, 106, 1), // "X"
QT_MOC_LITERAL(14, 108, 1), // "Y"
QT_MOC_LITERAL(15, 110, 1), // "Z"
QT_MOC_LITERAL(16, 112, 1), // "W"
QT_MOC_LITERAL(17, 114, 9), // "send_move"
QT_MOC_LITERAL(18, 124, 13), // "send_settings"
QT_MOC_LITERAL(19, 138, 5), // "speed"
QT_MOC_LITERAL(20, 144, 10), // "temperatur"
QT_MOC_LITERAL(21, 155, 8), // "filament"
QT_MOC_LITERAL(22, 164, 9), // "send_stop"
QT_MOC_LITERAL(23, 174, 16), // "send_getPosition"
QT_MOC_LITERAL(24, 191, 16), // "send_setPosition"
QT_MOC_LITERAL(25, 208, 7) // "recived"

    },
    "cnc_basefunctions\0Log\0\0s\0errorLog\0"
    "send\0command\0value1\0value2\0value3\0"
    "value4\0answer_repeatrequest\0move_wait\0"
    "X\0Y\0Z\0W\0send_move\0send_settings\0speed\0"
    "temperatur\0filament\0send_stop\0"
    "send_getPosition\0send_setPosition\0"
    "recived"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_cnc_basefunctions[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x06 /* Public */,
       4,    1,   72,    2, 0x06 /* Public */,
       5,    5,   75,    2, 0x06 /* Public */,
      11,    0,   86,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    4,   87,    2, 0x0a /* Public */,
      17,    4,   96,    2, 0x0a /* Public */,
      18,    3,  105,    2, 0x0a /* Public */,
      22,    0,  112,    2, 0x0a /* Public */,
      23,    0,  113,    2, 0x0a /* Public */,
      24,    4,  114,    2, 0x0a /* Public */,
      25,    5,  123,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Char, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float,    6,    7,    8,    9,   10,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float,   13,   14,   15,   16,
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float,   13,   14,   15,   16,
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float,   19,   20,   21,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float,   13,   14,   15,   16,
    QMetaType::Void, QMetaType::Char, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float,    6,    7,    8,    9,   10,

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
        case 2: _t->send((*reinterpret_cast< char(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4])),(*reinterpret_cast< float(*)>(_a[5]))); break;
        case 3: _t->answer_repeatrequest(); break;
        case 4: _t->move_wait((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4]))); break;
        case 5: _t->send_move((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4]))); break;
        case 6: _t->send_settings((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3]))); break;
        case 7: _t->send_stop(); break;
        case 8: _t->send_getPosition(); break;
        case 9: _t->send_setPosition((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4]))); break;
        case 10: _t->recived((*reinterpret_cast< char(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4])),(*reinterpret_cast< float(*)>(_a[5]))); break;
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
            using _t = void (cnc_basefunctions::*)(char , float , float , float , float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cnc_basefunctions::send)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (cnc_basefunctions::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cnc_basefunctions::answer_repeatrequest)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject cnc_basefunctions::staticMetaObject = { {
    &QObject::staticMetaObject,
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
    return QObject::qt_metacast(_clname);
}

int cnc_basefunctions::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
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
void cnc_basefunctions::send(char _t1, float _t2, float _t3, float _t4, float _t5)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void cnc_basefunctions::answer_repeatrequest()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
