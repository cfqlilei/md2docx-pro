/****************************************************************************
** Meta object code from reading C++ file 'simple_batchconverter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../src/simple_batchconverter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'simple_batchconverter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SimpleBatchConverter_t {
    QByteArrayData data[13];
    char stringdata0[192];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SimpleBatchConverter_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SimpleBatchConverter_t qt_meta_stringdata_SimpleBatchConverter = {
    {
QT_MOC_LITERAL(0, 0, 20), // "SimpleBatchConverter"
QT_MOC_LITERAL(1, 21, 17), // "conversionStarted"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 18), // "conversionFinished"
QT_MOC_LITERAL(4, 59, 7), // "success"
QT_MOC_LITERAL(5, 67, 7), // "message"
QT_MOC_LITERAL(6, 75, 16), // "selectInputFiles"
QT_MOC_LITERAL(7, 92, 13), // "clearAllFiles"
QT_MOC_LITERAL(8, 106, 15), // "selectOutputDir"
QT_MOC_LITERAL(9, 122, 20), // "startBatchConversion"
QT_MOC_LITERAL(10, 143, 20), // "onConversionFinished"
QT_MOC_LITERAL(11, 164, 18), // "ConversionResponse"
QT_MOC_LITERAL(12, 183, 8) // "response"

    },
    "SimpleBatchConverter\0conversionStarted\0"
    "\0conversionFinished\0success\0message\0"
    "selectInputFiles\0clearAllFiles\0"
    "selectOutputDir\0startBatchConversion\0"
    "onConversionFinished\0ConversionResponse\0"
    "response"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SimpleBatchConverter[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,
       3,    2,   50,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   55,    2, 0x08 /* Private */,
       7,    0,   56,    2, 0x08 /* Private */,
       8,    0,   57,    2, 0x08 /* Private */,
       9,    0,   58,    2, 0x08 /* Private */,
      10,    1,   59,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    4,    5,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,

       0        // eod
};

void SimpleBatchConverter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SimpleBatchConverter *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->conversionStarted(); break;
        case 1: _t->conversionFinished((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: _t->selectInputFiles(); break;
        case 3: _t->clearAllFiles(); break;
        case 4: _t->selectOutputDir(); break;
        case 5: _t->startBatchConversion(); break;
        case 6: _t->onConversionFinished((*reinterpret_cast< const ConversionResponse(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SimpleBatchConverter::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SimpleBatchConverter::conversionStarted)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SimpleBatchConverter::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SimpleBatchConverter::conversionFinished)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SimpleBatchConverter::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_SimpleBatchConverter.data,
    qt_meta_data_SimpleBatchConverter,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SimpleBatchConverter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SimpleBatchConverter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SimpleBatchConverter.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SimpleBatchConverter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void SimpleBatchConverter::conversionStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SimpleBatchConverter::conversionFinished(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
