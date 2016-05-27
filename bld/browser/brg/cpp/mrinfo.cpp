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
#include <string.h>
#include <wstd.h>
#include <dwarf.h>

#include "dietree.h"
#include "mrcompnt.h"
#include "mrdie.h"
#include "mrfile.h"
#include "mrinfo.h"
#include "mrinfpp.h"
#include "mrabbrev.h"
#include "mrline.h"
#include "mrreloc.h"

static char * MagicCompunitName = "\01\02\03\04Magic Compunit Name";

MergeInfoSection::MergeInfoSection( int numFiles, MergeAbbrevSection& ab,
                                    MergeLineSection& line )
                    : _abbrevs( ab )
                    , _line( line )
//---------------------------------------------------------------------------
{
    _compunitHdr = new MergeCompunitHdr;
    _relocator = new MergeRelocate( numFiles );
    _diesByName = new DIETree;
}

MergeInfoSection::~MergeInfoSection()
//-----------------------------------
{
    #if INSTRUMENTS
    print();
    #endif

    MergeDIE::ragnarok();

    delete _compunitHdr;
    delete _relocator;
    delete _diesByName;
}

uint_8 MergeInfoSection::getAddrSize()
//------------------------------------
{
    return _compunitHdr->_addressSize;
}

#if INSTRUMENTS
void MergeInfoSection::print()
//----------------------------
{
    Log.printf( "\nMergeInfoSection\n----------------\n" );
//    Log.printf( "     _diesByName: %5u entries\n", _diesByName->entries() );

}
#endif

void MergeInfoSection::relocPass( MergeInfoPP & post )
//----------------------------------------------------
{
    MergeDIE *  die;
    uint_32     offset = 0;

    offset += sizeof( MergeCompunitHdr );

    _diesByName->setToStart();
    die = _diesByName->next();              // set to first
    while( die ) {
        if( !die->assigned() ) {
            die->setNewOff( this, offset, post );
        }
        die = _diesByName->next();
    }

    post.addRequest( MergeOffset( 0, 0 ), offset, 0 );
    _compunitHdr->_infoLength = offset - sizeof(uint_32);
    _compunitHdr->_infoLength += 1;  // for discarded compunit-terminator
}

void MergeInfoSection::writePass( MergeFile * outFile,
                                  WCPtrOrderedVector<MergeFile> & inFiles )
//-------------------------------------------------------------------------
{
    uint_32     len;
    uint_32     size;
    char *      block;
    const int   BufSize = 512;

    MergeStringHdl::ragnarok();
    _diesByName->freeDirectory();

    MergeInfoPP postProcess( inFiles.entries() );

    relocPass( postProcess );

    block = new char [ BufSize ];
    memset( block, 0, BufSize );
    len = _compunitHdr->_infoLength + sizeof(uint_32);
    while( len ) {
        size = (BufSize < len) ? BufSize : len;
        outFile->writeBlock( block, size );
        len -= size;
    }
    delete [] block;

    outFile->seekSect( DR_DEBUG_INFO, 0 );
    outFile->writeBlock( _compunitHdr, sizeof(MergeCompunitHdr) );
    postProcess.execute( this, *outFile, inFiles );
}

void MergeInfoSection::scanFile( MergeFile * file, uint_8 indx )
//--------------------------------------------------------------
{
    uint_32     len;
    MergeOffset moff( indx, 0 );

    len = file->getDRSizes()[ DR_DEBUG_INFO ];

    readCompUnitHdr( file, moff );

    while( moff.offset < len ) {
        readCompUnitHdr( file, moff );
    }
}

void MergeInfoSection::readCompUnitHdr( MergeFile * file, MergeOffset& moff )
//---------------------------------------------------------------------------
{
    uint_32     abbCode;
    MergeOffset startOff;
    MergeOffset compunitStart = moff;

    file->readBlock( DR_DEBUG_INFO, moff.offset, _compunitHdr,
                        sizeof( MergeCompunitHdr ) );

    #if INSTRUMENTS
    Log.printf( "%s:  length = %lx, version = %d, abbrev = %lx, address size = %hx\n",
    file->getFileName(), _compunitHdr->_infoLength, _compunitHdr->_version, _compunitHdr->_abbrevIdx,
    _compunitHdr->_addressSize );
    #endif

    // read the compunit and all its children
    startOff = moff;
    abbCode = file->readULEB128( DR_DEBUG_INFO, moff.offset );
    readDIE( file, startOff, NULL, moff, abbCode );

    // read the compunit terminator
    while( moff.offset - compunitStart.offset <
                _compunitHdr->_infoLength + sizeof( uint_32 ) ) {

        abbCode = file->readULEB128( DR_DEBUG_INFO, moff.offset );
    }
}

