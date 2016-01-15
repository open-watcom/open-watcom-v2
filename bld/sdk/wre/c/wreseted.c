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


#include "commonui.h"
#include <stdlib.h>

#include "watcom.h"
#include "wreglbl.h"
#include "wreseted.h"
#include "wrdll.h"

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

bool WRESetEditWithWResID( HWND edit, WResID *id )
{
    char    *cp;
    bool    ok;

    cp = NULL;

    ok = ((edit != (HWND)NULL) && id != NULL);

    if( ok ) {
        ok = ((cp = WResIDToStr( id )) != NULL);
    }

    if( ok ) {
        ok = WRESetEditWithStr( edit, cp );
    }

    if( cp != NULL ) {
        WRMemFree( cp );
    }

    return( ok );
}

bool WRESetEditWithStr( HWND edit, char *str )
{
    bool    ok;

    ok = ( edit != (HWND)NULL && str != NULL );
    if( ok ) {
        SendMessage( edit, WM_SETTEXT, 0, (LPARAM)(LPSTR)str );
    }

    return( ok );
}

bool WRESetLBoxWithStr( HWND lbox, char *str, void *data )
{
    bool    ok;
    int     index;

    ok = ( lbox != (HWND)NULL && str != NULL );

    if( ok ) {
        index = (int)SendMessage( lbox, LB_ADDSTRING, 0, (LPARAM)(LPSTR)str );
        ok = ( index != LB_ERR && index != LB_ERRSPACE );
    }

    if( ok ) {
        ok = ( SendMessage( lbox, LB_SETITEMDATA, index, (LPARAM)(LPVOID)data ) != LB_ERR );
    }

    return( ok );
}

bool WRESetLBoxWithWResID( HWND lbox, WResID *id, void *data )
{
    bool    ok;
    char    *name;

    name = NULL;

    ok = ( lbox != (HWND)NULL && id != NULL );

    ok = ok && ((name = WResIDToStr( id )) != NULL);

    ok = ok && WRESetLBoxWithStr( lbox, name, data );

    if( name != NULL ) {
        WRMemFree( name );
    }

    return( ok );
}

char *WREGetStrFromEdit( HWND edit, bool *mod )
{
    char    *cp;
    LRESULT text_length;
    LRESULT text_copied;

    text_copied = 0;

    if( mod != NULL ) {
        /* find out if the edit field has changed */
        if( SendMessage( edit, EM_GETMODIFY, 0, 0 ) ) {
            *mod = TRUE;
        } else {
            *mod = FALSE;
        }
    }

    text_length = SendMessage( edit, WM_GETTEXTLENGTH, 0, 0 );

    cp = (char *)WRMemAlloc( text_length + 1 );
    if( cp == NULL ) {
        return( NULL );
    }

    text_copied = SendMessage( edit, WM_GETTEXT, text_length + 1, (LPARAM)(LPSTR)cp );

    if( text_copied > text_length ) {
        WRMemFree( cp );
        return( NULL );
    }

    cp[text_length] = '\0';

    return( cp );
}

WResID *WREGetWResIDFromEdit( HWND edit, bool *mod )
{
    WResID  *rp;
    uint_16 ordID;
    char    *cp;
    char    *ep;

    rp = NULL;

    cp = WREGetStrFromEdit( edit, mod );

    /* find out if the edit field has changed */
    if( mod == NULL || *mod ) {
        if( cp == NULL ) {
            return( NULL );
        }
        ordID = (uint_16)strtoul( cp, &ep, 0 );
        if( *ep == '\0' ) {
            rp = WResIDFromNum( ordID );
        } else {
            rp = WResIDFromStr( cp );
        }
    }

    if( cp != NULL ) {
        WRMemFree( cp );
    }

    return( rp );
}
