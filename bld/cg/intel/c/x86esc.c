/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include "_cgstd.h"
#include "coderep.h"
#include "addrname.h"
#include "symdbg.h"
#include "system.h"
#include "model.h"
#include "jumps.h"
#include "zoiks.h"
#include "x86objd.h"
#include "objout.h"
#include "cgauxcc.h"
#include "cgauxinf.h"
#include "dbsyms.h"
#include "objio.h"
#include "optmain.h"
#include "opttell.h"
#include "intrface.h"
#include "x86obj.h"
#include "x86esc.h"
#include "encode.h"
#include "pccode.h"
#include "pcencode.h"
#include "x86enc.h"
#include "x86nopli.h"
#include "feprotos.h"


static void     DoRelocRef( cg_sym_handle sym, cg_class class, segment_id segid, offset val, escape_class esc_attr );
static void     OutShortDisp( label_handle lbl );
static void     OutCodeDisp( label_handle lbl, fix_class f, bool rel, oc_class class );

bool    CodeHasAbsPatch( oc_entry *code )
/***************************************/
{
    byte        *curr;
    byte        *final;

    curr = code->data;
    final = curr + code->hdr.reclen - offsetof( oc_entry, data );
    while( curr < final ) {
        if( *curr++ == ESC ) {
            if( *curr++ == ESC_ABS ) {
                return( true );
            }
        }
    }
    return( false );
}


void    DoAbsPatch( abspatch_handle *handle, int len )
/****************************************************/
{
    EmitByte( ESC );
    EmitByte( ESC_ABS );
    if( len == 1 ) {
        EmitByte( BYTE_PATCH );
    } else if( len == 2 ) {
        EmitByte( WORD_PATCH );
    } else { /*4*/
        EmitByte( LONG_PATCH );
    }
    *handle = NewAbsPatch();
    EmitPtr( *handle );
}


void  DoFESymRef( cg_sym_handle sym, cg_class class, offset val, fe_fixup_types fixup_type )
/******************************************************************************************/
{
    fe_attr             attr;
    escape_class        esc_attr;

    esc_attr = 0;
    switch( fixup_type ) {
    case FE_FIX_BASE:
        val = 0;
        esc_attr |= ESCA_BASE;  /* base segment relocation */
        break;
    case FE_FIX_SELF:
        esc_attr |= ESCA_SELF;  /* self relative offset relocation */
        break;
    }
    if( val != 0 ) {
        esc_attr |= ESCA_OFST;  /* offset follows*/
    }
    if( class == CG_FE ) {
        attr = FEAttr( sym );
        if( ((esc_attr & ESCA_BASE) == 0)
          && _IsTargetModel( CGSW_X86_WINDOWS )
          && (attr & FE_PROC)
          && ((call_class_target)(pointer_uint)FindAuxInfoSym( sym, FEINF_CALL_CLASS_TARGET ) & FECALL_X86_FAR_CALL) ) {
            esc_attr |= ESCA_LDOF;
        }
        if( UseImportForm( attr ) ) {
            EmitByte( ESC );
            EmitByte( ESC_IMP | esc_attr );
            EmitPtr( sym );
            if( esc_attr & ESCA_OFST ) {
                EmitOffset( val );
            }
        } else if( attr & FE_GLOBAL ) {
            DoRelocRef( sym, CG_FE, AskSegID( sym, CG_FE ), val, esc_attr );
        } else {
            // handle mismatch Fix it!
            DoRelocRef( (cg_sym_handle)AskForSymLabel( sym, CG_FE ), CG_LBL, AskSegID( sym, CG_FE ), val, esc_attr );
        }
    } else {                                /* CG_TBL, CG_LBL or CG_BCK*/
        DoRelocRef( sym, class, AskSegID( sym, class ), val, esc_attr );
    }
}


void    DoSymRef( name *opnd, offset val, bool base )
/***************************************************/
{
    DoFESymRef( opnd->v.symbol, opnd->m.memory_type, val, base ? FE_FIX_BASE : FE_FIX_OFF );
}


void    DoSegRef( segment_id segid )
/**********************************/
{
    EmitByte( ESC );
    EmitByte( ESC_REL | ESCA_BASE | ESCA_OFST );
    EmitSegId( segid );
    EmitOffset( 0 );
}

