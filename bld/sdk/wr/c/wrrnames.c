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
#include <stdio.h>
#include <string.h>
#include "win1632.h"
#include "wrglbl.h"
#include "wrinfo.h"
#include "wrmem.h"
#include "wrstrdup.h"
#include "wrmsg.h"
#include "wrcmsg.h"
#include "wrrnames.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

int WR_EXPORT WRSetLBoxWithStr( HWND lbox, char *str, void *data )
{
    int         ok;
    LRESULT     index;

    ok = ( ( lbox != (HWND)NULL ) && str );

    if( ok ) {
        index = SendMessage( lbox, LB_ADDSTRING, 0, (LPARAM) (LPCSTR) str );
        ok = ( ( index != LB_ERR ) && ( index != LB_ERRSPACE ) );
    }

    if( ok ) {
        SendMessage( lbox, LB_SETITEMDATA, index, (LPARAM)data );
        ok = ( index != LB_ERR );
    }

    return( ok );
}

static int WRSetLBoxWithLangNode( HWND lbox, WResResNode *rnode,
                                  WResLangNode *lnode, uint_16 type )
{
    int         ok;
    char        *cp;

    cp = NULL;

    ok = ( ( lbox != (HWND)NULL ) && rnode && lnode );

    ok = ok && ( ( cp = WRGetResName( rnode, type ) ) != NULL );

    ok = ok && WRSetLBoxWithStr( lbox, cp, lnode );

    if( cp ) {
        WRMemFree( cp );
    }

    return ( ok );
}

static int WRSetLBoxWithResNode( HWND lbox, WResResNode *rnode, int type )
{
    WResLangNode        *lnode;
    int                 ok;

    ok = ( ( lbox != (HWND)NULL ) && rnode );

    if( ok ) {
        lnode = rnode->Head;
        while( ok && lnode ) {
            ok = WRSetLBoxWithLangNode( lbox, rnode, lnode, type );
            if( lnode == rnode->Tail ) {
                lnode = NULL;
            } else {
                lnode = lnode->Next;
            }
        }
    }

    return( ok );
}

int WR_EXPORT WRSetResNamesFromTypeNode( HWND lbox, WResTypeNode *tnode )
{
    WResResNode *rnode;
    char        *str;
    int         ok;
    int         type;

    ok = ( ( lbox != (HWND)NULL ) && tnode );

    if( ok ) {
        if( !tnode->Info.TypeName.IsName ) {
            type = tnode->Info.TypeName.ID.Num;
        } else {
            type = 0;
        }
        SendMessage( lbox, WM_SETREDRAW, FALSE, 0 );
        rnode = tnode->Head;
        if( ok && ( type == (uint_16)RT_STRING ) ) {
            str = WRAllocRCString( WR_ALLSTRINGS );
            if( str ) {
                ok = WRSetLBoxWithStr( lbox, str, NULL );
                WRFreeRCString( str );
            } else {
                ok = FALSE;
            }
        } else {
            while( ok && rnode ) {
                ok = WRSetLBoxWithResNode( lbox, rnode, type );
                if( rnode == tnode->Tail ) {
                    rnode = NULL;
                } else {
                    rnode = rnode->Next;
                }
            }
        }
        if( ok ) {
            SendMessage( lbox, WM_SETREDRAW, TRUE, 0 );
            InvalidateRect( lbox, NULL, TRUE );
        } else {
            SendMessage( lbox, LB_RESETCONTENT, 0, 0 );
        }
    }

    return( ok );
}

char * WR_EXPORT WRGetResName( WResResNode *rnode, uint_16 type )
{
    WResID   *id;
    int       num;
    char     *str;
    char     *text;
    char     *cp;

    cp = NULL;

    if( rnode ) {
        id = &(rnode->Info.ResName);
        if( type == (uint_16)RT_STRING ) {
            num = id->ID.Num;
            if( num ) {
                text = WRAllocRCString( WR_STRINGIDS );
                if( text ) {
                    // alloc string large enough for 'text' and 20 digits
                    str = (char *)WRMemAlloc( strlen(text) + 21 );
                    if( str ) {
                        sprintf( str, text, (num-1)*16, num*16-1 );
                        cp = WRStrDup( str );
                        WRMemFree( str );
                    }
                    WRFreeRCString( text );
                }
            }
        } else {
            cp = WResIDToStr( id );
        }
    }

    return( cp );
}

