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


#include "standard.h"
#include "coderep.h"
#include "ocentry.h"
#include "escape.h"
#include "objrep.h"
#include "system.h"
#include "model.h"
#include "jumps.h"
#include "zoiks.h"
#include "fppatch.h"
#include "feprotos.h"

extern  void            DbgSetBase();
extern  void            OutAbsPatch(abspatch*,patch_attr);
extern  void            OutFPPatch(fp_patches);
extern  void            OutImport(sym_handle,fix_class,bool);
extern  void            OutRTImport(int,fix_class);
extern  void            OutRTImportRel(int,fix_class,bool rel);
extern  void            OutSelect(bool);
extern  bool            AskIfRTLabel(label_handle);
extern  sym_handle      AskForLblSym(label_handle);
extern  byte            ReverseCondition(byte);
extern  void            DbgRtnEnd(pointer,offset);
extern  void            DbgBlkEnd(pointer,offset);
extern  void            DbgEpiBeg(pointer,offset);
extern  void            DbgProEnd(pointer,offset);
extern  void            DbgBlkBeg(pointer,offset);
extern  void            DbgRtnBeg(pointer,offset);
extern  offset          AskLocation();
extern  void            TellScrapLabel(label_handle);
extern  void            OutLineNum( cg_linenum,bool);
extern  void            GenKillLabel(pointer);
extern  void            TellKeepLabel(label_handle);
extern  void            OutDataInt(int);
extern  void            OutDataLong(long);
extern  void            OutPatch(label_handle,patch_attr);
extern  void            OutReloc(seg_id,fix_class,bool);
extern  seg_id          AskOP();
extern  void            OutLabel(label_handle);
extern  void            OutDataByte(byte);
extern  void            OutDBytes(unsigned_32,byte*);
extern  void            SetUpObj(bool);
extern  offset          AskAddress(label_handle);
extern  label_handle    AskForSymLabel(pointer,cg_class);
extern  segment_id      AskSegID(pointer,cg_class);
extern  void            EmitOffset(offset);
extern  void            EmitPtr(pointer);
extern  abspatch        *NewAbsPatch();
extern  void            EmitByte(byte);
extern  void            EmitSegId(seg_id);
extern  void            InsertByte(byte);
extern  int             OptInsSize(oc_class,oc_dest_attr);
extern bool             IsFarFunc(sym_handle);
extern  unsigned        SavePendingLine(unsigned);
extern bool             AskIfUniqueLabel(label_handle);
extern bool             UseImportForm(fe_attr);
extern bool             AskIfCommonLabel(label_handle);
extern void             OutSpecialCommon(int,fix_class,bool);

extern byte             *NopLists[];

/* Grammar of Escapes :*/
/**/
/* Sequence                                             Meaning*/
/* ========                                             ========*/
/**/
/* ESC, ESC                                             actual ESC byte */
/* ESC, IMP <LDOF|OFST|BASE|SELF>, sym_handle <,offset> import reference */
/* ESC, REL <LDOF|BASE|OFST>, segid                     relocate, with seg-id*/
/* ESC, SYM <LDOF|OFST|SELF>, sym_handle <,offset>      unknown sym ref*/
/* ESC, LBL <LDOF|OFST|SELF>, segid, lbl_handle, <,offset> ptr reference*/
/* ESC, ABS objhandle, len, offset                      object patch*/
/* ESC, FUN byte                                        for 87 instructions*/
/**/
/*       OFST bit on means offset follows*/
/*       LDOF means loader resolved offset*/
/*       BASE means use F_BASE relocation*/

extern  bool    CodeHasAbsPatch( oc_entry *code ) {
/*************************************************/
    byte        *curr;
    byte        *final;

    curr = &code->data[ 0 ];
    final = curr + code->reclen - sizeof( oc_header );
    while( curr < final ) {
        if( *curr++ == ESC ) {
            if( *curr++ == ABS ) return( TRUE );
        }
    }
    return( FALSE );
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


extern  void  DoFESymRef( sym_handle sym, cg_class class, offset val,
                                int fixup ) {
/*******************************************************************/

    fe_attr             attr;
    byte                kind;

    kind = 0;
    switch( fixup ) {
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
         && ( attr & FE_PROC ) && IsFarFunc( sym ) ) {
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
            DoRelocRef( AskForSymLabel( sym, CG_FE ), CG_LBL,
                        AskSegID( sym, CG_FE ), val, kind );
        }
    } else {                                /* CG_TBL, CG_LBL or CG_BCK*/
        DoRelocRef( sym, class, AskSegID( sym, class ), val, kind );
    }
}


