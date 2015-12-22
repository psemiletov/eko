/****************************************************************************
** Meta object code from reading C++ file 'fman.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "fman.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fman.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CFMan_t {
    QByteArrayData data[18];
    char stringdata[211];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CFMan_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CFMan_t qt_meta_stringdata_CFMan = {
    {
QT_MOC_LITERAL(0, 0, 5),
QT_MOC_LITERAL(1, 6, 14),
QT_MOC_LITERAL(2, 21, 0),
QT_MOC_LITERAL(3, 22, 4),
QT_MOC_LITERAL(4, 27, 11),
QT_MOC_LITERAL(5, 39, 20),
QT_MOC_LITERAL(6, 60, 9),
QT_MOC_LITERAL(7, 70, 12),
QT_MOC_LITERAL(8, 83, 5),
QT_MOC_LITERAL(9, 89, 7),
QT_MOC_LITERAL(10, 97, 6),
QT_MOC_LITERAL(11, 104, 22),
QT_MOC_LITERAL(12, 127, 7),
QT_MOC_LITERAL(13, 135, 8),
QT_MOC_LITERAL(14, 144, 32),
QT_MOC_LITERAL(15, 177, 12),
QT_MOC_LITERAL(16, 190, 13),
QT_MOC_LITERAL(17, 204, 5)
    },
    "CFMan\0file_activated\0\0path\0dir_changed\0"
    "current_file_changed\0just_name\0"
    "tv_activated\0index\0refresh\0dir_up\0"
    "cb_fman_currentChanged\0current\0previous\0"
    "header_view_sortIndicatorChanged\0"
    "logicalIndex\0Qt::SortOrder\0order\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CFMan[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06,
       4,    1,   57,    2, 0x06,
       5,    2,   60,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       7,    1,   65,    2, 0x0a,
       9,    0,   68,    2, 0x0a,
      10,    0,   69,    2, 0x0a,
      11,    2,   70,    2, 0x0a,
      14,    2,   75,    2, 0x0a,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    3,    6,

 // slots: parameters
    QMetaType::Void, QMetaType::QModelIndex,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::QModelIndex,   12,   13,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 16,   15,   17,

       0        // eod
};

void CFMan::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CFMan *_t = static_cast<CFMan *>(_o);
        switch (_id) {
        case 0: _t->file_activated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->dir_changed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->current_file_changed((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->tv_activated((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 4: _t->refresh(); break;
        case 5: _t->dir_up(); break;
        case 6: _t->cb_fman_currentChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 7: _t->header_view_sortIndicatorChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< Qt::SortOrder(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CFMan::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CFMan::file_activated)) {
                *result = 0;
            }
        }
        {
            typedef void (CFMan::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CFMan::dir_changed)) {
                *result = 1;
            }
        }
        {
            typedef void (CFMan::*_t)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CFMan::current_file_changed)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject CFMan::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_CFMan.data,
      qt_meta_data_CFMan,  qt_static_metacall, 0, 0}
};


const QMetaObject *CFMan::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CFMan::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CFMan.stringdata))
        return static_cast<void*>(const_cast< CFMan*>(this));
    return QTreeView::qt_metacast(_clname);
}

int CFMan::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
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
void CFMan::file_activated(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CFMan::dir_changed(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CFMan::current_file_changed(const QString & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
