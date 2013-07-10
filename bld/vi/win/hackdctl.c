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
* Description:  The implementation of the data control library and the
*               dynamic template library for VIW.
*
****************************************************************************/

#include "vi.h"
#include <stdio.h>
#include <string.h>
#include "ctltype.h"
#include "dyntpl.h"

///////////////////////////////////////////////////////////////////////////
// Missing information is marked by MISSING!!!
//
// Original files came from watcom11\samples\win\datactl\*.C
// (heavily modified and tweaked to fit for VIW)
// Modifications made by Felix Fontein (felix@amuq.net).

///////////////////////////////////////////////////////////////////////////
// The forward declarations of the control element handlers

// The *_start functions will be called on dialog initialization
// and reset, and they should initialize the state of the control
// elements based on the data in the dialog data struct.
//   first parameter: pointer to the control element data
//   second parameter: the program's instance handle
//   third parameter: the dialog's window handle
//   fourth parameter: pointer to dialog data struct
//   fifth parameter: boolean (no idea what it is for) (MISSING!!!)
static BOOL ctl_check_start( ctl_elt *, WPI_INST, HWND, void *, BOOL );
static BOOL ctl_radio_start( ctl_elt *, WPI_INST, HWND, void *, BOOL );
static BOOL ctl_text_start( ctl_elt *, WPI_INST, HWND, void *, BOOL );
static BOOL ctl_combo_start( ctl_elt *, WPI_INST, HWND, void *, BOOL );
static BOOL ctl_dcombo_start( ctl_elt *, WPI_INST, HWND, void *, BOOL );
static BOOL ctl_int_start( ctl_elt *, WPI_INST, HWND, void *, BOOL );
static BOOL ctl_float_start( ctl_elt *, WPI_INST, HWND, void *, BOOL );
static BOOL ctl_rint_start( ctl_elt *, WPI_INST, HWND, void *, BOOL );
static BOOL ctl_rfloat_start( ctl_elt *, WPI_INST, HWND, void *, BOOL );

// The *_finish functions will be called to retain the control element
// data into the dialog data struct.
//   first parameter: pointer to the control element data
//   second parameter: the program's instance handle
//   third parameter: the dialog's window handle
//   fourth parameter: pointer to dialog data struct
//   fifth parameter: finish_type (no idea what it is for) (MISSING!!!)
static BOOL ctl_check_finish( ctl_elt *, WPI_INST, HWND, void *, finish_type );
static BOOL ctl_radio_finish( ctl_elt *, WPI_INST, HWND, void *, finish_type );
static BOOL ctl_text_finish( ctl_elt *, WPI_INST, HWND, void *, finish_type );
static BOOL ctl_combo_finish( ctl_elt *, WPI_INST, HWND, void *, finish_type );
static BOOL ctl_dcombo_finish( ctl_elt *, WPI_INST, HWND, void *, finish_type );
static BOOL ctl_int_finish( ctl_elt *, WPI_INST, HWND, void *, finish_type );
static BOOL ctl_float_finish( ctl_elt *, WPI_INST, HWND, void *, finish_type );
static BOOL ctl_rint_finish( ctl_elt *, WPI_INST, HWND, void *, finish_type );
static BOOL ctl_rfloat_finish( ctl_elt *, WPI_INST, HWND, void *, finish_type );

// The *_modified functions will be called to query if the control element
// was modified.
//   first parameter: pointer to the control element data
//   second parameter: the wParam of the message
//   third parameter: the lParam of the message
static BOOL ctl_check_modified( ctl_elt *, WPI_PARAM1, WPI_PARAM2 );
static BOOL ctl_text_modified( ctl_elt *, WPI_PARAM1, WPI_PARAM2 );
static BOOL ctl_combo_modified( ctl_elt *, WPI_PARAM1, WPI_PARAM2 );
static BOOL ctl_radio_modified( ctl_elt *, WPI_PARAM1, WPI_PARAM2 );