static void     DoRelocRef( cg_sym_handle sym, cg_class class,
                    segment_id segid, offset val, escape_class esc_attr )
/***********************************************************************/
{
    offset              addr;
    label_handle        lbl;

    if( esc_attr & ESCA_BASE ) {                /* don't need offset*/
        EmitByte( ESC );
        EmitByte( ESC_REL | esc_attr | ESCA_OFST );
        EmitSegId( segid );
        EmitOffset( 0 );
    } else {
        lbl = AskForSymLabel( sym, class );
        addr = AskAddress( lbl );
        if( (addr != ADDR_UNKNOWN) && !AskIfCommonLabel( lbl ) ) {
            EmitByte( ESC );
            EmitByte( ESC_REL | esc_attr | ESCA_OFST );
            EmitSegId( segid );
            val += addr;
            EmitOffset( val );
        } else if( class == CG_FE ) {
            EmitByte( ESC );
            EmitByte( ESC_SYM | esc_attr );     /* patch may be needed*/
            EmitPtr( sym );
            if( esc_attr & ESCA_OFST ) {
                EmitOffset( val );
            }
        } else {
            DoLblRef( lbl, segid, val, esc_attr );
        }
    }
}

void    DoLblRef( label_handle lbl, segment_id segid,
                    offset val, escape_class esc_attr )
/*****************************************************/
{
    EmitByte( ESC );
    EmitByte( ESC_LBL | esc_attr );
    EmitSegId( segid );
    EmitPtr( lbl );
    if( esc_attr & ESCA_OFST ) {
        EmitOffset( val );
    }
}

static void SendBytes( const byte *ptr, unsigned len )
/****************************************************/
{
    if( len != 0 ) {
        OutDBytes( len, ptr );
    }
}

#define OC_INFO_NOT_DEBUG   OC_INFO_SELECT

static oc_class SaveDbgOc = OC_INFO_NOT_DEBUG;
static pointer  SaveDbgPtr;
static offset   LastUnique = ADDR_UNKNOWN;

static  void    DumpSavedDebug( void )
/******************************/
{
    switch( SaveDbgOc ) {
    case OC_INFO_DBG_RTN_BEG:
        DbgRtnBeg( SaveDbgPtr, AskLocation() );
        break;
    case OC_INFO_DBG_BLK_BEG:
        DbgBlkBeg( SaveDbgPtr, AskLocation() );
        break;
    case OC_INFO_DBG_EPI_BEG:
        DbgEpiBeg( SaveDbgPtr, AskLocation() );
        break;
    }
    SaveDbgOc = OC_INFO_NOT_DEBUG;
}

void DoAlignment( int len )
/*************************/
{
    const byte          *ptr;
    int                 nop;
    int                 i;
    unsigned            save_line;

    save_line = SavePendingLine( 0 );
    /* get the size of the largest NOP pattern  */
    i = 0;
    for( nop = NopLists[i][0]; len > nop; len -= nop ) {
        SendBytes( &NopLists[i][1], nop );
        i = !i;
    }
    /* find the correct size NOP pattern to use */
    for( ptr = &NopLists[i][1]; nop != len; --nop )
        ptr += nop;
    SendBytes( ptr, len );
    SavePendingLine( save_line );
}


static  void    ExpandCJ( any_oc *oc )
/************************************/
{
    oc_class            class;
    fix_class           f;
    bool                rel;
    obj_length          objlen;

    class = oc->oc_header.class;
    objlen = oc->oc_header.objlen;
    if( OC_BASE_CLASS( class ) == OC_JCOND ) {
        if( objlen == OptInsSize( OC_JCOND, OC_DEST_NEAR ) ) {
            if( _CPULevel( CPU_386 ) ) {
                _OutJCondNear( oc->oc_jcond.cond );
            } else {
                _OutJCond( ReverseCondition( oc->oc_jcond.cond ) );
                OutDataByte( OptInsSize( OC_JMP, OC_DEST_NEAR ) );
                _OutJNear;
            }
            OutCodeDisp( oc->oc_jcond.handle, F_OFFSET, true, class );
        } else {
            _OutJCond( oc->oc_jcond.cond );
            OutShortDisp( oc->oc_jcond.handle );
        }
    } else if( OC_BASE_CLASS( class ) == OC_JMP && objlen == OptInsSize( OC_JMP, OC_DEST_SHORT ) ) {
        _OutJShort;
        OutShortDisp( oc->oc_handle.handle );
    } else {
        if( class & OC_ATTR_FAR ) {
            f = F_PTR;
            rel = false;
            if( OC_BASE_CLASS( class ) == OC_CALL ) {
                if( objlen == OptInsSize( OC_CALL, OC_DEST_CHEAP ) ) {
                    f = F_OFFSET;
                    rel = true;
                    class &= ~ OC_ATTR_FAR;
                    _OutCCyp;
                } else {
                    _OutCFar;
                }
            } else {
                _OutJFar;
            }
        } else {
            f = F_OFFSET;
            rel = true;
            if( OC_BASE_CLASS( class ) == OC_CALL ) {
                _OutCNear;
            } else {
                _OutJNear;
            }
        }
        OutCodeDisp( oc->oc_handle.handle, f, rel, class );
    }
}


