#ifndef CTLTYPE_H
#define CTLTYPE_H
/****************************************************************
    If you have field types that you want to add, then
    do the following:

        1) set the CTL_WANT_USER macro BEFORE including
           ctltype.h.

        2) make your own version of ctluser.h. See the default
           file to determine the necessary changes. 'ctluser.h'
           is included by ctltype.h when the CTL_WANT_USER
           macro is set.

        3) make your own version of ctluser.c. See the default
           file to determine the necessary changes.

        4) Define the process routines (see ctluser.c) for each
           field type. The module that defines these routines
           needs to include <windows.h> and "ctltype.h".

        5) link in your own ctluser.c (the library version will
           not be used).

    If you don't want to add your own field types, but want
    to get rid of the support for some of the default
    field types, then do the following:

        1) get your own copy of ctluser.c. Modify the file according
        to the comments at the top of the file.

        2) link in your own ctluser.c (the library version will
           not be used).

***************************************************************/
#define CTL_USER        50      // first user field #

typedef enum {
    CTL_CHECK,          // standard check field
    CTL_RADIO,          // standard set of independent radio buttons
    CTL_TEXT,           // basic text edit field (string)
    CTL_COMBO,          // combo box (drop down optional)
    CTL_DCOMBO,         // dynamic combo box (values can change at runtime)
    CTL_INT,            // basic integer field (with type checking)
    CTL_FLOAT,          // basic float field (with type checking)
    CTL_RINT,           // int field with range (with error checking)
    CTL_RFLOAT          // float field with range (with error checking)
} ctl_type;

BOOL ctl_dlg_init( HANDLE, HWND dlg, void *ptr, void *ctl_ptr);
BOOL ctl_dlg_done( HANDLE, HWND dlg, void *ptr, void *ctl_ptr);
void ctl_dlg_process( void *ctl_ptr, WORD wParam, LONG lParam );


/* RADIO BUTTON: - 'data_offset' points to int: 1 origin (0 means none).
                 - 'control' is first control in group */
typedef struct {
    int                 end_control;    // end control of radio button group
} ctl_radio;

/* EDIT TEXT: - 'data_offset' points to text buffer */
typedef struct {
    int                 text_size;      // size of text buffer
} ctl_text;

/* COMBO BOX: - 'data_offset' points to int. The 'origin' below defines
                the origin of the int (-128 to 127) */
typedef struct {
    signed char         origin;         // special meaning: see above
    WORD                start_id;       // string tbl ID of start of list
    WORD                end_id;         // string tbl ID of end of list
} ctl_combo;

/* DYNAMIC COMBO BOX: - size and list data can change at runtime. 'fetch'
                        routine called to get data ('elt' is ZERO origin).
                        set 'done' to TRUE when 'one past end' elt is
                        asked for */
typedef struct {
    signed              char    origin;         // special meaning: see above
    char                *(*fetch)( int elt );
} ctl_dcombo;

/* RANGE INT: 'data_offset' points to int. */
typedef struct {
    int                 min;
    int                 max;            // max < min means no max
} ctl_rint;

/* RANGE FLOAT: 'data_offset' points to float */
typedef struct {
    float               min;
    float               max;            // max < min means no max
} ctl_rfloat;

#ifdef CTL_WANT_USER
#include "ctlinfo.h"            // ctl_info defined here
#else
typedef union {
    ctl_radio           radio;
    ctl_text            text;
    ctl_combo           combo;
    ctl_dcombo          dcombo;
    ctl_rint            rint;
    ctl_rfloat          rfloat;
} ctl_info;
#endif

typedef struct {
    ctl_type            type;
    int                 control;
    BOOL                modified;
    unsigned int        data_offset;
    ctl_info            info;
} ctl_elt;

typedef struct {
    int                 num_ctls;
    ctl_elt             elts[1];        // 'num_ctls' elts
} clt_def;

typedef struct {
    BOOL                (*setup)( ctl_elt *, HANDLE, HWND, void * );
    BOOL                (*finish)( ctl_elt *, HANDLE, HWND, void * );
    BOOL                (*modified)( ctl_elt *, int, LONG );
} ctl_action;

#define _value_bool( ptr, elt ) *((BOOL*)((char *)(ptr) + elt->data_offset))
#define _value_int( ptr, elt ) *((int*)((char *)(ptr) + elt->data_offset))
#define _value_float( ptr, elt ) *((float*)((char *)(ptr) + elt->data_offset))
#define _str_ptr( ptr, elt ) (char *)((char *)(ptr) + elt->data_offset)

#endif
