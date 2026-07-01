/****************************************************************************
** Meta object code from reading C++ file 'qscadaobjectinfodialog.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../QScadaObject/qscadaobjectinfodialog.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qscadaobjectinfodialog.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN22QScadaObjectInfoDialogE_t {};
} // unnamed namespace

template <> constexpr inline auto QScadaObjectInfoDialog::qt_create_metaobjectdata<qt_meta_tag_ZN22QScadaObjectInfoDialogE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QScadaObjectInfoDialog",
        "deletePressed",
        "",
        "QScadaObjectInfo*",
        "savePressed",
        "updateWithObjectInfo",
        "info",
        "geometryUpdated",
        "on_pushButton_2_pressed",
        "on_pushButton_pressed"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'deletePressed'
        QtMocHelpers::SignalData<void(QScadaObjectInfo *)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 },
        }}),
        // Signal 'savePressed'
        QtMocHelpers::SignalData<void(QScadaObjectInfo *)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 },
        }}),
        // Slot 'updateWithObjectInfo'
        QtMocHelpers::SlotData<void(QScadaObjectInfo *)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 6 },
        }}),
        // Slot 'geometryUpdated'
        QtMocHelpers::SlotData<void(QScadaObjectInfo *)>(7, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 3, 2 },
        }}),
        // Slot 'on_pushButton_2_pressed'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_pushButton_pressed'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QScadaObjectInfoDialog, qt_meta_tag_ZN22QScadaObjectInfoDialogE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QScadaObjectInfoDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22QScadaObjectInfoDialogE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22QScadaObjectInfoDialogE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN22QScadaObjectInfoDialogE_t>.metaTypes,
    nullptr
} };

void QScadaObjectInfoDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QScadaObjectInfoDialog *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->deletePressed((*reinterpret_cast< std::add_pointer_t<QScadaObjectInfo*>>(_a[1]))); break;
        case 1: _t->savePressed((*reinterpret_cast< std::add_pointer_t<QScadaObjectInfo*>>(_a[1]))); break;
        case 2: _t->updateWithObjectInfo((*reinterpret_cast< std::add_pointer_t<QScadaObjectInfo*>>(_a[1]))); break;
        case 3: _t->geometryUpdated((*reinterpret_cast< std::add_pointer_t<QScadaObjectInfo*>>(_a[1]))); break;
        case 4: _t->on_pushButton_2_pressed(); break;
        case 5: _t->on_pushButton_pressed(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QScadaObjectInfoDialog::*)(QScadaObjectInfo * )>(_a, &QScadaObjectInfoDialog::deletePressed, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (QScadaObjectInfoDialog::*)(QScadaObjectInfo * )>(_a, &QScadaObjectInfoDialog::savePressed, 1))
            return;
    }
}

const QMetaObject *QScadaObjectInfoDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QScadaObjectInfoDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22QScadaObjectInfoDialogE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int QScadaObjectInfoDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void QScadaObjectInfoDialog::deletePressed(QScadaObjectInfo * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void QScadaObjectInfoDialog::savePressed(QScadaObjectInfo * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}
QT_WARNING_POP
