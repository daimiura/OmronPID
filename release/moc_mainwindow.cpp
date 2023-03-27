/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[92];
    char stringdata0[1329];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 12), // "periodicWork"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 8), // "makePlot"
QT_MOC_LITERAL(4, 34, 9), // "TempCheck"
QT_MOC_LITERAL(5, 44, 13), // "keyPressEvent"
QT_MOC_LITERAL(6, 58, 10), // "QKeyEvent*"
QT_MOC_LITERAL(7, 69, 3), // "key"
QT_MOC_LITERAL(8, 73, 15), // "keyReleaseEvent"
QT_MOC_LITERAL(9, 89, 9), // "formatHex"
QT_MOC_LITERAL(10, 99, 5), // "value"
QT_MOC_LITERAL(11, 105, 5), // "digit"
QT_MOC_LITERAL(12, 111, 11), // "waitForMSec"
QT_MOC_LITERAL(13, 123, 4), // "msec"
QT_MOC_LITERAL(14, 128, 10), // "panalOnOff"
QT_MOC_LITERAL(15, 139, 2), // "IO"
QT_MOC_LITERAL(16, 142, 8), // "showTime"
QT_MOC_LITERAL(17, 151, 14), // "allowSetNextSV"
QT_MOC_LITERAL(18, 166, 4), // "read"
QT_MOC_LITERAL(19, 171, 29), // "QModbusDataUnit::RegisterType"
QT_MOC_LITERAL(20, 201, 4), // "type"
QT_MOC_LITERAL(21, 206, 6), // "adress"
QT_MOC_LITERAL(22, 213, 4), // "size"
QT_MOC_LITERAL(23, 218, 9), // "readReady"
QT_MOC_LITERAL(24, 228, 7), // "request"
QT_MOC_LITERAL(25, 236, 24), // "QModbusPdu::FunctionCode"
QT_MOC_LITERAL(26, 261, 4), // "code"
QT_MOC_LITERAL(27, 266, 3), // "cmd"
QT_MOC_LITERAL(28, 270, 14), // "askTemperature"
QT_MOC_LITERAL(29, 285, 4), // "mute"
QT_MOC_LITERAL(30, 290, 11), // "askSetPoint"
QT_MOC_LITERAL(31, 302, 5), // "askMV"
QT_MOC_LITERAL(32, 308, 10), // "askMVupper"
QT_MOC_LITERAL(33, 319, 10), // "askMVlower"
QT_MOC_LITERAL(34, 330, 10), // "getSetting"
QT_MOC_LITERAL(35, 341, 5), // "setAT"
QT_MOC_LITERAL(36, 347, 6), // "atFlag"
QT_MOC_LITERAL(37, 354, 5), // "setSV"
QT_MOC_LITERAL(38, 360, 2), // "SV"
QT_MOC_LITERAL(39, 363, 9), // "writeData"
QT_MOC_LITERAL(40, 373, 11), // "waitTimming"
QT_MOC_LITERAL(41, 385, 16), // "generateSaveFile"
QT_MOC_LITERAL(42, 402, 8), // "isIgnore"
QT_MOC_LITERAL(43, 411, 5), // "check"
QT_MOC_LITERAL(44, 417, 4), // "temp"
QT_MOC_LITERAL(45, 422, 9), // "isViolate"
QT_MOC_LITERAL(46, 432, 15), // "QVector<double>"
QT_MOC_LITERAL(47, 448, 5), // "vtemp"
QT_MOC_LITERAL(48, 454, 6), // "isDrop"
QT_MOC_LITERAL(49, 461, 4), // "diff"
QT_MOC_LITERAL(50, 466, 4), // "mode"
QT_MOC_LITERAL(51, 471, 15), // "setTextTempDrop"
QT_MOC_LITERAL(52, 487, 29), // "on_pushButton_Connect_clicked"
QT_MOC_LITERAL(53, 517, 31), // "on_pushButton_AskStatus_clicked"
QT_MOC_LITERAL(54, 549, 27), // "on_pushButton_SetSV_clicked"
QT_MOC_LITERAL(55, 577, 28), // "on_pushButton_GetPID_clicked"
QT_MOC_LITERAL(56, 606, 29), // "on_pushButton_Control_clicked"
QT_MOC_LITERAL(57, 636, 34), // "on_comboBox_AT_currentIndexCh..."
QT_MOC_LITERAL(58, 671, 5), // "index"
QT_MOC_LITERAL(59, 677, 36), // "on_comboBox_Mode_currentIndex..."
QT_MOC_LITERAL(60, 714, 41), // "on_comboBox_MemAddress_curren..."
QT_MOC_LITERAL(61, 756, 4), // "arg1"
QT_MOC_LITERAL(62, 761, 37), // "on_doubleSpinBox_MVlower_valu..."
QT_MOC_LITERAL(63, 799, 37), // "on_doubleSpinBox_MVupper_valu..."
QT_MOC_LITERAL(64, 837, 28), // "on_checkBox_dataSave_toggled"
QT_MOC_LITERAL(65, 866, 7), // "checked"
QT_MOC_LITERAL(66, 874, 25), // "on_pushButton_Log_toggled"
QT_MOC_LITERAL(67, 900, 38), // "on_spinBox_TempRecordTime_val..."
QT_MOC_LITERAL(68, 939, 29), // "on_pushButton_RunStop_toggled"
QT_MOC_LITERAL(69, 969, 28), // "on_actionOpen_File_triggered"
QT_MOC_LITERAL(70, 998, 52), // "on_action_Setting_parameters_..."
QT_MOC_LITERAL(71, 1051, 32), // "on_action_Setting_plot_triggered"
QT_MOC_LITERAL(72, 1084, 28), // "on_actionHelp_Page_triggered"
QT_MOC_LITERAL(73, 1113, 11), // "HelpPicNext"
QT_MOC_LITERAL(74, 1125, 15), // "fillDataAndPlot"
QT_MOC_LITERAL(75, 1141, 4), // "date"
QT_MOC_LITERAL(76, 1146, 2), // "PV"
QT_MOC_LITERAL(77, 1149, 2), // "MV"
QT_MOC_LITERAL(78, 1152, 3), // "Run"
QT_MOC_LITERAL(79, 1156, 4), // "Stop"
QT_MOC_LITERAL(80, 1161, 4), // "Quit"
QT_MOC_LITERAL(81, 1166, 16), // "setIntervalAskMV"
QT_MOC_LITERAL(82, 1183, 18), // "setIntervalAskTemp"
QT_MOC_LITERAL(83, 1202, 12), // "setSafeLimit"
QT_MOC_LITERAL(84, 1215, 10), // "setNumbers"
QT_MOC_LITERAL(85, 1226, 14), // "setIgnoreRange"
QT_MOC_LITERAL(86, 1241, 22), // "setParametersTempCheck"
QT_MOC_LITERAL(87, 1264, 15), // "setIgnoreEnable"
QT_MOC_LITERAL(88, 1280, 14), // "fillDifference"
QT_MOC_LITERAL(89, 1295, 8), // "setColor"
QT_MOC_LITERAL(90, 1304, 10), // "colorindex"
QT_MOC_LITERAL(91, 1315, 13) // "calcMovingAve"

    },
    "MainWindow\0periodicWork\0\0makePlot\0"
    "TempCheck\0keyPressEvent\0QKeyEvent*\0"
    "key\0keyReleaseEvent\0formatHex\0value\0"
    "digit\0waitForMSec\0msec\0panalOnOff\0IO\0"
    "showTime\0allowSetNextSV\0read\0"
    "QModbusDataUnit::RegisterType\0type\0"
    "adress\0size\0readReady\0request\0"
    "QModbusPdu::FunctionCode\0code\0cmd\0"
    "askTemperature\0mute\0askSetPoint\0askMV\0"
    "askMVupper\0askMVlower\0getSetting\0setAT\0"
    "atFlag\0setSV\0SV\0writeData\0waitTimming\0"
    "generateSaveFile\0isIgnore\0check\0temp\0"
    "isViolate\0QVector<double>\0vtemp\0isDrop\0"
    "diff\0mode\0setTextTempDrop\0"
    "on_pushButton_Connect_clicked\0"
    "on_pushButton_AskStatus_clicked\0"
    "on_pushButton_SetSV_clicked\0"
    "on_pushButton_GetPID_clicked\0"
    "on_pushButton_Control_clicked\0"
    "on_comboBox_AT_currentIndexChanged\0"
    "index\0on_comboBox_Mode_currentIndexChanged\0"
    "on_comboBox_MemAddress_currentTextChanged\0"
    "arg1\0on_doubleSpinBox_MVlower_valueChanged\0"
    "on_doubleSpinBox_MVupper_valueChanged\0"
    "on_checkBox_dataSave_toggled\0checked\0"
    "on_pushButton_Log_toggled\0"
    "on_spinBox_TempRecordTime_valueChanged\0"
    "on_pushButton_RunStop_toggled\0"
    "on_actionOpen_File_triggered\0"
    "on_action_Setting_parameters_for_TempCheck_triggered\0"
    "on_action_Setting_plot_triggered\0"
    "on_actionHelp_Page_triggered\0HelpPicNext\0"
    "fillDataAndPlot\0date\0PV\0MV\0Run\0Stop\0"
    "Quit\0setIntervalAskMV\0setIntervalAskTemp\0"
    "setSafeLimit\0setNumbers\0setIgnoreRange\0"
    "setParametersTempCheck\0setIgnoreEnable\0"
    "fillDifference\0setColor\0colorindex\0"
    "calcMovingAve"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      69,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  359,    2, 0x0a /* Public */,
       3,    0,  360,    2, 0x0a /* Public */,
       4,    0,  361,    2, 0x0a /* Public */,
       5,    1,  362,    2, 0x08 /* Private */,
       8,    1,  365,    2, 0x08 /* Private */,
       9,    2,  368,    2, 0x08 /* Private */,
      12,    1,  373,    2, 0x08 /* Private */,
      14,    1,  376,    2, 0x08 /* Private */,
      16,    0,  379,    2, 0x08 /* Private */,
      17,    0,  380,    2, 0x08 /* Private */,
      18,    3,  381,    2, 0x08 /* Private */,
      23,    0,  388,    2, 0x08 /* Private */,
      24,    2,  389,    2, 0x08 /* Private */,
      28,    1,  394,    2, 0x08 /* Private */,
      28,    0,  397,    2, 0x28 /* Private | MethodCloned */,
      30,    1,  398,    2, 0x08 /* Private */,
      30,    0,  401,    2, 0x28 /* Private | MethodCloned */,
      31,    1,  402,    2, 0x08 /* Private */,
      31,    0,  405,    2, 0x28 /* Private | MethodCloned */,
      32,    1,  406,    2, 0x08 /* Private */,
      32,    0,  409,    2, 0x28 /* Private | MethodCloned */,
      33,    1,  410,    2, 0x08 /* Private */,
      33,    0,  413,    2, 0x28 /* Private | MethodCloned */,
      34,    0,  414,    2, 0x08 /* Private */,
      35,    1,  415,    2, 0x08 /* Private */,
      37,    1,  418,    2, 0x08 /* Private */,
      39,    0,  421,    2, 0x08 /* Private */,
      40,    0,  422,    2, 0x08 /* Private */,
      41,    0,  423,    2, 0x08 /* Private */,
      42,    2,  424,    2, 0x08 /* Private */,
      45,    1,  429,    2, 0x08 /* Private */,
      48,    2,  432,    2, 0x08 /* Private */,
      51,    1,  437,    2, 0x08 /* Private */,
      52,    0,  440,    2, 0x08 /* Private */,
      53,    0,  441,    2, 0x08 /* Private */,
      54,    0,  442,    2, 0x08 /* Private */,
      55,    0,  443,    2, 0x08 /* Private */,
      56,    0,  444,    2, 0x08 /* Private */,
      57,    1,  445,    2, 0x08 /* Private */,
      59,    1,  448,    2, 0x08 /* Private */,
      60,    1,  451,    2, 0x08 /* Private */,
      62,    1,  454,    2, 0x08 /* Private */,
      63,    1,  457,    2, 0x08 /* Private */,
      64,    1,  460,    2, 0x08 /* Private */,
      66,    1,  463,    2, 0x08 /* Private */,
      67,    1,  466,    2, 0x08 /* Private */,
      68,    1,  469,    2, 0x08 /* Private */,
      69,    0,  472,    2, 0x08 /* Private */,
      70,    0,  473,    2, 0x08 /* Private */,
      71,    0,  474,    2, 0x08 /* Private */,
      72,    0,  475,    2, 0x08 /* Private */,
      73,    0,  476,    2, 0x08 /* Private */,
      74,    4,  477,    2, 0x08 /* Private */,
      78,    0,  486,    2, 0x08 /* Private */,
      79,    0,  487,    2, 0x08 /* Private */,
      80,    0,  488,    2, 0x08 /* Private */,
      81,    0,  489,    2, 0x08 /* Private */,
      82,    0,  490,    2, 0x08 /* Private */,
      83,    0,  491,    2, 0x08 /* Private */,
      84,    0,  492,    2, 0x08 /* Private */,
      85,    0,  493,    2, 0x08 /* Private */,
      86,    1,  494,    2, 0x08 /* Private */,
      86,    0,  497,    2, 0x28 /* Private | MethodCloned */,
      87,    0,  498,    2, 0x08 /* Private */,
      88,    1,  499,    2, 0x08 /* Private */,
      88,    0,  502,    2, 0x28 /* Private | MethodCloned */,
      89,    1,  503,    2, 0x08 /* Private */,
      89,    0,  506,    2, 0x28 /* Private | MethodCloned */,
      91,    1,  507,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::QString, QMetaType::Int, QMetaType::Int,   10,   11,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 19, QMetaType::UShort, QMetaType::Int,   20,   21,   22,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 25, QMetaType::QByteArray,   26,   27,
    QMetaType::Void, QMetaType::Bool,   29,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   29,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   29,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   29,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   29,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   36,
    QMetaType::Void, QMetaType::Double,   38,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Bool, QMetaType::Bool, QMetaType::Double,   43,   44,
    QMetaType::Bool, 0x80000000 | 46,   47,
    QMetaType::Bool, QMetaType::Double, QMetaType::Int,   49,   50,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   58,
    QMetaType::Void, QMetaType::Int,   58,
    QMetaType::Void, QMetaType::QString,   61,
    QMetaType::Void, QMetaType::Double,   61,
    QMetaType::Void, QMetaType::Double,   61,
    QMetaType::Void, QMetaType::Bool,   65,
    QMetaType::Void, QMetaType::Bool,   65,
    QMetaType::Void, QMetaType::Int,   61,
    QMetaType::Void, QMetaType::Bool,   65,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QDateTime, QMetaType::Double, QMetaType::Double, QMetaType::Double,   75,   76,   38,   77,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   29,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Double, QMetaType::Bool,   29,
    QMetaType::Double,
    QMetaType::Void, QMetaType::Int,   90,
    QMetaType::Void,
    QMetaType::Double, 0x80000000 | 46,   47,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->periodicWork(); break;
        case 1: _t->makePlot(); break;
        case 2: _t->TempCheck(); break;
        case 3: _t->keyPressEvent((*reinterpret_cast< QKeyEvent*(*)>(_a[1]))); break;
        case 4: _t->keyReleaseEvent((*reinterpret_cast< QKeyEvent*(*)>(_a[1]))); break;
        case 5: { QString _r = _t->formatHex((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 6: _t->waitForMSec((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->panalOnOff((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->showTime(); break;
        case 9: _t->allowSetNextSV(); break;
        case 10: _t->read((*reinterpret_cast< QModbusDataUnit::RegisterType(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 11: _t->readReady(); break;
        case 12: _t->request((*reinterpret_cast< QModbusPdu::FunctionCode(*)>(_a[1])),(*reinterpret_cast< QByteArray(*)>(_a[2]))); break;
        case 13: _t->askTemperature((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: _t->askTemperature(); break;
        case 15: _t->askSetPoint((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 16: _t->askSetPoint(); break;
        case 17: _t->askMV((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: _t->askMV(); break;
        case 19: _t->askMVupper((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 20: _t->askMVupper(); break;
        case 21: _t->askMVlower((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 22: _t->askMVlower(); break;
        case 23: _t->getSetting(); break;
        case 24: _t->setAT((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 25: _t->setSV((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 26: _t->writeData(); break;
        case 27: _t->waitTimming(); break;
        case 28: { bool _r = _t->generateSaveFile();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 29: { bool _r = _t->isIgnore((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 30: { bool _r = _t->isViolate((*reinterpret_cast< QVector<double>(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 31: { bool _r = _t->isDrop((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 32: _t->setTextTempDrop((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 33: _t->on_pushButton_Connect_clicked(); break;
        case 34: _t->on_pushButton_AskStatus_clicked(); break;
        case 35: _t->on_pushButton_SetSV_clicked(); break;
        case 36: _t->on_pushButton_GetPID_clicked(); break;
        case 37: _t->on_pushButton_Control_clicked(); break;
        case 38: _t->on_comboBox_AT_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 39: _t->on_comboBox_Mode_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 40: _t->on_comboBox_MemAddress_currentTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 41: _t->on_doubleSpinBox_MVlower_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 42: _t->on_doubleSpinBox_MVupper_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 43: _t->on_checkBox_dataSave_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 44: _t->on_pushButton_Log_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 45: _t->on_spinBox_TempRecordTime_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 46: _t->on_pushButton_RunStop_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 47: _t->on_actionOpen_File_triggered(); break;
        case 48: _t->on_action_Setting_parameters_for_TempCheck_triggered(); break;
        case 49: _t->on_action_Setting_plot_triggered(); break;
        case 50: _t->on_actionHelp_Page_triggered(); break;
        case 51: _t->HelpPicNext(); break;
        case 52: _t->fillDataAndPlot((*reinterpret_cast< const QDateTime(*)>(_a[1])),(*reinterpret_cast< const double(*)>(_a[2])),(*reinterpret_cast< const double(*)>(_a[3])),(*reinterpret_cast< const double(*)>(_a[4]))); break;
        case 53: _t->Run(); break;
        case 54: _t->Stop(); break;
        case 55: _t->Quit(); break;
        case 56: _t->setIntervalAskMV(); break;
        case 57: _t->setIntervalAskTemp(); break;
        case 58: _t->setSafeLimit(); break;
        case 59: _t->setNumbers(); break;
        case 60: _t->setIgnoreRange(); break;
        case 61: _t->setParametersTempCheck((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 62: _t->setParametersTempCheck(); break;
        case 63: _t->setIgnoreEnable(); break;
        case 64: { double _r = _t->fillDifference((*reinterpret_cast< bool(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = std::move(_r); }  break;
        case 65: { double _r = _t->fillDifference();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = std::move(_r); }  break;
        case 66: _t->setColor((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 67: _t->setColor(); break;
        case 68: { double _r = _t->calcMovingAve((*reinterpret_cast< QVector<double>(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QModbusDataUnit::RegisterType >(); break;
            }
            break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QModbusPdu::FunctionCode >(); break;
            }
            break;
        case 30:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<double> >(); break;
            }
            break;
        case 68:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<double> >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 69)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 69;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 69)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 69;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
