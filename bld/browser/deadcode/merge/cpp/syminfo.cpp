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


// System includes --------------------------------------------------------

#include <stdio.h>

// Project includes -------------------------------------------------------

#include "syminfo.h"

const int SymInfoPoolSize = 1024;
#pragma warning 549 5           // sizeof contains compiler genned info.
MemoryPool SymbolInfo::_pool( sizeof( SymbolInfo ), SymInfoPoolSize,
                                "SymbolInfo" );
#pragma warning 549 3

SymbolInfo::SymbolInfo( int mbrIndex,
                        uint_32 offset,
                        uint_8 * data,
                        Abbreviation * ab,
                        char * name,
                        bool isExternal )
/***************************************/
    : DebugObject( name )
    , _data( data )
    , _mbrIndex( mbrIndex )
    , _isExternal( isExternal )
    , _abbrev( ab )
    , _unresolvedRefCount( 0 )
    , _fileIndex( 0 )
    , _oldOffset( offset )
    , _newOffset( -1 )
    , _length( -1 )
    , _tag( 0 )
    , _siblingRef( NULL )
    , _hasChildren( FALSE )
    , _refs(10,20)
{
    _refs.exceptions( WCExcept::index_range | WCExcept::out_of_memory );
}

SymbolInfo::~SymbolInfo()
/***********************/
{
    int i;

    #if DEBUG_LIST
        static int num = 0;
        static int minn = 65535;
        static int avgn = 0;
        static int maxn = 0;

        num += 1;
        if( _refs.entries() < minn ) minn = _refs.entries();
        if( _refs.entries() > maxn ) maxn = _refs.entries();
        avgn += _refs.entries();

        printf( "SymbolInfo::~SymbolInfo() -- _refs contains %d entries\tnum %d min %d max %d avg %d\n", _refs.entries(), num, minn, maxn, avgn / num );
    #endif

    for( i = 0; i < _refs.entries(); i += 1 ) {
        delete _refs[ i ];
    }

    if( _siblingRef != NULL ) {
        delete _siblingRef;
    }
}

void SymbolInfo::setTag( uint_32 tag )
/************************************/
{
    _tag = tag;
}

uint_32 SymbolInfo::tag()
/***********************/
{
    return( _tag );
}

void SymbolInfo::setFileIndex( int fileIndex )
/******************************************/
{
    _fileIndex = fileIndex;
}

int SymbolInfo::fileIndex()
/*************************/
{
    return( _fileIndex );
}

void SymbolInfo::setLength( uint_32 length )
/******************************************/
{
    _length = length;
}

uint_32 SymbolInfo::length()
/**************************/
{
    return( _length );
}

void SymbolInfo::setAbbrev( Abbreviation * ab )
/*********************************************/
{
    _abbrev = ab;
}

Abbreviation * SymbolInfo::abbrev()
/*********************************/
{
    return( _abbrev );
}

int SymbolInfo::numUnresolved()
/*****************************/
{
    return( _unresolvedRefCount );
}

bool SymbolInfo::isExternal()
/***************************/
{
    return _isExternal;
}

void SymbolInfo::setNewOffset( uint_32 offset )
/*********************************************/
{
    _newOffset = offset;
}

uint_32 SymbolInfo::newOffset()
/*****************************/
{
    return( _newOffset );
}

uint_32 SymbolInfo::oldOffset()
/*****************************/
{
    return( _oldOffset );
}

RefVector & SymbolInfo::refs()
/****************************/
{
    return( _refs );
}

uint_8 * SymbolInfo::data()
/*************************/
{
    return( _data );
}

void SymbolInfo::addReference( MergeReference * ref )
/***************************************************/
{
    _refs.insert( ref );

    _unresolvedRefCount += 1;
}

void SymbolInfo::referenceResolved()
/**********************************/
{
    _unresolvedRefCount -= 1;
}

int SymbolInfo::mbrIndex()
/************************/
{
    return( _mbrIndex );
}

void SymbolInfo::setSiblingRef( MergeReference * sibRef )
/*******************************************************/
{
    _siblingRef = sibRef;
    _unresolvedRefCount += 1;
}

MergeReference * SymbolInfo::siblingRef()
/***************************************/
{
    return( _siblingRef );
}

void SymbolInfo::setHasChildren( bool flag )
/*****************************************/
{
    _hasChildren = flag;
}

bool SymbolInfo::hasChildren()
/***************************/
{
    return( _hasChildren );
}

#if DEBUG_DUMP
void SymbolInfo::dumpData()
/*************************/
{
    printf( "+---- Symbol [ %s ] -------------------------+\n", name() );
    printf( "  Mbr Index = [%d]\n", _mbrIndex );
    printf( "  Old Offset = [0x%lx]\n", _oldOffset );

    printf( "  References :\n" );
    int i;

    for( i = 0; i < _refs.entries(); i += 1 ) {
        _refs[ i ]->dumpData();
    }

    printf( "+--------------------------------------------+\n" );
}
#endif

void * SymbolInfo::operator new( size_t )
/***************************************/
{
    return( _pool.alloc() );
}

void SymbolInfo::operator delete( void * mem )
/********************************************/
{
    _pool.free( mem );
}

