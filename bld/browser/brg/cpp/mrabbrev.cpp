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


#include <wcvector.h>
#include <wchash.h>
#include <wchiter.h>

#include "mrfile.h"
#include "mrabbrev.h"

/*-------------------------- MergeAbbrev ------------------------*/

const int MergeAbbrevPoolSize = 100;
static MemoryPool MergeAbbrev::_pool( sizeof( MergeAbbrev ),
                                      "MergeAbbrev",
                                      MergeAbbrevPoolSize );
static MemoryPool MergeAbbrev::_hPool(
                WCValHashDictItemSize( uint_32, MergeAbbrev * ),
                "MergeAbbrev Hash",
                MergeAbbrevPoolSize );

MergeAbbrev::MergeAbbrev()
//------------------------
{
    setup();
}

MergeAbbrev::MergeAbbrev( uint_32 code )
                : _code( code )
//------------------------
{
    setup();
}

static void * MergeAbbrev::allocHNode( size_t )
//---------------------------------------------
{
    return _hPool.alloc();
}

static void MergeAbbrev::freeHNode( void * item, size_t )
//-------------------------------------------------------
{
    _hPool.free( item );
}

void MergeAbbrev::setup()
//-----------------------
{
    _attribs = new WCValOrderedVector<MergeAttrib>;
}

MergeAbbrev::~MergeAbbrev()
//-------------------------
{
    _attribs->clear();
    delete _attribs;
}

MergeAttrib& MergeAbbrev::operator[] ( int idx )
//----------------------------------------------
{
    return (*_attribs)[ idx ];
}

uint MergeAbbrev::entries()
//-------------------------
{
    return _attribs->entries();
}

void MergeAbbrev::readAbbrev( MergeFile * file, uint_32& off )
//------------------------------------------------------------
{
    uint_32 attrib;
    uint_32 form;

    _tag = file->readULEB128( DR_DEBUG_ABBREV, off );
    _children = file->readByte( DR_DEBUG_ABBREV, off );

    for(;;) {
        attrib = file->readULEB128( DR_DEBUG_ABBREV, off );
        form = file->readULEB128( DR_DEBUG_ABBREV, off );

        if( attrib == 0 ) {
            break;
        } else {
            _attribs->insert( MergeAttrib( attrib, form ) );
        }
    }
}

void MergeAbbrev::writeAbbrev( MergeFile * outFile )
//--------------------------------------------------
{
    int i;

    outFile->writeULEB128( _code );
    outFile->writeULEB128( _tag );
    outFile->writeByte( _children );

    for( i = 0; i < _attribs->entries(); i += 1 ) {
        MergeAttrib& att( (*_attribs)[ i ] );
        outFile->writeULEB128( att.attrib() );
        outFile->writeULEB128( att.form() );
    }
    outFile->writeULEB128( 0 ); // zero attrib / zero form marks end of abbrev
    outFile->writeULEB128( 0 );
}

static void MergeAbbrev::skipAbbrev( MergeFile * file, uint_32& off )
//-------------------------------------------------------------------
{
    uint_32 attrib;

    file->readULEB128( DR_DEBUG_ABBREV, off );      // discard tag
    file->readByte( DR_DEBUG_ABBREV, off );         // skip child ptr

    for(;;) {
        attrib = file->readULEB128( DR_DEBUG_ABBREV, off );
        file->readULEB128( DR_DEBUG_ABBREV, off );          // discard form

        if( attrib == 0 ) {
            break;
        }
    }
}

/*--------------------- MergeAbbrevSection ------------------------*/

static uint HashFunc( const uint_32 & r ) { return (uint)r; }

MergeAbbrevSection::MergeAbbrevSection()
//---------------------------------------
{
    _abbrevs = new WCValHashDict<uint_32, MergeAbbrev *>(
                                              HashFunc, 47,
                                              MergeAbbrev::allocHNode,
                                              MergeAbbrev::freeHNode );
}

MergeAbbrevSection::~MergeAbbrevSection()
//---------------------------------------
{
    WCValHashDictIter<uint_32, MergeAbbrev *>   iter( *_abbrevs );

    #if INSTRUMENTS
        Log.printf( "\nMergeAbbrevSection\n------------------\n" );
        Log.printf( "MergeAbbrevSection::_abbrevs:          %u entries, loaded %%%3.2f\n", _abbrevs->entries(), 100.0 * ((double)_abbrevs->entries() / (double)_abbrevs->buckets()) );
    #endif

    while( ++ iter ) {
        delete iter.value();
    }

    delete _abbrevs;
}

void MergeAbbrevSection::scanFile( MergeFile * file, uint_8 )
//-----------------------------------------------------------
// read abbrev section into the hash table, discarding duplicates
{
    uint_32         sectOff = 0;
    uint_32         code;

    for(;;) {
        code = file->readULEB128( DR_DEBUG_ABBREV, sectOff );

        if( code == 0 ) break;

        if( _abbrevs->contains( code ) ) {
            MergeAbbrev::skipAbbrev( file, sectOff );
        } else {
            MergeAbbrev * tmp;

            tmp = new MergeAbbrev( code );
            tmp->readAbbrev( file, sectOff );

            (*_abbrevs)[ code ] = tmp;
        }
    }

    #if INSTRUMENTS
    Log.printf( "    %s .debug_abbrev - %ld bytes\n", file->getFileName(), file->getDRSizes()[ DR_DEBUG_ABBREV ] );
    #endif
}

void MergeAbbrevSection::writePass( MergeFile * outFile )
//-------------------------------------------------------
// write out the abbreviation table
// outFile must have been startWriteSect( DR_DEBUG_ABBREV ); before
// and endWriteSect() afterwards
{
    WCValHashDictIter<uint_32,MergeAbbrev *> iter( *_abbrevs );

    while( ++ iter ) {
        iter.value()->writeAbbrev( outFile );
    }

    outFile->writeULEB128( 0 );
}

MergeAbbrev * MergeAbbrevSection::getAbbrev( uint_32 code )
//---------------------------------------------------------
// find the abbreviation for a given code
{
    MergeAbbrev * abb;
    int           found;

    found = _abbrevs->find( code, abb );

    #if INSTRUMENTS
    if( !found ) {
        Log.printf( "MergeAbbrevSection::getAbbrev -- bad code %lu!\n", code );
        return NULL;
    }
    #endif

    InfoAssert( found );

    return abb;
}
