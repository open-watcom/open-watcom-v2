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
#include <wstd.h>
#include <dwarf.h>

#include "dietree.h"
#include "mrabbrev.h"
#include "mrdie.h"
#include "mrfile.h"
#include "mrinfo.h"
#include "mrinfpp.h"
#include "mrreloc.h"

static PatriciaTree MergeStringHdl::_strings;
static void MergeStringHdl::ragnarok()
//------------------------------------
{
    _strings.ragnarok();
}

const int MergeDIEPoolSize = 32;
static MemoryPool MergeDIE::_pool( sizeof( MergeDIE ), "MergeDIE",
                                      MergeDIEPoolSize );

MergeDIE::MergeDIE()
            : _offset( 0, 0 )
            , _nameKey()
            , _firstChild( -1, 0 )
            , _sibling( -1, 0 )
            , _newOffset( 0 )
            , _length( 0 )
            , _occurs( 0 )
//--------------------------
{
    _flagInt = 0;   // set all flags to FALSE
}

MergeDIE::MergeDIE( MergeDIE * parent,
                    const MergeOffset& offset,
                    const MergeNameKey& nmKey,
                    const MergeOffset& firstChild,
                    const MergeOffset& sibling,
                    bool definition, DIELen_T length )
            : _parent( parent )
            , _offset( offset )
            , _nameKey( nmKey )
            , _firstChild( firstChild )
            , _sibling( sibling )
            , _newOffset( 0 )
            , _length( length )
            , _occurs( 0 )
//-------------------------------------------------------------------------
{
    _flagInt = 0;   // set all flags to FALSE
    _flags._definition = (definition) ? 1 : 0;
}

MergeDIE::MergeDIE( const MergeDIE& other )
//-----------------------------------------
{
    *this = other;
}

MergeDIE::~MergeDIE()
//-------------------
{
    // WARNING --   this destructor probably won't be called for
    //              all dies, since the ragnarok call is used to
    //              free all allocated dies
}

void MergeDIE::ragnarok()
//-----------------------
// free all MergeDie's allocated with the
// new operator, but DONT CALL THE DESTRUCTOR
{
    _pool.ragnarok();
}

MergeDIE::operator const MergeNameKey&() const
//--------------------------------------------
{
    return _nameKey;
}

MergeDIE::operator const MergeOffset&() const
//--------------------------------------------
{
    return _offset;
}

#if INSTRUMENTS
MergeDIE::operator const char*() const
//------------------------------------
{
    static char buffer[ 1024 ];
    char *      off = buffer;
    int         len;

    len = sprintf( off, "[nm:%s,off:%s,nw:%#lx,",
            _nameKey.getString(), _offset.getString(), _newOffset );
    off += len;

    len = sprintf( off, "ch:%s,", _firstChild.getString() );
    off += len;

    len = sprintf( off, "sib:%s,", _sibling.getString() );
    off += len;

    len = sprintf( off, "%s,len:%d,occ:%u]",
            definition() ? "<definition>" : "<declaration>", _length, _occurs );

    return buffer;
}
#endif

