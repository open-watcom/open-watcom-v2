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
#include "addrname.h"
#include "symdbg.h"
#include "ocentry.h"
#include "system.h"
#include "model.h"
#include "jumps.h"
#include "zoiks.h"
#include "fppatch.h"
#include "regset.h"
#include "rttable.h"
#include "i86obj.h"
#include "objout.h"
#include "cgauxinf.h"
#include "dbsyms.h"
#include "objio.h"
#include "optmain.h"
#include "intrface.h"
#include "feprotos.h"


extern  void            OutFPPatch(fp_patches);
extern  void            OutImport(cg_sym_handle,fix_class,bool);
extern  void            OutRTImport(rt_class,fix_class);
extern  void            OutRTImportRel(rt_class,fix_class,bool rel);
extern  void            OutSelect(bool);
extern  void            TellScrapLabel(label_handle);
extern  void            GenKillLabel(label_handle);
extern  void            TellKeepLabel(label_handle);
extern  void            OutDataShort(unsigned_16);
extern  void            OutDataLong(unsigned_32);
extern  void            OutPatch(label_handle,patch_attr);
extern  void            OutReloc(segment_id,fix_class,bool);
extern  void            OutDataByte(byte);
extern  void            OutDBytes(unsigned,const byte*);
extern  void            SetUpObj(bool);
extern  void            EmitOffset(offset);
extern  void            EmitPtr(pointer);
extern  abspatch_handle NewAbsPatch( void );
extern  void            EmitByte(byte);
extern  void            EmitSegId(segment_id);
extern  void            InsertByte(byte);
extern  int             OptInsSize(oc_class,oc_dest_attr);
extern  unsigned        SavePendingLine(unsigned);
extern bool             UseImportForm(fe_attr);
extern void             OutSpecialCommon(import_handle,fix_class,bool);

static void             DoRelocRef( cg_sym_handle sym, cg_class class, segment_id seg, offset val, escape_class kind );
static  void            OutShortDisp( label_handle lbl );
static  void            OutCodeDisp( label_handle lbl, fix_class f, bool rel, oc_class class );

extern byte             *NopLists[];

/* Grammar of Escapes :*/
/**/
/* Sequence                                                 Meaning*/
/* ========                                                 ========*/
/**/
/* ESC, ESC                                                 actual ESC byte */
/* ESC, IMP <LDOF|OFST|BASE|SELF>, cg_sym_handle <,offset>  import reference */
/* ESC, REL <LDOF|BASE|OFST>, segid                         relocate, with seg-id */
/* ESC, SYM <LDOF|OFST|SELF>, cg_sym_handle <,offset>       unknown sym ref */
/* ESC, LBL <LDOF|OFST|SELF>, segid, lbl_handle, <,offset>  ptr reference */
/* ESC, ABS objhandle, len, offset                          object patch */
/* ESC, FUN byte                                            for 87 instructions*/
/**/
/*       OFST bit on means offset follows*/
/*       LDOF means loader resolved offset*/
/*       BASE means use F_BASE relocation*/

extern  bool    CodeHasAbsPatch( oc_entry *code ) {
/*************************************************/
    byte        *curr;
    byte        *final;

    curr = code->data;
    final = curr + code->hdr.reclen - offsetof( oc_entry, data );
    while( curr < final ) {
        if( *curr++ == ESC ) {
            if( *curr++ == ABS ) {
                return( true );
            }
        }
    }
    return( false );
}


