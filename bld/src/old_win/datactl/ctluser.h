#ifndef CTLUSER_H
#define CTLUSER_H
/*  Define the control IDs of your field types here. Define an ENUM
    list as follows:
        enum {
            CTL_YOUR_FIELD_1            = CTL_USER,
            CTL_YOUR_FIELD_2,
            ...
        };
*/

/*  Add your field data structures to this union.
    **** NOTE: make the enum names above and the ctl_... typedef names below
               the same, except in upper case. This is necessary since
               PARSECTL.EXE assumes this when building the data control
               file */
typedef union {
    ctl_radio           radio;
    ctl_text            text;
    ctl_combo           combo;
    ctl_dcombo          dcombo;
    ctl_rint            rint;
    ctl_rfloat          rfloat;
} ctl_info;


#endif

