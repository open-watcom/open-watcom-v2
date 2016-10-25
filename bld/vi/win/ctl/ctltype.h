/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  This file defines the data control library.
*               Note: support for user controls has been removed in
*                     the hacked re-implementation of ctl.lib.
*
****************************************************************************/

// Everything unknown is marked with MISSING!!!

#ifndef CTLTYPE_H
#define CTLTYPE_H

#include <wpitypes.h>

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

/////////////////////////////////////////////////////////////////////////////////
// Control element type

typedef enum {
    CTL_CHECK = 1,      // standard check field
    CTL_RADIO,          // standard set of independent radio buttons
    CTL_TEXT,           // basic text edit field (string)
    CTL_COMBO,          // combo box (drop down optional)
    CTL_DCOMBO,         // dynamic combo box (values can change at runtime)
    CTL_INT,            // basic integer field (with type checking)
    CTL_FLOAT,          // basic float field (with type checking)
    CTL_RINT,           // int field with range (with error checking)
    CTL_RFLOAT,         // float field with range (with error checking)
    CTL_ESCOMBO,        // editable string combo box (drop down) (implementation MISSING!!!)
    CTL_DHCOMBO         // dynamic combo box, with HWND parm (implementation MISSING!!!)
};
typedef signed char ctl_type;

///////////////////////////////////////////////////////////////////////////////
// The ctl_dlg_ functions

// Initialize a dialog by the control
bool ctl_dlg_init( WPI_INST, HWND dlg, void *ptr, void *ctl_ptr );

// Reset a dialog by the control
bool ctl_dlg_reset( WPI_INST, HWND dlg, void *ptr, void *ctl_ptr, bool );

// (MISSING!!!)
bool ctl_dlg_init_no_proc( WPI_INST, HWND dlg, void *ptr, void *ctl_ptr );

// (MISSING!!!)
bool ctl_dlg_reset_no_proc( WPI_INST, HWND dlg, void *ptr, void *ctl_ptr, bool );

// Get the data ...
bool ctl_dlg_done( WPI_INST, HWND dlg, void *ptr, void *ctl_ptr );

// (MISSING!!!)
bool ctl_dlg_validate( WPI_INST, HWND dlg, void *ptr, void *ctl_ptr, bool );

// (MISSING!!!)
bool ctl_dlg_check( WPI_INST, HWND dlg, void * ptr, void * ctl_ptr );

// (MISSING!!!)
void ctl_dlg_process( void *ctl_ptr, WPI_PARAM1 wparam, WPI_PARAM2 lparam );

// (MISSING!!!)
bool ctl_dlg_enable( void *ctl_ptr, int ctl_id );

// (MISSING!!!)
bool ctl_dlg_disable( void *ctl_ptr, int ctl_id );

// (MISSING!!!)
void ctl_dlg_free( void *ctl_ptr );

// (MISSING!!!)
void * ctl_dlg_copy( void *ctl_ptr );

// (MISSING!!!)
bool ctl_dlg_modified( void *ctl_ptr );

///////////////////////////////////////////////////////////////////////////////
// The specific control element's data structs

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
                        set 'done' to true when 'one past end' elt is
                        asked for */
typedef struct {
    signed char         origin;         // special meaning: see above
    char                *(*fetch)( int elt );
} ctl_dcombo;

typedef struct {
    signed char         origin;         // special meaning: see above
    char                *(*fetch)( HWND dlg, int elt );
} ctl_dhcombo;

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

typedef struct {
    char                *(*fetch)( int elt );
    void                (*set)( char **str, char *value );
} ctl_escombo;

#define FULL_SEL MAKELONG( 0, 32767 )   // useful for EM_SETSEL in validation

#ifdef CTL_WANT_USER
#include <ctluser.h>            // ctl_info defined here
#else
typedef union {
    ctl_radio           radio;
    ctl_text            text;
    ctl_combo           combo;
    ctl_dcombo          dcombo;
    ctl_rint            rint;
    ctl_rfloat          rfloat;
    ctl_escombo         escombo;
    ctl_dhcombo         dhcombo;
} ctl_info;
#endif

typedef struct ctl_elt {
    ctl_type            type;
    int                 control;
    bool                modified;
    void                (* get)( void *ptr, struct ctl_elt *elt, void *data );
    void                (* set)( void *ptr, struct ctl_elt *elt, void *data );
    ctl_info            info;
} ctl_elt;

// The control definition
typedef struct {
    int                 num_ctls;
    ctl_elt             elts[1];        // 'num_ctls' elts
                        // The array of control datas is stored in a continuous
                        // block of memory.
} ctl_def;


/////////////////////////////////////////////////////////////////////////////////
// Definitions of the action table (only needed for the implementation)

// no idea where this is needed (MISSING!!!)
enum {
    FINISH_DONE = 0,
    FINISH_VALIDATE,
    FINISH_CHECK
};
typedef int finish_type;

typedef struct {
    bool                (*setup)( ctl_elt *, WPI_INST, HWND, void *, bool );
    bool                (*finish)( ctl_elt *, WPI_INST, HWND, void *, finish_type );
    bool                (*modified)( ctl_elt *, WPI_PARAM1, WPI_PARAM2 );
} ctl_action;

#endif
