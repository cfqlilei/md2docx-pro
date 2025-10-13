/****************************************************************************
** Meta object code from reading C++ file 'embeddedserver.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../src/embeddedserver.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'embeddedserver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_EmbeddedServer_t {
    QByteArrayData data[17];
    char stringdata0[242];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_EmbeddedServer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_EmbeddedServer_t qt_meta_stringdata_EmbeddedServer = {
    {
QT_MOC_LITERAL(0, 0, 14), // "EmbeddedServer"
QT_MOC_LITERAL(1, 15, 13), // "serverStarted"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 13), // "serverStopped"
QT_MOC_LITERAL(4, 44, 11), // "serverError"
QT_MOC_LITERAL(5, 56, 5), // "error"
QT_MOC_LITERAL(6, 62, 17), // "healthCheckResult"
QT_MOC_LITERAL(7, 80, 9), // "isHealthy"
QT_MOC_LITERAL(8, 90, 15), // "onServerStarted"
QT_MOC_LITERAL(9, 106, 16), // "onServerFinished"
QT_MOC_LITERAL(10, 123, 8), // "exitCode"
QT_MOC_LITERAL(11, 132, 20), // "QProcess::ExitStatus"
QT_MOC_LITERAL(12, 153, 10), // "exitStatus"
QT_MOC_LITERAL(13, 164, 13), // "onServerError"
QT_MOC_LITERAL(14, 178, 22), // "QProcess::ProcessError"
QT_MOC_LITERAL(15, 201, 21), // "onHealthCheckFinished"
QT_MOC_LITERAL(16, 223, 18) // "performHealthCheck"

    },
    "EmbeddedServer\0serverStarted\0\0"
    "serverStopped\0serverError\0error\0"
    "healthCheckResult\0isHealthy\0onServerStarted\0"
    "onServerFinished\0exitCode\0"
    "QProcess::ExitStatus\0exitStatus\0"
    "onServerError\0QProcess::ProcessError\0"
    "onHealthCheckFinished\0performHealthCheck"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_EmbeddedServer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x06 /* Public */,
       3,    0,   60,    2, 0x06 /* Public */,
       4,    1,   61,    2, 0x06 /* Public */,
       6,    1,   64,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    0,   67,    2, 0x08 /* Private */,
       9,    2,   68,    2, 0x08 /* Private */,
      13,    1,   73,    2, 0x08 /* Private */,
      15,    0,   76,    2, 0x08 /* Private */,
      16,    0,   77,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::Bool,    7,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 11,   10,   12,
    QMetaType::Void, 0x80000000 | 14,    5,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void EmbeddedServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<EmbeddedServer *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->serverStarted(); break;
        case 1: _t->serverStopped(); break;
        case 2: _t->serverError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->healthCheckResult((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->onServerStarted(); break;
        case 5: _t->onServerFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        case 6: _t->onServerError((*reinterpret_cast< QProcess::ProcessError(*)>(_a[1]))); break;
        case 7: _t->onHealthCheckFinished(); break;
        case 8: _t->performHealthCheck(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (EmbeddedServer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EmbeddedServer::serverStarted)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (EmbeddedServer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EmbeddedServer::serverStopped)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (EmbeddedServer::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EmbeddedServer::serverError)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (EmbeddedServer::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EmbeddedServer::healthCheckResult)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject EmbeddedServer::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_EmbeddedServer.data,
    qt_meta_data_EmbeddedServer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *EmbeddedServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EmbeddedServer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_EmbeddedServer.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int EmbeddedServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void EmbeddedServer::serverStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void EmbeddedServer::serverStopped()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void EmbeddedServer::serverError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void EmbeddedServer::healthCheckResult(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
