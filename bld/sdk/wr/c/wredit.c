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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include "watcom.h"
#include "wrglbl.h"
#include "wredit.h"

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

void WRSetEditWithStr( char *cp, HWND hDlg, int id )
{
    if( cp != NULL ) {
        SendDlgItemMessage( hDlg, id, WM_SETTEXT, 0, (LPARAM)(LPSTR)cp );
    }
}

char *WRGetStrFromEdit( HWND hDlg, int id, BOOL *mod )
{
    char        *cp;
    int         text_length;
    int         text_copied;

    text_copied = 0;

    if( mod ) {
        /* find out if the edit field has changed */
        if( SendDlgItemMessage( hDlg, id, EM_GETMODIFY, 0, 0 ) ) {
            *mod = TRUE;
        } else {
            *mod = FALSE;
        }
    }

    text_length = SendDlgItemMessage( hDlg, id, WM_GETTEXTLENGTH, 0, 0 );

    cp = (char *)WRMemAlloc( text_length + 1 );
    if( cp == NULL ) {
        return( NULL );
    }

    text_copied = SendDlgItemMessage( hDlg, id, WM_GETTEXT, text_length + 1, (LPARAM)(LPSTR)cp );

    if( text_copied > text_length ) {
        WRMemFree( cp );
        return( NULL );
    }

    cp[text_length] = '\0';

    return( cp );
}

void WRSetEditWithULONG( unsigned long val, int base, HWND hDlg, int id )
{
    char        temp[35];

    ultoa( val, temp, base );
    if( base == 16 ) {
        memmove( temp + 2, temp, 33 );
        temp[0] = '0';
        temp[1] = 'x';
    } else if( base == 8 ) {
        memmove( temp + 1, temp, 34 );
        temp[0] = '0';
    }
    WRSetEditWithStr( temp, hDlg, id );
}

void WRSetEditWithSLONG( signed long val, int base, HWND hDlg, int id )
{
    char        temp[35];
    ltoa( val, temp, base );
    WRSetEditWithStr( temp, hDlg, id );
}

char *WRGetStrFromListBox( HWND hDlg, int id, int index )
{
    char        *cp;
    int         text_length;
    int         text_copied;
    int         count;

    count = (int)SendDlgItemMessage( hDlg, id, LB_GETCOUNT, 0, 0 );
    if( count == 0 || count == LB_ERR || count < index ) {
        return( NULL );
    }

    text_copied = 0;
    text_length = SendDlgItemMessage( hDlg, id, LB_GETTEXTLEN, index, 0 );
    cp = (char *)WRMemAlloc( text_length + 1 );
    if( cp == NULL ) {
        return( NULL );
    }

    text_copied = SendDlgItemMessage( hDlg, id, LB_GETTEXT, index, (LPARAM)(LPSTR)cp );

    if( text_copied > text_length ) {
        WRMemFree( cp );
        return( NULL );
    }

    cp[text_length] = '\0';

    return( cp );
}

BOOL WRGetSLONGFromEdit( HWND hDlg, int id, BOOL *mod, signed long *value )
{
    signed long val;
    char        *cp;
    char        *ep;
    BOOL        ret;

    val = 0;
    ret = TRUE;

    cp = WRGetStrFromEdit( hDlg, id, mod );

    /* find out if the edit field has changed */
    if( mod == NULL || *mod ) {
        if( cp == NULL ) {
            return( FALSE );
        }
        val = (signed long)strtol( cp, &ep, 0 );
        if( *ep != '\0' ) {
            if( mod != NULL ) {
                *mod = FALSE;
            }
            val = 0;
            ret = FALSE;
        }
    }

    if( cp != NULL ) {
        WRMemFree( cp );
    }

    if( value != NULL ) {
        *value = val;
    }

    return( ret );
}
