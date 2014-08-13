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


#include <wwindows.h>
#include <stdio.h>
#include <string.h>
#include "watcom.h"
#include "wrglbl.h"
#include "wrstrdup.h"
#include "wrmsg.h"

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

bool WRAPI WRSetLBoxWithStr( HWND lbox, char *str, void *data )
{
    bool        ok;
    LRESULT     index;

    ok = (lbox != (HWND)NULL && str != NULL);

    if( ok ) {
        index = SendMessage( lbox, LB_ADDSTRING, 0, (LPARAM)(LPSTR)str );
        ok = (index != LB_ERR && index != LB_ERRSPACE);
    }

    if( ok ) {
        SendMessage( lbox, LB_SETITEMDATA, index, (LPARAM)data );
        ok = (index != LB_ERR);
    }

    return( ok );
}

static bool WRSetLBoxWithLangNode( HWND lbox, WResResNode *rnode,
                                  WResLangNode *lnode, uint_16 type )
{
    bool        ok;
    char        *cp;

    cp = NULL;

    ok = (lbox != (HWND)NULL && rnode != NULL && lnode != NULL);

    ok = ok && ((cp = WRGetResName( rnode, type )) != NULL);

    ok = ok && WRSetLBoxWithStr( lbox, cp, lnode );

    if( cp != NULL ) {
        MemFree( cp );
    }

    return( ok );
}

static bool WRSetLBoxWithResNode( HWND lbox, WResResNode *rnode, int type )
{
    WResLangNode        *lnode;
    bool                ok;

    ok = (lbox != (HWND)NULL && rnode != NULL);

    if( ok ) {
        lnode = rnode->Head;
        while( ok && lnode != NULL ) {
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

bool WRAPI WRSetResNamesFromTypeNode( HWND lbox, WResTypeNode *tnode )
{
    WResResNode *rnode;
    char        *str;
    bool        ok;
    int         type;

    ok = (lbox != (HWND)NULL && tnode != NULL);

    if( ok ) {
        if( !tnode->Info.TypeName.IsName ) {
            type = tnode->Info.TypeName.ID.Num;
        } else {
            type = 0;
        }
        SendMessage( lbox, WM_SETREDRAW, FALSE, 0 );
        rnode = tnode->Head;
        if( ok && type == (uint_16)(pointer_int)(pointer_int)RT_STRING ) {
            str = WRAllocRCString( WR_ALLSTRINGS );
            if( str != NULL ) {
                ok = WRSetLBoxWithStr( lbox, str, NULL );
                WRFreeRCString( str );
            } else {
                ok = false;
            }
        } else {
            while( ok && rnode != NULL ) {
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

char * WRAPI WRGetResName( WResResNode *rnode, uint_16 type )
{
    WResID  *id;
    int     num;
    char    *str;
    char    *text;
    char    *cp;

    cp = NULL;

    if( rnode != NULL ) {
        id = &rnode->Info.ResName;
        if( type == (uint_16)(pointer_int)RT_STRING ) {
            num = id->ID.Num;
            if( num != 0 ) {
                text = WRAllocRCString( WR_STRINGIDS );
                if( text != NULL ) {
                    // alloc string large enough for 'text' and 20 digits
                    str = (char *)MemAlloc( strlen( text ) + 21 );
                    if( str != NULL ) {
                        sprintf( str, text, (num - 1) * 16, num * 16 - 1 );
                        cp = WRStrDup( str );
                        MemFree( str );
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
