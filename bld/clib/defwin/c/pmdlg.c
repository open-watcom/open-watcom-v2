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
* Description:  Presentation Manager default windowing dialogs.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "win.h"
#include "pmdlgid.h"

#define DISPLAY(x)              WinMessageBox( HWND_DESKTOP, NULL, x, "Error", 0, MB_APPLMODAL | MB_NOICON | MB_OK | MB_MOVEABLE );
#define MAX_INTEGER_FIELD       11
typedef struct {
    USHORT      x;
    USHORT      y;
    USHORT      cx;
    USHORT      cy;
    USHORT      id;
    USHORT      children;
    ULONG       style;
    ULONG       nclass;
    char        *cclass;
    char        *text;
    char        *presparms;
    char        *ctldata;
    ULONG       *ctldatalen;
} itemdata;

/*
 * Note:
 *
 * The ctldata/ctldatalen, serve a dual purpose.  In most cases, control data
 * consists of one long, so instead of entering the four bytes in an array,
 * we set the ULONG ctldatalen equal to those bytes, and pass a NULL for
 * ctldata.  If you have more control data, then you will have to use an arry
 * of bytes and plase the length of the array into the ULONG ctldatalen.
 * if you have no data, set ctldatalen = 0 and pass a NULL string to "", for
 * ctldata.
 *
 * For certains things ctldata is required, this is true for dialog boxes.
 * I did not find anywhere where this was documented and had to figure it out
 * by dissecting a .res file.
 *
 */

static itemdata _getint[] = {
{  12,  45, 215, 45, DID_SLBC_DLG, 6,
   FS_DLGBORDER | WS_CLIPSIBLINGS | WS_VISIBLE | WS_SAVEBITS,
   (ULONG)WC_FRAME, NULL , "Set Number Of Lines Between Clears", "", NULL,
   (ULONG*)(FCF_SYSMENU | FCF_TITLEBAR) },
{  4,  29, 150, 8, 101, 0,
    SS_TEXT | DT_LEFT,
    (ULONG)WC_STATIC, NULL, "Number Of Lines Between Clears:", "" , "", 0},
{  157,  29, 47, 8, DID_SLBC_FIELD, 0,
    ES_RIGHT | ES_MARGIN | WS_TABSTOP,
    (ULONG)WC_ENTRYFIELD, NULL, "10000", "" , "", 0},
{  4,  4, 40, 14, DID_OK, 0,
    BS_PUSHBUTTON | BS_AUTOSIZE | WS_TABSTOP,
    (ULONG)WC_BUTTON, NULL, "~OK", "" , "", 0},
{  58,  4, 40, 14, DID_DEFAULT, 0,
    BS_PUSHBUTTON | BS_AUTOSIZE | WS_TABSTOP,
    (ULONG)WC_BUTTON, NULL, "~Default", "" , "", 0},
{  112,  4, 40, 14, DID_CANCEL, 0,
    BS_PUSHBUTTON | BS_AUTOSIZE | WS_TABSTOP,
    (ULONG)WC_BUTTON, NULL, "Cancel", "" , "", 0},
{  166,  4, 40, 14, DID_HELP, 0,
    BS_HELP | BS_NOPOINTERFOCUS | WS_TABSTOP,
    (ULONG)WC_BUTTON, NULL, "Help", "" , "", 0},
};

#define MAX_INT_ITEMS sizeof( _getint ) / sizeof( itemdata )

char *ultoa( unsigned long int __value, char *__buf, int __radix );

static char *titleName = "Open Watcom Default Windowing System";
static char *helpMsg = "Enter the number of lines of text you would like to keep in the window at any time";

static void setIntegerValue( HWND hwndDlg, USHORT id, ULONG val ) {
//=============================================================

    char        buff[MAX_INTEGER_FIELD+1];

    ultoa( val, buff, 10 );
    WinSetDlgItemText( hwndDlg, id, buff );
}


