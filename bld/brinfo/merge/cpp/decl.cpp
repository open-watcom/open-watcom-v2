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


#include "wpch.hpp"
#include "decl.hpp"
#include "cache.hpp"
#include "reindex.hpp"
#include "handle.hpp"

// Silence the compiler warning about taking the "sizeof" a
// class with virtual functions...
#pragma warning 549 9
static Pool DeclRec::_declPool( DeclPool, sizeof(DeclRec) );
#pragma warning 549 3

extern Browser  *CurrBrowser;

DeclList::DeclList()
/******************/
{
    _currIndex = -1;
    _currLink = NULL;
}


DeclList::~DeclList()
/*******************/
{
    ClearAndDelete();
}


DeclRec * DeclList::First()
/*************************/
{
    Hashable    *current;
    int         index;

    current = NULL;

    for( index=0; index<_numBuckets; index++ ){
        current = _table[index];
        if( current != NULL ){
            _currIndex = index;
            break;
        }
    }
    _currLink = (DeclRec *) current;
    if( current != NULL ){
        return (DeclRec *) current;
    } else {
        return NULL;
    }
}


DeclRec * DeclList::Next()
/************************/
{
    DeclRec     *result;
    Hashable    *current;
    int         index;

    current = _currLink;
    if( current == NULL ){
        return NULL;
    }

    current = current->next;

    if( current == NULL ){
        for( index=_currIndex+1; index < _numBuckets; index++ ){
            current = _table[index];
            if( current != NULL ){
                break;
            }
        }
        _currIndex = index;
    }

    _currLink = (DeclRec *) current;
    if( current == NULL ){
        result = NULL;
    } else {
        result = (DeclRec *) current;
    }

    return result;
}


WBool DeclList::SaveTo( CacheOutFile *cache )
/*****************************************/
{
    Hashable    *current;
    DeclRec     *currDecl;
    int         i;

    cache->StartComponent( "Declarations" );
    cache->AddDword( Count() );
    for( i=0; i<_numBuckets; i++ ){
        current = _table[i];
        while( current != NULL ){
            currDecl = (DeclRec *) current;
            cache->AddDword( currDecl->index );
            cache->AddData( &currDecl->attribs, BRI_SIZE_SYMBOLATTRIBUTES );
            cache->AddDword( currDecl->nameID );
            cache->AddDword( currDecl->typeID );
            cache->AddDword( currDecl->enclosing );
            cache->AddDword( indexOf( currDecl->_nextDecl ) );
            current = current->next;
        }
    }
    cache->EndComponent();
    return TRUE;
}


WBool DeclList::LoadFrom( CacheInFile *cache )
/******************************************/
{
    WBool       result;
    DeclRec     *newDecl;
    Hashable    *current;
    ReOrdering  *declIndex;
    int         declCount;
    uint_32     id;
    int         i;

    result = cache->OpenComponent( "Declarations" );

    if( !result ){
        return result;
    }

    declIndex = CurrBrowser->declIndex();
    cache->ReadDword( &declCount );
    while( declCount > 0 ){
        newDecl = new DeclRec;
        cache->ReadDword( &newDecl->index );
        declIndex->ChangeTo( newDecl->index, (uint_32) newDecl );
        newDecl->attribs = (BRI_SymbolAttributes) 0;
        cache->ReadData( &newDecl->attribs, BRI_SIZE_SYMBOLATTRIBUTES );
        cache->ReadDword( &newDecl->nameID );
        cache->ReadDword( &newDecl->typeID );
        cache->ReadDword( &newDecl->enclosing );
        cache->ReadDword( &newDecl->_nextDecl );
        Insert( newDecl );
        declCount--;
    }

    // Correct the values of the _nextDecl pointers
    for( i=0; i<_numBuckets; i++ ){
        current = _table[i];
        while( current != NULL ){
            newDecl = (DeclRec *) current;
            id = declIndex->NewId( (uint_32) newDecl->_nextDecl );
            newDecl->_nextDecl = (DeclRec *) id;
            current = current->next;
        }
    }

    cache->CloseComponent();
    return result;
}
