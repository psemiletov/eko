/****************************************************************************
** Meta object code from reading C++ file 'document.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "document.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'document.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CTransportControl_t {
    QByteArrayData data[4];
    char stringdata[36];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CTransportControl_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CTransportControl_t qt_meta_stringdata_CTransportControl = {
    {
QT_MOC_LITERAL(0, 0, 17),
QT_MOC_LITERAL(1, 18, 10),
QT_MOC_LITERAL(2, 29, 0),
QT_MOC_LITERAL(3, 30, 4)
    },
    "CTransportControl\0play_pause\0\0stop\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CTransportControl[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x06,
       3,    0,   25,    2, 0x06,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CTransportControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CTransportControl *_t = static_cast<CTransportControl *>(_o);
        switch (_id) {
        case 0: _t->play_pause(); break;
        case 1: _t->stop(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CTransportControl::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CTransportControl::play_pause)) {
                *result = 0;
            }
        }
        {
            typedef void (CTransportControl::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CTransportControl::stop)) {
                *result = 1;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject CTransportControl::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CTransportControl.data,
      qt_meta_data_CTransportControl,  qt_static_metacall, 0, 0}
};


const QMetaObject *CTransportControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CTransportControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CTransportControl.stringdata))
        return static_cast<void*>(const_cast< CTransportControl*>(this));
    return QObject::qt_metacast(_clname);
}

int CTransportControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void CTransportControl::play_pause()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void CTransportControl::stop()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
struct qt_meta_stringdata_CUndoElement_t {
    QByteArrayData data[1];
    char stringdata[14];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CUndoElement_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CUndoElement_t qt_meta_stringdata_CUndoElement = {
    {
QT_MOC_LITERAL(0, 0, 12)
    },
    "CUndoElement\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CUndoElement[] = {

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

void CUndoElement::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject CUndoElement::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CUndoElement.data,
      qt_meta_data_CUndoElement,  qt_static_metacall, 0, 0}
};


const QMetaObject *CUndoElement::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CUndoElement::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CUndoElement.stringdata))
        return static_cast<void*>(const_cast< CUndoElement*>(this));
    return QObject::qt_metacast(_clname);
}

int CUndoElement::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_CTimeRuler_t {
    QByteArrayData data[1];
    char stringdata[12];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CTimeRuler_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CTimeRuler_t qt_meta_stringdata_CTimeRuler = {
    {
QT_MOC_LITERAL(0, 0, 10)
    },
    "CTimeRuler\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CTimeRuler[] = {

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

void CTimeRuler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject CTimeRuler::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CTimeRuler.data,
      qt_meta_data_CTimeRuler,  qt_static_metacall, 0, 0}
};


const QMetaObject *CTimeRuler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CTimeRuler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CTimeRuler.stringdata))
        return static_cast<void*>(const_cast< CTimeRuler*>(this));
    return QWidget::qt_metacast(_clname);
}

int CTimeRuler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_CWaveform_t {
    QByteArrayData data[3];
    char stringdata[26];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CWaveform_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CWaveform_t qt_meta_stringdata_CWaveform = {
    {
QT_MOC_LITERAL(0, 0, 9),
QT_MOC_LITERAL(1, 10, 13),
QT_MOC_LITERAL(2, 24, 0)
    },
    "CWaveform\0timer_timeout\0\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CWaveform[] = {

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
       1,    0,   19,    2, 0x0a,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void CWaveform::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CWaveform *_t = static_cast<CWaveform *>(_o);
        switch (_id) {
        case 0: _t->timer_timeout(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject CWaveform::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CWaveform.data,
      qt_meta_data_CWaveform,  qt_static_metacall, 0, 0}
};


const QMetaObject *CWaveform::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CWaveform::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CWaveform.stringdata))
        return static_cast<void*>(const_cast< CWaveform*>(this));
    return QWidget::qt_metacast(_clname);
}

int CWaveform::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_CWaveEdit_t {
    QByteArrayData data[4];
    char stringdata[46];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CWaveEdit_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CWaveEdit_t qt_meta_stringdata_CWaveEdit = {
    {
QT_MOC_LITERAL(0, 0, 9),
QT_MOC_LITERAL(1, 10, 27),
QT_MOC_LITERAL(2, 38, 0),
QT_MOC_LITERAL(3, 39, 5)
    },
    "CWaveEdit\0scb_horizontal_valueChanged\0"
    "\0value\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CWaveEdit[] = {

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

void CWaveEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CWaveEdit *_t = static_cast<CWaveEdit *>(_o);
        switch (_id) {
        case 0: _t->scb_horizontal_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject CWaveEdit::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CWaveEdit.data,
      qt_meta_data_CWaveEdit,  qt_static_metacall, 0, 0}
};


const QMetaObject *CWaveEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CWaveEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CWaveEdit.stringdata))
        return static_cast<void*>(const_cast< CWaveEdit*>(this));
    return QWidget::qt_metacast(_clname);
}

int CWaveEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_CDocument_t {
    QByteArrayData data[1];
    char stringdata[11];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CDocument_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CDocument_t qt_meta_stringdata_CDocument = {
    {
QT_MOC_LITERAL(0, 0, 9)
    },
    "CDocument\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CDocument[] = {

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

void CDocument::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject CDocument::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CDocument.data,
      qt_meta_data_CDocument,  qt_static_metacall, 0, 0}
};


const QMetaObject *CDocument::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CDocument::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CDocument.stringdata))
        return static_cast<void*>(const_cast< CDocument*>(this));
    return QObject::qt_metacast(_clname);
}

int CDocument::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_CDocumentHolder_t {
    QByteArrayData data[3];
    char stringdata[30];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CDocumentHolder_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CDocumentHolder_t qt_meta_stringdata_CDocumentHolder = {
    {
QT_MOC_LITERAL(0, 0, 15),
QT_MOC_LITERAL(1, 16, 11),
QT_MOC_LITERAL(2, 28, 0)
    },
    "CDocumentHolder\0open_recent\0\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CDocumentHolder[] = {

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
       1,    0,   19,    2, 0x0a,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void CDocumentHolder::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CDocumentHolder *_t = static_cast<CDocumentHolder *>(_o);
        switch (_id) {
        case 0: _t->open_recent(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject CDocumentHolder::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CDocumentHolder.data,
      qt_meta_data_CDocumentHolder,  qt_static_metacall, 0, 0}
};


const QMetaObject *CDocumentHolder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CDocumentHolder::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CDocumentHolder.stringdata))
        return static_cast<void*>(const_cast< CDocumentHolder*>(this));
    return QObject::qt_metacast(_clname);
}

int CDocumentHolder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_CDSP_t {
    QByteArrayData data[1];
    char stringdata[6];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CDSP_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CDSP_t qt_meta_stringdata_CDSP = {
    {
QT_MOC_LITERAL(0, 0, 4)
    },
    "CDSP\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CDSP[] = {

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

void CDSP::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject CDSP::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CDSP.data,
      qt_meta_data_CDSP,  qt_static_metacall, 0, 0}
};


const QMetaObject *CDSP::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CDSP::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CDSP.stringdata))
        return static_cast<void*>(const_cast< CDSP*>(this));
    return QObject::qt_metacast(_clname);
}

int CDSP::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_CVLevelMeter_t {
    QByteArrayData data[1];
    char stringdata[14];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CVLevelMeter_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CVLevelMeter_t qt_meta_stringdata_CVLevelMeter = {
    {
QT_MOC_LITERAL(0, 0, 12)
    },
    "CVLevelMeter\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CVLevelMeter[] = {

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

void CVLevelMeter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject CVLevelMeter::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CVLevelMeter.data,
      qt_meta_data_CVLevelMeter,  qt_static_metacall, 0, 0}
};


const QMetaObject *CVLevelMeter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CVLevelMeter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CVLevelMeter.stringdata))
        return static_cast<void*>(const_cast< CVLevelMeter*>(this));
    return QWidget::qt_metacast(_clname);
}

int CVLevelMeter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_CFxRackWindow_t {
    QByteArrayData data[8];
    char stringdata[103];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CFxRackWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CFxRackWindow_t qt_meta_stringdata_CFxRackWindow = {
    {
QT_MOC_LITERAL(0, 0, 13),
QT_MOC_LITERAL(1, 14, 22),
QT_MOC_LITERAL(2, 37, 0),
QT_MOC_LITERAL(3, 38, 8),
QT_MOC_LITERAL(4, 47, 12),
QT_MOC_LITERAL(5, 60, 5),
QT_MOC_LITERAL(6, 66, 12),
QT_MOC_LITERAL(7, 79, 22)
    },
    "CFxRackWindow\0tm_level_meter_timeout\0"
    "\0apply_fx\0cb_l_changed\0value\0cb_r_changed\0"
    "dial_gain_valueChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CFxRackWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x0a,
       3,    0,   40,    2, 0x0a,
       4,    1,   41,    2, 0x0a,
       6,    1,   44,    2, 0x0a,
       7,    1,   47,    2, 0x0a,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    5,

       0        // eod
};

void CFxRackWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CFxRackWindow *_t = static_cast<CFxRackWindow *>(_o);
        switch (_id) {
        case 0: _t->tm_level_meter_timeout(); break;
        case 1: _t->apply_fx(); break;
        case 2: _t->cb_l_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->cb_r_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->dial_gain_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject CFxRackWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CFxRackWindow.data,
      qt_meta_data_CFxRackWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *CFxRackWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CFxRackWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CFxRackWindow.stringdata))
        return static_cast<void*>(const_cast< CFxRackWindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int CFxRackWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_CFxRack_t {
    QByteArrayData data[13];
    char stringdata[130];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CFxRack_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CFxRack_t qt_meta_stringdata_CFxRack = {
    {
QT_MOC_LITERAL(0, 0, 7),
QT_MOC_LITERAL(1, 8, 12),
QT_MOC_LITERAL(2, 21, 0),
QT_MOC_LITERAL(3, 22, 4),
QT_MOC_LITERAL(4, 27, 12),
QT_MOC_LITERAL(5, 40, 5),
QT_MOC_LITERAL(6, 46, 6),
QT_MOC_LITERAL(7, 53, 6),
QT_MOC_LITERAL(8, 60, 13),
QT_MOC_LITERAL(9, 74, 15),
QT_MOC_LITERAL(10, 90, 18),
QT_MOC_LITERAL(11, 109, 7),
QT_MOC_LITERAL(12, 117, 11)
    },
    "CFxRack\0fx_activated\0\0path\0tv_activated\0"
    "index\0add_fx\0del_fx\0bt_up_clicked\0"
    "bt_down_clicked\0bypass_dataChanged\0"
    "topLeft\0bottomRight\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CFxRack[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       4,    1,   52,    2, 0x0a,
       6,    0,   55,    2, 0x0a,
       7,    0,   56,    2, 0x0a,
       8,    0,   57,    2, 0x0a,
       9,    0,   58,    2, 0x0a,
      10,    2,   59,    2, 0x0a,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QModelIndex,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::QModelIndex,   11,   12,

       0        // eod
};

void CFxRack::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CFxRack *_t = static_cast<CFxRack *>(_o);
        switch (_id) {
        case 0: _t->fx_activated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->tv_activated((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 2: _t->add_fx(); break;
        case 3: _t->del_fx(); break;
        case 4: _t->bt_up_clicked(); break;
        case 5: _t->bt_down_clicked(); break;
        case 6: _t->bypass_dataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CFxRack::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CFxRack::fx_activated)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject CFxRack::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CFxRack.data,
      qt_meta_data_CFxRack,  qt_static_metacall, 0, 0}
};


const QMetaObject *CFxRack::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CFxRack::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CFxRack.stringdata))
        return static_cast<void*>(const_cast< CFxRack*>(this));
    return QObject::qt_metacast(_clname);
}

int CFxRack::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void CFxRack::fx_activated(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_CFxTreeView_t {
    QByteArrayData data[1];
    char stringdata[13];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CFxTreeView_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CFxTreeView_t qt_meta_stringdata_CFxTreeView = {
    {
QT_MOC_LITERAL(0, 0, 11)
    },
    "CFxTreeView\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CFxTreeView[] = {

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

void CFxTreeView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject CFxTreeView::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_CFxTreeView.data,
      qt_meta_data_CFxTreeView,  qt_static_metacall, 0, 0}
};


const QMetaObject *CFxTreeView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CFxTreeView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CFxTreeView.stringdata))
        return static_cast<void*>(const_cast< CFxTreeView*>(this));
    return QTreeView::qt_metacast(_clname);
}

int CFxTreeView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
