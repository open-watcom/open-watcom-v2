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


#include <string.h>
#include "wresall.h"
#include "reserr.h"
#include "wresrtns.h"


static WResTypeNode *newTypeNode( const WResID *type )
{
    WResTypeNode        *newnode;
    int                 extrabytes;

    extrabytes = WResIDExtraBytes( type );
    newnode = WRESALLOC( sizeof( WResTypeNode ) + extrabytes );
    if (newnode != NULL) {
        newnode->Next = NULL;
        newnode->Prev = NULL;
        newnode->Head = NULL;
        newnode->Tail = NULL;
        newnode->Info.NumResources = 0;
        memcpy( &(newnode->Info.TypeName), type, sizeof(WResID) + extrabytes );
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

static WResResNode *newResNode( const WResID *name )
{
    WResResNode         *newnode;
    int                 extrabytes;

    extrabytes = WResIDExtraBytes( name );
    newnode = WRESALLOC( sizeof( WResResNode ) + extrabytes );
    if( newnode == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
    } else {
        newnode->Next = NULL;
        newnode->Prev = NULL;
        newnode->Head = NULL;
        newnode->Tail = NULL;
        newnode->Info.NumResources = 0;
        memcpy( &(newnode->Info.ResName), name, sizeof(WResID) + extrabytes );
    }

    return( newnode );
}

/*
 * WResAddResource - Add the new entry to the directory. If type is NULL the
 *                   default is used.  If the entry is already in the
 *                   directory don't add anything, set duplicate TRUE
 *                   and return an error. Return is TRUE if any error has
 *                   occured (including duplicate entry)
 */
bool WResAddResource( const WResID *type, const WResID *name,
                    uint_16 memflags, long offset, uint_32 length,
                    WResDir currdir, const WResLangType *lang,
                    bool *duplicate )
/************************************************************/
{
    bool                rc;
    WResDirWindow       dup;

    rc = WResAddResource2( type, name, memflags, offset, length, currdir,
                             lang, &dup, NULL );
    if( duplicate != NULL ) {
        *duplicate = !WResIsEmptyWindow( dup );
    }
    return( rc );
}

bool WResAddResource2( const WResID *type, const WResID *name,
                    uint_16 memflags, long offset, uint_32 length,
                    WResDir currdir, const WResLangType *lang,
                    WResDirWindow *duplicate, void *fileinfo )
/************************************************************/
{

    WResTypeNode        *currtype;
    WResResNode         *currres;
    WResLangNode        *currlang;

    /* set duplicate FALSE so other errors will have it set correctly */
    if( duplicate != NULL ) {
        WResSetEmptyWindow( duplicate );
    }
    currres = NULL;

    currtype = __FindType( type, currdir );
    if (currtype != NULL) {
        /* if the type is in there already check for a duplicate resource */
        currres = __FindRes( name, currtype );
        if (currres != NULL) {
            currlang = __FindLang( lang, currres );
            if( currlang != NULL ) {
                if( duplicate != NULL ) {
                    WResMakeWindow( duplicate, currtype, currres, currlang );
                }
                WRES_ERROR( WRS_DUP_ENTRY )
                return( true );
            }
        }
    }
    if( currtype == NULL ) {
        /* otherwise add the type to the list */
        currtype = newTypeNode( type );
        if (currtype == NULL) {
            return( true );
        }
        ResAddLLItemAtEnd( (void**)&(currdir->Head), (void**)&(currdir->Tail), currtype );
        /* adjust the count of the number of types */
        currdir->NumTypes += 1;
    }

    if( currres  == NULL ) {
        /* add the resource to the current type */
        currres = newResNode( name );
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
