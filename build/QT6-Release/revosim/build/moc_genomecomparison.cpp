/****************************************************************************
** Meta object code from reading C++ file 'genomecomparison.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../revosim/genomecomparison.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'genomecomparison.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_GenomeComparison_t {
    uint offsetsAndSizes[20];
    char stringdata0[17];
    char stringdata1[12];
    char stringdata2[1];
    char stringdata3[13];
    char stringdata4[15];
    char stringdata5[8];
    char stringdata6[7];
    char stringdata7[17];
    char stringdata8[4];
    char stringdata9[4];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_GenomeComparison_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_GenomeComparison_t qt_meta_stringdata_GenomeComparison = {
    {
        QT_MOC_LITERAL(0, 16),  // "GenomeComparison"
        QT_MOC_LITERAL(17, 11),  // "resetTables"
        QT_MOC_LITERAL(29, 0),  // ""
        QT_MOC_LITERAL(30, 12),  // "deleteGenome"
        QT_MOC_LITERAL(43, 14),  // "compareGenomes"
        QT_MOC_LITERAL(58, 7),  // "setAuto"
        QT_MOC_LITERAL(66, 6),  // "toggle"
        QT_MOC_LITERAL(73, 16),  // "updateGenomeName"
        QT_MOC_LITERAL(90, 3),  // "row"
        QT_MOC_LITERAL(94, 3)   // "col"
    },
    "GenomeComparison",
    "resetTables",
    "",
    "deleteGenome",
    "compareGenomes",
    "setAuto",
    "toggle",
    "updateGenomeName",
    "row",
    "col"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_GenomeComparison[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   44,    2, 0x08,    1 /* Private */,
       3,    0,   45,    2, 0x08,    2 /* Private */,
       4,    0,   46,    2, 0x08,    3 /* Private */,
       5,    1,   47,    2, 0x08,    4 /* Private */,
       7,    2,   50,    2, 0x08,    6 /* Private */,

 // slots: parameters
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    8,    9,

       0        // eod
};

Q_CONSTINIT const QMetaObject GenomeComparison::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_GenomeComparison.offsetsAndSizes,
    qt_meta_data_GenomeComparison,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_GenomeComparison_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<GenomeComparison, std::true_type>,
        // method 'resetTables'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'deleteGenome'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'compareGenomes'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'setAuto'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'updateGenomeName'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void GenomeComparison::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GenomeComparison *>(_o);
        (void)_t;
        switch (_id) {
        case 0: { bool _r = _t->resetTables();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 1: { bool _r = _t->deleteGenome();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 2: { bool _r = _t->compareGenomes();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 3: _t->setAuto((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 4: _t->updateGenomeName((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject *GenomeComparison::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GenomeComparison::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GenomeComparison.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int GenomeComparison::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
