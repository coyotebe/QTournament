/****************************************************************************
** Meta object code from reading C++ file 'TeamMngr.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "TeamMngr.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TeamMngr.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QTournament__TeamMngr_t {
    QByteArrayData data[13];
    char stringdata[156];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_QTournament__TeamMngr_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_QTournament__TeamMngr_t qt_meta_stringdata_QTournament__TeamMngr = {
    {
QT_MOC_LITERAL(0, 0, 21),
QT_MOC_LITERAL(1, 22, 15),
QT_MOC_LITERAL(2, 38, 0),
QT_MOC_LITERAL(3, 39, 13),
QT_MOC_LITERAL(4, 53, 13),
QT_MOC_LITERAL(5, 67, 11),
QT_MOC_LITERAL(6, 79, 10),
QT_MOC_LITERAL(7, 90, 21),
QT_MOC_LITERAL(8, 112, 6),
QT_MOC_LITERAL(9, 119, 14),
QT_MOC_LITERAL(10, 134, 4),
QT_MOC_LITERAL(11, 139, 7),
QT_MOC_LITERAL(12, 147, 7)
    },
    "QTournament::TeamMngr\0beginCreateTeam\0"
    "\0endCreateTeam\0newTeamSeqNum\0teamRenamed\0"
    "teamSeqNum\0teamAssignmentChanged\0"
    "Player\0affectedPlayer\0Team\0oldTeam\0"
    "newTeam\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QTournament__TeamMngr[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06,
       3,    1,   35,    2, 0x06,
       5,    1,   38,    2, 0x06,
       7,    3,   41,    2, 0x06,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, 0x80000000 | 8, 0x80000000 | 10, 0x80000000 | 10,    9,   11,   12,

       0        // eod
};

void QTournament::TeamMngr::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TeamMngr *_t = static_cast<TeamMngr *>(_o);
        switch (_id) {
        case 0: _t->beginCreateTeam(); break;
        case 1: _t->endCreateTeam((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->teamRenamed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->teamAssignmentChanged((*reinterpret_cast< const Player(*)>(_a[1])),(*reinterpret_cast< const Team(*)>(_a[2])),(*reinterpret_cast< const Team(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (TeamMngr::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TeamMngr::beginCreateTeam)) {
                *result = 0;
            }
        }
        {
            typedef void (TeamMngr::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TeamMngr::endCreateTeam)) {
                *result = 1;
            }
        }
        {
            typedef void (TeamMngr::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TeamMngr::teamRenamed)) {
                *result = 2;
            }
        }
        {
            typedef void (TeamMngr::*_t)(const Player & , const Team & , const Team & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TeamMngr::teamAssignmentChanged)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject QTournament::TeamMngr::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QTournament__TeamMngr.data,
      qt_meta_data_QTournament__TeamMngr,  qt_static_metacall, 0, 0}
};


const QMetaObject *QTournament::TeamMngr::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QTournament::TeamMngr::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QTournament__TeamMngr.stringdata))
        return static_cast<void*>(const_cast< TeamMngr*>(this));
    if (!strcmp(_clname, "GenericObjectManager"))
        return static_cast< GenericObjectManager*>(const_cast< TeamMngr*>(this));
    return QObject::qt_metacast(_clname);
}

int QTournament::TeamMngr::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void QTournament::TeamMngr::beginCreateTeam()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QTournament::TeamMngr::endCreateTeam(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QTournament::TeamMngr::teamRenamed(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QTournament::TeamMngr::teamAssignmentChanged(const Player & _t1, const Team & _t2, const Team & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
