/****************************************************************************
** Meta object code from reading C++ file 'eko.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "eko.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'eko.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CChangeFormatWindow_t {
    QByteArrayData data[4];
    char stringdata[54];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CChangeFormatWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CChangeFormatWindow_t qt_meta_stringdata_CChangeFormatWindow = {
    {
QT_MOC_LITERAL(0, 0, 19),
QT_MOC_LITERAL(1, 20, 26),
QT_MOC_LITERAL(2, 47, 0),
QT_MOC_LITERAL(3, 48, 4)
    },
    "CChangeFormatWindow\0format_currentIndexChanged\0"
    "\0text\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CChangeFormatWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x0a,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void CChangeFormatWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CChangeFormatWindow *_t = static_cast<CChangeFormatWindow *>(_o);
        switch (_id) {
        case 0: _t->format_currentIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject CChangeFormatWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CChangeFormatWindow.data,
      qt_meta_data_CChangeFormatWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *CChangeFormatWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CChangeFormatWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CChangeFormatWindow.stringdata))
        return static_cast<void*>(const_cast< CChangeFormatWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int CChangeFormatWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_CAboutWindow_t {
    QByteArrayData data[3];
    char stringdata[28];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CAboutWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CAboutWindow_t qt_meta_stringdata_CAboutWindow = {
    {
QT_MOC_LITERAL(0, 0, 12),
QT_MOC_LITERAL(1, 13, 12),
QT_MOC_LITERAL(2, 26, 0)
    },
    "CAboutWindow\0update_image\0\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CAboutWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x0a,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void CAboutWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CAboutWindow *_t = static_cast<CAboutWindow *>(_o);
        switch (_id) {
        case 0: _t->update_image(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject CAboutWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CAboutWindow.data,
      qt_meta_data_CAboutWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *CAboutWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CAboutWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CAboutWindow.stringdata))
        return static_cast<void*>(const_cast< CAboutWindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int CAboutWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_CEKO_t {
    QByteArrayData data[146];
    char stringdata[2529];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CEKO_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CEKO_t qt_meta_stringdata_CEKO = {
    {
QT_MOC_LITERAL(0, 0, 4),
QT_MOC_LITERAL(1, 5, 12),
QT_MOC_LITERAL(2, 18, 0),
QT_MOC_LITERAL(3, 19, 15),
QT_MOC_LITERAL(4, 35, 17),
QT_MOC_LITERAL(5, 53, 20),
QT_MOC_LITERAL(6, 74, 13),
QT_MOC_LITERAL(7, 88, 14),
QT_MOC_LITERAL(8, 103, 15),
QT_MOC_LITERAL(9, 119, 15),
QT_MOC_LITERAL(10, 135, 17),
QT_MOC_LITERAL(11, 153, 22),
QT_MOC_LITERAL(12, 176, 5),
QT_MOC_LITERAL(13, 182, 30),
QT_MOC_LITERAL(14, 213, 26),
QT_MOC_LITERAL(15, 240, 1),
QT_MOC_LITERAL(16, 242, 19),
QT_MOC_LITERAL(17, 262, 19),
QT_MOC_LITERAL(18, 282, 34),
QT_MOC_LITERAL(19, 317, 4),
QT_MOC_LITERAL(20, 322, 37),
QT_MOC_LITERAL(21, 360, 5),
QT_MOC_LITERAL(22, 366, 36),
QT_MOC_LITERAL(23, 403, 43),
QT_MOC_LITERAL(24, 447, 24),
QT_MOC_LITERAL(25, 472, 5),
QT_MOC_LITERAL(26, 478, 30),
QT_MOC_LITERAL(27, 509, 14),
QT_MOC_LITERAL(28, 524, 12),
QT_MOC_LITERAL(29, 537, 12),
QT_MOC_LITERAL(30, 550, 11),
QT_MOC_LITERAL(31, 562, 11),
QT_MOC_LITERAL(32, 574, 19),
QT_MOC_LITERAL(33, 594, 4),
QT_MOC_LITERAL(34, 599, 25),
QT_MOC_LITERAL(35, 625, 9),
QT_MOC_LITERAL(36, 635, 9),
QT_MOC_LITERAL(37, 645, 19),
QT_MOC_LITERAL(38, 665, 16),
QT_MOC_LITERAL(39, 682, 24),
QT_MOC_LITERAL(40, 707, 15),
QT_MOC_LITERAL(41, 723, 12),
QT_MOC_LITERAL(42, 736, 12),
QT_MOC_LITERAL(43, 749, 9),
QT_MOC_LITERAL(44, 759, 9),
QT_MOC_LITERAL(45, 769, 25),
QT_MOC_LITERAL(46, 795, 16),
QT_MOC_LITERAL(47, 812, 4),
QT_MOC_LITERAL(48, 817, 21),
QT_MOC_LITERAL(49, 839, 23),
QT_MOC_LITERAL(50, 863, 18),
QT_MOC_LITERAL(51, 882, 11),
QT_MOC_LITERAL(52, 894, 11),
QT_MOC_LITERAL(53, 906, 7),
QT_MOC_LITERAL(54, 914, 4),
QT_MOC_LITERAL(55, 919, 27),
QT_MOC_LITERAL(56, 947, 4),
QT_MOC_LITERAL(57, 952, 11),
QT_MOC_LITERAL(58, 964, 15),
QT_MOC_LITERAL(59, 980, 16),
QT_MOC_LITERAL(60, 997, 16),
QT_MOC_LITERAL(61, 1014, 17),
QT_MOC_LITERAL(62, 1032, 17),
QT_MOC_LITERAL(63, 1050, 18),
QT_MOC_LITERAL(64, 1069, 23),
QT_MOC_LITERAL(65, 1093, 4),
QT_MOC_LITERAL(66, 1098, 16),
QT_MOC_LITERAL(67, 1115, 4),
QT_MOC_LITERAL(68, 1120, 6),
QT_MOC_LITERAL(69, 1127, 5),
QT_MOC_LITERAL(70, 1133, 13),
QT_MOC_LITERAL(71, 1147, 7),
QT_MOC_LITERAL(72, 1155, 14),
QT_MOC_LITERAL(73, 1170, 18),
QT_MOC_LITERAL(74, 1189, 9),
QT_MOC_LITERAL(75, 1199, 8),
QT_MOC_LITERAL(76, 1208, 6),
QT_MOC_LITERAL(77, 1215, 7),
QT_MOC_LITERAL(78, 1223, 7),
QT_MOC_LITERAL(79, 1231, 11),
QT_MOC_LITERAL(80, 1243, 13),
QT_MOC_LITERAL(81, 1257, 7),
QT_MOC_LITERAL(82, 1265, 23),
QT_MOC_LITERAL(83, 1289, 17),
QT_MOC_LITERAL(84, 1307, 16),
QT_MOC_LITERAL(85, 1324, 4),
QT_MOC_LITERAL(86, 1329, 21),
QT_MOC_LITERAL(87, 1351, 15),
QT_MOC_LITERAL(88, 1367, 22),
QT_MOC_LITERAL(89, 1390, 22),
QT_MOC_LITERAL(90, 1413, 16),
QT_MOC_LITERAL(91, 1430, 15),
QT_MOC_LITERAL(92, 1446, 15),
QT_MOC_LITERAL(93, 1462, 11),
QT_MOC_LITERAL(94, 1474, 10),
QT_MOC_LITERAL(95, 1485, 11),
QT_MOC_LITERAL(96, 1497, 7),
QT_MOC_LITERAL(97, 1505, 21),
QT_MOC_LITERAL(98, 1527, 22),
QT_MOC_LITERAL(99, 1550, 10),
QT_MOC_LITERAL(100, 1561, 23),
QT_MOC_LITERAL(101, 1585, 19),
QT_MOC_LITERAL(102, 1605, 25),
QT_MOC_LITERAL(103, 1631, 21),
QT_MOC_LITERAL(104, 1653, 11),
QT_MOC_LITERAL(105, 1665, 16),
QT_MOC_LITERAL(106, 1682, 16),
QT_MOC_LITERAL(107, 1699, 14),
QT_MOC_LITERAL(108, 1714, 16),
QT_MOC_LITERAL(109, 1731, 18),
QT_MOC_LITERAL(110, 1750, 17),
QT_MOC_LITERAL(111, 1768, 16),
QT_MOC_LITERAL(112, 1785, 19),
QT_MOC_LITERAL(113, 1805, 13),
QT_MOC_LITERAL(114, 1819, 14),
QT_MOC_LITERAL(115, 1834, 19),
QT_MOC_LITERAL(116, 1854, 14),
QT_MOC_LITERAL(117, 1869, 15),
QT_MOC_LITERAL(118, 1885, 21),
QT_MOC_LITERAL(119, 1907, 15),
QT_MOC_LITERAL(120, 1923, 9),
QT_MOC_LITERAL(121, 1933, 13),
QT_MOC_LITERAL(122, 1947, 13),
QT_MOC_LITERAL(123, 1961, 13),
QT_MOC_LITERAL(124, 1975, 29),
QT_MOC_LITERAL(125, 2005, 25),
QT_MOC_LITERAL(126, 2031, 27),
QT_MOC_LITERAL(127, 2059, 18),
QT_MOC_LITERAL(128, 2078, 34),
QT_MOC_LITERAL(129, 2113, 18),
QT_MOC_LITERAL(130, 2132, 27),
QT_MOC_LITERAL(131, 2160, 43),
QT_MOC_LITERAL(132, 2204, 24),
QT_MOC_LITERAL(133, 2229, 24),
QT_MOC_LITERAL(134, 2254, 36),
QT_MOC_LITERAL(135, 2291, 7),
QT_MOC_LITERAL(136, 2299, 8),
QT_MOC_LITERAL(137, 2308, 25),
QT_MOC_LITERAL(138, 2334, 26),
QT_MOC_LITERAL(139, 2361, 30),
QT_MOC_LITERAL(140, 2392, 30),
QT_MOC_LITERAL(141, 2423, 42),
QT_MOC_LITERAL(142, 2466, 22),
QT_MOC_LITERAL(143, 2489, 1),
QT_MOC_LITERAL(144, 2491, 21),
QT_MOC_LITERAL(145, 2513, 14)
    },
    "CEKO\0leaving_tune\0\0file_import_raw\0"
    "fn_insert_silence\0fn_silence_selection\0"
    "generate_sine\0generate_noise\0"
    "nav_play_looped\0view_show_mixer\0"
    "help_system_check\0cb_play_looped_changed\0"
    "value\0cb_show_meterbar_in_db_changed\0"
    "spb_max_undos_valueChanged\0i\0"
    "slot_transport_play\0slot_transport_stop\0"
    "cmb_icon_sizes_currentIndexChanged\0"
    "text\0cmb_sound_dev_out_currentIndexChanged\0"
    "index\0cmb_sound_dev_in_currentIndexChanged\0"
    "cmb_mono_recording_mode_currentIndexChanged\0"
    "cb_monitor_input_changed\0state\0"
    "cmb_panner_currentIndexChanged\0"
    "stop_recording\0fm_full_info\0fman_refresh\0"
    "fman_rename\0fman_delete\0fman_drives_changed\0"
    "path\0fman_current_file_changed\0full_path\0"
    "just_name\0fman_file_activated\0"
    "fman_dir_changed\0fman_fname_entry_confirm\0"
    "fman_create_dir\0fman_add_bmk\0fman_del_bmk\0"
    "fman_open\0fman_home\0fman_places_itemActivated\0"
    "QListWidgetItem*\0item\0fman_select_by_regexp\0"
    "fman_deselect_by_regexp\0cb_button_saves_as\0"
    "pageChanged\0file_record\0newFile\0open\0"
    "fman_items_select_by_regexp\0mode\0"
    "file_reload\0file_export_mp3\0"
    "file_last_opened\0file_use_palette\0"
    "file_open_session\0file_save_version\0"
    "file_change_format\0cb_altmenu_stateChanged\0"
    "test\0view_use_profile\0save\0saveAs\0"
    "about\0close_current\0ed_copy\0ed_copy_to_new\0"
    "ed_copy_to_new_fmt\0ed_delete\0ed_paste\0"
    "ed_cut\0ed_undo\0ed_redo\0ed_deselect\0"
    "ed_select_all\0ed_trim\0edit_copy_current_fname\0"
    "fn_stereo_to_mono\0fun_51_to_stereo\0"
    "algo\0fn_51_to_stereo_dlike\0fn_51_to_stereo\0"
    "fn_mono_to_stereo_half\0fn_mono_to_stereo_full\0"
    "fn_swap_channels\0fn_copy_channel\0"
    "fn_mute_channel\0fn_fade_out\0fn_fade_in\0"
    "fn_stat_rms\0fn_norm\0fn_apply_vol_envelope\0"
    "fn_delete_vol_envelope\0fn_reverse\0"
    "fn_detect_average_value\0fn_dc_offset_detect\0"
    "fn_dc_offset_fix_manually\0"
    "fn_dc_offset_fix_auto\0search_find\0"
    "search_find_next\0search_find_prev\0"
    "view_toggle_fs\0view_stay_on_top\0"
    "nav_goto_right_tab\0nav_goto_left_tab\0"
    "nav_focus_to_fif\0nav_focus_to_editor\0"
    "help_show_gpl\0help_show_news\0"
    "help_show_changelog\0help_show_todo\0"
    "session_save_as\0main_tab_page_changed\0"
    "profile_save_as\0file_info\0man_find_find\0"
    "man_find_next\0man_find_prev\0"
    "spb_def_channels_valueChanged\0"
    "bt_set_def_format_clicked\0"
    "pb_choose_temp_path_clicked\0"
    "cb_locale_override\0"
    "ed_locale_override_editingFinished\0"
    "cb_session_restore\0cb_use_trad_dialogs_changed\0"
    "cmb_proxy_video_decoder_currentIndexChanged\0"
    "pb_assign_hotkey_clicked\0"
    "pb_remove_hotkey_clicked\0"
    "slot_lv_menuitems_currentItemChanged\0"
    "current\0previous\0slot_app_fontname_changed\0"
    "slot_app_font_size_changed\0"
    "slot_style_currentIndexChanged\0"
    "slot_sl_icons_size_sliderMoved\0"
    "cmb_buffer_size_frames_currentIndexChanged\0"
    "spb_ogg_q_valueChanged\0d\0fman_naventry_confirm\0"
    "view_use_theme\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CEKO[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
     129,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  659,    2, 0x08,
       3,    0,  660,    2, 0x08,
       4,    0,  661,    2, 0x08,
       5,    0,  662,    2, 0x08,
       6,    0,  663,    2, 0x08,
       7,    0,  664,    2, 0x08,
       8,    0,  665,    2, 0x08,
       9,    0,  666,    2, 0x08,
      10,    0,  667,    2, 0x08,
      11,    1,  668,    2, 0x08,
      13,    1,  671,    2, 0x08,
      14,    1,  674,    2, 0x08,
      16,    0,  677,    2, 0x08,
      17,    0,  678,    2, 0x08,
      18,    1,  679,    2, 0x08,
      20,    1,  682,    2, 0x08,
      22,    1,  685,    2, 0x08,
      23,    1,  688,    2, 0x08,
      24,    1,  691,    2, 0x08,
      26,    1,  694,    2, 0x08,
      27,    0,  697,    2, 0x08,
      28,    0,  698,    2, 0x08,
      29,    0,  699,    2, 0x08,
      30,    0,  700,    2, 0x08,
      31,    0,  701,    2, 0x08,
      32,    1,  702,    2, 0x08,
      34,    2,  705,    2, 0x08,
      37,    1,  710,    2, 0x08,
      38,    1,  713,    2, 0x08,
      39,    0,  716,    2, 0x08,
      40,    0,  717,    2, 0x08,
      41,    0,  718,    2, 0x08,
      42,    0,  719,    2, 0x08,
      43,    0,  720,    2, 0x08,
      44,    0,  721,    2, 0x08,
      45,    1,  722,    2, 0x08,
      48,    0,  725,    2, 0x08,
      49,    0,  726,    2, 0x08,
      50,    0,  727,    2, 0x08,
      51,    1,  728,    2, 0x08,
      52,    0,  731,    2, 0x08,
      53,    0,  732,    2, 0x08,
      54,    0,  733,    2, 0x08,
      55,    1,  734,    2, 0x08,
      57,    0,  737,    2, 0x08,
      58,    0,  738,    2, 0x08,
      59,    0,  739,    2, 0x08,
      60,    0,  740,    2, 0x08,
      61,    0,  741,    2, 0x08,
      62,    0,  742,    2, 0x08,
      63,    0,  743,    2, 0x08,
      64,    1,  744,    2, 0x08,
      65,    0,  747,    2, 0x08,
      66,    0,  748,    2, 0x08,
      67,    0,  749,    2, 0x08,
      68,    0,  750,    2, 0x08,
      69,    0,  751,    2, 0x08,
      70,    0,  752,    2, 0x08,
      71,    0,  753,    2, 0x08,
      72,    0,  754,    2, 0x08,
      73,    0,  755,    2, 0x08,
      74,    0,  756,    2, 0x08,
      75,    0,  757,    2, 0x08,
      76,    0,  758,    2, 0x08,
      77,    0,  759,    2, 0x08,
      78,    0,  760,    2, 0x08,
      79,    0,  761,    2, 0x08,
      80,    0,  762,    2, 0x08,
      81,    0,  763,    2, 0x08,
      82,    0,  764,    2, 0x08,
      83,    0,  765,    2, 0x08,
      84,    1,  766,    2, 0x08,
      86,    0,  769,    2, 0x08,
      87,    0,  770,    2, 0x08,
      88,    0,  771,    2, 0x08,
      89,    0,  772,    2, 0x08,
      90,    0,  773,    2, 0x08,
      91,    0,  774,    2, 0x08,
      92,    0,  775,    2, 0x08,
      93,    0,  776,    2, 0x08,
      94,    0,  777,    2, 0x08,
      95,    0,  778,    2, 0x08,
      96,    0,  779,    2, 0x08,
      97,    0,  780,    2, 0x08,
      98,    0,  781,    2, 0x08,
      99,    0,  782,    2, 0x08,
     100,    0,  783,    2, 0x08,
     101,    0,  784,    2, 0x08,
     102,    0,  785,    2, 0x08,
     103,    0,  786,    2, 0x08,
     104,    0,  787,    2, 0x08,
     105,    0,  788,    2, 0x08,
     106,    0,  789,    2, 0x08,
     107,    0,  790,    2, 0x08,
     108,    0,  791,    2, 0x08,
     109,    0,  792,    2, 0x08,
     110,    0,  793,    2, 0x08,
     111,    0,  794,    2, 0x08,
     112,    0,  795,    2, 0x08,
     113,    0,  796,    2, 0x08,
     114,    0,  797,    2, 0x08,
     115,    0,  798,    2, 0x08,
     116,    0,  799,    2, 0x08,
     117,    0,  800,    2, 0x08,
     118,    1,  801,    2, 0x08,
     119,    0,  804,    2, 0x08,
     120,    0,  805,    2, 0x08,
     121,    0,  806,    2, 0x08,
     122,    0,  807,    2, 0x08,
     123,    0,  808,    2, 0x08,
     124,    1,  809,    2, 0x08,
     125,    0,  812,    2, 0x08,
     126,    0,  813,    2, 0x08,
     127,    1,  814,    2, 0x08,
     128,    0,  817,    2, 0x08,
     129,    1,  818,    2, 0x08,
     130,    1,  821,    2, 0x08,
     131,    1,  824,    2, 0x08,
     132,    0,  827,    2, 0x08,
     133,    0,  828,    2, 0x08,
     134,    2,  829,    2, 0x08,
     137,    1,  834,    2, 0x08,
     138,    1,  837,    2, 0x08,
     139,    1,  840,    2, 0x08,
     140,    1,  843,    2, 0x08,
     141,    1,  846,    2, 0x08,
     142,    1,  849,    2, 0x08,
     144,    0,  852,    2, 0x08,
     145,    0,  853,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   19,
    QMetaType::Void, QMetaType::Int,   21,
    QMetaType::Void, QMetaType::Int,   21,
    QMetaType::Void, QMetaType::Int,   21,
    QMetaType::Void, QMetaType::Int,   25,
    QMetaType::Void, QMetaType::Int,   21,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   33,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   35,   36,
    QMetaType::Void, QMetaType::QString,   35,
    QMetaType::Void, QMetaType::QString,   35,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 46,   47,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   21,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   56,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   25,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   85,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   21,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   25,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   25,
    QMetaType::Void, QMetaType::Int,   25,
    QMetaType::Void, QMetaType::Int,   21,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 46, 0x80000000 | 46,  135,  136,
    QMetaType::Void, QMetaType::QString,   19,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void, QMetaType::QString,   19,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::QString,   19,
    QMetaType::Void, QMetaType::Double,  143,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CEKO::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CEKO *_t = static_cast<CEKO *>(_o);
        switch (_id) {
        case 0: _t->leaving_tune(); break;
        case 1: _t->file_import_raw(); break;
        case 2: _t->fn_insert_silence(); break;
        case 3: _t->fn_silence_selection(); break;
        case 4: _t->generate_sine(); break;
        case 5: _t->generate_noise(); break;
        case 6: _t->nav_play_looped(); break;
        case 7: _t->view_show_mixer(); break;
        case 8: _t->help_system_check(); break;
        case 9: _t->cb_play_looped_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->cb_show_meterbar_in_db_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->spb_max_undos_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->slot_transport_play(); break;
        case 13: _t->slot_transport_stop(); break;
        case 14: _t->cmb_icon_sizes_currentIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: _t->cmb_sound_dev_out_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->cmb_sound_dev_in_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->cmb_mono_recording_mode_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->cb_monitor_input_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->cmb_panner_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->stop_recording(); break;
        case 21: _t->fm_full_info(); break;
        case 22: _t->fman_refresh(); break;
        case 23: _t->fman_rename(); break;
        case 24: _t->fman_delete(); break;
        case 25: _t->fman_drives_changed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 26: _t->fman_current_file_changed((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 27: _t->fman_file_activated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 28: _t->fman_dir_changed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 29: _t->fman_fname_entry_confirm(); break;
        case 30: _t->fman_create_dir(); break;
        case 31: _t->fman_add_bmk(); break;
        case 32: _t->fman_del_bmk(); break;
        case 33: _t->fman_open(); break;
        case 34: _t->fman_home(); break;
        case 35: _t->fman_places_itemActivated((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 36: _t->fman_select_by_regexp(); break;
        case 37: _t->fman_deselect_by_regexp(); break;
        case 38: _t->cb_button_saves_as(); break;
        case 39: _t->pageChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 40: _t->file_record(); break;
        case 41: _t->newFile(); break;
        case 42: _t->open(); break;
        case 43: _t->fman_items_select_by_regexp((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 44: _t->file_reload(); break;
        case 45: _t->file_export_mp3(); break;
        case 46: _t->file_last_opened(); break;
        case 47: _t->file_use_palette(); break;
        case 48: _t->file_open_session(); break;
        case 49: _t->file_save_version(); break;
        case 50: _t->file_change_format(); break;
        case 51: _t->cb_altmenu_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 52: _t->test(); break;
        case 53: _t->view_use_profile(); break;
        case 54: { bool _r = _t->save();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 55: { bool _r = _t->saveAs();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 56: _t->about(); break;
        case 57: _t->close_current(); break;
        case 58: _t->ed_copy(); break;
        case 59: _t->ed_copy_to_new(); break;
        case 60: _t->ed_copy_to_new_fmt(); break;
        case 61: _t->ed_delete(); break;
        case 62: _t->ed_paste(); break;
        case 63: _t->ed_cut(); break;
        case 64: _t->ed_undo(); break;
        case 65: _t->ed_redo(); break;
        case 66: _t->ed_deselect(); break;
        case 67: _t->ed_select_all(); break;
        case 68: _t->ed_trim(); break;
        case 69: _t->edit_copy_current_fname(); break;
        case 70: _t->fn_stereo_to_mono(); break;
        case 71: _t->fun_51_to_stereo((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 72: _t->fn_51_to_stereo_dlike(); break;
        case 73: _t->fn_51_to_stereo(); break;
        case 74: _t->fn_mono_to_stereo_half(); break;
        case 75: _t->fn_mono_to_stereo_full(); break;
        case 76: _t->fn_swap_channels(); break;
        case 77: _t->fn_copy_channel(); break;
        case 78: _t->fn_mute_channel(); break;
        case 79: _t->fn_fade_out(); break;
        case 80: _t->fn_fade_in(); break;
        case 81: _t->fn_stat_rms(); break;
        case 82: _t->fn_norm(); break;
        case 83: _t->fn_apply_vol_envelope(); break;
        case 84: _t->fn_delete_vol_envelope(); break;
        case 85: _t->fn_reverse(); break;
        case 86: _t->fn_detect_average_value(); break;
        case 87: _t->fn_dc_offset_detect(); break;
        case 88: _t->fn_dc_offset_fix_manually(); break;
        case 89: _t->fn_dc_offset_fix_auto(); break;
        case 90: _t->search_find(); break;
        case 91: _t->search_find_next(); break;
        case 92: _t->search_find_prev(); break;
        case 93: _t->view_toggle_fs(); break;
        case 94: _t->view_stay_on_top(); break;
        case 95: _t->nav_goto_right_tab(); break;
        case 96: _t->nav_goto_left_tab(); break;
        case 97: _t->nav_focus_to_fif(); break;
        case 98: _t->nav_focus_to_editor(); break;
        case 99: _t->help_show_gpl(); break;
        case 100: _t->help_show_news(); break;
        case 101: _t->help_show_changelog(); break;
        case 102: _t->help_show_todo(); break;
        case 103: _t->session_save_as(); break;
        case 104: _t->main_tab_page_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 105: _t->profile_save_as(); break;
        case 106: _t->file_info(); break;
        case 107: _t->man_find_find(); break;
        case 108: _t->man_find_next(); break;
        case 109: _t->man_find_prev(); break;
        case 110: _t->spb_def_channels_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 111: _t->bt_set_def_format_clicked(); break;
        case 112: _t->pb_choose_temp_path_clicked(); break;
        case 113: _t->cb_locale_override((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 114: _t->ed_locale_override_editingFinished(); break;
        case 115: _t->cb_session_restore((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 116: _t->cb_use_trad_dialogs_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 117: _t->cmb_proxy_video_decoder_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 118: _t->pb_assign_hotkey_clicked(); break;
        case 119: _t->pb_remove_hotkey_clicked(); break;
        case 120: _t->slot_lv_menuitems_currentItemChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 121: _t->slot_app_fontname_changed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 122: _t->slot_app_font_size_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 123: _t->slot_style_currentIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 124: _t->slot_sl_icons_size_sliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 125: _t->cmb_buffer_size_frames_currentIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 126: _t->spb_ogg_q_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 127: _t->fman_naventry_confirm(); break;
        case 128: _t->view_use_theme(); break;
        default: ;
        }
    }
}

const QMetaObject CEKO::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_CEKO.data,
      qt_meta_data_CEKO,  qt_static_metacall, 0, 0}
};


const QMetaObject *CEKO::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CEKO::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CEKO.stringdata))
        return static_cast<void*>(const_cast< CEKO*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int CEKO::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 129)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 129;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 129)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 129;
    }
    return _id;
}
struct qt_meta_stringdata_CApplication_t {
    QByteArrayData data[1];
    char stringdata[14];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CApplication_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CApplication_t qt_meta_stringdata_CApplication = {
    {
QT_MOC_LITERAL(0, 0, 12)
    },
    "CApplication\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CApplication[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void CApplication::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject CApplication::staticMetaObject = {
    { &QApplication::staticMetaObject, qt_meta_stringdata_CApplication.data,
      qt_meta_data_CApplication,  qt_static_metacall, 0, 0}
};


const QMetaObject *CApplication::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CApplication::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CApplication.stringdata))
        return static_cast<void*>(const_cast< CApplication*>(this));
    return QApplication::qt_metacast(_clname);
}

int CApplication::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QApplication::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
