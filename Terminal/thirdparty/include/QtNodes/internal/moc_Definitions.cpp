/****************************************************************************
** Meta object code from reading C++ file 'Definitions.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../include/QtNodes/internal/Definitions.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Definitions.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QtNodes_t {
    QByteArrayData data[28];
    char stringdata0[245];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QtNodes_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QtNodes_t qt_meta_stringdata_QtNodes = {
    {
QT_MOC_LITERAL(0, 0, 7), // "QtNodes"
QT_MOC_LITERAL(1, 8, 8), // "NodeRole"
QT_MOC_LITERAL(2, 17, 4), // "Type"
QT_MOC_LITERAL(3, 22, 8), // "Position"
QT_MOC_LITERAL(4, 31, 4), // "Size"
QT_MOC_LITERAL(5, 36, 14), // "CaptionVisible"
QT_MOC_LITERAL(6, 51, 7), // "Caption"
QT_MOC_LITERAL(7, 59, 5), // "Style"
QT_MOC_LITERAL(8, 65, 12), // "InternalData"
QT_MOC_LITERAL(9, 78, 11), // "InPortCount"
QT_MOC_LITERAL(10, 90, 12), // "OutPortCount"
QT_MOC_LITERAL(11, 103, 6), // "Widget"
QT_MOC_LITERAL(12, 110, 9), // "NodeFlags"
QT_MOC_LITERAL(13, 120, 8), // "NodeFlag"
QT_MOC_LITERAL(14, 129, 7), // "NoFlags"
QT_MOC_LITERAL(15, 137, 9), // "Resizable"
QT_MOC_LITERAL(16, 147, 6), // "Locked"
QT_MOC_LITERAL(17, 154, 8), // "PortRole"
QT_MOC_LITERAL(18, 163, 4), // "Data"
QT_MOC_LITERAL(19, 168, 8), // "DataType"
QT_MOC_LITERAL(20, 177, 20), // "ConnectionPolicyRole"
QT_MOC_LITERAL(21, 198, 16), // "ConnectionPolicy"
QT_MOC_LITERAL(22, 215, 3), // "One"
QT_MOC_LITERAL(23, 219, 4), // "Many"
QT_MOC_LITERAL(24, 224, 8), // "PortType"
QT_MOC_LITERAL(25, 233, 2), // "In"
QT_MOC_LITERAL(26, 236, 3), // "Out"
QT_MOC_LITERAL(27, 240, 4) // "None"

    },
    "QtNodes\0NodeRole\0Type\0Position\0Size\0"
    "CaptionVisible\0Caption\0Style\0InternalData\0"
    "InPortCount\0OutPortCount\0Widget\0"
    "NodeFlags\0NodeFlag\0NoFlags\0Resizable\0"
    "Locked\0PortRole\0Data\0DataType\0"
    "ConnectionPolicyRole\0ConnectionPolicy\0"
    "One\0Many\0PortType\0In\0Out\0None"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtNodes[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       5,   14, // enums/sets
       0,    0, // constructors
       4,       // flags
       0,       // signalCount

 // enums: name, alias, flags, count, data
       1,    1, 0x2,   10,   39,
      12,   13, 0x1,    3,   59,
      17,   17, 0x2,    5,   65,
      21,   21, 0x2,    2,   75,
      24,   24, 0x2,    3,   79,

 // enum data: key, value
       2, uint(QtNodes::NodeRole::Type),
       3, uint(QtNodes::NodeRole::Position),
       4, uint(QtNodes::NodeRole::Size),
       5, uint(QtNodes::NodeRole::CaptionVisible),
       6, uint(QtNodes::NodeRole::Caption),
       7, uint(QtNodes::NodeRole::Style),
       8, uint(QtNodes::NodeRole::InternalData),
       9, uint(QtNodes::NodeRole::InPortCount),
      10, uint(QtNodes::NodeRole::OutPortCount),
      11, uint(QtNodes::NodeRole::Widget),
      14, uint(QtNodes::NoFlags),
      15, uint(QtNodes::Resizable),
      16, uint(QtNodes::Locked),
      18, uint(QtNodes::PortRole::Data),
      19, uint(QtNodes::PortRole::DataType),
      20, uint(QtNodes::PortRole::ConnectionPolicyRole),
       5, uint(QtNodes::PortRole::CaptionVisible),
       6, uint(QtNodes::PortRole::Caption),
      22, uint(QtNodes::ConnectionPolicy::One),
      23, uint(QtNodes::ConnectionPolicy::Many),
      25, uint(QtNodes::PortType::In),
      26, uint(QtNodes::PortType::Out),
      27, uint(QtNodes::PortType::None),

       0        // eod
};

QT_INIT_METAOBJECT const QMetaObject QtNodes::staticMetaObject = { {
    nullptr,
    qt_meta_stringdata_QtNodes.data,
    qt_meta_data_QtNodes,
    nullptr,
    nullptr,
    nullptr
} };

QT_WARNING_POP
QT_END_MOC_NAMESPACE
