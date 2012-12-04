/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Thu 22. Nov 20:22:17 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      50,   45,   11,   11, 0x08,
      79,   45,   11,   11, 0x08,
     105,   45,   11,   11, 0x08,
     131,   45,   11,   11, 0x08,
     163,   45,   11,   11, 0x08,
     192,   45,   11,   11, 0x08,
     230,  222,   11,   11, 0x08,
     256,  222,   11,   11, 0x08,
     281,   45,   11,   11, 0x08,
     317,   45,   11,   11, 0x08,
     352,   45,   11,   11, 0x08,
     387,   11,   11,   11, 0x08,
     411,  222,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0on_generateEnvironment_clicked()\0"
    "arg1\0on_numSeed_valueChanged(int)\0"
    "on_sMax_valueChanged(int)\0"
    "on_sMin_valueChanged(int)\0"
    "on_velMax2_valueChanged(double)\0"
    "on_cvelMax_valueChanged(int)\0"
    "on_buffSpin_valueChanged(int)\0checked\0"
    "on_periodic_clicked(bool)\0"
    "on_blurChk_clicked(bool)\0"
    "on_numGenerations_valueChanged(int)\0"
    "on_convergeAt_valueChanged(double)\0"
    "on_factorSpin_valueChanged(double)\0"
    "on_pushButton_clicked()\0"
    "on_checkBox_2_toggled(bool)\0"
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->on_generateEnvironment_clicked(); break;
        case 1: _t->on_numSeed_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->on_sMax_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_sMin_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->on_velMax2_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: _t->on_cvelMax_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->on_buffSpin_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->on_periodic_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->on_blurChk_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->on_numGenerations_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->on_convergeAt_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 11: _t->on_factorSpin_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 12: _t->on_pushButton_clicked(); break;
        case 13: _t->on_checkBox_2_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
