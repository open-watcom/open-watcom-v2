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


#include <wchash.h>

#include "brmerge.h"
#include "mrdie.h"
#include "mrnmkey.h"
#include "mroffset.h"
#include "mrreloc.h"

static MemoryPool MergeRelocate::_hPool(
                WCValHashDictItemSize( uint_32, MergeDIE * ),
                "MergeRelocate Replaced Hash", 32 );

static uint HashFunc( const uint_32 & r ) { return (uint)r; }

MergeRelocate::MergeRelocate( int numFiles )
                : _numFiles( numFiles )
//------------------------------------------
{
    int i;
    typedef WCValHashDict<uint_32,MergeDIE *> * HashPtr;

    _replacedDies = new HashPtr[ numFiles ];

    for( i = 0; i < _numFiles; i += 1 ) {
        _replacedDies[ i ] = new WCValHashDict<uint_32,MergeDIE *>(
                                          HashFunc, 1024,
                                          MergeRelocate::allocHNode,
                                          MergeRelocate::freeHNode );
    }
}

MergeRelocate::~MergeRelocate()
//-----------------------------
{
    int i;

    #if INSTRUMENTS
    Log.printf( "\nMergeRelocate\n--------------\n" );
    Log.printf( "   _replacedDies: %5u entries\n", _numFiles );
    for( i = 0; i < _numFiles; i += 1 ) {
        WCValHashDict<uint_32,MergeDIE *> * dict( _replacedDies[ i ] );
        Log.printf( "         [ %3d ]: %5u entries, loaded %3.1f%%\n", i, dict->entries(), 100.0 * ((double)dict->entries() / (double)dict->buckets()) );
    }
    #endif

    for( i = 0; i < _numFiles; i += 1 ) {
        delete _replacedDies[ i ];
    }

    delete _replacedDies;
    _hPool.ragnarok();
}

void MergeRelocate::addReloc( MergeOffset & off, MergeDIE * replacer )
//--------------------------------------------------------------------
{
//    _replacedDies[ off.fileIdx ]->insert( off.offset, replacer );

    #if ( INSTRUMENTS == INSTRUMENTS_FULL_LOGGING )
        MergeDIE * replaced = getReloc( off );
        Log.printf( "    reloc from %s %s to ",
//                        ((const MergeNameKey&)(*replaced)).getString(),
//                        ((const MergeOffset&)(*replaced)).getString() );
                        ( replaced != NULL ) ? replaced->name().getString() : "NULL",
                        ( replaced != NULL ) ? replaced->offset().getString() : "NULL" );
//        Log.printf( "%s %s\n", ((const MergeNameKey&)(*replacer)).getString(),
//                        ((const MergeOffset&)(*replacer)).getString() );
        Log.printf( "%s %s\n",
                        ( replacer != NULL ) ? replacer->name().getString() : "NULL",
                        ( replacer != NULL ) ? replacer->offset().getString() : "NULL" );
    #endif
    _replacedDies[ off.fileIdx ]->insert( off.offset, replacer );
}


MergeDIE * MergeRelocate::getReloc( const MergeOffset & off )
//-----------------------------------------------------------
{
    MergeDIE * ret;

    if( off.fileIdx >= 0 && off.fileIdx < _numFiles ) {
        if( _replacedDies[ off.fileIdx ]->find( off.offset, ret ) ) {
            return ret;
        }
    }
    return NULL;
}
