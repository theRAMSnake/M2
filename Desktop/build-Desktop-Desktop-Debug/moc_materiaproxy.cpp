/****************************************************************************
** Meta object code from reading C++ file 'materiaproxy.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Desktop/materiaproxy.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'materiaproxy.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MateriaProxy_t {
    QByteArrayData data[11];
    char stringdata0[176];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MateriaProxy_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MateriaProxy_t qt_meta_stringdata_MateriaProxy = {
    {
QT_MOC_LITERAL(0, 0, 12), // "MateriaProxy"
QT_MOC_LITERAL(1, 13, 17), // "sigLoadAffinities"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 12), // "sigLoadGoals"
QT_MOC_LITERAL(4, 45, 22), // "sigConfigureAffinities"
QT_MOC_LITERAL(5, 68, 30), // "std::vector<materia::Affinity>"
QT_MOC_LITERAL(6, 99, 18), // "onAffinitiesLoaded"
QT_MOC_LITERAL(7, 118, 10), // "affinities"
QT_MOC_LITERAL(8, 129, 13), // "onGoalsLoaded"
QT_MOC_LITERAL(9, 143, 26), // "std::vector<materia::Goal>"
QT_MOC_LITERAL(10, 170, 5) // "goals"

    },
    "MateriaProxy\0sigLoadAffinities\0\0"
    "sigLoadGoals\0sigConfigureAffinities\0"
    "std::vector<materia::Affinity>\0"
    "onAffinitiesLoaded\0affinities\0"
    "onGoalsLoaded\0std::vector<materia::Goal>\0"
    "goals"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MateriaProxy[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    0,   40,    2, 0x06 /* Public */,
       4,    1,   41,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   44,    2, 0x08 /* Private */,
       8,    1,   47,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    2,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 5,    7,
    QMetaType::Void, 0x80000000 | 9,   10,

       0        // eod
};

void MateriaProxy::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MateriaProxy *_t = static_cast<MateriaProxy *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sigLoadAffinities(); break;
        case 1: _t->sigLoadGoals(); break;
        case 2: _t->sigConfigureAffinities((*reinterpret_cast< std::vector<materia::Affinity>(*)>(_a[1]))); break;
        case 3: _t->onAffinitiesLoaded((*reinterpret_cast< std::vector<materia::Affinity>(*)>(_a[1]))); break;
        case 4: _t->onGoalsLoaded((*reinterpret_cast< std::vector<materia::Goal>(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (MateriaProxy::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MateriaProxy::sigLoadAffinities)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (MateriaProxy::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MateriaProxy::sigLoadGoals)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (MateriaProxy::*_t)(std::vector<materia::Affinity> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MateriaProxy::sigConfigureAffinities)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MateriaProxy::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MateriaProxy.data,
      qt_meta_data_MateriaProxy,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MateriaProxy::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MateriaProxy::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MateriaProxy.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MateriaProxy::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void MateriaProxy::sigLoadAffinities()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void MateriaProxy::sigLoadGoals()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void MateriaProxy::sigConfigureAffinities(std::vector<materia::Affinity> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