// The control table: Contains the
ctl_action Ctl_int_actions[] =
{
    { NULL, NULL, NULL }, // empty dummy, since we are starting to count from one
    { ctl_check_start, ctl_check_finish, ctl_check_modified },
    { ctl_radio_start, ctl_radio_finish, ctl_radio_modified },
    { ctl_text_start, ctl_text_finish, ctl_text_modified },
    { ctl_combo_start, ctl_combo_finish, ctl_combo_modified },
    { ctl_dcombo_start, ctl_dcombo_finish, ctl_combo_modified },
    { ctl_int_start, ctl_int_finish, ctl_text_modified },
    { ctl_float_start, ctl_float_finish, ctl_text_modified },
    { ctl_int_start, ctl_rint_finish, ctl_text_modified },
    { ctl_float_start, ctl_rfloat_finish, ctl_text_modified }
    // missing: CTL_ESCOMBO,        // editable string combo box (drop down) (MISSING!!!)
    // missing: CTL_DHCOMBO,        // dynamic combo box, with HWND parm (MISSING!!!)
};

///////////////////////////////////////////////////////////////////////////////
// The ctl_dlg_ functions

BOOL ctl_dlg_init( WPI_INST inst, HWND dlg, void *ptr, void *ctl_ptr)
/*******************************************************************/
/* initialize a data control. Returns TRUE if it gets up OK. Could return
   FALSE if a data error occurs */
{
    int                 num;
    ctl_elt             *elt;
    clt_def             *ctl = ctl_ptr;      // so app doesn't have to do type cast;

    // enumerate all control elements.
    for( num = ctl->num_ctls, elt = ctl->elts; num > 0; --num, ++elt ) {
        // Initialize control element.
        if( !(Ctl_int_actions[elt->type].setup)( elt, inst, dlg, ptr, TRUE ) ) {
            return( FALSE );
        }
        // Set modified marker to FALSE (i.e. not modified).
        elt->modified = FALSE;
    }

    return( TRUE );
}

BOOL ctl_dlg_done( WPI_INST inst, HWND dlg, void *ptr, void *ctl_ptr)
/*******************************************************************/
/* finalize a data control. Returns TRUE if all field verification is ok,
   FALSE otherwise */
{
    int                 num;
    ctl_elt             *elt;
    clt_def             *ctl = ctl_ptr;      // so app doesn't have to do type cast;

    // enumerate all control elements.
    for( num = ctl->num_ctls, elt = ctl->elts; num > 0; --num, ++elt ) {
        // was the element modified?
        if( elt->modified ) {
            // yes: query new state
            if( !(Ctl_int_actions[elt->type].finish)( elt, inst, dlg, ptr, FINISH_DONE ) ) {
                // some error occured: terminate with error
                return( FALSE );
            }
        }
    }

    return( TRUE );
}

