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
#include "handle.hpp"
#include "reindex.hpp"
#include "defn.hpp"
#include "cache.hpp"


// Silence the compiler warning about taking the "sizeof" a
// class with virtual functions...
#pragma warning 549 9
static Pool DefnRec::_defnPool( DefnPool, sizeof(DefnRec) );
#pragma warning 549 3


DefnList::~DefnList()
/*******************/
{
    DefnRec     *curr;

    // Delete the _tempDefns.  A little tricky 'cos the LList
    // class can't do it for us.
    curr = _tempDefns.First();
    while( curr != NULL ){
        delete curr;
        curr = _tempDefns.Next();
    }
}


void DefnList::AddDefinition( DefnRec * defn )
/********************************************/
{
    DefnRec     *other;

    other = FirstDefnFor( defn->index );
    while( other != NULL ){
        if( other->column == defn->column &&
            other->line == defn->line &&
            other->filenameID == defn->filenameID ){
            break;
        }
        other = NextDefn();
    }
    if( other == NULL ){
        Insert( defn );
    } else {
        delete defn;
    }
}


void DefnList::AddTempDefinition( DefnRec * defn )
/************************************************/
{
    _tempDefns.Append( defn );
}


void DefnList::AcceptDefns()
/**************************/
{
    DefnRec     *curr;
    DefnRec     *other;

    curr = _tempDefns.First();
    while( curr != NULL ){
        other = FirstDefnFor(curr->index);
        while( other != NULL ){
            if( other->column == curr->column &&
                other->line == curr->line &&
                other->filenameID == curr->filenameID ){
                break;
            }
            other = NextDefn();
        }
        if( other == NULL ){
            Insert( curr );
        } else {
            delete curr;
        }
        curr = _tempDefns.Next();
    }

    _tempDefns.Clear();
}


DefnRec *DefnList::First()
/************************/
{
    Hashable    *current;
    int         index;

    current = NULL;

    for( index = 0; index<_numBuckets; index++ ){
        current = _table[index];
        if( current != NULL ){
            _currIndex = index;
            break;
        }
    }
    _current = (DefnRec *) current;
    if( current != NULL ){
        return (DefnRec *) current;
    } else {
        return NULL;
    }
}


DefnRec *DefnList::Next()
/***********************/
{
    DefnRec     *result;
    Hashable    *current;
    int         index;


    current = _current;
    if( current == NULL ){
        return NULL;
    }

    current = current->next;

    if( current == NULL ){
        for( index = _currIndex+1; index < _numBuckets; index++ ){
            current = _table[index];
            if( current != NULL ){
                break;
            }
        }
        _currIndex = index;
    }

    _current = (DefnRec *) current;
    if( current == NULL ){
        result = NULL;
    } else {
        result = (DefnRec *) current;
    }

    return result;
}


DefnRec *DefnList::FirstDefnFor( BRI_SymbolID symbol )
/****************************************************/
{
    DefnRec *   result = NULL;

    _current = _table[Hash(symbol)];
    while( _current != NULL && _current->index < symbol ){
        _current = _current->next;
    }
    if( _current != NULL && _current->index == symbol ){
        result = (DefnRec *) _current;
    } else {
        _current = NULL;
    }

    return result;
}


DefnRec *DefnList::NextDefn()
/***************************/
{
    DefnRec *   result = NULL;
    if( _current != NULL && _current->next != NULL ){
        if( _current->next->index == _current->index ){
            _current = _current->next;
            result = (DefnRec *) _current;
        } else {
            _current = NULL;
        }
    }

    return result;
}


WBool DefnList::SaveTo( CacheOutFile *cache )
/*****************************************/
{
    Hashable    *current;
    DefnRec     *currDefn;
    int         i;

    cache->StartComponent( "Definitions" );
    cache->AddDword( Count() );
    for( i=0; i<_numBuckets; i++ ){
        current = _table[i];
        while( current != NULL ){
            currDefn = (DefnRec *) current;
            cache->AddDword( currDefn->index );
            cache->AddDword( currDefn->column );
            cache->AddDword( currDefn->line );
            cache->AddDword( currDefn->filenameID );
            current = current->next;
        }
    }
    cache->EndComponent();
    return TRUE;
}


WBool DefnList::LoadFrom( CacheInFile *cache )
/******************************************/
{
    WBool       result;
    DefnRec     *newDefn;
    int         defnCount;

    result = cache->OpenComponent( "Definitions" );

    if( !result ){
        return result;
    }

    cache->ReadDword( &defnCount );
    while( defnCount > 0 ){
        newDefn = new DefnRec;
        cache->ReadDword( &newDefn->index );
        cache->ReadDword( &newDefn->column );
        cache->ReadDword( &newDefn->line );
        cache->ReadDword( &newDefn->filenameID );
        Insert( newDefn );
        defnCount--;
    }
    cache->CloseComponent();
    return result;
}
