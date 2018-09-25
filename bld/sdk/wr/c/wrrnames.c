/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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


#include "wrglbl.h"
#include "wrstrdup.h"
#include "wrmsg.h"
#include "wresdefn.h"


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

bool WRAPI WRSetLBoxWithStr( HWND lbox, const char *str, void *data )
{
    bool        ok;
    LRESULT     index;

    ok = (lbox != (HWND)NULL && str != NULL);

    if( ok ) {
        index = SendMessage( lbox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)str );
        ok = ( index != LB_ERR && index != LB_ERRSPACE );
    }

    if( ok ) {
        ok = ( SendMessage( lbox, LB_SETITEMDATA, (WPARAM)index, (LPARAM)data ) != LB_ERR );
    }

    return( ok );
}

static bool WRSetLBoxWithLangNode( HWND lbox, WResResNode *rnode,
                                  WResLangNode *lnode, uint_16 type_id )
{
    bool        ok;
    char        *cp;

    cp = NULL;

    ok = (lbox != (HWND)NULL && rnode != NULL && lnode != NULL);

    ok = ok && ((cp = WRGetResName( rnode, type_id )) != NULL);

    ok = ok && WRSetLBoxWithStr( lbox, cp, lnode );

    if( cp != NULL ) {
        MemFree( cp );
    }

    return( ok );
}

static bool WRSetLBoxWithResNode( HWND lbox, WResResNode *rnode, uint_16 type_id )
{
    WResLangNode        *lnode;
    bool                ok;

    ok = (lbox != (HWND)NULL && rnode != NULL);
    if( ok ) {
        for( lnode = rnode->Head; ok && lnode != NULL; lnode = lnode->Next ) {
            ok = WRSetLBoxWithLangNode( lbox, rnode, lnode, type_id );
            if( lnode == rnode->Tail ) {
                break;
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
    uint_16     type_id;

    type_id = 0;
    ok = (lbox != (HWND)NULL && tnode != NULL);

    if( ok ) {
        if( !tnode->Info.TypeName.IsName ) {
            type_id = tnode->Info.TypeName.ID.Num;
        }
        SendMessage( lbox, WM_SETREDRAW, FALSE, 0 );
        if( ok && type_id == RESOURCE2INT( RT_STRING ) ) {
            str = WRAllocRCString( WR_ALLSTRINGS );
            if( str != NULL ) {
                ok = WRSetLBoxWithStr( lbox, str, NULL );
                WRFreeRCString( str );
            } else {
                ok = false;
            }
        } else {
            for( rnode = tnode->Head; ok && rnode != NULL; rnode = rnode->Next ) {
                ok = WRSetLBoxWithResNode( lbox, rnode, type_id );
                if( rnode == tnode->Tail ) {
                    break;
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

char * WRAPI WRGetResName( WResResNode *rnode, uint_16 type_id )
{
    WResID  *id;
    int     num;
    char    *str;
    char    *text;
    char    *cp;

    cp = NULL;

    if( rnode != NULL ) {
        id = &rnode->Info.ResName;
        if( type_id == RESOURCE2INT( RT_STRING ) ) {
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