extern  void    DoSymRef( name *opnd, offset val, bool base ) {
/*************************************************************/

    DoFESymRef( opnd->v.symbol, opnd->m.memory_type, val,
                base ? FE_FIX_BASE : FE_FIX_OFF );
}


extern  void    DoSegRef( seg_id seg ) {
/**************************************/

    EmitByte( ESC );
    EmitByte( REL | BASE | OFST );
    EmitSegId( seg );
    EmitOffset( 0 );
}

static  void    DoRelocRef( sym_handle sym, cg_class class,
                            seg_id seg, offset val, byte kind ) {
/***************************************************************/

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

extern  void    DoLblRef( label_handle lbl, seg_id seg,
                          offset val, byte kind ) {
/*************************************************/

    EmitByte( ESC );
    EmitByte( LBL | kind );
    EmitSegId( seg );
    EmitPtr( lbl );
    if( kind & OFST ) {
        EmitOffset( val );
    }
}

static void SendBytes( byte *ptr, unsigned len ) {
/************************************************/

    if( len != 0 ) OutDBytes( len, ptr );
}

#define INFO_NOT_DEBUG      INFO_SELECT
static oc_class SaveDbgOc = INFO_NOT_DEBUG;
static pointer  SaveDbgPtr;
static offset   LastUnique = ADDR_UNKNOWN;

static  void    DumpSavedDebug()
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


static  void    ExpandCJ( any_oc *oc ) {
/**************************************/

    label_handle        lbl;
    oc_class            class;
    fix_class           f;
    bool                rel;

    lbl = oc->oc_handle.handle;
    class = oc->oc_entry.class;
    if( (class & GET_BASE) == OC_JCOND ) {
        if( oc->oc_entry.objlen == OptInsSize( OC_JCOND, OC_DEST_NEAR ) ) {
            if( _CPULevel( CPU_386 ) ) {
                _OutJCondNear( oc->oc_jcond.cond );
            } else {
                _OutJCond( ReverseCondition( oc->oc_jcond.cond ) );
                OutDataByte( OptInsSize( OC_JMP, OC_DEST_NEAR ) );
                _OutJNear;
            }
            OutCodeDisp( lbl, F_OFFSET, TRUE, class );
        } else {
            _OutJCond( oc->oc_jcond.cond );
            OutShortDisp( lbl );
        }
    } else if( (class & GET_BASE) == OC_JMP
         && oc->oc_entry.objlen == OptInsSize( OC_JMP, OC_DEST_SHORT ) ) {
        _OutJShort;
        OutShortDisp( lbl );
    } else {
        if( class & ATTR_FAR ) {
            f = F_PTR;
            rel = FALSE;
            if( ( class & GET_BASE ) == OC_CALL ) {
                if( oc->oc_entry.objlen == OptInsSize(OC_CALL, OC_DEST_CHEAP) ){
                    f = F_OFFSET;
                    rel = TRUE;
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
            rel = TRUE;
            if( ( class & GET_BASE ) == OC_CALL ) {
                _OutCNear;
            } else {
                _OutJNear;
            }
        }
        OutCodeDisp( lbl, f, rel, class );
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
    sym_handle          sym;

    sym = AskForLblSym( lbl );
    if( AskIfRTLabel( lbl ) ) {
        OutRTImport( (int)sym, f );
        if( class & ATTR_FAR ) {
            _OutFarD( 0, 0 );
        } else {
            _OutFarOff( 0 );
        }
    } else if( AskIfCommonLabel( lbl ) ) {
        OutSpecialCommon( (int)sym, f, rel );
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
            OutReloc( AskSegID( sym, CG_FE ), F_BASE, FALSE );
            _OutFarSeg( 0 );
        }
    }
}


static  label_handle    ExpandObj( byte *cur, int explen ) {
/**********************************************************/

    pointer             fini;
    byte                key;
    label_handle        lbl;
    sym_handle          sym;
    offset              val;
    seg_id              seg;
    fix_class           class;
    bool                rel;
    int                 len;

    lbl = NULL;
    fini = cur + explen;
    while( cur < fini ) {
        len = 0;
        while( cur[len] != ESC ) {
            ++len;
            if( cur + len >= fini ) break;
        }
        if( len != 0 ) {
            OutDBytes( len, cur );
            cur += len;
            if( cur >= fini ) break;
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
            rel = TRUE;
        } else {
            rel = FALSE;
        }
        switch( key & ~MASK ) {
        case REL:
            seg = *(seg_id *)cur;
            cur += sizeof( seg_id );
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
            seg = *(seg_id *)cur;
            cur += sizeof( seg_id );
            lbl = *(pointer *)cur;
            cur += sizeof( pointer );
            if( AskIfRTLabel( lbl ) ) {
                OutRTImportRel( (int)AskForLblSym( lbl ), F_OFFSET, FALSE );
                val = 0;
            } else {
                if( AskIfCommonLabel( lbl ) ) {
                    OutSpecialCommon( (int)AskForLblSym( lbl ), class, rel );
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
            OutImport( *(sym_handle *)cur, class, rel );
            cur += sizeof( sym_handle * );
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
            _OutFarOff( val );          /* offset*/
        } else if( class == F_PTR ) {
            _OutFarOff( val );
            _OutFarSeg( 0 );        /* segment*/
        }
    }
    return( lbl );
}

extern  void    OutputOC( any_oc *oc, any_oc *next_lbl ) {
/********************************************************/

    label_handle        lbl;
    sym_handle          sym;
    oc_class            base;
    int                 len;
    offset              lc;
    byte                *ptr;

    base = oc->oc_entry.class & GET_BASE;
    if( base != OC_LABEL ) {
        DumpSavedDebug();
    }
    SetUpObj( FALSE );
    switch( base ) {
    case OC_CODE:
    case OC_DATA:
        ExpandObj( &oc->oc_entry.data, oc->oc_entry.reclen-sizeof(oc_header) );
        break;
    case OC_IDATA:
        if( next_lbl != NULL ) { /* cause next_lbl to need no alignment */
            len = -( AskLocation() + oc->oc_entry.objlen );
            len &= next_lbl->oc_entry.objlen;
            DoAlignment( len );
        }
        OutSelect( TRUE );
        SendBytes( &oc->oc_entry.data[ 0 ], oc->oc_entry.objlen );
        OutSelect( FALSE );
        break;
    case OC_BDATA:
        SendBytes( &oc->oc_entry.data[ 0 ], oc->oc_entry.objlen );
        break;
    case OC_LABEL:
        /* figure out number of bytes to pad */
        lc = AskLocation();
        len = -lc & oc->oc_entry.objlen;
        if( AskIfUniqueLabel( oc->oc_handle.handle ) ) {
            if( (lc == LastUnique) && (len == 0) ) {
                /* Two unique labels have ended up next to each other.
                   Pad out to next label alignment boundry. */
                len = oc->oc_entry.objlen + 1;
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
            OutRTImport( (int)sym, F_OFFSET );
            lc = 0;
        } else if( AskIfCommonLabel( lbl ) ) {
            OutSpecialCommon( (int)sym, F_OFFSET, FALSE );
            lc = 0;
        } else if( sym != NULL && UseImportForm( FEAttr( sym ) ) ) {
            OutImport( sym, F_OFFSET, FALSE );
            lc = 0;
        } else {
            OutReloc( AskOP(), F_OFFSET, FALSE );
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
        ptr = &oc->oc_entry.data;
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
        lbl = ExpandObj( ptr, oc->oc_entry.reclen - sizeof(oc_header) - 1 - len );
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
        if( oc->oc_entry.class & ATTR_FAR ) {
            len |= B_RET_LONG;
        }
        if( oc->oc_entry.class & ATTR_IRET ) {
            len |= B_RET_IRET;
        }
        if( oc->oc_entry.class & ATTR_POP ) {
            OutDataByte( len );
            OutDataInt( oc->oc_ret.pops );
        } else {
            OutDataByte( len | B_RET_NOPOP );
        }
        break;
    case OC_INFO:
        base = oc->oc_entry.class & INFO_MASK;
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
            OutLineNum( 0, FALSE ); /* Kill pending line number */
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

