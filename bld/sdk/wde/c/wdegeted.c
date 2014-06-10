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


#include "wdeglbl.h"
#include "wde_wres.h"
#include "wdegeted.h"

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

char *WdeGetStrFromCombo( HWND hDlg, int id )
{
    return( WdeGetStrFromEdit( hDlg, id, NULL ) );
}

uint_32 WdeGetUINT32FromCombo( HWND hDlg, int id )
{
    return( WdeGetUINT32FromEdit( hDlg, id, NULL ) );
}

char *WdeGetStrFromEdit( HWND hDlg, int id, bool *mod )
{
    char    *cp;
    int     text_length;
    int     text_copied;

    text_copied = 0;

    if( mod != NULL ) {
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

    text_copied = SendDlgItemMessage( hDlg, id, WM_GETTEXT, text_length + 1,
                                      (LPARAM)(LPSTR)cp );

    if( text_copied > text_length ) {
        WRMemFree( cp );
        return( NULL );
    }

    cp[text_length] = '\0';

    return( cp );
}

char *WdeGetStrFromListBox( HWND hDlg, int id, int index )
{
    char    *cp;
    int     text_length;
    int     text_copied;
    int     count;

    count = SendDlgItemMessage( hDlg, id, LB_GETCOUNT, 0, 0 );
    if( count == 0 || count == LB_ERR || count < index ) {
        return( NULL );
    }

    text_copied = 0;

    text_length = SendDlgItemMessage( hDlg, id, LB_GETTEXTLEN, index, 0 );

    cp = (char *)WRMemAlloc( text_length + 1 );
    if( cp == NULL ) {
        return( NULL );
    }

    text_copied = SendDlgItemMessage( hDlg, id, LB_GETTEXT, index,
                                      (LPARAM)(LPSTR)cp );

    if( text_copied != text_length ) {
        WRMemFree( cp );
        return( NULL );
    }

    cp[text_length] = '\0';

    return( cp );
}

char *WdeGetStrFromComboLBox( HWND hDlg, int id, int index )
{
    char    *cp;
    int     text_length;
    int     text_copied;
    int     count;

    count = SendDlgItemMessage( hDlg, id, CB_GETCOUNT, 0, 0 );
    if( count == 0 || count == CB_ERR || count < index ) {
        return( NULL );
    }

    text_copied = 0;

    text_length = SendDlgItemMessage( hDlg, id, CB_GETLBTEXTLEN, index, 0 );

    cp = (char *)WRMemAlloc( text_length + 1 );
    if( cp == NULL ) {
        return( NULL );
    }

    text_copied = SendDlgItemMessage( hDlg, id, CB_GETLBTEXT, index,
                                      (LPARAM)(LPSTR)cp );

    if( text_copied != text_length ) {
        WRMemFree( cp );
        return( NULL );
    }

    cp[text_length] = '\0';

    return( cp );
}

void WdeSetEditWithUINT16( uint_16 val, int base, HWND hDlg, int id )
{
    WdeSetEditWithUINT32( (uint_32)val, base, hDlg, id );
}

void WdeSetEditWithSINT16( int_16 val, int base, HWND hDlg, int id )
{
    WdeSetEditWithSINT32( (int_32)val, base, hDlg, id );
}

void WdeSetEditWithUINT32( uint_32 val, int base, HWND hDlg, int id )
{
    char temp[35];

    ultoa( val, temp, base );
    if( base == 16 ) {
        memmove( temp + 2, temp, 33 );
        temp[0] = '0';
        temp[1] = 'x';
    } else if( base == 8 ) {
        memmove( temp + 1, temp, 34 );
        temp[0] = '0';
    }
    WdeSetEditWithStr( temp, hDlg, id );
}

void WdeSetEditWithSINT32( int_32 val, int base, HWND hDlg, int id )
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
    WdeSetEditWithStr( temp, hDlg, id );
}

void WdeSetComboWithStr( char *cp, HWND hDlg, int id )
{
    WdeSetEditWithStr( cp, hDlg, id );
}

void WdeSetEditWithStr( char *cp, HWND hDlg, int id )
{
    if( cp != NULL ) {
        SendDlgItemMessage( hDlg, id, WM_SETTEXT, 0, (LPARAM)(LPSTR)cp );
    }
}

ControlClass *WdeGetControlClassFromEdit( HWND hDlg, int id, bool *mod )
{
    ControlClass    *cc;
    int             class;
    char            *cp;
    char            *ep;

    cc = NULL;

    cp = WdeGetStrFromEdit( hDlg, id, mod );

    /* find out if the edit field has changed */
    if( mod == NULL || *mod ) {
        if( cp == NULL ) {
            return( NULL );
        }
        class = (int)strtoul( cp, &ep, 0 );
        if( *ep == '\0' ) {
            cc = ResNumToControlClass( (uint_8)class );
        } else if( (class = WdeGetClassFromClassName( cp )) != 0 ) {
            cc = ResNumToControlClass( (uint_8)class );
        } else {
            cc = WdeStrToControlClass( cp );
        }
    }

    if( cp != NULL ) {
        WRMemFree( cp );
    }

    return( cc );
}

