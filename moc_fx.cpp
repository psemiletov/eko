/****************************************************************************
** Meta object code from reading C++ file 'fx.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "fx.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fx.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AFx_t {
    QByteArrayData data[1];
    char stringdata[5];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_AFx_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_AFx_t qt_meta_stringdata_AFx = {
    {
QT_MOC_LITERAL(0, 0, 3)
    },
    "AFx\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AFx[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void AFx::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject AFx::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_AFx.data,
      qt_meta_data_AFx,  qt_static_metacall, 0, 0}
};


const QMetaObject *AFx::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AFx::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AFx.stringdata))
        return static_cast<void*>(const_cast< AFx*>(this));
    return QObject::qt_metacast(_clname);
}

int AFx::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_CFxSimpleEQ_t {
    QByteArrayData data[6];
    char stringdata[85];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CFxSimpleEQ_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CFxSimpleEQ_t qt_meta_stringdata_CFxSimpleEQ = {
    {
QT_MOC_LITERAL(0, 0, 11),
QT_MOC_LITERAL(1, 12, 21),
QT_MOC_LITERAL(2, 34, 0),
QT_MOC_LITERAL(3, 35, 5),
QT_MOC_LITERAL(4, 41, 21),
QT_MOC_LITERAL(5, 63, 20)
    },
    "CFxSimpleEQ\0dial_low_valueChanged\0\0"
    "value\0dial_mid_valueChanged\0"
    "dial_hi_valueChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CFxSimpleEQ[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x0a,
       4,    1,   32,    2, 0x0a,
       5,    1,   35,    2, 0x0a,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,

       0        // eod
};

void CFxSimpleEQ::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CFxSimpleEQ *_t = static_cast<CFxSimpleEQ *>(_o);
        switch (_id) {
        case 0: _t->dial_low_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->dial_mid_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->dial_hi_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject CFxSimpleEQ::staticMetaObject = {
    { &AFx::staticMetaObject, qt_meta_stringdata_CFxSimpleEQ.data,
      qt_meta_data_CFxSimpleEQ,  qt_static_metacall, 0, 0}
};


const QMetaObject *CFxSimpleEQ::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CFxSimpleEQ::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CFxSimpleEQ.stringdata))
        return static_cast<void*>(const_cast< CFxSimpleEQ*>(this));
    return AFx::qt_metacast(_clname);
}

int CFxSimpleEQ::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AFx::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
struct qt_meta_stringdata_CFxSimpleAmp_t {
    QByteArrayData data[4];
    char stringdata[44];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CFxSimpleAmp_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CFxSimpleAmp_t qt_meta_stringdata_CFxSimpleAmp = {
    {
QT_MOC_LITERAL(0, 0, 12),
QT_MOC_LITERAL(1, 13, 22),
QT_MOC_LITERAL(2, 36, 0),
QT_MOC_LITERAL(3, 37, 5)
    },
    "CFxSimpleAmp\0dial_gain_valueChanged\0"
    "\0value\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CFxSimpleAmp[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x0a,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,

       0        // eod
};

void CFxSimpleAmp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CFxSimpleAmp *_t = static_cast<CFxSimpleAmp *>(_o);
        switch (_id) {
        case 0: _t->dial_gain_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject CFxSimpleAmp::staticMetaObject = {
    { &AFx::staticMetaObject, qt_meta_stringdata_CFxSimpleAmp.data,
      qt_meta_data_CFxSimpleAmp,  qt_static_metacall, 0, 0}
};


const QMetaObject *CFxSimpleAmp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CFxSimpleAmp::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CFxSimpleAmp.stringdata))
        return static_cast<void*>(const_cast< CFxSimpleAmp*>(this));
    return AFx::qt_metacast(_clname);
}

int CFxSimpleAmp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AFx::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_CFxSimpleOverdrive_t {
    QByteArrayData data[4];
    char stringdata[50];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CFxSimpleOverdrive_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CFxSimpleOverdrive_t qt_meta_stringdata_CFxSimpleOverdrive = {
    {
QT_MOC_LITERAL(0, 0, 18),
QT_MOC_LITERAL(1, 19, 22),
QT_MOC_LITERAL(2, 42, 0),
QT_MOC_LITERAL(3, 43, 5)
    },
    "CFxSimpleOverdrive\0dial_gain_valueChanged\0"
    "\0value\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CFxSimpleOverdrive[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x0a,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,

       0        // eod
};

void CFxSimpleOverdrive::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CFxSimpleOverdrive *_t = static_cast<CFxSimpleOverdrive *>(_o);
        switch (_id) {
        case 0: _t->dial_gain_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject CFxSimpleOverdrive::staticMetaObject = {
    { &AFx::staticMetaObject, qt_meta_stringdata_CFxSimpleOverdrive.data,
      qt_meta_data_CFxSimpleOverdrive,  qt_static_metacall, 0, 0}
};


const QMetaObject *CFxSimpleOverdrive::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CFxSimpleOverdrive::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CFxSimpleOverdrive.stringdata))
        return static_cast<void*>(const_cast< CFxSimpleOverdrive*>(this));
    return AFx::qt_metacast(_clname);
}

int CFxSimpleOverdrive::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AFx::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_CFxPitchShift_t {
    QByteArrayData data[4];
    char stringdata[40];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CFxPitchShift_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CFxPitchShift_t qt_meta_stringdata_CFxPitchShift = {
    {
QT_MOC_LITERAL(0, 0, 13),
QT_MOC_LITERAL(1, 14, 17),
QT_MOC_LITERAL(2, 32, 0),
QT_MOC_LITERAL(3, 33, 5)
    },
    "CFxPitchShift\0spb_ratio_changed\0\0value\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CFxPitchShift[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x0a,

 // slots: parameters
    QMetaType::Void, QMetaType::Double,    3,

       0        // eod
};

void CFxPitchShift::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CFxPitchShift *_t = static_cast<CFxPitchShift *>(_o);
        switch (_id) {
        case 0: _t->spb_ratio_changed((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject CFxPitchShift::staticMetaObject = {
    { &AFx::staticMetaObject, qt_meta_stringdata_CFxPitchShift.data,
      qt_meta_data_CFxPitchShift,  qt_static_metacall, 0, 0}
};


const QMetaObject *CFxPitchShift::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CFxPitchShift::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CFxPitchShift.stringdata))
        return static_cast<void*>(const_cast< CFxPitchShift*>(this));
    return AFx::qt_metacast(_clname);
}

int CFxPitchShift::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AFx::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
