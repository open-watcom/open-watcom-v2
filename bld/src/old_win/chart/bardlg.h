/**** DO NOT MODIFY THIS FILE BY HAND. CREATED BY PARSECTL ****/


struct {
    int            num_ctls;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_rfloat            d1;
            ctl_info      d2;
        } d3;
    } d0;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_rint            d1;
            ctl_info      d2;
        } d3;
    } d1;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_rint            d1;
            ctl_info      d2;
        } d3;
    } d2;
    struct {
        ctl_type      type;
        int           control;
        BOOL          modified;
        unsigned int  data_offset;
        union {
            ctl_rint            d1;
            ctl_info      d2;
        } d3;
    } d3;
} DlgData = {
4,
{ CTL_RFLOAT, DLG_VALUE, FALSE,offsetof( dlg_data, value ) , 0, -1 },
{ CTL_RINT, DLG_RED, FALSE,offsetof( dlg_data, red ) , 0, 255 },
{ CTL_RINT, DLG_GREEN, FALSE,offsetof( dlg_data, green ) , 0, 255 },
{ CTL_RINT, DLG_BLUE, FALSE,offsetof( dlg_data, blue ) , 0, 255 },
};