void MergeInfoSection::readDIE( MergeFile * file, MergeOffset startOff,
                                MergeDIE * prt, MergeOffset& moff,
                                uint_32 abbCode )
//-------------------------------------------------------------------
// read a die and all its children, adding to _dies.  return false when
// the abbrev code is zero (recursive)
{
    MergeDIE *      die;                // one we read
    MergeAbbrev *   abbrev;

    MergeOffset     child;              // offset of first child
    MergeOffset     sibling;

    bool            ext = true;         // external or static?
    bool            defn = true;        // is a definition?
    const char *    name = NULL;
    int             i;
    DIELen_T        length;
    int             lenDelta = 0;       // delta from changing file idx

    if( abbCode == 0 ) {
        length = (DIELen_T) (moff.offset - startOff.offset);

        MergeNameKey nmKey( abbCode, ext, NULL, 0 );
        die = new MergeDIE( prt, startOff, nmKey, child, sibling, defn, length );

        die = die->collision( _diesByName );

        #if INSTRUMENTS
        if( abbrev == NULL ) {
            Log.printf( "zoiks! %s\n", startOff.getString() );
        }
        #endif

        return;     //<------------------- early return
    }

    abbrev = _abbrevs.getAbbrev( abbCode );

    #if INSTRUMENTS
    if( abbrev == NULL ) {
        Log.printf( "ABBREV == NULL!  offset is %s, %s\n", startOff.getString(), moff.getString() );
    }
    #endif

    InfoAssert( abbrev != NULL );       // NYI throw

    for( i = 0; i < abbrev->entries(); i += 1 ) {
        MergeAttrib & att( (*abbrev)[ i ] );

        switch( att.attrib() ) {
        case DW_AT_name:
            name = file->readString( DR_DEBUG_INFO, moff.offset );
            if( abbrev->tag() == DW_TAG_compile_unit ) {
                name = MagicCompunitName;
            }
            break;
        case DW_AT_sibling:
            sibling.fileIdx = moff.fileIdx;
            sibling.offset = file->readDWord( DR_DEBUG_INFO, moff.offset );
            break;
        case DW_AT_external:
            ext = file->readByte( DR_DEBUG_INFO, moff.offset );
            break;
        case DW_AT_declaration:
            defn = !file->readByte( DR_DEBUG_INFO, moff.offset );
            break;
        case DW_AT_decl_file:
            lenDelta = getFileLenDelta( file, moff, att.form() );
            break;
        default:
            file->skipForm( DR_DEBUG_INFO, moff.offset, att.form(),
                            getAddrSize() );
        }
    }

    if( abbrev->hasChildren() ) {
        child = moff;
    }

    length = (DIELen_T)(moff.offset - startOff.offset + lenDelta);

    MergeNameKey nmKey( abbrev->tag(), ext, name, 0 );
    die = new MergeDIE( prt, startOff, nmKey, child, sibling, defn, length );
    die = die->collision( _diesByName );
    getReloc().addReloc( startOff, die );

    if( abbrev->hasChildren() ) {
        while( 1 ) {
            startOff = moff;
            abbCode = file->readULEB128( DR_DEBUG_INFO, moff.offset );

            if( abbCode == 0 ) break;

            readDIE( file, startOff, die, moff, abbCode );
        }
    }

    if( !die->siblingSet() ) {
        die->setSibling( moff );
    }
}

int MergeInfoSection::getFileLenDelta( MergeFile * file,
                                        MergeOffset& moff,
                                        uint_32 form )
//-------------------------------------------------------------
{
    uint_32 oldIdx;
    uint_32 newIdx;
    int     delta;

    switch( form ) {
    case DW_FORM_udata:
        oldIdx = file->readULEB128( DR_DEBUG_INFO, moff.offset );
        break;

    case DW_FORM_sdata:
        oldIdx = file->readSLEB128( DR_DEBUG_INFO, moff.offset );
        break;

    default:
        InternalAssert( 0 /* unknown form for fileLenDelta */ );
        file->skipForm( DR_DEBUG_INFO, moff.offset, form, getAddrSize() );
        return 0;
    }

    newIdx = _line.getNewFileIdx( moff.fileIdx, (uint_16) oldIdx );
    delta = MergeFile::ULEB128Len( newIdx );
    delta -= MergeFile::ULEB128Len( oldIdx );

    #if ( INSTRUMENTS == INSTRUMENTS_FULL_LOGGING )
    Log.printf( "FileLenDelta from %x to %x -- %d\n", oldIdx, newIdx, delta );
    #endif

    return (DIELen_T) delta;
}