MergeDIE * MergeDIE::collision( DIETree * tree )
//----------------------------------------------
// return pointer victor die
// -- only update first->_occurs if this die is inserted.
{
    MergeDIE *      other;
    MergeDIE *      first;
    int             i;
    uint_16         unique;

    first = tree->find( _nameKey );

    if( !first ) {
        tree->insert( this );
        return this;
    }

    _nameKey._unique = (uint_16) (first->_occurs + 1);
    _occurs = (uint_16) (first->_occurs + 1);

    if( _nameKey._name.getString() == NULL ) {
        // don't merge NULL names -- this wastes space!
        // but can't think of good way to merge them yet.
        first->_occurs += 1;
        tree->insert( this );
        return this;
    }

    other = first;
    for( i = 0; i < first->_occurs + 1; i += 1, other = tree->next() ) {
        if( other == NULL || !(other->name()._name == name()._name) ) {
            break;                  // <---- end of loop
        }

        if( _parent != other->_parent ) {
            #if (INSTRUMENTS == INSTRUMENTS_FULL_LOGGING)
            Log.printf( "  not joined as parents not equal -- %p != %p\n",
                        _parent, other->_parent );
            #endif
            continue;
        }

        if( !_nameKey._extern && _offset.fileIdx != other->_offset.fileIdx ) {
            // don't merge two statics defined in seperate files
            first->_occurs += 1;
            tree->insert( this );
            return this;
        }

        if( other->definition() && !definition() ) {
            // other is definition, this is declaration
            delete this;
            return other;
        }

        if( !other->definition() && definition() ) {
            // this is definition, other is declaration
            // don't change the _nameKey for the other guy, but
            // assign all other fields from this die to the other
            unique = other->_nameKey._unique;
            *other = *this;
            other->_nameKey._unique = unique;
            delete this;
            return other;
        }

        if( other->_parent == other->_parent ) {
            // now, both are declarations or both are definitions
            delete this;
            return other;
        }
    }

    first->_occurs += 1;
    tree->insert( this );
    return this;   // can't merge
}

void MergeDIE::setNewOff( MergeInfoSection * sect, uint_32 & newOffset,
                          MergeInfoPP & pp )
//---------------------------------------------------------------------
{
    MergeDIE * child;

    if( _parent && !_parent->assigned() ) {
        _parent->setNewOff( sect, newOffset, pp );

        // if our parent is a normal parent, it will call setNewOff
        // for all its kids (including us).  Compile_Units don't
        // call setNewOff for kids, so we can't return.

        if( _parent->_nameKey._tag != DW_TAG_compile_unit ) {
            return;
        }
    }

    #if (INSTRUMENTS == INSTRUMENTS_FULL_LOGGING)
        Log.printf( "%s at %lx\n", offset().getString(), newOffset );
    #endif

    _flags._assigned = 1;
    _newOffset = newOffset;

    newOffset += _length;

    if( _nameKey._tag != DW_TAG_compile_unit ) {
        child = sect->getReloc().getReloc( firstChild() );
        while( child != NULL ) {
            #if INSTRUMENTS
            if( child->assigned() ) {
                Log.printf( "%s: child", (const char *)(*this) );
                Log.printf( " %s already Assigned!\n", (const char *)(*child) );
                break;
            }
            #endif
            child->setNewOff( sect, newOffset, pp );
            child = sect->getReloc().getReloc( child->sibling() );
        }

        if( firstChild().fileIdx >= 0 ) {
            // leave space for a 0 - terminator byte to be placed
            // although this is already written when the file is zeroed.

            newOffset += 1;
        }
        _newSibOffset = newOffset;

    } else {
        _newSibOffset = 0;      // FIXME -- this is wrong, but works
    }
    pp.addRequest( offset(), _newOffset, _newSibOffset );
}

#if 0
void MergeDIE::writeDIE( MergeInfoSection * sect, MergeFile & outFile,
                         WCPtrOrderedVector<MergeFile> & inFiles )
//---------------------------------------------------------------------
{
    MergeDIE * child;

    if( !written() ) {
        writeSelf( sect, outFile, inFiles );

        child = sect->find( firstChild() );
        while( child != NULL ) {
            child->writeDIE( sect, outFile, inFiles );
            child = sect->find( child->sibling() );
        }

        if( _firstChild.fileIdx >= 0 ) {
            outFile.writeULEB128( 0 );      // 0 abbrev code terminates children
        }
    }
}
#endif


#if 0
bool MergeDIE::writeSpecialAttribs( MergeInfoSection * sect,
                                    MergeAttrib & att, uint_32 & offset,
                                    MergeFile & outFile,
                                    WCPtrOrderedVector<MergeFile>& inFiles )
