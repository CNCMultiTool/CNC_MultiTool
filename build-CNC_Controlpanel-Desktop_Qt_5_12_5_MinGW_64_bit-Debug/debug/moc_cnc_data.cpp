/****************************************************************************
** Meta object code from reading C++ file 'cnc_data.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../CNC_Controlpanel/cnc_data.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cnc_data.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_cnc_data_t {
    QByteArrayData data[11];
    char stringdata0[74];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_cnc_data_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_cnc_data_t qt_meta_stringdata_cnc_data = {
    {
QT_MOC_LITERAL(0, 0, 8), // "cnc_data"
QT_MOC_LITERAL(1, 9, 3), // "Log"
QT_MOC_LITERAL(2, 13, 0), // ""
QT_MOC_LITERAL(3, 14, 1), // "s"
QT_MOC_LITERAL(4, 16, 8), // "errorLog"
QT_MOC_LITERAL(5, 25, 13), // "show_position"
QT_MOC_LITERAL(6, 39, 13), // "show_settings"
QT_MOC_LITERAL(7, 53, 14), // "show_endswitch"
QT_MOC_LITERAL(8, 68, 1), // "X"
QT_MOC_LITERAL(9, 70, 1), // "Y"
QT_MOC_LITERAL(10, 72, 1) // "Z"

    },
    "cnc_data\0Log\0\0s\0errorLog\0show_position\0"
    "show_settings\0show_endswitch\0X\0Y\0Z"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_cnc_data[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    1,   42,    2, 0x06 /* Public */,
       5,    0,   45,    2, 0x06 /* Public */,
       6,    0,   46,    2, 0x06 /* Public */,
       7,    3,   47,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float,    8,    9,   10,

       0        // eod
};

void cnc_data::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<cnc_data *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Log((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->errorLog((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->show_position(); break;
        case 3: _t->show_settings(); break;
        case 4: _t->show_endswitch((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (cnc_data::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cnc_data::Log)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (cnc_data::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cnc_data::errorLog)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (cnc_data::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cnc_data::show_position)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (cnc_data::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cnc_data::show_settings)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (cnc_data::*)(float , float , float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&cnc_data::show_endswitch)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject cnc_data::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_cnc_data.data,
    qt_meta_data_cnc_data,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *cnc_data::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *cnc_data::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_cnc_data.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int cnc_data::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void cnc_data::Log(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void cnc_data::errorLog(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void cnc_data::show_position()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void cnc_data::show_settings()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void cnc_data::show_endswitch(float _t1, float _t2, float _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
