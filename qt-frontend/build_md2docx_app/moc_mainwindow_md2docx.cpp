/****************************************************************************
** Meta object code from reading C++ file 'mainwindow_md2docx.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../src/mainwindow_md2docx.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow_md2docx.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindowMd2Docx_t {
    QByteArrayData data[10];
    char stringdata0[147];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindowMd2Docx_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindowMd2Docx_t qt_meta_stringdata_MainWindowMd2Docx = {
    {
QT_MOC_LITERAL(0, 0, 17), // "MainWindowMd2Docx"
QT_MOC_LITERAL(1, 18, 22), // "checkBackendConnection"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 21), // "onHealthCheckFinished"
QT_MOC_LITERAL(4, 64, 7), // "success"
QT_MOC_LITERAL(5, 72, 19), // "onConversionStarted"
QT_MOC_LITERAL(6, 92, 20), // "onConversionFinished"
QT_MOC_LITERAL(7, 113, 7), // "message"
QT_MOC_LITERAL(8, 121, 15), // "onConfigChanged"
QT_MOC_LITERAL(9, 137, 9) // "showAbout"

    },
    "MainWindowMd2Docx\0checkBackendConnection\0"
    "\0onHealthCheckFinished\0success\0"
    "onConversionStarted\0onConversionFinished\0"
    "message\0onConfigChanged\0showAbout"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindowMd2Docx[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x08 /* Private */,
       3,    1,   45,    2, 0x08 /* Private */,
       5,    0,   48,    2, 0x08 /* Private */,
       6,    2,   49,    2, 0x08 /* Private */,
       8,    0,   54,    2, 0x08 /* Private */,
       9,    0,   55,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    4,    7,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindowMd2Docx::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindowMd2Docx *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->checkBackendConnection(); break;
        case 1: _t->onHealthCheckFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->onConversionStarted(); break;
        case 3: _t->onConversionFinished((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 4: _t->onConfigChanged(); break;
        case 5: _t->showAbout(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindowMd2Docx::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindowMd2Docx.data,
    qt_meta_data_MainWindowMd2Docx,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindowMd2Docx::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindowMd2Docx::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindowMd2Docx.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindowMd2Docx::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