void WdeSetEditWithControlClass( ControlClass *cc, HWND hDlg, int id )
{
    char *cp;

    cp = WdeControlClassToStr( cc );
    if( cp != NULL ) {
        WdeSetEditWithStr( cp, hDlg, id );
        WRMemFree( cp );
    }
}

void WdeSetEditWithResNameOr( ResNameOrOrdinal *rp, HWND hDlg, int id )
{
    char *cp;

    if( rp != NULL ) {
        cp = WdeResNameOrOrdinalToStr( rp, 10 );
        if( cp != NULL ) {
            WdeSetEditWithStr( cp, hDlg, id );
            WRMemFree( cp );
        }
    } else {
        WdeSetEditWithStr( "", hDlg, id );
    }
}

void WdeSetEditWithWResID( WResID *ip, HWND hDlg, int id )
{
    char *cp;

    if( ip == NULL ) {
        return;
    }

    cp = WResIDToStr( ip );
    if( cp != NULL ) {
        WdeSetEditWithStr( cp, hDlg, id );
        WRMemFree( cp );
    }
}

void WdeSetEditWithWResHelpID( WResHelpID *ip, HWND hDlg, int id )
{
    char *cp;

    if( ip == NULL ) {
        return;
    }

    cp = WResHelpIDToStr( ip );
    if( cp != NULL ) {
        WdeSetEditWithStr( cp, hDlg, id );
        WRMemFree( cp );
    }
}

ResNameOrOrdinal *WdeGetResNameOrFromEdit( HWND hDlg, int id, bool *mod )
{
    ResNameOrOrdinal *rp;
    char             *cp;

    rp = NULL;

    cp = WdeGetStrFromEdit( hDlg, id, mod );

    /* find out if the edit field has changed */
    if( mod == NULL || *mod ) {
        if( cp == NULL ) {
            return( NULL );
        }
        if( *cp == '\0' ) {
            WRMemFree( cp );
            return( NULL );
        }
        rp = WdeStrToResNameOrOrdinal( cp );
    }

    if( cp != NULL ) {
        WRMemFree( cp );
    }

    return( rp );
}

WResID *WdeGetWResIDFromEdit( HWND hDlg, int id, bool *mod )
{
    WResID  *rp;
    uint_16 ordID;
    char    *cp;
    char    *ep;

    rp = NULL;

    cp = WdeGetStrFromEdit( hDlg, id, mod );

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

WResHelpID *WdeGetWResHelpIDFromEdit( HWND hDlg, int id, bool *mod )
{
    WResHelpID  *rp;
    uint_32     ordID;
    char        *cp;
    char        *ep;

    rp = NULL;

    cp = WdeGetStrFromEdit( hDlg, id, mod );

    /* find out if the edit field has changed */
    if( mod == NULL || *mod ) {
        if( cp == NULL ) {
            return( NULL );
        }
        ordID = (uint_32)strtoul( cp, &ep, 0 );
        if( *ep == '\0' ) {
            rp = WResHelpIDFromNum( ordID );
        } else {
            rp = WResHelpIDFromStr( cp );
        }
    }

    if( cp != NULL ) {
        WRMemFree( cp );
    }

    return( rp );
}

int_16 WdeGetSINT16FromEdit( HWND hDlg, int id, bool *mod )
{
    return( (int_16)WdeGetSINT32FromEdit( hDlg, id, mod ) );
}

int_32 WdeGetSINT32FromEdit( HWND hDlg, int id, bool *mod )
{
    int_32  val;
    char    *cp;
    char    *ep;

    val = 0;

    cp = WdeGetStrFromEdit( hDlg, id, mod );

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
        WRMemFree( cp );
    }

    return( val );
}

uint_16 WdeGetUINT16FromEdit( HWND hDlg, int id, bool *mod )
{
    return( (uint_16)WdeGetUINT32FromEdit( hDlg, id, mod ) );
}

uint_32 WdeGetUINT32FromEdit( HWND hDlg, int id, bool *mod )
{
    uint_32 val;
    char    *cp;
    char    *ep;

    val = 0;

    cp = WdeGetStrFromEdit( hDlg, id, mod );

    /* find out if the edit field has changed */
    if( mod == NULL || *mod ) {
        if( cp == NULL ) {
            return( 0 );
        }
        val = (uint_32)strtoul( cp, &ep, 0 );
        if( *ep != '\0' ) {
            if( mod != NULL ) {
                *mod = FALSE;
            }
            val = 0;
        }
    }

    if( cp != NULL ) {
        WRMemFree( cp );
    }

    return( val );
}
