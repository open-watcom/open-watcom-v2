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
#include "wrglbl.h"
#include "wrfindt.h"
#include "wrmem.h"

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

int WR_EXPORT WRDoesNameExist( WResDir dir, WResID *type, WResID *res )
{
    WResTypeNode        *type_node;
    WResResNode         *res_node;

    type_node = WRFindTypeNodeFromWResID( dir, type );
    if( type_node ) {
        res_node = WRFindResNodeFromWResID( type_node, res );
        if( res_node ) {
            return( TRUE );
        }
    }

    return( FALSE );
}

WResTypeNode * WR_EXPORT WRFindTypeNodeFromWResID( WResDir dir, WResID *type )
{
    WResTypeNode *type_node;
    char         *type_name;

    type_node = NULL;

    if ( dir && type ) {
        if ( type->IsName ) {
            type_name = WResIDToStr ( type );
            if ( type_name ) {
                type_node = WRFindTypeNode ( dir, 0, type_name );
                WRMemFree ( type_name );
            }
        } else {
            type_node = WRFindTypeNode ( dir, type->ID.Num, NULL );
        }
    }

    return ( type_node );
}

WResResNode * WR_EXPORT WRFindResNodeFromWResID ( WResTypeNode *type,
                                                  WResID *res )
{
    WResResNode *res_node;
    char        *res_name;

    res_node = NULL;

    if ( type && res ) {
        if ( res->IsName ) {
            res_name = WResIDToStr ( res );
            if ( res_name ) {
                res_node = WRFindResNode ( type, 0, res_name );
                WRMemFree ( res_name );
            }
        } else {
            res_node = WRFindResNode ( type, res->ID.Num, NULL );
        }
    }

    return ( res_node );
}

WResLangNode * WR_EXPORT WRFindLangNodeFromLangType ( WResResNode *rnode,
                                                      WResLangType *lang )
{
    WResLangNode *lnode;

    if ( rnode && lang ) {
        lnode = rnode->Head;
        while ( lnode ) {
            if ( ( lnode->Info.lang.lang == lang->lang )  &&
                 ( lnode->Info.lang.sublang == lang->sublang ) ) {
                return ( lnode );
            }
            if ( lnode == rnode->Tail ) {
                break;
            }
            lnode = lnode->Next;
        }
    }

    return ( NULL );
}

WResTypeNode * WR_EXPORT WRFindTypeNode ( WResDir dir, uint_16 type,
                                          char *type_name )
{
    WResTypeNode *type_node;

    if ( dir == NULL ) {
        return ( NULL );
    }

    type_node = dir->Head;

    while (type_node != NULL) {

        if (WRIsCorrectNode (&type_node->Info.TypeName, type, type_name)) {
            return ( type_node );
        }

        if ( type_node == dir->Tail ) {
            break;
        }

        type_node = type_node->Next;
    }

    return ( NULL );

}

WResResNode * WR_EXPORT WRFindResNode ( WResTypeNode *type, uint_16 res,
                                        char *res_name )
{
    WResResNode *res_node;

    if ( type == NULL ) {
        return ( NULL );
    }

    res_node = type->Head;

    while ( res_node != NULL ) {

        if (WRIsCorrectNode (&res_node->Info.ResName, res, res_name)) {
            return ( res_node );
        }

        if ( res_node == type->Tail ) {
            break;
        }

        res_node = res_node->Next;
    }

    return ( NULL );

}

int WRIsCorrectNode ( WResID *node, uint_16 id, char *name )
{
    char *type_name;
    int  ret;

    ret = FALSE;

    if ( !node->IsName ) {
        if ( node->ID.Num == id ) {
            ret = TRUE;
        }
    } else if ( name ) {
        type_name = WResIDToStr ( node );
        if ( type_name ) {
            if ( !stricmp ( type_name, name ) ) {
                ret = TRUE;
            }
            WRMemFree ( type_name );
        }
    }

    return ( ret );
}

