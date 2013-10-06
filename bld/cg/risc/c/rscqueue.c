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


#include "cgstd.h"
#include "coderep.h"
#include "procdef.h"
#include "ocentry.h"
#include "dbcue.h"
#include "owl.h"
#include "model.h"
#include "zoiks.h"
#include "utils.h"
#include "addrname.h"
#include "objout.h"

extern  void    ObjBytes( const void *, unsigned );
extern  void    OutReloc( code_lbl *, owl_reloc_type, unsigned );
extern  void    DoAlignment( offset );
extern  void    DbgSetBase( void );
extern  void    TellScrapLabel( code_lbl * );
extern  void    OutFuncStart( code_lbl *, offset, cg_linenum );
extern  void    OutFuncEnd( offset );
extern  void    OutPDataRec( code_lbl *, offset, offset );

extern  void    EncodeRet( oc_ret * );
extern  void    EncodeCall( oc_handle * );
extern  void    EncodeJump( oc_handle * );
extern  void    EncodeCond( oc_jcond * );

extern  void    DbgRtnBeg( dbg_rtn *, offset );
extern  void    DbgProEnd( dbg_rtn *, offset );
extern  void    DbgBlkBeg( dbg_block *, offset );
extern  void    DbgBlkEnd( dbg_block *, offset );
extern  void    DbgEpiBeg( dbg_rtn *, offset );
extern  void    DbgRtnEnd( dbg_rtn *, offset );

static  offset  LastUnique = ADDR_UNKNOWN;
static  any_oc  debugOC;

static  offset  procStart;
static  offset  prologueEnd;
static  code_lbl *procLabel;

static  void    saveDebug( any_oc *oc ) {
/***************************************/

    if( ( debugOC.oc_header.class & GET_BASE ) != OC_DEAD ) {
        _Zoiks( ZOIKS_103 );
    }
    Copy( oc, &debugOC, oc->oc_header.reclen );
}

static  void    dumpDebug( void ) {
/*********************************/

    offset              loc;
    oc_class            class;

    class = debugOC.oc_header.class;
    if( ( class & GET_BASE ) != OC_INFO ) return;
    loc = AskLocation();
    switch( class & INFO_MASK ) {
    case INFO_LINE:
        OutLineNum( debugOC.oc_linenum.line, debugOC.oc_linenum.label_line );
        break;
    case INFO_DBG_RTN_BEG:
        if( _IsModel( DBG_LOCALS ) ) {
            DbgRtnBeg( debugOC.oc_debug.ptr, loc );
        }
        break;
    case INFO_DBG_EPI_BEG:
        if( _IsModel( DBG_LOCALS ) ) {
            DbgEpiBeg( debugOC.oc_debug.ptr, loc );
        }
        break;
    case INFO_DBG_BLK_BEG:
        DbgBlkBeg( debugOC.oc_debug.ptr, loc );
        break;
    }
    debugOC.oc_header.class = OC_DEAD;
}

static  void    doInfo( any_oc *oc ) {
/************************************/

    oc_class            base;
    offset              lc;

    lc = 0;
    base = oc->oc_header.class & INFO_MASK;
    switch( base ) {
    case INFO_LDONE:
        TellScrapLabel( oc->oc_handle.handle );
        break;
    case INFO_DEAD_JMP:
        _Zoiks( ZOIKS_036 );
        break;
    case INFO_DBG_RTN_BEG:
    case INFO_DBG_EPI_BEG:
    case INFO_DBG_BLK_BEG:
    case INFO_FUNC_START:
    case INFO_LINE:
        saveDebug( oc );
        break;
    case INFO_DBG_BLK_END:
        DbgBlkEnd( oc->oc_debug.ptr, AskLocation() );
        break;
    case INFO_DBG_PRO_END:
        lc = AskLocation();
        if( _IsModel( DBG_LOCALS ) ) {
            DbgProEnd( oc->oc_debug.ptr, lc );
        }
        prologueEnd = lc;
        break;
    case INFO_DBG_RTN_END:
        if( _IsModel( NUMBERS ) ) {
            OutFuncEnd( lc );
        }
        if( _IsModel( DBG_LOCALS ) ) {
            DbgRtnEnd( oc->oc_debug.ptr, AskLocation() );
        }
        if( _IsModel( OBJ_COFF ) ) {
            // Only emit .pdata section into COFF objects; there might be some
            // better mechanism to decide whether .pdata should be emitted.
            lc = AskLocation();
            OutPDataRec( procLabel, lc - procStart, prologueEnd - procStart );
        }
        break;
    }
}

extern  void    OutputOC( any_oc *oc ) {
/**************************************/

    oc_class            base;
    offset              lc;
    offset              len;
    code_lbl            *lbl;

    base = oc->oc_header.class & GET_BASE;
    if( base != OC_LABEL ) {
        dumpDebug();
    }
    switch( base ) {
    case OC_RCODE:
        if( _HasReloc( &oc->oc_rins ) ) {
            OutReloc( oc->oc_rins.sym, oc->oc_rins.reloc, 0 );
            if( _IsModel( OBJ_COFF ) ) {
                // ELF doesn't do pair relocs, just don't emit them
                if( oc->oc_rins.reloc == OWL_RELOC_HALF_HI ) {
                    OutReloc( oc->oc_rins.sym, OWL_RELOC_PAIR, 0 );
                }
            }
        }
        ObjBytes( &oc->oc_rins.opcode, oc->oc_rins.hdr.objlen );
        break;
    case OC_CODE:
    case OC_DATA:
    case OC_BDATA:
    case OC_IDATA:
        ObjBytes( oc->oc_entry.data, oc->oc_entry.hdr.objlen );
        break;
    case OC_RET:
        EncodeRet( &oc->oc_ret );
        break;
    case OC_JMP:
        EncodeJump( &oc->oc_handle );
        break;
    case OC_CALL:
        EncodeCall( &oc->oc_handle );
        break;
    case OC_JCOND:
        EncodeCond( &oc->oc_jcond );
        break;
    case OC_LABEL:
        /* figure out number of bytes to pad */
        lc = AskLocation();
        len = -lc & oc->oc_handle.hdr.objlen;
        lbl = oc->oc_handle.handle;
        if( AskIfUniqueLabel( lbl ) ) {
            if( (lc == LastUnique) && (len == 0) ) {
                /* Two unique labels have ended up next to each other.
                   Pad out to next label alignment boundry. */
                len = oc->oc_handle.hdr.objlen + 1;
            }
            LastUnique = lc + len;
        }
        DoAlignment( len );
        OutLabel( lbl );            /* do patches*/
        if( _TstStatus( lbl, PROCEDURE ) ) {
            lc = AskLocation();
            procStart = lc;
            procLabel = lbl;
            if( _IsModel( NUMBERS ) ) {
                OutFuncStart( lbl, lc, oc->oc_handle.line );
            }
        }
        DbgSetBase();
        dumpDebug();
        break;
    case OC_INFO:
        doInfo( oc );
        break;
    default:
        _Zoiks( ZOIKS_036 );
    }
}
