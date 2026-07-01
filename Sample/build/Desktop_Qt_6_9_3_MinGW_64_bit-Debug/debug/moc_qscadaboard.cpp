/****************************************************************************
** Meta object code from reading C++ file 'qscadaboard.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../QScadaBoard/qscadaboard.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qscadaboard.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN11QScadaBoardE_t {};
} // unnamed namespace

template <> constexpr inline auto QScadaBoard::qt_create_metaobjectdata<qt_meta_tag_ZN11QScadaBoardE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QScadaBoard",
        "objectDoubleClicked",
        "",
        "QScadaObject*",
        "objectSelected",
        "newObjectCreated",
        "bringToFront",
        "sendToBack",
        "newObjectSelected",
        "id",
        "objectMove",
        "x",
        "y",
        "objectResize"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'objectDoubleClicked'
        QtMocHelpers::SignalData<void(QScadaObject *)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 },
        }}),
        // Signal 'objectSelected'
        QtMocHelpers::SignalData<void(QScadaObject *)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 },
        }}),
        // Signal 'newObjectCreated'
        QtMocHelpers::SignalData<void(QScadaObject *)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 },
        }}),
        // Slot 'bringToFront'
        QtMocHelpers::SlotData<void(QScadaObject *)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 },
        }}),
        // Slot 'sendToBack'
        QtMocHelpers::SlotData<void(QScadaObject *)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 },
        }}),
        // Slot 'newObjectSelected'
        QtMocHelpers::SlotData<void(int)>(8, 2, QMC::AccessProtected, QMetaType::Void, {{
            { QMetaType::Int, 9 },
        }}),
        // Slot 'objectMove'
        QtMocHelpers::SlotData<void(int, int)>(10, 2, QMC::AccessProtected, QMetaType::Void, {{
            { QMetaType::Int, 11 }, { QMetaType::Int, 12 },
        }}),
        // Slot 'objectResize'
        QtMocHelpers::SlotData<void(int, int)>(13, 2, QMC::AccessProtected, QMetaType::Void, {{
            { QMetaType::Int, 11 }, { QMetaType::Int, 12 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QScadaBoard, qt_meta_tag_ZN11QScadaBoardE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QScadaBoard::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11QScadaBoardE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11QScadaBoardE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11QScadaBoardE_t>.metaTypes,
    nullptr
} };

void QScadaBoard::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QScadaBoard *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->objectDoubleClicked((*reinterpret_cast< std::add_pointer_t<QScadaObject*>>(_a[1]))); break;
        case 1: _t->objectSelected((*reinterpret_cast< std::add_pointer_t<QScadaObject*>>(_a[1]))); break;
        case 2: _t->newObjectCreated((*reinterpret_cast< std::add_pointer_t<QScadaObject*>>(_a[1]))); break;
        case 3: _t->bringToFront((*reinterpret_cast< std::add_pointer_t<QScadaObject*>>(_a[1]))); break;
        case 4: _t->sendToBack((*reinterpret_cast< std::add_pointer_t<QScadaObject*>>(_a[1]))); break;
        case 5: _t->newObjectSelected((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->objectMove((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 7: _t->objectResize((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QScadaObject* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QScadaObject* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QScadaObject* >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QScadaObject* >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QScadaObject* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QScadaBoard::*)(QScadaObject * )>(_a, &QScadaBoard::objectDoubleClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (QScadaBoard::*)(QScadaObject * )>(_a, &QScadaBoard::objectSelected, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (QScadaBoard::*)(QScadaObject * )>(_a, &QScadaBoard::newObjectCreated, 2))
            return;
    }
}

const QMetaObject *QScadaBoard::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QScadaBoard::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11QScadaBoardE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int QScadaBoard::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void QScadaBoard::objectDoubleClicked(QScadaObject * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void QScadaBoard::objectSelected(QScadaObject * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void QScadaBoard::newObjectCreated(QScadaObject * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
QT_WARNING_POP
