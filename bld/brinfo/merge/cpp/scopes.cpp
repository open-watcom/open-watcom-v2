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
#include "scopes.hpp"
#include "decl.hpp"
#include "types.hpp"
#include "reindex.hpp"
#include "cache.hpp"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

extern  Browser *               CurrBrowser;


// Silence the compiler warning about taking the "sizeof" a
// class with virtual functions...
#pragma warning 549 9
static Pool ScopeRec::_scopePool( ScopePool, sizeof(ScopeRec), 0x40 );
#pragma warning 549 3


ScopeTable::ScopeTable()
    : HashTable<ScopeRec>( 67 )         // a nice small prime number
/*****************************/
{
    _head = NULL;
    _current = NULL;
    _state = USE;
}


ScopeTable::~ScopeTable()
/***********************/
{
    // empty
}


ScopeRec * ScopeTable::OpenScope(ScopeRec *scope, BRI_StringID fnName,
                                 BRI_TypeID fnType)
/********************************************************************/
{
    ScopeRec *  child;
    BRI_ScopeID trueID;
    int         isOldScope = FALSE;

    BRI_SymbolID        oldSym;
    BRI_TypeID          typeID;
    DeclRec             *declRec;
    DeclRec             *firstDecl;

    WAssert( _state == USE );

    // First, have we already opened this scope in the
    // *current* .BRM file?
    trueID = CurrBrowser->scopeIndex()->NewId( scope->index );
    if( trueID != (BRI_ScopeID) 0x0 ){
        delete scope;
        scope = Lookup( trueID );
        goto done;
    }

    // Second, are we trying to open file scope again?
    if( scope->flags == BRI_ST_File && _head != NULL ){
        delete scope;
        scope = _head;
        goto done;
    }

    // Now the hard part...determine if this scope is a class
    // or function scope defined in a previous .BRM file...
    if( _current != NULL ){
        if( scope->flags == BRI_ST_Function ){
            child = _current->firstChild;
            for( ; child != NULL; child = child->firstSibling ){
                if( child->index >= CurrBrowser->scopeIndex()->LastTopID() ){
                    continue;
                }
                if( child->flags == BRI_ST_Function ){
                    oldSym = child->symbolID;
                    declRec = CurrBrowser->declList()->Lookup( oldSym );
                    if( declRec != NULL &&
                        declRec->nameID == fnName &&
                        declRec->typeID == fnType ){
                        break;
                    }
                }
            }

            if( child != NULL ){
                // Do NOT re-open a function scope.
                _state = IGNORE;
                _ignoreDepth = 1;
                goto OldFunctionScope;
            } else {
                // Now try to find the corresponding function symbol.
                firstDecl = declRec = _current->symbols.Find( fnName );
                while( declRec != NULL ){
                    if( (declRec->attribs & BRI_SA_TypeMask) == BRI_SA_Function ){
                        if( declRec->nameID == fnName &&
                            declRec->typeID == fnType ){
                            break;
                        }
                    }
                    declRec = declRec->_nextDecl;
                }
                if( declRec != NULL ){
                    scope->symbolID = declRec->index;
                } else {
                    // No function symbol was found.  Hence it must appear
                    // later in the .BRM file.  Create a temporary function
                    // symbol as a placeholder.
                    declRec = new DeclRec;
                    declRec->index = CurrBrowser->declIndex()->StealId();
                    declRec->attribs = (BRI_SymbolAttributes)
                                (BRI_SA_Function | BRI_SA_Temporary);
                    declRec->nameID = fnName;
                    declRec->typeID = fnType;
                    declRec->enclosing = indexOf( _current );
                    CurrBrowser->declList()->Insert( declRec );
                    if( firstDecl == NULL ){
                        declRec->_nextDecl = NULL;
                        _current->symbols.Insert( fnName, declRec );
                    } else {
                        declRec->_nextDecl = firstDecl->_nextDecl;
                        firstDecl->_nextDecl = declRec;
                    }
                    scope->symbolID = declRec->index;
                }
            }
        } else if( scope->flags == BRI_ST_Class ){
            typeID = CurrBrowser->typeIndex()->NewId( scope->typeID );
            scope->typeID = typeID;

            child = _current->firstChild;
            for( ; child != NULL; child = child->firstSibling ){
                if( child->index >= CurrBrowser->scopeIndex()->LastTopID() ){
                    continue;
                }
                if( child->flags == BRI_ST_Class ){
                    if( child->typeID == typeID ){
                        break;
                    }
                }
            }

            // Re-opening a class scope is okay...
            if( child != NULL ){
                delete scope;
                scope = child;
                goto done;
            }
        }
    }

    // If we still want to open this scope, add it to
    // the hash table and the scope tree.
    scope->index = CurrBrowser->scopeIndex()->Change(scope->index);
    Insert( scope );

    scope->firstSibling = NULL;
    scope->firstChild = NULL;
    scope->lastChild = NULL;

    if( _current == NULL ){
        WAssert( scope->flags == BRI_ST_File );
        _head = scope;
    } else {
        if( _current->lastChild == NULL ){
            _current->firstChild = scope;
            _current->lastChild = scope;
        } else {
            _current->lastChild->firstSibling = scope;
            _current->lastChild = scope;
        }
    }
    scope->parent = _current;

done:
    if( _current != NULL ){
        _stack.Push( _current );
    }
    _current = scope;
    return _current;

OldFunctionScope:
    delete scope;
    return _current;
}