static  void    OutShortDisp( label_handle lbl ) {
/************************************************/

    offset              addr;

    addr = AskAddress( lbl );
    if( addr == ADDR_UNKNOWN ) {
        OutPatch( lbl, _SHORT_PATCH );
        addr = 0;
    }
    _OutShortD( addr );
}


static  void    OutCodeDisp( label_handle lbl, fix_class f,
                             bool rel, oc_class class ) {
/********************************************************/

    offset              addr;
    cg_sym_handle       sym;

    sym = AskForLblSym( lbl );
    if( AskIfRTLabel( lbl ) ) {
        OutRTImport( SYM2RTIDX( sym ), f );
        if( class & OC_ATTR_FAR ) {
            _OutFarD( 0, 0 );
        } else {
            _OutFarOff( 0 );
        }
    } else if( AskIfCommonLabel( lbl ) ) {
        OutSpecialCommon( SYM2IMPHDL( sym ), f, rel );
        _OutFarOff( 0 );
    } else if( sym != NULL && UseImportForm( FEAttr( sym ) ) ) { /* 90-05-22 */
        OutImport( sym, f, rel );
        if( class & OC_ATTR_FAR ) {
            _OutFarD( 0, 0 );
        } else {
            _OutFarOff( 0 );
        }
    } else {                /* patch, to be done later*/
        addr = AskAddress( lbl );
        if( ( class & OC_ATTR_FAR ) == 0 ) {
            if( addr == ADDR_UNKNOWN ) {
                OutPatch( lbl, _NEAR_PATCH );
                addr = 0;
            }
            _OutNearD( addr );
        } else {
            if( addr == ADDR_UNKNOWN ) {
                OutPatch( lbl, _OFFSET_PATCH );
            }
            _OutFarOff( addr );
            OutReloc( AskSegID( sym, CG_FE ), F_BASE, false );
            _OutFarSeg( 0 );
        }
    }
}


