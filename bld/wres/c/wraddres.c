/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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


#include <string.h>
#include "wresall.h"
#include "reserr.h"
#include "wresrtns.h"


static size_t WResIDSize( const WResID *id )
/******************************************/
{
    size_t  size;

    if( id->IsName ) {
        size = offsetof( WResID, ID.Name.Name ) + id->ID.Name.NumChars;
    } else {
        size = offsetof( WResID, ID.Num ) + sizeof( id->ID.Num );
    }
    return( size );
}

static WResTypeNode *newTypeNode( const WResID *type_id )
{
    WResTypeNode        *newnode;
    size_t              id_size;

    id_size = WResIDSize( type_id );
    newnode = WRESALLOC( offsetof( WResTypeNode, Info.TypeName ) + id_size );
    if( newnode != NULL ) {
        newnode->Next = NULL;
        newnode->Prev = NULL;
        newnode->Head = NULL;
        newnode->Tail = NULL;
        newnode->Info.NumResources = 0;
        memcpy( &(newnode->Info.TypeName), type_id, id_size );
    } else {
        WRES_ERROR( WRS_MALLOC_FAILED );
    }
    return( newnode );
}

static WResLangNode *newLangNode( uint_16 memflags, uint_32 offset,
                                  uint_32 length, const WResLangType *lang,
                                  void *fileinfo )
{
    WResLangNode        *newnode;

    newnode = WRESALLOC( sizeof( WResLangNode ) );
    if( newnode == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
    } else {
        newnode->Next = NULL;
        newnode->Prev = NULL;
        newnode->data = NULL;
        newnode->fileInfo = fileinfo;
        newnode->Info.MemoryFlags = memflags;
        newnode->Info.Offset = offset;
        newnode->Info.Length = length;
        if( lang == NULL ) {
            newnode->Info.lang.lang = DEF_LANG;
            newnode->Info.lang.sublang = DEF_SUBLANG;
        } else {
            newnode->Info.lang = *lang;
        }
    }
    return( newnode );
}

static WResResNode *newResNode( const WResID *res_id )
{
    WResResNode         *newnode;
    size_t              id_size;

    id_size = WResIDSize( res_id );
    newnode = WRESALLOC( offsetof( WResResNode, Info.ResName ) + id_size );
    if( newnode == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
    } else {
        newnode->Next = NULL;
        newnode->Prev = NULL;
        newnode->Head = NULL;
        newnode->Tail = NULL;
        newnode->Info.NumResources = 0;
        memcpy( &(newnode->Info.ResName), res_id, id_size );
    }

    return( newnode );
}

/*
 * WResAddResource - Add the new entry to the directory. If type is NULL the
 *                   default is used.  If the entry is already in the
 *                   directory don't add anything, set duplicate true
 *                   and return an error. Return is true if any error has
 *                   occured (including duplicate entry)
 */
bool WResAddResource( const WResID *type_id, const WResID *res_id,
                    uint_16 memflags, long offset, uint_32 length,
                    WResDir currdir, const WResLangType *lang,
                    bool *duplicate )
/************************************************************/
{
    bool                rc;
    WResDirWindow       dup;

    rc = WResAddResource2( type_id, res_id, memflags, offset, length, currdir,
                             lang, &dup, NULL );
    if( duplicate != NULL ) {
        *duplicate = !WResIsEmptyWindow( dup );
    }
    return( rc );
}

bool WResAddResource2( const WResID *type_id, const WResID *res_id,
                    uint_16 memflags, long offset, uint_32 length,
                    WResDir currdir, const WResLangType *lang,
                    WResDirWindow *duplicate, void *fileinfo )
/************************************************************/
{

    WResTypeNode        *currtype;
    WResResNode         *currres;
    WResLangNode        *currlang;

    /* set duplicate false so other errors will have it set correctly */
    if( duplicate != NULL ) {
        WResSetEmptyWindow( duplicate );
    }
    currres = NULL;

    currtype = __FindType( type_id, currdir );
    if( currtype != NULL ) {
        /* if the type is in there already check for a duplicate resource */
        currres = __FindRes( res_id, currtype );
        if( currres != NULL ) {
            currlang = __FindLang( lang, currres );
            if( currlang != NULL ) {
                if( duplicate != NULL )
                    WResMakeWindow( duplicate, currtype, currres, currlang );
                return( WRES_ERROR( WRS_DUP_ENTRY ) );
            }
        }
    }
    if( currtype == NULL ) {
        /* otherwise add the type to the list */
        currtype = newTypeNode( type_id );
        if( currtype == NULL ) {
            return( true );
        }
        ResAddLLItemAtEnd( (void**)&(currdir->Head), (void**)&(currdir->Tail), currtype );
        /* adjust the count of the number of types */
        currdir->NumTypes += 1;
    }

    if( currres  == NULL ) {
        /* add the resource to the current type */
        currres = newResNode( res_id );
        if( currres == NULL ) {
            return( true );
        }
        ResAddLLItemAtEnd( (void**)&(currtype->Head), (void**)&(currtype->Tail), currres );
        /* adjust the counts of the number of resources */
        currtype->Info.NumResources += 1;
        currdir->NumResources += 1;
    }
    currlang = newLangNode( memflags, offset, length, lang, fileinfo );
    if( currlang == NULL ) {
        return( true );
    }
    ResAddLLItemAtEnd( (void**)&(currres->Head), (void**)&(currres->Tail), currlang );
    currres->Info.NumResources ++;

    /* no error has occured */
    return( false );
}
