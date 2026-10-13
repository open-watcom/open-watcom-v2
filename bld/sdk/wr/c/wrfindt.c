/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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

/* forward declaration */
static bool WRIsCorrectNode( WResID *node, uint_16 id, const char *name );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

bool WRAPI WRDoesNameExist( WResDir dir, WResID *type, WResID *res )
{
    WResTypeNode        *typenode;
    WResResNode         *resnode;

    typenode = WRFindTypeNodeFromWResID( dir, type );
    if( typenode != NULL ) {
        resnode = WRFindResNodeFromWResID( typenode, res );
        if( resnode != NULL ) {
            return( true );
        }
    }

    return( false );
}

WResTypeNode * WRAPI WRFindTypeNodeFromWResID( WResDir dir, WResID *type )
{
    WResTypeNode *typenode;
    char         *type_name;

    typenode = NULL;

    if( dir != NULL && type != NULL ) {
        if( type->IsName ) {
            type_name = WResIDToStr( type );
            if( type_name != NULL ) {
                typenode = WRFindTypeNode( dir, 0, type_name );
                MemFree( type_name );
            }
        } else {
            typenode = WRFindTypeNode( dir, type->ID.Num, NULL );
        }
    }

    return( typenode );
}

WResResNode * WRAPI WRFindResNodeFromWResID( WResTypeNode *typenode, WResID *res )
{
    WResResNode *resnode;
    char        *res_name;

    resnode = NULL;

    if( typenode != NULL && res != NULL ) {
        if( res->IsName ) {
            res_name = WResIDToStr( res );
            if( res_name != NULL ) {
                resnode = WRFindResNode( typenode, 0, res_name );
                MemFree( res_name );
            }
        } else {
            resnode = WRFindResNode( typenode, res->ID.Num, NULL );
        }
    }

    return( resnode );
}

WResLangNode *WRAPI WRFindLangNodeFromLangType( WResResNode *resnode, WResLangType *lang )
{
    WResLangNode *langnode;

    if( resnode != NULL && lang != NULL ) {
        for( langnode = resnode->Head; langnode != NULL; langnode = langnode->next ) {
            if( langnode->Info.lang.lang == lang->lang  &&
                langnode->Info.lang.sublang == lang->sublang ) {
                return( langnode );
            }
            if( langnode == resnode->Tail ) {
                break;
            }
        }
    }

    return( NULL );
}

WResTypeNode * WRAPI WRFindTypeNode( WResDir dir, uint_16 type, const char *type_name )
{
    WResTypeNode *typenode;

    if( dir == NULL ) {
        return( NULL );
    }

    for( typenode = dir->Head; typenode != NULL; typenode = typenode->next ) {
        if( WRIsCorrectNode( &typenode->Info.TypeName, type, type_name ) ) {
            return( typenode );
        }
        if( typenode == dir->Tail ) {
            break;
        }
    }

    return( NULL );

}

WResResNode * WRAPI WRFindResNode( WResTypeNode *typenode, uint_16 res, const char *res_name )
{
    WResResNode *resnode;

    if( typenode == NULL ) {
        return( NULL );
    }

    for( resnode = typenode->Head; resnode != NULL; resnode = resnode->next ) {
        if( WRIsCorrectNode( &resnode->Info.ResName, res, res_name ) ) {
            return( resnode );
        }
        if( resnode == typenode->Tail ) {
            break;
        }
    }

    return( NULL );

}

static bool WRIsCorrectNode( WResID *node, uint_16 id, const char *name )
{
    char *type_name;
    bool ret;

    ret = false;

    if( !node->IsName ) {
        if( node->ID.Num == id ) {
            ret = true;
        }
    } else if( name != NULL ) {
        type_name = WResIDToStr( node );
        if( type_name != NULL ) {
            if( stricmp( type_name, name ) == 0 ) {
                ret = true;
            }
            MemFree( type_name );
        }
    }

    return( ret );
}