extern  void    DoAbsPatch( abspatch_handle *handle, int len ) {
/**************************************************************/

    EmitByte( ESC );
    EmitByte( ABS );
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


extern  void    DoFunnyRef( int segover ) {
/*****************************************/

/* yea, i know it's backwards*/
    InsertByte( segover );
    InsertByte( FUN );
    InsertByte( ESC );
}


extern  void  DoFESymRef( cg_sym_handle sym, cg_class class, offset val, fe_fixup_types fixup_type )
/**************************************************************************************************/
{
    fe_attr             attr;
    escape_class        kind;

    kind = 0;
    switch( fixup_type ) {
    case FE_FIX_BASE:
        val = 0;
        kind |= BASE; /* base segment relocation */
        break;
    case FE_FIX_SELF:
        kind |= SELF; /* self relative offset relocation */
        break;
    }
    if( val != 0 ) {
        kind |= OFST;           /* offset follows*/
    }
    if( class == CG_FE ) {
        attr = FEAttr( sym );
        if( ((kind & BASE) == 0) && _IsTargetModel( WINDOWS )
         && ( attr & FE_PROC ) && (*(call_class *)FindAuxInfoSym( sym, CALL_CLASS ) & FAR_CALL) ) {
            kind |= LDOF;
        }
        if( UseImportForm( attr ) ) { /* 90-05-22 */
            EmitByte( ESC );
            EmitByte( IMP | kind );
            EmitPtr( sym );
            if( kind & OFST ) {
                EmitOffset( val );
            }
        } else if( attr & FE_GLOBAL ) {
            DoRelocRef( sym, CG_FE, AskSegID( sym, CG_FE ), val, kind );
        } else {
            // handle mismatch Fix it!
            DoRelocRef( (cg_sym_handle)AskForSymLabel( sym, CG_FE ), CG_LBL, AskSegID( sym, CG_FE ), val, kind );
        }
    } else {                                /* CG_TBL, CG_LBL or CG_BCK*/
        DoRelocRef( sym, class, AskSegID( sym, class ), val, kind );
    }
}


extern  void    DoSymRef( name *opnd, offset val, bool base ) {
/*************************************************************/

    DoFESymRef( opnd->v.symbol, opnd->m.memory_type, val, base ? FE_FIX_BASE : FE_FIX_OFF );
}


extern  void    DoSegRef( segment_id seg ) {
/******************************************/

    EmitByte( ESC );
    EmitByte( REL | BASE | OFST );
    EmitSegId( seg );
    EmitOffset( 0 );
}

static  void    DoRelocRef( cg_sym_handle sym, cg_class class,
                    segment_id seg, offset val, escape_class kind )
/*****************************************************************/
{
    offset              addr;
    label_handle        lbl;

    if( kind & BASE ) {                       /* don't need offset*/
        EmitByte( ESC );
        EmitByte( REL | kind | OFST );
        EmitSegId( seg );
        EmitOffset( 0 );
    } else {
        lbl = AskForSymLabel( sym, class );
        addr = AskAddress( lbl );
        if( (addr != ADDR_UNKNOWN) && !AskIfCommonLabel( lbl ) ) {
            EmitByte( ESC );
            EmitByte( REL | kind | OFST );
            EmitSegId( seg );
            val += addr;
            EmitOffset( val );
        } else if( class == CG_FE ) {
            EmitByte( ESC );
            EmitByte( SYM | kind );         /* patch may be needed*/
            EmitPtr( sym );
            if( kind & OFST ) {
                EmitOffset( val );
            }
        } else {
            DoLblRef( lbl, seg, val, kind );
        }
    }
}

extern  void    DoLblRef( label_handle lbl, segment_id seg,
                        offset val, escape_class kind )
/*****************************************************/
{
    EmitByte( ESC );
    EmitByte( LBL | kind );
    EmitSegId( seg );
    EmitPtr( lbl );
    if( kind & OFST ) {
        EmitOffset( val );
    }
}

static void SendBytes( const void *ptr, unsigned len )
/****************************************************/
{
    if( len != 0 ) {
        OutDBytes( len, ptr );
    }
}

#define INFO_NOT_DEBUG      INFO_SELECT
static oc_class SaveDbgOc = INFO_NOT_DEBUG;
static pointer  SaveDbgPtr;
static offset   LastUnique = ADDR_UNKNOWN;

static  void    DumpSavedDebug( void )
/******************************/
{
    switch( SaveDbgOc ) {
    case INFO_DBG_RTN_BEG:
        DbgRtnBeg( SaveDbgPtr, AskLocation() );
        break;
    case INFO_DBG_BLK_BEG:
        DbgBlkBeg( SaveDbgPtr, AskLocation() );
        break;
    case INFO_DBG_EPI_BEG:
        DbgEpiBeg( SaveDbgPtr, AskLocation() );
        break;
    }
    SaveDbgOc = INFO_NOT_DEBUG;
}

static void DoAlignment( int len ) {
/**********************************/

    byte                *ptr;
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
    for( ptr = &NopLists[i][1]; nop != len; --nop ) ptr += nop;
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
    if( (class & GET_BASE) == OC_JCOND ) {
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
    } else if( (class & GET_BASE) == OC_JMP && objlen == OptInsSize( OC_JMP, OC_DEST_SHORT ) ) {
        _OutJShort;
        OutShortDisp( oc->oc_handle.handle );
    } else {
        if( class & ATTR_FAR ) {
            f = F_PTR;
            rel = false;
            if( ( class & GET_BASE ) == OC_CALL ) {
                if( objlen == OptInsSize( OC_CALL, OC_DEST_CHEAP ) ) {
                    f = F_OFFSET;
                    rel = true;
                    class &= ~ ATTR_FAR;
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
            if( ( class & GET_BASE ) == OC_CALL ) {
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
        if( class & ATTR_FAR ) {
            _OutFarD( 0, 0 );
        } else {
            _OutFarOff( 0 );
        }
    } else if( AskIfCommonLabel( lbl ) ) {
        OutSpecialCommon( (import_handle)(pointer_int)sym, f, rel );
        _OutFarOff( 0 );
    } else if( sym != NULL && UseImportForm( FEAttr( sym ) ) ) { /* 90-05-22 */
        OutImport( sym, f, rel );
        if( class & ATTR_FAR ) {
            _OutFarD( 0, 0 );
        } else {
            _OutFarOff( 0 );
        }
    } else {                /* patch, to be done later*/
        addr = AskAddress( lbl );
        if( ( class & ATTR_FAR ) == 0 ) {
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


static  label_handle ExpandObj( byte *cur, int explen ) {
/**********************************************************/

    byte                *fini;
    escape_class        key;
    label_handle        lbl;
    cg_sym_handle       sym;
    offset              val = 0;
    segment_id          seg;
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
        key = *cur++;
        if( key == ESC ) {
            OutDataByte( ESC );
            continue;
        }
        if( key & BASE ) {
            class = F_BASE;
        } else if( key & LDOF ) {
            class = F_LDR_OFFSET;
        } else {
            class = F_OFFSET;
        }
        if( key & SELF ) {
            rel = true;
        } else {
            rel = false;
        }
        switch( key & ~MASK ) {
        case REL:
            seg = *(segment_id *)cur;
            cur += sizeof( segment_id );
            OutReloc( seg, class, rel );
            val = 0;
            break;
        case SYM:          /* never BASE*/
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
        case LBL:          /* never BASE*/
            seg = *(segment_id *)cur;
            cur += sizeof( segment_id );
            lbl = *(pointer *)cur;
            cur += sizeof( pointer );
            if( AskIfRTLabel( lbl ) ) {
                OutRTImportRel( SYM2RTIDX( AskForLblSym( lbl ) ), F_OFFSET, false );
                val = 0;
            } else {
                if( AskIfCommonLabel( lbl ) ) {
                    OutSpecialCommon( (import_handle)(pointer_int)AskForLblSym( lbl ), class, rel );
                } else {
                    OutReloc( seg, class, rel );
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
        case IMP:
            OutImport( *(pointer *)cur, class, rel );
            cur += sizeof( pointer );
            val = 0;
            break;
        case ABS:
            val = *cur;
            cur++;
            OutAbsPatch( *(pointer *)cur, val );
            cur += sizeof( pointer );
            continue;
        case FUN:
            OutFPPatch( *cur );
            cur++;
            continue;
        default:
            _Zoiks( ZOIKS_038 );
            break;
        }
        if( key & OFST ) {
            val += *(offset *)cur;
            cur += sizeof( offset );
        }
        if( class == F_BASE ) {
            _OutFarSeg( val );
        } else if ( class == F_OFFSET || class == F_LDR_OFFSET ) {
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

    base = oc->oc_header.class & GET_BASE;
    if( base == OC_RET && (oc->oc_header.class & ATTR_NORET) )
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
            OutSpecialCommon( (import_handle)(pointer_int)sym, F_OFFSET, false );
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
        len = M_RET;
        base = oc->oc_header.class;
        if( base & ATTR_FAR ) {
            len |= B_RET_LONG;
        }
        if( base & ATTR_IRET ) {
            len |= B_RET_IRET;
        }
        if( base & ATTR_POP ) {
            OutDataByte( len );
            OutDataShort( oc->oc_ret.pops );
        } else {
            OutDataByte( len | B_RET_NOPOP );
        }
        break;
    case OC_INFO:
        base = oc->oc_header.class & INFO_MASK;
        switch( base ) {
        case INFO_LINE:
            OutLineNum( oc->oc_linenum.line, oc->oc_linenum.label_line );
            break;
        case INFO_LDONE:
            TellScrapLabel( oc->oc_handle.handle );
            break;
        case INFO_DEAD_JMP:
            _Zoiks( ZOIKS_036 );
            break;
        case INFO_DBG_RTN_BEG:
        case INFO_DBG_BLK_BEG:
        case INFO_DBG_EPI_BEG:
            SaveDbgOc = base;
            SaveDbgPtr = oc->oc_debug.ptr;
            break;
        case INFO_DBG_BLK_END:
            DbgBlkEnd( oc->oc_debug.ptr, AskLocation() );
            break;
        case INFO_DBG_PRO_END:
            DbgProEnd( oc->oc_debug.ptr, AskLocation() );
            break;
        case INFO_DBG_RTN_END:
            DbgRtnEnd( oc->oc_debug.ptr, AskLocation() );
            OutLineNum( 0, false ); /* Kill pending line number */
            break;
        case INFO_SELECT:
            OutSelect( oc->oc_select.starts );
            break;
        }
        break;
    default :
        _Zoiks( ZOIKS_036 );
        break;
    }
}
