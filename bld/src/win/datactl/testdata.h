/**** DO NOT MODIFY THIS FILE BY HAND. CREATED BY PARSECTL ****/


struct {
    int            num_ctls;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_radio            d1;
            ctl_info      d2;
        } d3;
    } d0;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_combo            d1;
            ctl_info      d2;
        } d3;
    } d1;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_combo            d1;
            ctl_info      d2;
        } d3;
    } d2;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_info      d2;
        } d3;
    } d3;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_text            d1;
            ctl_info      d2;
        } d3;
    } d4;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_info      d2;
        } d3;
    } d5;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_info      d2;
        } d3;
    } d6;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_dcombo            d1;
            ctl_info      d2;
        } d3;
    } d7;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_rint            d1;
            ctl_info      d2;
        } d3;
    } d8;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_rfloat            d1;
            ctl_info      d2;
        } d3;
    } d9;
} CtlData = {
10,
{ CTL_RADIO, DLG_RADIO_START, FALSE,offsetof( dlg_data, radio ) , DLG_RADIO_END },
{ CTL_COMBO, DLG_LIST_1, FALSE,offsetof( dlg_data, list_1 ) , 0, STR_LIST_1_START, STR_LIST_1_END },
{ CTL_COMBO, DLG_LIST_2, FALSE,offsetof( dlg_data, list_2 ) , 1, STR_LIST_2_START, STR_LIST_2_END },
{ CTL_CHECK, DLG_CHECK, FALSE,offsetof( dlg_data, check ) },
{ CTL_TEXT, DLG_TEXT, FALSE,offsetof( dlg_data, text ) , 100 },
{ CTL_INT, DLG_NUMBER, FALSE,offsetof( dlg_data, number ) },
{ CTL_FLOAT, DLG_VALUE, FALSE,offsetof( dlg_data, value ) },
{ CTL_DCOMBO, DLG_LIST_3, FALSE,offsetof( dlg_data, list_3 ) , 1, GetDcomboData },
{ CTL_RINT, DLG_RANGE_NUM, FALSE,offsetof( dlg_data, range_num ) , 0, -1 },
{ CTL_RFLOAT, DLG_RANGE_VALUE, FALSE,offsetof( dlg_data, range_value ) , -5, 10 },
};

