/****************************************************************************
** Meta object code from reading C++ file 'httpapi.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../src/httpapi.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'httpapi.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_HttpApi_t {
    QByteArrayData data[25];
    char stringdata0[400];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HttpApi_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HttpApi_t qt_meta_stringdata_HttpApi = {
    {
QT_MOC_LITERAL(0, 0, 7), // "HttpApi"
QT_MOC_LITERAL(1, 8, 19), // "healthCheckFinished"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 8), // "isOnline"
QT_MOC_LITERAL(4, 38, 14), // "configReceived"
QT_MOC_LITERAL(5, 53, 10), // "ConfigData"
QT_MOC_LITERAL(6, 64, 6), // "config"
QT_MOC_LITERAL(7, 71, 13), // "configUpdated"
QT_MOC_LITERAL(8, 85, 7), // "success"
QT_MOC_LITERAL(9, 93, 7), // "message"
QT_MOC_LITERAL(10, 101, 15), // "configValidated"
QT_MOC_LITERAL(11, 117, 24), // "singleConversionFinished"
QT_MOC_LITERAL(12, 142, 18), // "ConversionResponse"
QT_MOC_LITERAL(13, 161, 8), // "response"
QT_MOC_LITERAL(14, 170, 23), // "batchConversionFinished"
QT_MOC_LITERAL(15, 194, 13), // "errorOccurred"
QT_MOC_LITERAL(16, 208, 5), // "error"
QT_MOC_LITERAL(17, 214, 21), // "onHealthCheckFinished"
QT_MOC_LITERAL(18, 236, 19), // "onGetConfigFinished"
QT_MOC_LITERAL(19, 256, 22), // "onUpdateConfigFinished"
QT_MOC_LITERAL(20, 279, 24), // "onValidateConfigFinished"
QT_MOC_LITERAL(21, 304, 26), // "onSingleConversionFinished"
QT_MOC_LITERAL(22, 331, 25), // "onBatchConversionFinished"
QT_MOC_LITERAL(23, 357, 14), // "onNetworkError"
QT_MOC_LITERAL(24, 372, 27) // "QNetworkReply::NetworkError"

    },
    "HttpApi\0healthCheckFinished\0\0isOnline\0"
    "configReceived\0ConfigData\0config\0"
    "configUpdated\0success\0message\0"
    "configValidated\0singleConversionFinished\0"
    "ConversionResponse\0response\0"
    "batchConversionFinished\0errorOccurred\0"
    "error\0onHealthCheckFinished\0"
    "onGetConfigFinished\0onUpdateConfigFinished\0"
    "onValidateConfigFinished\0"
    "onSingleConversionFinished\0"
    "onBatchConversionFinished\0onNetworkError\0"
    "QNetworkReply::NetworkError"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HttpApi[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   84,    2, 0x06 /* Public */,
       4,    1,   87,    2, 0x06 /* Public */,
       7,    2,   90,    2, 0x06 /* Public */,
      10,    2,   95,    2, 0x06 /* Public */,
      11,    1,  100,    2, 0x06 /* Public */,
      14,    1,  103,    2, 0x06 /* Public */,
      15,    1,  106,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      17,    0,  109,    2, 0x08 /* Private */,
      18,    0,  110,    2, 0x08 /* Private */,
      19,    0,  111,    2, 0x08 /* Private */,
      20,    0,  112,    2, 0x08 /* Private */,
      21,    0,  113,    2, 0x08 /* Private */,
      22,    0,  114,    2, 0x08 /* Private */,
      23,    1,  115,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    8,    9,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    8,    9,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void, QMetaType::QString,   16,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 24,   16,

       0        // eod
};

void HttpApi::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<HttpApi *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->healthCheckFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->configReceived((*reinterpret_cast< const ConfigData(*)>(_a[1]))); break;
        case 2: _t->configUpdated((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->configValidated((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 4: _t->singleConversionFinished((*reinterpret_cast< const ConversionResponse(*)>(_a[1]))); break;
        case 5: _t->batchConversionFinished((*reinterpret_cast< const ConversionResponse(*)>(_a[1]))); break;
        case 6: _t->errorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->onHealthCheckFinished(); break;
        case 8: _t->onGetConfigFinished(); break;
        case 9: _t->onUpdateConfigFinished(); break;
        case 10: _t->onValidateConfigFinished(); break;
        case 11: _t->onSingleConversionFinished(); break;
        case 12: _t->onBatchConversionFinished(); break;
        case 13: _t->onNetworkError((*reinterpret_cast< QNetworkReply::NetworkError(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 13:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply::NetworkError >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (HttpApi::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HttpApi::healthCheckFinished)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (HttpApi::*)(const ConfigData & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HttpApi::configReceived)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (HttpApi::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HttpApi::configUpdated)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (HttpApi::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HttpApi::configValidated)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (HttpApi::*)(const ConversionResponse & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HttpApi::singleConversionFinished)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (HttpApi::*)(const ConversionResponse & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HttpApi::batchConversionFinished)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (HttpApi::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HttpApi::errorOccurred)) {
                *result = 6;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject HttpApi::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_HttpApi.data,
    qt_meta_data_HttpApi,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *HttpApi::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HttpApi::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HttpApi.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int HttpApi::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void HttpApi::healthCheckFinished(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void HttpApi::configReceived(const ConfigData & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void HttpApi::configUpdated(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void HttpApi::configValidated(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void HttpApi::singleConversionFinished(const ConversionResponse & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void HttpApi::batchConversionFinished(const ConversionResponse & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void HttpApi::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
