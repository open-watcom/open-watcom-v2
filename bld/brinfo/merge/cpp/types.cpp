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
#include "types.hpp"
#include "decl.hpp"
#include "scopes.hpp"
#include "cache.hpp"


#ifndef  TRUE
#define  TRUE 1
#endif
#ifndef  FALSE
#define  FALSE 0
#endif

extern  Browser *               CurrBrowser;


// Silence the compiler warning about taking the "sizeof" a
// class with virtual functions...
#pragma warning 549 9
static Pool TypeRec::_typePool( TypePool, sizeof(TypeRec) );
#pragma warning 549 3


TypeRec::~TypeRec()
/*****************/
{
    if( typeCode == BRI_TC_Function ){
        delete[] ra.args;
    }
}


TypeList::~TypeList()
/*******************/
{
    // empty
}


TypeRec * TypeList::AddType( TypeRec * type )
/*******************************************/
{
    TypeRec             *result;
    TypeRec             *current;
    TypeRec             *first;
    ScopeRec            *curScope;
    TypeCategory        *category;
    ReOrdering          *typeIndex;
    uint_32             key;
    int                 i;

    curScope = CurrBrowser->scopeList()->Current();
    typeIndex = CurrBrowser->typeIndex();

    /* This is tricky...we don't want duplicate types, but
       again C++ has such a wonderfully diverse type system...
    */
    switch( type->typeCode ){
        case BRI_TC_BaseType:
            category = &_baseTypes;
            key = type->bt.baseType;
            first = current = category->Find(key);
        break;

        case BRI_TC_Modifier:
            category = &_modifiers;
            key = type->fp.parent;
            first = current = category->Find(key);
            while( current != NULL ){
                if( current->fp.flags == type->fp.flags ){
                    break;
                }
                current = current->_next;
            }
        break;

        case BRI_TC_Pointer:
        case BRI_TC_Reference:
        case BRI_TC_TypeDef:
            category = &_ptrTypes;
            key = type->p.parent;
            first = current = category->Find(key);
            while( current != NULL ){
                if( current->typeCode == type->typeCode ){
                    break;
                }
                current = current->_next;
            }
        break;

        case BRI_TC_PtrToMember:
            category = &_ptrsToMembers;
            key = type->cm.host;
            first = current = category->Find(key);
            while( current != NULL ){
                if( current->cm.member == type->cm.member ){
                    break;
                }
                current = current->_next;
            }
        break;

        case BRI_TC_Array:
            category = &_arrays;
            key = type->se.element;
            first = current = category->Find(key);
            while( current != NULL ){
                if( current->se.size == type->se.size ){
                    break;
                }
                current = current->_next;
            }
        break;

        case BRI_TC_Function:
            category = &_functions;
            key = type->ra.args[0];
            first = current = category->Find(key);
            while( current != NULL ){
                if( current->ra.numArgs == type->ra.numArgs ){
                    for( i=1; i < current->ra.numArgs; i++ ){
                        if( current->ra.args[i] != type->ra.args[i] ){
                            break;
                        }
                    }
                    if( i == current->ra.numArgs ){
                        break;
                    }
                }
                current = current->_next;
            }
        break;

        case BRI_TC_Class:
        case BRI_TC_Struct:
        case BRI_TC_Union:
        case BRI_TC_Enum:
            // this is the only really odd case...we only have to
            // check the classes/enums in the current scope.
            category = &curScope->types;
            key = type->ns.nameID;
            first = current = category->Find(key);
            while( current != NULL ){
                if( current->typeCode == type->typeCode ){
                    break;
                }
                current = current->_next;
            }
        break;

        case BRI_TC_BitField:
            category = &_bitFields;
            key = type->w.width;
            first = current = category->Find(key);
        break;
    }

    if( current != NULL ){
        typeIndex->ChangeTo( type->index, current->index );
        // delete type;
        result = current;
    } else {
        type->index = typeIndex->Change( type->index );
        if( category != NULL ){
            if( first == NULL ){
                category->Insert( key, type );
                type->_next = NULL;
            } else {
                type->_next = first->_next;
                first->_next = type;
            }
        }
        Insert( type );
        result = type;
    }

    return result;
}