static BOOL getIntegerValue( HWND hwndDlg, USHORT id, ULONG *pval ) {
//===============================================================

    char        buff[MAX_INTEGER_FIELD+1];
    char        *ptr;
    ULONG       val;
    ULONG       len;

    len = WinQueryDlgItemText( hwndDlg, id, MAX_INTEGER_FIELD+1, buff );
    /* strtoul will accept negative numbers */
    ptr = buff;
    while( isspace( (unsigned char)*ptr ) )
        ++ptr;
    if( *ptr == '-' )
        return( FALSE );
    val = strtoul( ptr, &ptr, 10 );
    if( len == 0 )
        return( FALSE ); // blank entry
    while( isspace( (unsigned char)*ptr ) )
        ++ptr;
    if( *ptr == '\0' ) {
        *pval = val;
        return( TRUE );
    } else {
        return( FALSE );
    }
}


static BOOL chkIntegerValue( HWND hwndDlg, USHORT id,
                                 ULONG *pval, char *errmsg ) {
//============================================================

    if( getIntegerValue( hwndDlg, id, pval ) ) {
        return( TRUE );
    } else {
        _Error( hwndDlg, titleName, errmsg );
        return( FALSE );
    }
}

/*
 * _GetIntervalBox - control dialog for getting interval
 */
static  MRESULT EXPENTRY IntervalDialogProc( HWND hwndDlg, USHORT msg,
                                         MPARAM mp1, MPARAM mp2 ) {
//=================================================================

    USHORT              dlg_id;
    HWND                dlg_hwnd;

    // Main procedure,this is an oversized case statement evaluating all the
    // events

    switch( msg ) {
    case WM_INITDLG:
        setIntegerValue( hwndDlg, DID_SLBC_FIELD, _AutoClearLines );
        // Initialize all necessary dialogsa
        break;
    case WM_COMMAND:
        // Check for any button pushes and act upon them
        dlg_id = SHORT1FROMMP( mp1 );
        dlg_hwnd = WinWindowFromID( hwndDlg, dlg_id );
        switch( dlg_id ) {
        case DID_DEFAULT:
            setIntegerValue( hwndDlg, DID_SLBC_FIELD, DEFAULT_CLEAR_INTERVAL );
            break;
        case DID_OK:
            if ( chkIntegerValue( hwndDlg, DID_SLBC_FIELD, &(_AutoClearLines ),
                                  "Value must be greater than 0!" ) ) {
                WinDismissDlg( hwndDlg, 1 );
            }
            break;
        case DID_CANCEL:
            WinDismissDlg( hwndDlg, 0 );
            break;
        }
        break;
    case WM_CLOSE:
        // CLose the window if necessary
        WinDismissDlg( hwndDlg, 0 );
        break;
    case WM_HELP:
        WinMessageBox( HWND_DESKTOP, hwndDlg, helpMsg, "Help", 0, MB_APPLMODAL | MB_NOICON | MB_OK | MB_MOVEABLE );
    default:
        return( WinDefDlgProc( hwndDlg, msg, mp1, mp2 ) );
    }
    return( 0 );
}


/*
 * _GetClearInterval - return number of lines between clearing
 */
void _GetClearInterval( void )
{
    GLOBALHANDLE        data,new;
    int                 i;

    data = _DialogTemplate( TEMP_TYPE, 437, 0xffff );

    if( data == NULL ) return;
    for( i=0;i<MAX_INT_ITEMS;i++ ) {
        new = _AddControl( data, _getint[i].style,
                _getint[i].x, _getint[i].y, _getint[i].cx, _getint[i].cy,
                _getint[i].id, _getint[i].children, _getint[i].nclass,
                _getint[i].cclass, _getint[i].text, _getint[i].presparms,
                _getint[i].ctldata, _getint[i].ctldatalen );
        if( new == NULL  ) {
            PMfree( data );
            return;
        }
        data = new;
    }

    data = _DoneAddingControls( data );
    _DynamicDialogBox( (PFNWP)IntervalDialogProc, _MainWindow, data );

} /* _GetClearInterval */