//----------------------------------------------------------------------
// check for special-case attributes and write them out.  return true
// if the attribute was handled.
{
    MergeFile *     in( inFiles[ _offset.fileIdx ] );
    MergeOffset     ref;
    uint_8          fileIdx;

    switch( att.attrib() ) {
    case DW_AT_sibling:
        in->skipForm( DR_DEBUG_INFO, offset, att.form(), sect->getAddrSize() );
        outFile.writeForm( att.form(), _newSibOffset, sect->getAddrSize() );
        return TRUE;

    case DW_AT_decl_file:
        fileIdx = (uint_8) in->readForm( DR_DEBUG_INFO, offset, att.form(), sect->getAddrSize() );
        outFile.writeForm( att.form(),
                            sect->getNewFileIdx( _offset.fileIdx, fileIdx ),
                            sect->getAddrSize() );
        return TRUE;

    case DW_AT_macro_info:                  // NYI
    case DW_AT_WATCOM_references_start:     // NYI
        in->copyFormTo( outFile, DR_DEBUG_INFO, offset,
                        att.form(), sect->getAddrSize() );
        return TRUE;
    }

    return FALSE;
}
#endif

#if 0
void MergeDIE::writeSelf( MergeInfoSection * sect, MergeFile & outFile,
                          WCPtrOrderedVector<MergeFile>& inFiles )
//----------------------------------------------------------------------
{
    MergeFile *     in( inFiles[ _offset.fileIdx ] );
    MergeAbbrev *   abbrev;
    uint_32         abbcode;
    uint_32         offset( _offset.offset );
    MergeOffset     ref;
    MergeDIE *      referredTo;     // target of a reference
    bool            updateRef;      // update a reference?
    int             i;

    #if INSTRUMENTS
    if( _newOffset != outFile.tell( DR_DEBUG_INFO ) ) {
        Log.printf( "die not at right new offset! actually %lx, should be %lx, %s\n", outFile.tell( DR_DEBUG_INFO ), _newOffset, (const char *)(*this) );
    }
    #endif
    InternalAssert( _newOffset == outFile.tell( DR_DEBUG_INFO ) );

    abbcode = in->readULEB128( DR_DEBUG_INFO, offset );
    if( abbcode == 0 ) {
        outFile.writeULEB128( 0 );
        return;                     //<------------------ early return
    }
    abbrev = sect->getAbbrev( abbcode );

    InfoAssert( abbrev != NULL );

    ref.fileIdx = _offset.fileIdx;

    outFile.writeULEB128( abbcode );

    for( i = 0; i < abbrev->entries(); i += 1 ) {
        MergeAttrib & att( (*abbrev)[ i ] );

        if( writeSpecialAttribs( sect, att, offset, outFile, inFiles ) ) {
            continue;   // <---------- unusual flow
        }

        switch( att.form() ) {
        case DW_FORM_ref4:
        case DW_FORM_ref2:
        case DW_FORM_ref1:
        case DW_FORM_ref_addr:
        case DW_FORM_ref_udata:
            ref.offset = in->readForm( DR_DEBUG_INFO, offset,
                                        att.form(), sect->getAddrSize() );
            updateRef = TRUE;
            break;
        case DW_FORM_ref8:
            InternalAssert( 0 /* can't handle 8-byte references */ );
            break;
        default:
            updateRef = FALSE;
        }

        if( updateRef ) {
            referredTo = sect->getReloc().getReloc( ref );
            if( referredTo == NULL ) {
                referredTo = sect->find( ref );
            }

            #if INSTRUMENTS
                if( referredTo == NULL ) {
                    Log.printf( "Ack -- can't find a replacement %s", _offset.getString() );
                    Log.printf( " for %#x %s!\n", att.attrib(), ref.getString() );
                }
            #endif
            InfoAssert( referredTo != NULL );

            switch( att.form() ) {
            case DW_FORM_ref1:
            case DW_FORM_ref2:
            case DW_FORM_ref4:
            case DW_FORM_ref_addr:
            case DW_FORM_ref_udata:
                outFile.writeForm( att.form(), referredTo->_newOffset,
                                    sect->getAddrSize() );
            break;
            case DW_FORM_ref8:
                InternalAssert( 0 /* can't handle 8-byte references */ );
                break;
            }
        } else {
            in->copyFormTo( outFile, DR_DEBUG_INFO, offset,
                            att.form(), sect->getAddrSize() );
        }
    }

    _flags._written = 1;
}
#endif
