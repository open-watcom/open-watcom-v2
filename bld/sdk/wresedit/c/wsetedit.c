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


#include "precomp.h"
#include <stdlib.h>
#include <string.h>
#include "wglbl.h"
#include "wmem.h"
#include "wsetedit.h"
#include "wrdll.h"
#include "wrutil.h"

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

// changes all occurrences of chars in 'from' to '\to' in str
char *WConvertStringFrom( char *str, char *from, char *to )
{
    return( WRConvertStringFrom( str, from, to ) );
}

// changes all occurrences of 'from' to '\to' in str
char *WConvertFrom( char *str, char from, char to )
{
    return( WRConvertFrom( str, from, to ) );
}

// changes all occurrences of '\from' to 'to' in str
char *WConvertTo( char *str, char to, char from )
{
    return( WRConvertTo( str, to, from ) );
}

// changes all occurrences of the \chars in 'from' to 'to' in str
char *WConvertStringTo( char *str, char *to, char *from )
{
    return( WRConvertStringTo( str, to, from ) );
}

Bool WSetEditWithSINT32( HWND edit, int_32 val, int base )
{
    char temp[35];

    ltoa( val, temp, base );

    if( base == 16 ) {
        memmove( temp + 2, temp, 33 );
        temp[0] = '0';
        temp[1] = 'x';
    } else if( base == 8 ) {
        memmove( temp + 1, temp, 34 );
        temp[0] = '0';
    }

    return( WSetEditWithStr( edit, temp ) );
}

Bool WSetEditWithWResID( HWND edit, WResID *id )
{
    char    *cp;
    Bool    ok;

    cp = NULL;

    ok = (edit != (HWND)NULL && id != NULL);

    if( ok ) {
        ok = ((cp = WResIDToStr( id )) != NULL);
    }

    if( ok ) {
        ok = WSetEditWithStr( edit, cp );
    }

    if( cp != NULL ) {
        WMemFree( cp );
    }

    return( ok );
}

Bool WSetEditWithStr( HWND edit, char *str )
{
    Bool      ok;

    if( ok = (edit != (HWND)NULL && str != NULL) ) {
        SendMessage( edit, WM_SETTEXT, 0, (LPARAM)(LPCSTR)str );
    }

    return( ok );
}

Bool WSetLBoxWithStr( HWND lbox, char *str, void *data )
{
    Bool      ok;
    LRESULT   index;

    ok = (lbox != (HWND)NULL && str != NULL);

    if( ok ) {
        index = SendMessage( lbox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)str );
        ok = (index != LB_ERR && index != LB_ERRSPACE);
    }

    if( ok ) {
        SendMessage( lbox, LB_SETITEMDATA, index, (LPARAM)data );
        ok = (index != LB_ERR);
    }

    return( ok );
}

Bool WInsertLBoxWithStr( HWND lbox, int pos, char *str, void *data )
{
    Bool      ok;
    LRESULT   index;

    ok = (lbox != (HWND)NULL && str != NULL);

    if( ok ) {
        index = SendMessage( lbox, LB_INSERTSTRING, (WPARAM)pos, (LPARAM)(LPCSTR)str );
        ok = (index != LB_ERR && index != LB_ERRSPACE);
    }

    if( ok ) {
        SendMessage( lbox, LB_SETITEMDATA, index, (LPARAM)data );
        ok = (index != LB_ERR);
    }

    return( ok );
}

Bool WSetLBoxWithWResID( HWND lbox, WResID *id, void *data )
{
    Bool    ok;
    char    *name;

    name = NULL;

    ok = (lbox != (HWND)NULL && id != NULL);

    ok = ok && ((name = WResIDToStr( id )) != NULL);

    ok = ok && WSetLBoxWithStr( lbox, name, data );

    if( name != NULL ) {
        WMemFree( name );
    }

    return( ok );
}

char *WGetStrFromEdit( HWND edit, Bool *mod )
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

    cp = (char *)WMemAlloc( text_length + 1 );
    if( cp == NULL ) {
        return( NULL );
    }

    text_copied = SendMessage ( edit, WM_GETTEXT, text_length + 1, (LPARAM)cp );

    if( text_copied > text_length ) {
        WMemFree( cp );
        return( NULL );
    }

    cp[text_length] = '\0';

    return( cp );
}

WResID *WGetWResIDFromEdit( HWND edit, Bool *mod )
{
    WResID  *rp;
    uint_16 ordID;
    char    *cp;
    char    *ep;

    rp = NULL;

    cp = WGetStrFromEdit( edit, mod );

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
        WMemFree( cp );
    }

    return( rp );
}

int_32 WGetSINT32FromEdit( HWND edit, Bool *mod )
{
    int_32  val;
    char    *cp;
    char    *ep;

    val = 0;

    cp = WGetStrFromEdit( edit, mod );

    /* find out if the edit field has changed */
    if( mod == NULL || *mod ) {
        if( cp == NULL ) {
            return( 0 );
        }
        val = (int_32)strtol( cp, &ep, 0 );
        if( *ep != '\0' ) {
            if( mod != NULL ) {
                *mod = FALSE;
            }
            val = 0;
        }
    }

    if( cp != NULL ) {
        WMemFree( cp );
    }

    return( val );
}

char *WGetStrFromComboLBox( HWND combo, int index )
{
    char        *cp;
    LRESULT     text_length;
    LRESULT     text_copied;
    LRESULT     count;

    if( index == -1 ) {
        index = (int)SendMessage( combo, CB_GETCURSEL, 0, 0 );
        if( index == CB_ERR ) {
            return( NULL );
        }
    }

    count = SendMessage( combo, CB_GETCOUNT, 0, 0 );
    if( count == 0 || count == CB_ERR || count < index ) {
        return( NULL );
    }

    text_copied = 0;

    text_length = SendMessage( combo, CB_GETLBTEXTLEN, index, 0 );

    cp = (char *)WMemAlloc( text_length + 1 );
    if( cp == NULL ) {
        return( NULL );
    }

    text_copied = SendMessage( combo, CB_GETLBTEXT, index, (LPARAM)cp );

    if( text_copied != text_length ) {
        WMemFree( cp );
        return( NULL );
    }

    cp[text_length] = '\0';

    return( cp );
}