void ScopeTable::OpenDummyScope()
/*******************************/
{
    WAssert( _state==IGNORE );
    _ignoreDepth += 1;
}


ScopeRec * ScopeTable::CloseScope()
/*********************************/
{
    if( _state == USE ){
        _current = _stack.Pop();
    } else {
        _ignoreDepth -= 1;
        if( _ignoreDepth == 0 ){
            _state = USE;
        }
    }
    return _current;
}


ScopeRec * ScopeTable::Lookup( BRI_ScopeID id )
/*********************************************/
{
    return HashTable<ScopeRec>::Lookup( id );
}


ScopeRec * ScopeTable::First()
/****************************/
{
    ScopeRec    *result = _head;

    _current = result;
    return result;
}


ScopeRec * ScopeTable::Next()
/***************************/
{
    if( _current != NULL ){
        if( _current->firstChild != NULL ){
            _current = _current->firstChild;
        } else if( _current->firstSibling != NULL ){
            _current = _current->firstSibling;
        } else {
            ScopeRec *  temp = _current->parent;
            while( temp != NULL && temp->firstSibling == NULL ){
                temp = temp->parent;
            }
            if( temp != NULL ){
                _current = temp->firstSibling;
            } else {
                _current = NULL;
            }
        }
    }

    return _current;
}


ScopeRec *ScopeTable::FindClassScope( BRI_TypeID id )
/***************************************************/
{
    Hashable    *current;
    ScopeRec    *scope;
    int         i;

    for( i=0; i<_numBuckets; i++ ){
        current = _table[i];
        while( current != NULL ){
            scope = (ScopeRec*) current;
            if( scope->flags == BRI_ST_Class &&
                scope->typeID == id ){
                goto found;
            }
            current = current->next;
        }
    }
    return NULL;
found:
    return scope;
}


ScopeRec *ScopeTable::FindFuncScope( BRI_SymbolID id )
/****************************************************/
{
    Hashable    *current;
    ScopeRec    *scope;
    int         i;

    for( i=0; i<_numBuckets; i++ ){
        current = _table[i];
        while( current != NULL ){
            scope = (ScopeRec*) current;
            if( scope->flags == BRI_ST_Function &&
                scope->symbolID == id ){
                goto found;
            }
            current = current->next;
        }
    }
    return NULL;
found:
    return scope;
}


