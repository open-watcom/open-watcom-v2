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


#include <stdlib.h>
#include <wcvector.h>

#include "mrabbrev.h"
#include "mrdie.h"
#include "mrfile.h"
#include "mrline.h"
#include "mroffset.h"
#include "mrinfo.h"
#include "mrinfpp.h"
#include "mrreloc.h"

typedef InfoPPReq *     PPReqP;
typedef InfoPPReqNode * PPReqNP;

MergeInfoPP::MergeInfoPP( int numFiles )
            : _numFiles( numFiles )
            , _pool( sizeof( InfoPPReq ),
                     "MergeInfoPP::_pool", 4 )
//-----------------------------------------------
{
    int i;

    _requests = new PPReqP[ _numFiles ];
    _reqCount = new int[ _numFiles ];

    for( i = 0; i < _numFiles; i += 1 ) {
        _requests[ i ] = (InfoPPReq *) _pool.alloc();
        new( _requests[ i ] ) InfoPPReq();
        _reqCount[ i ] = 0;
    }
}

MergeInfoPP::~MergeInfoPP()
//-------------------------
{
    #if INSTRUMENTS
    int i;

    Log.printf( "\nMergeInfoPP\n-----------\n" );
    Log.printf( "       _requests: %5u entries\n", _numFiles );
    for( i = 0; i < _numFiles; i += 1 ) {
        Log.printf( "         [ %3d ]: %5u entries\n", i, _reqCount[ i ] );
    }
    #endif

    delete [] _requests;
    delete [] _reqCount;
    _pool.ragnarok();
}

void MergeInfoPP::addRequest( const MergeOffset & in, uint_32 outOff,
                              uint_32 sibOff )
//-------------------------------------------------------------------
{
    InfoPPReq *     req;

    _reqCount[ in.fileIdx ]++;

    req = _requests[ in.fileIdx ];
    if( req->unused >= PPREQSIZE ) {
        req = (InfoPPReq *) _pool.alloc(); // constructor not called!
        req->unused = 0;
        req->next = _requests[ in.fileIdx ];
        _requests[ in.fileIdx ] = req;
    }

    req->nodes[ req->unused ].inOff = in.offset;
    req->nodes[ req->unused ].outOff = outOff;
    req->nodes[ req->unused ].sibOff = sibOff;
    req->unused++;
}

int CompReq( const InfoPPReqNode ** lhs, const InfoPPReqNode ** rhs )
//-------------------------------------------------------------------
{
    if( (*rhs)->inOff > (*lhs)->inOff ) {
        return( -1 );
    } else if( (*rhs)->inOff == (*lhs)->inOff ) {
        return( 0 );
    } else {
        return( 1 );
    }
}

typedef (*icbvv)(const void *, const void *);

void MergeInfoPP::execute( MergeInfoSection * sect,
                            MergeFile & outFile,
                            WCPtrOrderedVector<MergeFile> & inFiles )
//-------------------------------------------------------------------
{
    uint        i;
    uint        j;
    uint        k;
    PPReqNP *   vector;
    InfoPPReq * req;
    uint        maxSize = 0;

    for( i = 0; i < _numFiles; i += 1 ) {
        maxSize = (_reqCount[ i ] > maxSize) ? _reqCount[ i ] : maxSize;
    }

    vector = new PPReqNP[ maxSize ];

    for( i = 0; i < _numFiles; i += 1 ) {
        k = 0;
        for( j = 0, req = _requests[ i ]; j < _reqCount[ i ]; j += 1 ) {
            vector[ j ] = &req->nodes[ k++ ];
            if( k >= req->unused ) {
                req = req->next;
                k = 0;
            }
        }

        // now j == _reqCount[ i ]

        if( j ) {
            qsort( vector, j, sizeof( InfoPPReqNode * ), (icbvv) &CompReq );

            doFile( sect, outFile, *inFiles[ i ], i, vector, j );
        }
    }

    delete [] vector;
}

void MergeInfoPP::doFile( MergeInfoSection * sect,
                          MergeFile & outFile,
                          MergeFile & inFile,
                          uint fileIdx,
                          InfoPPReqNode ** reqs,
                          uint numReqs )
//------------------------------------------------------------------
{
    MergeAbbrevSection &    abbrevs( sect->getAbbrev() );
    MergeLineSection &      line( sect->getLine() );
    MergeAbbrev *   abbrev;
    uint_32         abbcode;
    uint_32         offset;
    uint            attribIdx;
    uint            index;
    uint_8          addrSize;
    uint_8          relFile;
    InfoPPReqNode * node;
    MergeOffset     ref( (uint_8) fileIdx, 0 );
    MergeRelocate & reloc( sect->getReloc() );
    MergeDIE *      referredTo;

    addrSize = sect->getAddrSize();

    for( index = 0; index < numReqs; index += 1 ) {
        node = reqs[ index ];
        offset = node->inOff;
        outFile.seekSect( DR_DEBUG_INFO, node->outOff );

        if( offset == 0 ) {
            outFile.writeULEB128( 0 );
            continue;                       // <----------- unusual flow
        }

        abbcode = inFile.readULEB128( DR_DEBUG_INFO, offset );
        if( abbcode == 0 ) {
            outFile.writeULEB128( 0 );
            continue;                       // <----------- unusual flow
        }

        abbrev = abbrevs.getAbbrev( abbcode );
        outFile.writeULEB128( abbcode );

        for( attribIdx = 0; attribIdx < abbrev->entries(); attribIdx += 1 ) {
            MergeAttrib & att( (*abbrev)[ attribIdx ] );

            switch( att.attrib() ) {
            case DW_AT_sibling:
                inFile.skipForm( DR_DEBUG_INFO, offset, att.form(), addrSize );
                outFile.writeForm( att.form(), node->sibOff, addrSize );
                break;
            case DW_AT_decl_file:
                relFile = (uint_8) inFile.readForm( DR_DEBUG_INFO, offset,
                                                 att.form(), addrSize );
                outFile.writeForm( att.form(),
                                    line.getNewFileIdx( (uint_8) fileIdx, relFile ),
                                    addrSize );
                break;
            case DW_AT_macro_info:                  // NYI
            case DW_AT_WATCOM_references_start:     // NYI
                inFile.copyFormTo( outFile, DR_DEBUG_INFO, offset,
                                    att.form(), addrSize );
                break;
            default:
                switch( att.form() ) {
                case DW_FORM_ref4:
                case DW_FORM_ref2:
                case DW_FORM_ref1:
                case DW_FORM_ref_addr:
                case DW_FORM_ref_udata:
                    ref.offset = inFile.readForm( DR_DEBUG_INFO, offset,
                                                att.form(), addrSize );
                    if( ref.offset == 0 ) {
                        outFile.writeForm( att.form(), 0, addrSize );
                    } else {
                        referredTo = reloc.getReloc( ref );

                        #if INSTRUMENTS
                            if( referredTo == NULL ) {
                                Log.printf( "Ack -- can't find a replacement" );
                                Log.printf( " for %#x %s!\n", att.attrib(), ref.getString() );
                            }
                        #endif

                        outFile.writeForm( att.form(), referredTo->getNewOff(),
                                            addrSize );
                    }
                    break;
                case DW_FORM_ref8: /* can't handle 8-byte references */
                default:
                    inFile.copyFormTo( outFile, DR_DEBUG_INFO, offset,
                                        att.form(), addrSize );
                }
            }
        }
    }
}
