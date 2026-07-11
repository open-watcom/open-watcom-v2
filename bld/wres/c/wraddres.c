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
    WResTypeNode        *typenode;
    size_t              id_size;

    id_size = WResIDSize( type_id );
    typenode = WRESALLOC( offsetof( WResTypeNode, Info.TypeName ) + id_size );
    if( typenode != NULL ) {
        typenode->Next = NULL;
        typenode->Prev = NULL;
        typenode->Head = NULL;
        typenode->Tail = NULL;
        typenode->Info.NumResources = 0;
        memcpy( &(typenode->Info.TypeName), type_id, id_size );
    } else {
        WRES_ERROR( WRS_MALLOC_FAILED );
    }
    return( typenode );
}

static WResLangNode *newLangNode( uint_16 memflags, uint_32 offset,
                                  uint_32 length, const WResLangType *lang,
                                  void *fileinfo )
{
    WResLangNode        *langnode;

    langnode = WRESALLOC( sizeof( WResLangNode ) );
    if( langnode == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
    } else {
        langnode->Next = NULL;
        langnode->Prev = NULL;
        langnode->data = NULL;
        langnode->fileInfo = fileinfo;
        langnode->Info.MemoryFlags = memflags;
        langnode->Info.Offset = offset;
        langnode->Info.Length = length;
        if( lang == NULL ) {
            langnode->Info.lang.lang = DEF_LANG;
            langnode->Info.lang.sublang = DEF_SUBLANG;
        } else {
            langnode->Info.lang = *lang;
        }
    }
    return( langnode );
}

static WResResNode *newResNode( const WResID *res_id )
{
    WResResNode         *resnode;
    size_t              id_size;

    id_size = WResIDSize( res_id );
    resnode = WRESALLOC( offsetof( WResResNode, Info.ResName ) + id_size );
    if( resnode == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
    } else {
        resnode->Next = NULL;
        resnode->Prev = NULL;
        resnode->Head = NULL;
        resnode->Tail = NULL;
        resnode->Info.NumResources = 0;
        memcpy( &(resnode->Info.ResName), res_id, id_size );
    }

    return( resnode );
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
                    WResDir dir, const WResLangType *lang,
                    bool *duplicate )
/************************************************************/
{
    bool                rc;
    WResDirWindow       dup;

    rc = WResAddResource2( type_id, res_id, memflags, offset, length, dir,
                             lang, &dup, NULL );
    if( duplicate != NULL ) {
        *duplicate = !WResIsEmptyWindow( dup );
    }
    return( rc );
}

bool WResAddResource2( const WResID *type_id, const WResID *res_id,
                    uint_16 memflags, long offset, uint_32 length,
                    WResDir dir, const WResLangType *lang,
                    WResDirWindow *duplicate, void *fileinfo )
/************************************************************/
{

    WResTypeNode        *typenode;
    WResResNode         *resnode;
    WResLangNode        *langnode;

    /* set duplicate false so other errors will have it set correctly */
    if( duplicate != NULL ) {
        WResSetEmptyWindow( duplicate );
    }
    resnode = NULL;

    typenode = __FindType( type_id, dir );
    if( typenode != NULL ) {
        /* if the type is in there already check for a duplicate resource */
        resnode = __FindRes( res_id, typenode );
        if( resnode != NULL ) {
            langnode = __FindLang( lang, resnode );
            if( langnode != NULL ) {
                if( duplicate != NULL )
                    WResMakeWindow( duplicate, typenode, resnode, langnode );
                return( WRES_ERROR( WRS_DUP_ENTRY ) );
            }
        }
    }
    if( typenode == NULL ) {
        /* otherwise add the type to the list */
        typenode = newTypeNode( type_id );
        if( typenode == NULL ) {
            return( true );
        }
        ResAddLLItemAtEnd( (void**)&(dir->Head), (void**)&(dir->Tail), typenode );
        /* adjust the count of the number of types */
        dir->NumTypes += 1;
    }

    if( resnode  == NULL ) {
        /* add the resource to the current type */
        resnode = newResNode( res_id );
        if( resnode == NULL ) {
            return( true );
        }
        ResAddLLItemAtEnd( (void**)&(typenode->Head), (void**)&(typenode->Tail), resnode );
        /* adjust the counts of the number of resources */
        typenode->Info.NumResources += 1;
        dir->NumResources += 1;
    }
    langnode = newLangNode( memflags, offset, length, lang, fileinfo );
    if( langnode == NULL ) {
        return( true );
    }
    ResAddLLItemAtEnd( (void**)&(resnode->Head), (void**)&(resnode->Tail), langnode );
    resnode->Info.NumResources ++;

    /* no error has occured */
    return( false );
}