static  label_handle ExpandObj( byte *cur, int explen )
/*****************************************************/
{
    byte                *fini;
    escape_class        esc_class;
    label_handle        lbl;
    cg_sym_handle       sym;
    offset              val = 0;
    segment_id          segid;
    fix_class           class;
    bool                rel;
    unsigned            len;

    lbl = NULL;
    fini = cur + explen;
    while( cur < fini ) {
        len = 0;
        while( cur[len] != ESC ) {
            ++len;
            if( cur + len >= fini ) {
                break;
            }
        }
        if( len != 0 ) {
            OutDBytes( len, cur );
            cur += len;
            if( cur >= fini ) {
                break;
            }
        }
        cur++;
        esc_class = *cur++;
        if( esc_class == ESC ) {
            OutDataByte( ESC );
            continue;
        }
        if( esc_class & ESCA_BASE ) {
            class = F_BASE;
        } else if( esc_class & ESCA_LDOF ) {
            class = F_LDR_OFFSET;
        } else {
            class = F_OFFSET;
        }
        if( esc_class & ESCA_SELF ) {
            rel = true;
        } else {
            rel = false;
        }
        switch( esc_class & ~ESCA_MASK ) {
        case ESC_REL:
            segid = *(segment_id *)cur;
            cur += sizeof( segment_id );
            OutReloc( segid, class, rel );
            val = 0;
            break;
        case ESC_SYM:       /* never BASE*/
            sym = *(pointer *)cur;
            cur += sizeof( pointer );
            lbl = AskForSymLabel( sym, CG_FE );
            val = AskAddress( lbl );
            if( val == ADDR_UNKNOWN ) {
                if( class == F_OFFSET || class == F_LDR_OFFSET ) {
                    OutPatch( lbl, ADD_PATCH | _OFFSET_PATCH );
                } else {
                    OutPatch( lbl, ADD_PATCH | WORD_PATCH );
                }
                val = 0;
            }
            OutReloc( AskSegID( sym, CG_FE ), class, rel );
            break;
        case ESC_LBL:       /* never BASE*/
            segid = *(segment_id *)cur;
            cur += sizeof( segment_id );
            lbl = *(pointer *)cur;
            cur += sizeof( pointer );
            if( AskIfRTLabel( lbl ) ) {
                OutRTImportRel( SYM2RTIDX( AskForLblSym( lbl ) ), F_OFFSET, false );
                val = 0;
            } else {
                if( AskIfCommonLabel( lbl ) ) {
                    OutSpecialCommon( SYM2IMPHDL( AskForLblSym( lbl ) ), class, rel );
                } else {
                    OutReloc( segid, class, rel );
                }
                val = AskAddress( lbl );
                if( val == ADDR_UNKNOWN ) {
                    if( class == F_OFFSET || class == F_LDR_OFFSET ) {
                        OutPatch( lbl, ADD_PATCH | _OFFSET_PATCH );
                    } else {
                        OutPatch( lbl, ADD_PATCH | WORD_PATCH );
                    }
                    val = 0;
                }
            }
            break;
        case ESC_IMP:
            OutImport( *(pointer *)cur, class, rel );
            cur += sizeof( pointer );
            val = 0;
            break;
        case ESC_ABS:
            val = *cur;
            cur++;
            OutAbsPatch( *(pointer *)cur, val );
            cur += sizeof( pointer );
            continue;
        case ESC_FUN:
            OutFPPatch( *cur );
            cur++;
            continue;
        default:
            _Zoiks( ZOIKS_038 );
            break;
        }
        if( esc_class & ESCA_OFST ) {
            val += *(offset *)cur;
            cur += sizeof( offset );
        }
        if( class == F_BASE ) {
            _OutFarSeg( val );
        } else if( class == F_OFFSET || class == F_LDR_OFFSET ) {
            _OutFarOff( val );      /* offset */
        } else if( class == F_PTR ) {
            _OutFarOff( val );
            _OutFarSeg( 0 );        /* segment */
        }
    }
    return( lbl );
}

