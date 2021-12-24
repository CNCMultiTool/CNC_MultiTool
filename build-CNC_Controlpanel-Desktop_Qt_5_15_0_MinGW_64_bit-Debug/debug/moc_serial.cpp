/****************************************************************************
** Meta object code from reading C++ file 'serial.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../CNC_Controlpanel/serial.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'serial.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Serial_t {
    QByteArrayData data[14];
    char stringdata0[111];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Serial_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Serial_t qt_meta_stringdata_Serial = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Serial"
QT_MOC_LITERAL(1, 7, 3), // "Log"
QT_MOC_LITERAL(2, 11, 0), // ""
QT_MOC_LITERAL(3, 12, 1), // "s"
QT_MOC_LITERAL(4, 14, 8), // "errorLog"
QT_MOC_LITERAL(5, 23, 11), // "show_serial"
QT_MOC_LITERAL(6, 35, 1), // "b"
QT_MOC_LITERAL(7, 37, 7), // "recLine"
QT_MOC_LITERAL(8, 45, 8), // "recBytes"
QT_MOC_LITERAL(9, 54, 4), // "data"
QT_MOC_LITERAL(10, 59, 16), // "serial_addToSend"
QT_MOC_LITERAL(11, 76, 3), // "mes"
QT_MOC_LITERAL(12, 80, 18), // "serial_sendTimeout"
QT_MOC_LITERAL(13, 99, 11) // "serial_read"

    },
    "Serial\0Log\0\0s\0errorLog\0show_serial\0b\0"
    "recLine\0recBytes\0data\0serial_addToSend\0"
    "mes\0serial_sendTimeout\0serial_read"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Serial[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       4,    1,   57,    2, 0x06 /* Public */,
       5,    1,   60,    2, 0x06 /* Public */,
       7,    1,   63,    2, 0x06 /* Public */,
       8,    1,   66,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    1,   69,    2, 0x0a /* Public */,
      12,    0,   72,    2, 0x0a /* Public */,
      13,    0,   73,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QByteArray,    9,

 // slots: parameters
    QMetaType::Void, QMetaType::QByteArray,   11,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Serial::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Serial *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Log((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->errorLog((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->show_serial((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->recLine((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->recBytes((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 5: _t->serial_addToSend((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 6: _t->serial_sendTimeout(); break;
        case 7: _t->serial_read(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Serial::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Serial::Log)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Serial::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Serial::errorLog)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Serial::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Serial::show_serial)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Serial::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Serial::recLine)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Serial::*)(const QByteArray & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Serial::recBytes)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Serial::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Serial.data,
    qt_meta_data_Serial,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Serial::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Serial::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Serial.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Serial::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void Serial::Log(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Serial::errorLog(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Serial::show_serial(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Serial::recLine(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Serial::recBytes(const QByteArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
