/****************************************************************************
** Meta object code from reading C++ file 'qtreeviewrightclick.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../inc/qtreeviewrightclick.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtreeviewrightclick.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QTreeViewRightClick[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x05,

 // slots: signature, parameters, type, tag, flags
      42,   36,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QTreeViewRightClick[] = {
    "QTreeViewRightClick\0\0rightClicked()\0"
    "event\0mousePressEvent(QMouseEvent*)\0"
};

void QTreeViewRightClick::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QTreeViewRightClick *_t = static_cast<QTreeViewRightClick *>(_o);
        switch (_id) {
        case 0: _t->rightClicked(); break;
        case 1: _t->mousePressEvent((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QTreeViewRightClick::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QTreeViewRightClick::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_QTreeViewRightClick,
      qt_meta_data_QTreeViewRightClick, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QTreeViewRightClick::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QTreeViewRightClick::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QTreeViewRightClick::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QTreeViewRightClick))
        return static_cast<void*>(const_cast< QTreeViewRightClick*>(this));
    return QTreeView::qt_metacast(_clname);
}

int QTreeViewRightClick::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void QTreeViewRightClick::rightClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
