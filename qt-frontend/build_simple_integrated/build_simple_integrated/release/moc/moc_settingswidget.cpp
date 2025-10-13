/****************************************************************************
** Meta object code from reading C++ file 'settingswidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../src/settingswidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'settingswidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SettingsWidget_t {
    QByteArrayData data[20];
    char stringdata0[286];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SettingsWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SettingsWidget_t qt_meta_stringdata_SettingsWidget = {
    {
QT_MOC_LITERAL(0, 0, 14), // "SettingsWidget"
QT_MOC_LITERAL(1, 15, 13), // "configChanged"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 16), // "selectPandocPath"
QT_MOC_LITERAL(4, 47, 14), // "testPandocPath"
QT_MOC_LITERAL(5, 62, 18), // "selectTemplateFile"
QT_MOC_LITERAL(6, 81, 17), // "clearTemplateFile"
QT_MOC_LITERAL(7, 99, 17), // "loadCurrentConfig"
QT_MOC_LITERAL(8, 117, 10), // "saveConfig"
QT_MOC_LITERAL(9, 128, 14), // "validateConfig"
QT_MOC_LITERAL(10, 143, 15), // "resetToDefaults"
QT_MOC_LITERAL(11, 159, 19), // "onPandocPathChanged"
QT_MOC_LITERAL(12, 179, 21), // "onTemplateFileChanged"
QT_MOC_LITERAL(13, 201, 16), // "onConfigReceived"
QT_MOC_LITERAL(14, 218, 10), // "ConfigData"
QT_MOC_LITERAL(15, 229, 6), // "config"
QT_MOC_LITERAL(16, 236, 15), // "onConfigUpdated"
QT_MOC_LITERAL(17, 252, 7), // "success"
QT_MOC_LITERAL(18, 260, 7), // "message"
QT_MOC_LITERAL(19, 268, 17) // "onConfigValidated"

    },
    "SettingsWidget\0configChanged\0\0"
    "selectPandocPath\0testPandocPath\0"
    "selectTemplateFile\0clearTemplateFile\0"
    "loadCurrentConfig\0saveConfig\0"
    "validateConfig\0resetToDefaults\0"
    "onPandocPathChanged\0onTemplateFileChanged\0"
    "onConfigReceived\0ConfigData\0config\0"
    "onConfigUpdated\0success\0message\0"
    "onConfigValidated"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SettingsWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   85,    2, 0x08 /* Private */,
       4,    0,   86,    2, 0x08 /* Private */,
       5,    0,   87,    2, 0x08 /* Private */,
       6,    0,   88,    2, 0x08 /* Private */,
       7,    0,   89,    2, 0x08 /* Private */,
       8,    0,   90,    2, 0x08 /* Private */,
       9,    0,   91,    2, 0x08 /* Private */,
      10,    0,   92,    2, 0x08 /* Private */,
      11,    0,   93,    2, 0x08 /* Private */,
      12,    0,   94,    2, 0x08 /* Private */,
      13,    1,   95,    2, 0x08 /* Private */,
      16,    2,   98,    2, 0x08 /* Private */,
      19,    2,  103,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   17,   18,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   17,   18,

       0        // eod
};

void SettingsWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SettingsWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->configChanged(); break;
        case 1: _t->selectPandocPath(); break;
        case 2: _t->testPandocPath(); break;
        case 3: _t->selectTemplateFile(); break;
        case 4: _t->clearTemplateFile(); break;
        case 5: _t->loadCurrentConfig(); break;
        case 6: _t->saveConfig(); break;
        case 7: _t->validateConfig(); break;
        case 8: _t->resetToDefaults(); break;
        case 9: _t->onPandocPathChanged(); break;
        case 10: _t->onTemplateFileChanged(); break;
        case 11: _t->onConfigReceived((*reinterpret_cast< const ConfigData(*)>(_a[1]))); break;
        case 12: _t->onConfigUpdated((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 13: _t->onConfigValidated((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SettingsWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SettingsWidget::configChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SettingsWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_SettingsWidget.data,
    qt_meta_data_SettingsWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SettingsWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SettingsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SettingsWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SettingsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void SettingsWidget::configChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
