/****************************************************************************
** Meta object code from reading C++ file 'singlefileconverter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "src/singlefileconverter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'singlefileconverter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SingleFileConverter_t {
    QByteArrayData data[16];
    char stringdata0[238];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SingleFileConverter_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SingleFileConverter_t qt_meta_stringdata_SingleFileConverter = {
    {
QT_MOC_LITERAL(0, 0, 19), // "SingleFileConverter"
QT_MOC_LITERAL(1, 20, 17), // "conversionStarted"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 18), // "conversionFinished"
QT_MOC_LITERAL(4, 58, 7), // "success"
QT_MOC_LITERAL(5, 66, 7), // "message"
QT_MOC_LITERAL(6, 74, 15), // "selectInputFile"
QT_MOC_LITERAL(7, 90, 15), // "selectOutputDir"
QT_MOC_LITERAL(8, 106, 15), // "startConversion"
QT_MOC_LITERAL(9, 122, 8), // "clearAll"
QT_MOC_LITERAL(10, 131, 18), // "onInputFileChanged"
QT_MOC_LITERAL(11, 150, 18), // "onOutputDirChanged"
QT_MOC_LITERAL(12, 169, 19), // "onOutputNameChanged"
QT_MOC_LITERAL(13, 189, 20), // "onConversionFinished"
QT_MOC_LITERAL(14, 210, 18), // "ConversionResponse"
QT_MOC_LITERAL(15, 229, 8) // "response"

    },
    "SingleFileConverter\0conversionStarted\0"
    "\0conversionFinished\0success\0message\0"
    "selectInputFile\0selectOutputDir\0"
    "startConversion\0clearAll\0onInputFileChanged\0"
    "onOutputDirChanged\0onOutputNameChanged\0"
    "onConversionFinished\0ConversionResponse\0"
    "response"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SingleFileConverter[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,
       3,    2,   65,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   70,    2, 0x08 /* Private */,
       7,    0,   71,    2, 0x08 /* Private */,
       8,    0,   72,    2, 0x08 /* Private */,
       9,    0,   73,    2, 0x08 /* Private */,
      10,    0,   74,    2, 0x08 /* Private */,
      11,    0,   75,    2, 0x08 /* Private */,
      12,    0,   76,    2, 0x08 /* Private */,
      13,    1,   77,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    4,    5,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14,   15,

       0        // eod
};

void SingleFileConverter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SingleFileConverter *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->conversionStarted(); break;
        case 1: _t->conversionFinished((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: _t->selectInputFile(); break;
        case 3: _t->selectOutputDir(); break;
        case 4: _t->startConversion(); break;
        case 5: _t->clearAll(); break;
        case 6: _t->onInputFileChanged(); break;
        case 7: _t->onOutputDirChanged(); break;
        case 8: _t->onOutputNameChanged(); break;
        case 9: _t->onConversionFinished((*reinterpret_cast< const ConversionResponse(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SingleFileConverter::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SingleFileConverter::conversionStarted)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SingleFileConverter::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SingleFileConverter::conversionFinished)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SingleFileConverter::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_SingleFileConverter.data,
    qt_meta_data_SingleFileConverter,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SingleFileConverter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SingleFileConverter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SingleFileConverter.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SingleFileConverter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void SingleFileConverter::conversionStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SingleFileConverter::conversionFinished(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