WBool TypeList::SaveTo( CacheOutFile *cache )
/*****************************************/
{
    Hashable    *current;
    TypeRec     *currType;
    int         i,j;

    cache->StartComponent( "Types" );
    cache->AddDword( Count() );
    for( i=0; i<_numBuckets; i++ ){
        current = _table[i];
        while( current != NULL ){
            currType = (TypeRec *) current;
            cache->AddDword( currType->index );
            cache->AddData( &currType->typeCode, BRI_SIZE_TYPECODE );
            switch( currType->typeCode ){
                case BRI_TC_BaseType:
                    cache->AddData( &currType->bt.baseType, BRI_SIZE_BASETYPES );
                break;

                case BRI_TC_Modifier:
                    cache->AddDword( currType->fp.flags );
                    cache->AddDword( currType->fp.parent );
                break;

                case BRI_TC_Pointer:
                case BRI_TC_Reference:
                case BRI_TC_TypeDef:
                    cache->AddDword( currType->p.parent );
                break;

                case BRI_TC_PtrToMember:
                    cache->AddDword( currType->cm.host );
                    cache->AddDword( currType->cm.member );
                break;

                case BRI_TC_Array:
                    cache->AddDword( currType->se.size );
                    cache->AddDword( currType->se.element );
                break;

                case BRI_TC_Function:
                    cache->AddDword( currType->ra.numArgs );
                    for( j=0; j<currType->ra.numArgs; j++ ){
                        cache->AddDword( currType->ra.args[j] );
                    }
                break;

                case BRI_TC_Class:
                case BRI_TC_Struct:
                case BRI_TC_Union:
                case BRI_TC_Enum:
                    cache->AddDword( currType->ns.nameID );
                    cache->AddDword( currType->ns.symbolID );
                break;


                case BRI_TC_BitField:
                    cache->AddDword( currType->w.width );
                break;
            }
            current = current->next;
        }
    }
    cache->EndComponent();
    return TRUE;
}


WBool TypeList::LoadFrom( CacheInFile *cache )
/******************************************/
{
    WBool               result;
    TypeRec             *newType;
    TypeRec             *oldType;
    ReOrdering          *typeIndex;
    int                 typeCount;
    int                 i;
    uint_32             id;
    AvlTree<TypeRec>    *category;
    uint_32             key;

    result = cache->OpenComponent( "Types" );

    if( !result ){
        return result;
    }

    typeIndex = CurrBrowser->typeIndex();
    cache->ReadDword( &typeCount );
    while( typeCount > 0 ){
        newType = new TypeRec;
        cache->ReadDword( &newType->index );
        typeIndex->ChangeTo( newType->index, (uint_32) newType );
        newType->typeCode = (BRI_TypeCode) 0;
        cache->ReadData( &newType->typeCode, BRI_SIZE_TYPECODE );
        switch( newType->typeCode ){
            case BRI_TC_BaseType:
                newType->bt.baseType = (BRI_BaseTypes) 0;
                cache->ReadData( &newType->bt.baseType, BRI_SIZE_BASETYPES );
                category = &_baseTypes;
                key = newType->bt.baseType;
            break;

            case BRI_TC_Modifier:
                cache->ReadDword( &newType->fp.flags );
                cache->ReadDword( &newType->fp.parent );
                category = &_modifiers;
                key = newType->fp.parent;
            break;

            case BRI_TC_Pointer:
            case BRI_TC_Reference:
            case BRI_TC_TypeDef:
                cache->ReadDword( &newType->p.parent );
                category = &_ptrTypes;
                key = newType->fp.parent;
            break;

            case BRI_TC_PtrToMember:
                cache->ReadDword( &newType->cm.host );
                cache->ReadDword( &newType->cm.member );
                category = &_ptrsToMembers;
                key = newType->cm.host;
            break;

            case BRI_TC_Array:
                cache->ReadDword( &newType->se.size );
                cache->ReadDword( &newType->se.element );
                category = &_arrays;
                key = newType->se.element;
            break;

            case BRI_TC_Function:
                cache->ReadDword( &id );
                newType->ra.numArgs = id;
                newType->ra.args = new BRI_TypeID[id];
                for( i=0; i<id; i++ ){
                    cache->ReadDword( &newType->ra.args[i] );
                }
                category = &_functions;
                key = newType->ra.args[0];
            break;

            case BRI_TC_Class:
            case BRI_TC_Struct:
            case BRI_TC_Union:
            case BRI_TC_Enum:
                cache->ReadDword( &newType->ns.nameID );
                cache->ReadDword( &newType->ns.symbolID );
                category = NULL;
            break;


            case BRI_TC_BitField:
                cache->ReadDword( &newType->w.width );
                category = &_bitFields;
                key = newType->w.width;
            break;
        }
        Insert( newType );
        if( category != NULL ){
            oldType = category->Find( key );
            if( oldType != NULL ){
                newType->_next = oldType->_next;
                oldType->_next = newType;
            } else {
                category->Insert( key, newType );
            }
        }
        typeCount--;
    }

    cache->CloseComponent();
    return result;
}