void ctl_dlg_process( void *ctl_ptr, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
/*************************************************************************/
/* this routine must be called for all WM_COMMAND events sent to the
   dialog. */
{
    clt_def             *ctl = ctl_ptr;      // so app doesn't have to do type cast;
    ctl_elt             *elt;
    int                 num;
    BOOL                mod;

    // enumerate all control elements.
    for( num = ctl->num_ctls, elt = ctl->elts; num > 0; --num, ++elt ) {
        // query if message modified the element
        mod = (Ctl_int_actions[elt->type].modified)( elt, wparam, lparam );
        // mark the modify state
        elt->modified |= mod;
        if( mod ) {
            // the element was modified:
            // no need to check the other elements then.
            break;
        }
    }
}

BOOL ctl_dlg_reset( WPI_INST ___a, HWND dlg, void *ptr, void *ctl_ptr, BOOL ___b )
/********************************************************************************/
/* this routine must be called to reset the dialog to the given state */
{
    ___b=___b;
    // simply call the ctl_dlg_init routine
    return ctl_dlg_init( ___a, dlg, ptr, ctl_ptr );
}

//////////////////////////////////////////////////////////////////////////////////
// The dyn_tpl_ functions

void dyn_tpl_init( void *dyn_def, HWND dlg_hld )
// Initialize the given dialog by the dynamic template.
{
    int                 num;
    dyn_tpl_dim         *dim; // pointer to the current dyn_tpl_dim object
    dyn_tpl_dlg         *dyn = dyn_def; // so app doesn't have to do type cast
    char                *dimptr = (char*)&dyn->tpls; // points to the start of the next
                                                     // dyn_tpl_dim object
    int                 *int_ctr; // used to count the ints
    int                 i;

    // TODO: add support for static templates

    for( num = dyn->num_tpls; num > 0; --num ) {
        // set dim to the current dyn_tpl_dim object
        dim = (dyn_tpl_dim *)dimptr;
        // find the size of the current dyn_tpl_dim object
        dimptr += sizeof( dyn_tpl_dim );
        int_ctr = (int *)dimptr;
        while( *int_ctr++ != -1 ); // the list of ints is at it seems terminated by -1 (MISSING!!!)
        dimptr = (char *)int_ctr;
        // Is the current object in use?
        if( dim->info.use ) {
            // Yes: query the new state
            dyn_dim_type state = dim->info.tpl_state( dlg_hld, TRUE );
            dim->info.state = state;
            // first, there comes a list in form "begin,end" which is zero terminated
            for( i = 0; dim->dyn_tpl[i] != 0; i += 2 ) {
                // extract two elements b and e from the list
                int b = dim->dyn_tpl[i], e = dim->dyn_tpl[i + 1];
                // mark all control elements in range [b; e] as state indicates.
                for( ; b <= e; ++b ) {
                    HWND h = GetDlgItem( dlg_hld, b );
                    ShowWindow( h, (dim->info.state == DYN_INVISIBLE) ? SW_HIDE : SW_SHOW );
                    EnableWindow( h, (dim->info.state == DYN_VISIBLE) ? TRUE : FALSE );
                }
            }
            // then there comes a list in form "item" which is zero terminated
            for( ++i; dim->dyn_tpl[i] != 0; ++i ) {
                // mark control element as state indicates.
                HWND h = GetDlgItem( dlg_hld, dim->dyn_tpl[i] );
                ShowWindow( h, (dim->info.state == DYN_INVISIBLE) ? SW_HIDE : SW_SHOW );
                EnableWindow( h, (dim->info.state == DYN_VISIBLE) ? TRUE : FALSE );
            }
            // I don't know what comes now, just take it as another list (MISSING!!!)
            for( ++i; dim->dyn_tpl[i] != -1; ++i ) {
                // mark control element as state indicates.
                HWND h = GetDlgItem( dlg_hld, dim->dyn_tpl[i] );
                ShowWindow( h, (dim->info.state == DYN_INVISIBLE) ? SW_HIDE : SW_SHOW );
                EnableWindow( h, (dim->info.state == DYN_VISIBLE) ? TRUE : FALSE );
            }
        }
    }
}

void dyn_tpl_process( void *dyn_def, HWND dlg_hld, WPI_PARAM1 parm1, WPI_PARAM2 parm2 )
// Process changes of the dynamic template for the given dialog.
{
    int                 num;
    dyn_tpl_dim         *dim; // pointer to the current dyn_tpl_dim object
    dyn_tpl_dlg         *dyn = dyn_def; // so app doesn't have to do type cast
    char                *dimptr = (char*)&dyn->tpls; // points to the start of the next
                                                     // dyn_tpl_dim object
    int                 *int_ctr; // used to count the ints
    int                 i;

    // TODO: add support for static templates

    for( num = dyn->num_tpls; num > 0; --num ) {
        // set dim to the current dyn_tpl_dim object
        dim = (dyn_tpl_dim *)dimptr;
        // find the size of the current dyn_tpl_dim object
        dimptr += sizeof( dyn_tpl_dim );
        int_ctr = (int *)dimptr;
        while( *int_ctr++ != -1 ); // the list of ints is at it seems terminated by -1 (MISSING!!!)
        dimptr = (char *)int_ctr;
        // Is the current object in use?
        if( dim->info.use ) {
            // check if the state has changed
            if( dim->info.tpl_check( parm1, parm2, dlg_hld ) ) {
                // query the new state
                dyn_dim_type state = dim->info.tpl_state( dlg_hld, TRUE );
                if( dim->info.state != state ) {
                    // the state has really changed:
                    dim->info.state = state;
                    // first, there comes a list in form "begin,end" which is zero terminated
                    for( i = 0; dim->dyn_tpl[i] != 0; i += 2 ) {
                        // extract two elements b and e from the list
                        int b = dim->dyn_tpl[i], e = dim->dyn_tpl[i + 1];
                        // mark all control elements in range [b; e] as state indicates.
                        for( ; b <= e; ++b ) {
                            HWND h = GetDlgItem( dlg_hld, b );
                            ShowWindow( h, (dim->info.state == DYN_INVISIBLE) ? SW_HIDE : SW_SHOW );
                            EnableWindow( h, (dim->info.state == DYN_VISIBLE) ? TRUE : FALSE );
                        }
                    }
                    // then there comes a list in form "item" which is zero terminated
                    for( ++i; dim->dyn_tpl[i] != 0; ++i ) {
                        // mark control element as state indicates.
                        HWND h = GetDlgItem( dlg_hld, dim->dyn_tpl[i] );
                        ShowWindow( h, (dim->info.state == DYN_INVISIBLE) ? SW_HIDE : SW_SHOW );
                        EnableWindow( h, (dim->info.state == DYN_VISIBLE) ? TRUE : FALSE );
                    }
                    // I don't know what comes now, just take it as another list (MISSING!!!)
                    for( ++i; dim->dyn_tpl[i] != -1; ++i ) {
                        // mark control element as state indicates.
                        HWND h = GetDlgItem( dlg_hld, dim->dyn_tpl[i] );
                        ShowWindow( h, (dim->info.state == DYN_INVISIBLE) ? SW_HIDE : SW_SHOW );
                        EnableWindow( h, (dim->info.state == DYN_VISIBLE) ? TRUE : FALSE );
                    }
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////
// The following code is copyied almost 1:1 from the datactl library
// out of the samples directory.

////////////////////////////////////////////////////////////////////////////
// Check field implementation

static BOOL ctl_check_start( ctl_elt *elt, WPI_INST inst, HWND dlg,
                             void *ptr, BOOL ___b )
/*****************************************************************/
/* start check field */
{
    ___b=___b;
    inst = inst;

    CheckDlgButton( dlg, elt->control, _value_bool( ptr, elt ) );

    return( TRUE );
}

static BOOL ctl_check_finish( ctl_elt *elt, WPI_INST inst, HWND dlg,
                              void *ptr, finish_type ___f )
/******************************************************************/
/* end check field */
{
    ___f=___f;
    inst = inst;

    _value_bool( ptr, elt ) = IsDlgButtonChecked( dlg, elt->control );

    return( TRUE );
}

static BOOL ctl_check_modified( ctl_elt *elt, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
/**********************************************************************************/
{
    WORD        cmd;
    WORD        id;

    lparam = lparam;
    id = LOWORD( wparam );
    cmd = GET_WM_COMMAND_CMD( wparam, lparam );
    if( id == elt->control &&
        (cmd == BN_CLICKED || cmd == BN_DOUBLECLICKED) ) {
        return( TRUE );
    }

    return( FALSE );
}

////////////////////////////////////////////////////////////////////////////
// Combo list box implementation

static WORD ctl_combo_add_msg( HWND, int );
static WORD ctl_combo_sel_msg( HWND, int );
static WORD ctl_combo_get_msg( HWND, int );
static WORD ctl_combo_clr_msg( HWND, int );

static BOOL ctl_combo_start( ctl_elt *elt, WPI_INST inst, HWND dlg,
                             void *ptr, BOOL ___b )
/*****************************************************************/
/* start a combo list box */
{
    char                value[50];
    WORD                id;
    int                 choose;
    int                 max;

    ___b=___b;

    choose = _value_int( ptr, elt ) - elt->info.combo.origin;

    if( choose < 0 ) {
        choose = 0;
    } else {
        max = elt->info.combo.end_id - elt->info.combo.start_id;
        if( choose > max ) {
            choose = max;
        }
    }

    SendDlgItemMessage( dlg, elt->control, ctl_combo_clr_msg( dlg, elt->control ), 0, 0L );
    for( id = elt->info.combo.start_id; id <= elt->info.combo.end_id; ++id ) {

        LoadString( inst, id, value, sizeof( value ) );
        value[49] = '\0';
        SendDlgItemMessage( dlg, elt->control, ctl_combo_add_msg( dlg, elt->control ), 0, (LPARAM)value );
    }
    SendDlgItemMessage( dlg, elt->control, ctl_combo_sel_msg( dlg, elt->control ), choose, 0 );

    return( TRUE );
}

static BOOL ctl_combo_finish( ctl_elt *elt, WPI_INST inst, HWND dlg,
                              void *ptr, finish_type ___f )
/******************************************************************/
/* finish a combo list box */
{
    ___f=___f;
    inst = inst;

    _value_int( ptr, elt ) = elt->info.combo.origin + SendDlgItemMessage( dlg, elt->control, ctl_combo_get_msg( dlg, elt->control ), 0, 0 );

    return( TRUE );
}

static BOOL ctl_combo_modified( ctl_elt *elt, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
/**************************************************************************/
{
    WORD        id;
    WORD        cmd;

    lparam = lparam;
    id = LOWORD( wparam );
    cmd = GET_WM_COMMAND_CMD( wparam, lparam );
    if( id == elt->control &&
        (cmd == CBN_SELCHANGE || cmd == LBN_SELCHANGE) ) {
        return( TRUE );
    }

    return( FALSE );
}

static BOOL is_listbox( HWND dlg, int ctl )
/*****************************************/
{
    char                buf[100];

    GetClassName( GetDlgItem( dlg, ctl ), buf, 100 );
    if( 0 == stricmp( buf, "listbox" ) ) {
        return( TRUE );
    }

    return( FALSE );
}

static WORD ctl_combo_add_msg( HWND dlg, int ctl )
/************************************************/
{
    if( is_listbox( dlg, ctl ) ) {
        return( LB_ADDSTRING );
    } else {
        return( CB_ADDSTRING );
    }
}

static WORD ctl_combo_sel_msg( HWND dlg, int ctl )
/************************************************/
{
    if( is_listbox( dlg, ctl ) ) {
        return( LB_SETCURSEL );
    } else {
        return( CB_SETCURSEL );
    }
}

static WORD ctl_combo_get_msg( HWND dlg, int ctl )
/************************************************/
{
    if( is_listbox( dlg, ctl ) ) {
        return( LB_GETCURSEL );
    } else {
        return( CB_GETCURSEL );
    }
}

static WORD ctl_combo_clr_msg( HWND dlg, int ctl )
/************************************************/
{
    if( is_listbox( dlg, ctl ) ) {
        return( LB_RESETCONTENT );
    } else {
        return( CB_RESETCONTENT );
    }
}

static WORD ctl_combo_add_msg( HWND, int );
static WORD ctl_combo_sel_msg( HWND, int );
static WORD ctl_combo_get_msg( HWND, int );

static BOOL ctl_dcombo_start( ctl_elt *elt, WPI_INST inst, HWND dlg,
                              void *ptr, BOOL ___b )
/******************************************************************/
/* start a dynamic combo list box */
{
    char                *str;
    int                 i;
    int                 value;

    ___b=___b;
    inst = inst;

    value = _value_int( ptr, elt ) - elt->info.dcombo.origin;

    for( i = 0;; ++i ) {
        str = (elt->info.dcombo.fetch)( i );
        if( str == NULL ) {
            break;
        }

        SendDlgItemMessage( dlg, elt->control, ctl_combo_add_msg( dlg, elt->control ), 0, (LPARAM)str );
    }

    if( value >= i ) {
        value = i - 1;
    }

    SendDlgItemMessage( dlg, elt->control, ctl_combo_sel_msg( dlg, elt->control ), value, 0 );

    return( TRUE );
}

static BOOL ctl_dcombo_finish( ctl_elt *elt, WPI_INST inst, HWND dlg,
                               void *ptr, finish_type ___f )
/*******************************************************************/
/* finish a dynamic combo list box */
{
    ___f=___f;
    inst = inst;

    _value_int( ptr, elt ) = elt->info.dcombo.origin + SendDlgItemMessage( dlg, elt->control, ctl_combo_get_msg( dlg, elt->control ), 0, 0 );

    return( TRUE );
}

////////////////////////////////////////////////////////////////////////////
// Float field implementation

static BOOL ctl_float_start( ctl_elt *elt, WPI_INST inst, HWND dlg,
                             void *ptr, BOOL ___b )
/*****************************************************************/
/* start a float field */
{
    char                buf[50];
    char                *str;
    int                 dec;
    float               value;

    ___b=___b;
    inst = inst;

    value = _value_float( ptr, elt );
    sprintf( buf, "%f", value );
    for( str = buf + strlen( buf ) - 1; *str == '0'; --str );
    for( dec = 0; *str != '.'; --str, ++dec );
    if( dec < 2 ) {
        dec = 2;
    }

    sprintf( buf, "%-.*f", dec, value );

    SetDlgItemText( dlg, elt->control, buf );

    return( TRUE );
}

static BOOL ctl_float_finish( ctl_elt *elt, WPI_INST inst, HWND dlg,
                              void *ptr, finish_type ___f )
/******************************************************************/
/* end a float field */
{
    char                str[50];
    float               value;

    ___f=___f;
    inst = inst;

    GetDlgItemText( dlg, elt->control, str, 50 );
    str[49] = '\0';

    if( 1 == sscanf( str, "%f", &value ) ) {
        _value_float( ptr, elt ) = value;
    } else {
        SetFocus( GetDlgItem( dlg, elt->control ) );
        MessageBox( dlg, "Invalid value: please re-enter it", NULL,
                    MB_APPLMODAL | MB_ICONHAND | MB_OK );
        return( FALSE );
    }

    return( TRUE );
}

////////////////////////////////////////////////////////////////////////////
// Ranged float field implementation

static BOOL ctl_rfloat_finish( ctl_elt *elt, WPI_INST inst, HWND dlg,
                               void *ptr, finish_type ___f )
/****************************** ************************************/
/* end an float field */
{
    float               value;
    char                str[100];
    BOOL                any_max;

    if( !ctl_float_finish( elt, inst, dlg, ptr, ___f ) ) {
        return( FALSE );
    }

    value = _value_float( ptr, elt );

    any_max = elt->info.rfloat.max >= elt->info.rfloat.min;
    if( value < elt->info.rfloat.min || (any_max && value > elt->info.rfloat.max) ) {
        SetFocus( GetDlgItem( dlg, elt->control ) );
        if( any_max ) {
            sprintf( str, "Value must be in the range %.2f to %.2f",
                     elt->info.rfloat.min, elt->info.rfloat.max );
        } else {
            sprintf( str, "Value must be greater than or equal to %.2f",
                     elt->info.rfloat.min );
        }

        MessageBox( dlg, str, NULL, MB_APPLMODAL | MB_ICONHAND | MB_OK );

        return( FALSE );
    }

    return( TRUE );
}

////////////////////////////////////////////////////////////////////////////
// Integer field implementation

static BOOL ctl_int_start( ctl_elt *elt, WPI_INST inst, HWND dlg, void *ptr, BOOL ___b )
/**************************************************************************************/
/* start an integer field */
{
    ___b=___b;
    inst = inst;

    SetDlgItemInt( dlg, elt->control, _value_int( ptr, elt ), TRUE );

    return( TRUE );
}

static BOOL ctl_int_finish( ctl_elt *elt, WPI_INST inst, HWND dlg,
                            void *ptr, finish_type ___f )
/****************************************************************/
/* end an int field */
{
    BOOL                ok;

    ___f=___f;
    inst = inst;

    _value_int( ptr, elt ) = GetDlgItemInt( dlg, elt->control, &ok, TRUE );

    if( !ok ) {
        SetFocus( GetDlgItem( dlg, elt->control ) );
        MessageBox( dlg, "Invalid integer: please re-enter it", NULL,
                    MB_APPLMODAL | MB_ICONHAND | MB_OK );
        return( FALSE );
    }

    return( TRUE );
}

////////////////////////////////////////////////////////////////////////////
// Ranged int field implementation

static BOOL ctl_rint_finish( ctl_elt *elt, WPI_INST inst, HWND dlg,
                             void *ptr, finish_type ___f )
/*****************************************************************/
/* end an int field */
{
    int                 value;
    char                str[100];
    BOOL                any_max;

    if( !ctl_int_finish( elt, inst, dlg, ptr, ___f ) ) {
        return( FALSE );
    }

    value = _value_int( ptr, elt );

    any_max = elt->info.rint.max >= elt->info.rint.min;
    if( value < elt->info.rint.min || (any_max && value > elt->info.rint.max) ) {
        SetFocus( GetDlgItem( dlg, elt->control ) );
        if( any_max ) {
            sprintf( str, "Integer must be in the range %d to %d",
                     elt->info.rint.min, elt->info.rint.max );
        } else {
            sprintf( str, "Integer must be greater than or equal to %d",
                     elt->info.rint.min );
        }

        MessageBox( dlg, str, NULL, MB_APPLMODAL | MB_ICONHAND | MB_OK );

        return( FALSE );
    }

    return( TRUE );
}

////////////////////////////////////////////////////////////////////////////
// Radio button implementation

static BOOL ctl_radio_start( ctl_elt *elt, WPI_INST inst, HWND dlg,
                             void *ptr, BOOL ___b )
/*****************************************************************/
/* start a radio button */
{
    int                 value;

    ___b=___b;
    inst = inst;

    value = _value_int( ptr, elt );

    if( value != 0 ) {
        CheckRadioButton( dlg, elt->control, elt->info.radio.end_control,
                          elt->control + _value_int( ptr, elt ) - 1 );
    }

    return( TRUE );
}

static BOOL ctl_radio_finish( ctl_elt *elt, WPI_INST inst, HWND dlg,
                              void *ptr, finish_type ___f )
/******************************************************************/
/* finish a radio button */
{
    int                 control;

    ___f=___f;
    inst = inst;

    for( control = elt->info.radio.end_control; control >= elt->control; --control ) {
        if( IsDlgButtonChecked( dlg, control ) ) {
            _value_int( ptr, elt ) = control - elt->control + 1;

            break;
        }
    }

    return( TRUE );
}

static BOOL ctl_radio_modified( ctl_elt *elt, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
/**********************************************************************/
{
    WORD        id;
    WORD        cmd;

    lparam = lparam;
    id = LOWORD( wparam );
    cmd = GET_WM_COMMAND_CMD( wparam, lparam );
    if( id >= elt->control && id <= elt->info.radio.end_control &&
        (cmd == BN_CLICKED || cmd == BN_DOUBLECLICKED) ) {
        return( TRUE );
    }

    return( FALSE );
}

////////////////////////////////////////////////////////////////////////////
// Text field implementation

static BOOL ctl_text_start( ctl_elt *elt, WPI_INST inst, HWND dlg, void *ptr, BOOL ___b )
/***************************************************************************************/
/* start a text field */
{
    ___b=___b;
    inst = inst;

    SetDlgItemText( dlg, elt->control, _str_ptr( ptr, elt ) );

    return( TRUE );
}

static BOOL ctl_text_finish( ctl_elt *elt, WPI_INST inst, HWND dlg,
                             void *ptr, finish_type ___f )
/*****************************************************************/
/* end a text field */
{
    char                *str;

    ___f=___f;
    inst = inst;

    str = _str_ptr( ptr, elt );

    GetDlgItemText( dlg, elt->control, str, elt->info.text.text_size );

    str[elt->info.text.text_size - 1]= '\0'; // in case of overflow

    return( TRUE );
}

static BOOL ctl_text_modified( ctl_elt *elt, WPI_PARAM1 wparam , WPI_PARAM2 lparam )
/**********************************************************************/
{
    WORD        id;
    WORD        cmd;

    id = LOWORD( wparam );
    cmd = GET_WM_COMMAND_CMD( wparam, lparam );

    if( id == elt->control && cmd == EN_CHANGE ) {
        return( TRUE );
    }

    return( FALSE );
}