void    OutputOC( any_oc *oc, any_oc *next_lbl )
/**********************************************/
{
    label_handle    lbl;
    cg_sym_handle   sym;
    oc_class        base;
    int             len;
    offset          lc;
    byte            *ptr;

    base = OC_BASE_CLASS( oc->oc_header.class );
    if( base == OC_NORET )
        return;
    if( base != OC_LABEL ) {
        DumpSavedDebug();
    }
    SetUpObj( false );
    switch( base ) {
    case OC_CODE:
    case OC_DATA:
        ExpandObj( oc->oc_entry.data, oc->oc_entry.hdr.reclen - offsetof( oc_entry, data ) );
        break;
    case OC_IDATA:
        if( next_lbl != NULL ) { /* cause next_lbl to need no alignment */
            len = -( AskLocation() + oc->oc_entry.hdr.objlen );
            len &= next_lbl->oc_entry.hdr.objlen;
            DoAlignment( len );
        }
        OutSelect( true );
        SendBytes( oc->oc_entry.data, oc->oc_entry.hdr.objlen );
        OutSelect( false );
        break;
    case OC_BDATA:
        SendBytes( oc->oc_entry.data, oc->oc_entry.hdr.objlen );
        break;
    case OC_LABEL:
        /* figure out number of bytes to pad */
        lc = AskLocation();
        len = -lc & oc->oc_handle.hdr.objlen;
        if( AskIfUniqueLabel( oc->oc_handle.handle ) ) {
            if( (lc == LastUnique) && (len == 0) ) {
                /* Two unique labels have ended up next to each other.
                   Pad out to next label alignment boundry. */
                len = oc->oc_handle.hdr.objlen + 1;
            }
            LastUnique = lc + len;
        }
        DoAlignment( len );
        OutLabel( oc->oc_handle.handle );           /* do patches*/
        DbgSetBase();
        DumpSavedDebug();
        break;
    case OC_LREF:
        lbl = oc->oc_handle.handle;
        sym = AskForLblSym( lbl );
        if( AskIfRTLabel( lbl ) ) {
            OutRTImport( SYM2RTIDX( sym ), F_OFFSET );
            lc = 0;
        } else if( AskIfCommonLabel( lbl ) ) {
            OutSpecialCommon( SYM2IMPHDL( sym ), F_OFFSET, false );
            lc = 0;
        } else if( sym != NULL && UseImportForm( FEAttr( sym ) ) ) {
            OutImport( sym, F_OFFSET, false );
            lc = 0;
        } else {
            OutReloc( AskOP(), F_OFFSET, false );
            lc = AskAddress( lbl );
            if( lc == ADDR_UNKNOWN ) {
                OutPatch( lbl, ADD_PATCH | _OFFSET_PATCH );
                lc = 0;
            }
        }
        _OutFarOff( lc );
        break;
    case OC_JMPI:
    case OC_CALLI:
    case OC_JCONDI:
        ptr = oc->oc_entry.data;
        len = 0;
        while( ptr[len] != M_CJINEAR ) {
            ++len; /* skip over top of any prefixes */
        }
        ++len;
        _OutOpndSize;
        ExpandObj( ptr, len );
        ptr += len;

    /* Need to put bits in 2nd byte of instruction distinguishing call vs.*/
    /* jump -- this is because the optimizer may change*/
    /* calls to jumps ...*/

        /* ASSUME RMR BYTE IS NOT DOUBLED BECAUSE OF ESCAPES*/
        if( base == OC_CALLI ) {
            *ptr |= B_IND_RMR_CALL;
        } else {
            *ptr |= B_IND_RMR_JMP;
        }
        OutDataByte( *ptr++ );
        lbl = ExpandObj( ptr, oc->oc_entry.hdr.reclen - offsetof( oc_entry, data ) - 1 - len );
        if( lbl != NULL && base == OC_JMPI ) {
            TellKeepLabel( lbl ); /* make sure label comes out*/
            GenKillLabel( lbl );  /* but kill it when it does*/
        }
        break;
    case OC_CALL:
    case OC_JMP:
    case OC_JCOND:
        ExpandCJ( oc );
        break;
    case OC_RET:
        _OutOpndSize;
        base = oc->oc_header.class;
        len = M_RET;
        if( base & OC_ATTR_IRET ) {
            len |= B_RET_IRET;
        }
        if( base & OC_ATTR_FAR ) {
            len |= B_RET_LONG;
        }
        if( (base & OC_ATTR_POP) == 0 ) {
            len |= B_RET_NOPOP;
        }
        OutDataByte( len );
        if( base & OC_ATTR_POP ) {
            OutDataShort( oc->oc_ret.pops );
        }
        break;
    case OC_INFO:
        base = OC_INFO_CLASS( oc->oc_header.class );
        switch( base ) {
        case OC_INFO_LINE:
            OutLineNum( oc->oc_linenum.line, oc->oc_linenum.label_line );
            break;
        case OC_INFO_LDONE:
            TellScrapLabel( oc->oc_handle.handle );
            break;
        case OC_INFO_DEAD_JMP:
            _Zoiks( ZOIKS_036 );
            break;
        case OC_INFO_DBG_RTN_BEG:
        case OC_INFO_DBG_BLK_BEG:
        case OC_INFO_DBG_EPI_BEG:
            SaveDbgOc = base;
            SaveDbgPtr = oc->oc_debug.ptr;
            break;
        case OC_INFO_DBG_BLK_END:
            DbgBlkEnd( oc->oc_debug.ptr, AskLocation() );
            break;
        case OC_INFO_DBG_PRO_END:
            DbgProEnd( oc->oc_debug.ptr, AskLocation() );
            break;
        case OC_INFO_DBG_RTN_END:
            DbgRtnEnd( oc->oc_debug.ptr, AskLocation() );
            OutLineNum( 0, false ); /* Kill pending line number */
            break;
        case OC_INFO_SELECT:
            OutSelect( oc->oc_select.starts );
            break;
        }
        break;
    default :
        _Zoiks( ZOIKS_036 );
        break;
    }
}
