/****************************************************************************
** Meta object code from reading C++ file 'customTableWidget.h'
**
** Created: Wed Apr 17 14:34:55 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "customTableWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'customTableWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_customTableWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      27,   19,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      55,   48,   18,   18, 0x0a,
      71,   48,   18,   18, 0x0a,
      88,   19,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_customTableWidget[] = {
    "customTableWidget\0\0row,col\0"
    "cellChanged(int,int)\0enable\0setEnabled(int)\0"
    "setEnabled(bool)\0setNodeValue(int,int)\0"
};

void customTableWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        customTableWidget *_t = static_cast<customTableWidget *>(_o);
        switch (_id) {
        case 0: _t->cellChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->setEnabled((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->setNodeValue((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData customTableWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject customTableWidget::staticMetaObject = {
    { &QTableView::staticMetaObject, qt_meta_stringdata_customTableWidget,
      qt_meta_data_customTableWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &customTableWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *customTableWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *customTableWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_customTableWidget))
        return static_cast<void*>(const_cast< customTableWidget*>(this));
    return QTableView::qt_metacast(_clname);
}

int customTableWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void customTableWidget::cellChanged(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