WBool ScopeTable::SaveTo( CacheOutFile *cache )
/*******************************************/
{
    ScopeRec            *current;
    DeclRec             *currDecl;
    TypeRec             *currType;

    cache->StartComponent( "Scopes" );
    cache->AddDword( indexOf( _head ) );
    cache->AddDword( Count() );
    current = First();
    while( current != NULL ){
        cache->AddDword( current->index );
        cache->AddDword( indexOf( current->firstChild ) );
        cache->AddDword( indexOf( current->lastChild ) );
        cache->AddDword( indexOf( current->firstSibling ) );
        cache->AddDword( indexOf( current->parent ) );
        cache->AddDword( current->symbols.Count() );
        currDecl = current->symbols.First();
        while( currDecl != NULL ){
            cache->AddDword( indexOf(currDecl) );
            currDecl = current->symbols.Next();
        }
        cache->AddDword( current->types.Count() );
        currType = current->types.First();
        while( currType != NULL ){
            cache->AddDword( current->types.CurrentKey() );
            cache->AddDword( indexOf(currType) );
            currType = current->types.Next();
        }
        cache->AddData( &current->flags, BRI_SIZE_SCOPETYPE );
        cache->AddDword( current->symbolID );
        current = Next();
    }
    cache->EndComponent();
    return TRUE;
}


WBool ScopeTable::LoadFrom( CacheInFile *cache )
/********************************************/
{
    WBool       result;
    ReOrdering  *scopeIndex;
    ReOrdering  *declIndex;
    ReOrdering  *typeIndex;
    uint_32     scopeCount;
    uint_32     declCount;
    uint_32     typeCount;
    uint_32     key;
    uint_32     id;
    ScopeRec    *newScope;
    DeclRec     *declPtr;
    TypeRec     *typePtr;

    result = cache->OpenComponent( "Scopes" );

    if( !result ){
        return result;
    }

    scopeIndex = CurrBrowser->scopeIndex();
    declIndex = CurrBrowser->declIndex();
    typeIndex = CurrBrowser->typeIndex();

    if( _stack.Count() > 0 ){
        _stack.Clear();
    }
    cache->ReadDword( &_head );
    cache->ReadDword( &scopeCount );
    while( scopeCount > 0 ){
        newScope = new ScopeRec;
        cache->ReadDword( &newScope->index );
        scopeIndex->ChangeTo( newScope->index, (uint_32) newScope );
        cache->ReadDword( &newScope->firstChild );
        cache->ReadDword( &newScope->lastChild );
        cache->ReadDword( &newScope->firstSibling );
        cache->ReadDword( &newScope->parent );
        cache->ReadDword( &declCount );
        while( declCount > 0 ){
            cache->ReadDword( &id );
            declPtr = (DeclRec *) declIndex->NewId( id );
            newScope->symbols.Insert( declPtr->nameID, declPtr );
            declCount--;
        }
        cache->ReadDword( &typeCount );
        while( typeCount > 0 ){
            cache->ReadDword( &key );
            cache->ReadDword( &id );
            typePtr = (TypeRec *) typeIndex->NewId( id );
            newScope->types.Insert( key, typePtr );
            typeCount--;
        }
        newScope->flags = (BRI_ScopeType) 0;
        cache->ReadData( &newScope->flags, BRI_SIZE_SCOPETYPE );
        cache->ReadDword( &newScope->symbolID );
        Insert( newScope );
        scopeCount--;
    }
    cache->CloseComponent();

    // Update all of those ScopeRec *'s
    id = scopeIndex->NewId( (uint_32) _head );
    _head = (ScopeRec *) id;
    newScope = First();
    while( newScope != NULL ){
        id = scopeIndex->NewId( (uint_32) newScope->firstChild );
        newScope->firstChild = (ScopeRec *) id;
        id = scopeIndex->NewId( (uint_32) newScope->lastChild );
        newScope->lastChild = (ScopeRec *) id;
        id = scopeIndex->NewId( (uint_32) newScope->firstSibling );
        newScope->firstSibling = (ScopeRec *) id;
        id = scopeIndex->NewId( (uint_32) newScope->parent );
        newScope->parent = (ScopeRec *) id;
        newScope = Next();
    }

    return result;
}
