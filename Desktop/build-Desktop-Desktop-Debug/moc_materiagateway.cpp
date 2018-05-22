/****************************************************************************
** Meta object code from reading C++ file 'materiagateway.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Desktop/materiagateway.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'materiagateway.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MateriaGateway_t {
    QByteArrayData data[19];
    char stringdata0[296];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MateriaGateway_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MateriaGateway_t qt_meta_stringdata_MateriaGateway = {
    {
QT_MOC_LITERAL(0, 0, 14), // "MateriaGateway"
QT_MOC_LITERAL(1, 15, 18), // "onAffinitiesLoaded"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 30), // "std::vector<materia::Affinity>"
QT_MOC_LITERAL(4, 66, 10), // "affinities"
QT_MOC_LITERAL(5, 77, 13), // "onGoalsLoaded"
QT_MOC_LITERAL(6, 91, 26), // "std::vector<materia::Goal>"
QT_MOC_LITERAL(7, 118, 5), // "goals"
QT_MOC_LITERAL(8, 124, 19), // "onGoalDetailsLoaded"
QT_MOC_LITERAL(9, 144, 11), // "materia::Id"
QT_MOC_LITERAL(10, 156, 2), // "id"
QT_MOC_LITERAL(11, 159, 26), // "std::vector<materia::Task>"
QT_MOC_LITERAL(12, 186, 5), // "tasks"
QT_MOC_LITERAL(13, 192, 31), // "std::vector<materia::Objective>"
QT_MOC_LITERAL(14, 224, 10), // "objectives"
QT_MOC_LITERAL(15, 235, 14), // "loadAffinities"
QT_MOC_LITERAL(16, 250, 9), // "loadGoals"
QT_MOC_LITERAL(17, 260, 15), // "loadGoalDetails"
QT_MOC_LITERAL(18, 276, 19) // "configureAffinities"

    },
    "MateriaGateway\0onAffinitiesLoaded\0\0"
    "std::vector<materia::Affinity>\0"
    "affinities\0onGoalsLoaded\0"
    "std::vector<materia::Goal>\0goals\0"
    "onGoalDetailsLoaded\0materia::Id\0id\0"
    "std::vector<materia::Task>\0tasks\0"
    "std::vector<materia::Objective>\0"
    "objectives\0loadAffinities\0loadGoals\0"
    "loadGoalDetails\0configureAffinities"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MateriaGateway[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       5,    1,   52,    2, 0x06 /* Public */,
       8,    3,   55,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      15,    0,   62,    2, 0x0a /* Public */,
      16,    0,   63,    2, 0x0a /* Public */,
      17,    1,   64,    2, 0x0a /* Public */,
      18,    1,   67,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 9, 0x80000000 | 11, 0x80000000 | 13,   10,   12,   14,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

void MateriaGateway::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MateriaGateway *_t = static_cast<MateriaGateway *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onAffinitiesLoaded((*reinterpret_cast< const std::vector<materia::Affinity>(*)>(_a[1]))); break;
        case 1: _t->onGoalsLoaded((*reinterpret_cast< const std::vector<materia::Goal>(*)>(_a[1]))); break;
        case 2: _t->onGoalDetailsLoaded((*reinterpret_cast< const materia::Id(*)>(_a[1])),(*reinterpret_cast< const std::vector<materia::Task>(*)>(_a[2])),(*reinterpret_cast< const std::vector<materia::Objective>(*)>(_a[3]))); break;
        case 3: _t->loadAffinities(); break;
        case 4: _t->loadGoals(); break;
        case 5: _t->loadGoalDetails((*reinterpret_cast< const materia::Id(*)>(_a[1]))); break;
        case 6: _t->configureAffinities((*reinterpret_cast< const std::vector<materia::Affinity>(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< materia::Id >(); break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< std::vector<materia::Objective> >(); break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< std::vector<materia::Task> >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< materia::Id >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (MateriaGateway::*_t)(const std::vector<materia::Affinity> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MateriaGateway::onAffinitiesLoaded)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (MateriaGateway::*_t)(const std::vector<materia::Goal> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MateriaGateway::onGoalsLoaded)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (MateriaGateway::*_t)(const materia::Id , const std::vector<materia::Task> , const std::vector<materia::Objective> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MateriaGateway::onGoalDetailsLoaded)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MateriaGateway::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MateriaGateway.data,
      qt_meta_data_MateriaGateway,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MateriaGateway::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MateriaGateway::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MateriaGateway.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MateriaGateway::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void MateriaGateway::onAffinitiesLoaded(const std::vector<materia::Affinity> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MateriaGateway::onGoalsLoaded(const std::vector<materia::Goal> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MateriaGateway::onGoalDetailsLoaded(const materia::Id _t1, const std::vector<materia::Task> _t2, const std::vector<materia::Objective> _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
