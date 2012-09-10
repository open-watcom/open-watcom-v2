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
* Description:  Emit DWARF symbol information.
*
****************************************************************************/


#include "standard.h"
#include "hostsys.h"
#include "coderep.h"
#include "pattern.h"
#include "procdef.h"
#include "cgdefs.h"
#include "cgmem.h"
#include "symdbg.h"
#include "model.h"
#include "ocentry.h"
#include "offset.h"
#include "zoiks.h"
#include "cgaux.h"
#include "typedef.h"
#include "types.h"
#include "dbgstrct.h"
#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include "dw.h"
#include "dwarf.h"
#include "dfdbg.h"
#include "dbcue.h"
#include "cgprotos.h"
#include "feprotos.h"
#include "optlbl.h"

extern  void            FEPtr(sym_handle,type_def *,offset);
extern  void            FEPtrBase(sym_handle);
extern  seg_id          AskOP(void);
extern  void            BackPtr( bck_info*, seg_id, offset, type_def* );
extern  void            BackBigOffset( bck_info *, seg_id, offset );
extern  void            BackPtrBase( bck_info *, seg_id );
extern  void            OutLabel(label_handle);
extern  void            DoBigBckPtr(back_handle,offset);
extern  name            *DeAlias(name*);
extern  seg_id          SetOP(seg_id);
extern  seg_id          AskCodeSeg(void);
extern  segment_id      AskSegID(pointer,cg_class);
extern  offset          AskLocation(void);
extern  void            SetLocation(offset);
extern  offset          AskMaxSize(void);
extern  void            SetBigLocation( long_offset loc );
extern  long_offset     AskBigLocation(void);
extern  long_offset     AskBigMaxSize(void);
extern  bool            NeedBaseSet(void);
extern  void            DataInt(short_offset);
extern  void            DataLong( long );
extern  void            DataBytes(unsigned_32,byte*);
extern  void            IterBytes( offset len, byte pat );
extern  void            DataLabel( label_handle );
extern  void            DoBigLblPtr(sym_handle);
extern  void            DoBigBckPtr(back_handle,offset);
extern dw_loc_handle    DBGLoc2DF( dbg_loc loc );
extern dw_loc_id        DBGLoc2DFCont( dbg_loc loc, dw_loc_id df_locid );
extern uint             DFStkReg( void );
extern uint             DFDisplayReg( void );
extern void             DFFEPtrRef( sym_handle sym );
extern char             GetMemModel( void );
extern  name            *DeAlias(name*);
extern  name            *AllocUserTemp(pointer,type_class_def);
extern  type_length     NewBase(name*);

extern    source_line_number    SrcLine;
extern    proc_def              *CurrProc;
extern    struct opcode_entry   DbgInfo[];

extern  void            DFBlkBeg( dbg_block *blk, offset lc );
static  void            DumpLocals( dbg_local *local );

#define CurrProc_debug ((dbg_rtn *)CurrProc->targ.debug)

dw_client                  Client;
static short               CurrFNo;
static bool                CcuDef;


struct sect_info DwarfSegs[ DW_DEBUG_MAX ];

static bck_info  *Pc_High;
static bck_info  *Pc_Low;
static bck_info  *Comp_High;
static bck_info  *ARange;

static void CLIWrite( dw_sectnum sect, const void *block, dw_size_t size ) {
/******************************************************************/
    struct sect_info   *curr;
    seg_id              old;
    long_offset         off;

    curr = &DwarfSegs[sect];
    old = SetOP( curr->seg );
    off = AskBigLocation();
    DataBytes( size, (byte*)block );
    SetOP( old );
}

static long CLITell( dw_sectnum sect ) {
/*********************************/
    struct sect_info   *curr;
    long_offset         off;
    seg_id              old;

   curr = &DwarfSegs[sect];
   old = SetOP( curr->seg );
   off = AskBigLocation();
   SetOP( old );
   return( off );
}

static void CLISeek( dw_sectnum sect, long offs, uint type ) {
/******************************************************/
    struct sect_info   *curr;
    long_offset         from;
    seg_id              old;

    curr = &DwarfSegs[sect];
    old = SetOP( curr->seg );
    switch( type ) {
    case DW_SEEK_CUR:
        from = AskBigLocation();
        break;
    case DW_SEEK_SET:
        from = 0;
        break;
    case DW_SEEK_END:
        from = AskBigMaxSize();
        break;
    }
    offs += from;
    SetBigLocation( offs );
    SetOP( old );
}

static void DoReloc( dw_sym_handle sym, dw_addr_offset disp ){
/**********************************/
    type_def            *ptr_type;

    ptr_type = TypeAddress( TY_NEAR_POINTER );
    FEPtr( (sym_handle) sym, ptr_type, disp );
}


static void DoSegReloc( dw_sym_handle sym ){
/*************************************/

    FEPtrBase( (sym_handle) sym );
}

static void DoLblReloc( bck_info *bck, long disp ){
/**********************************/
    type_def            *ptr_type;
    seg_id              id;

    id = AskSegID( bck, CG_BACK );
    ptr_type = TypeAddress( TY_NEAR_POINTER );
    BackPtr( bck, id, disp, ptr_type );
}

static void DoSegLblReloc( bck_info *bck ){
/**********************************/
    seg_id              id;

    id = AskSegID( bck, CG_BACK );
    BackPtrBase( bck, id );
}

static void DoSectOffset( dw_sectnum section  ){
/**********************************/
    bck_info    *bck;
    uint_32     pos;
    seg_id      id;

    pos = CLITell( section );
    bck = DwarfSegs[section].bck;
    id = DwarfSegs[section].seg;
    BackBigOffset( bck, id, pos );

}

typedef struct {
    unsigned    segment;
    long_offset offset;
} big_patch_handle;

static big_patch_handle UnitSize[1];

typedef struct {
    bck_info    *bck;
    int_32       disp;
}loc_range;

static void CLIReloc( dw_sectnum sect, dw_relocs reloc_type, ... ){
/******************************************************/
    static uint_32              const zero  = 0;
    struct sect_info           *curr;
    dw_sym_handle               sym;
    dw_sym_handle               bck;
    loc_range                   *low;
    loc_range                   *high;
    dw_sectnum                  section;
    va_list                     args;
    seg_id                      old;
    long_offset                 off;

    va_start( args, reloc_type );
    curr = &DwarfSegs[sect];
    old = SetOP( curr->seg );
    off = AskBigLocation();
    switch( reloc_type ) {
    case DW_W_LOW_PC:
        if( Pc_Low != NULL ){
            DoLblReloc( Pc_Low, 0 );
        }else{
            Zoiks( ZOIKS_107 ); /* No Low PC */
        }
        break;
    case DW_W_HIGH_PC:
        if( Pc_High != NULL ){
            DoLblReloc( Pc_High, 0 );
        }else{
            Zoiks( ZOIKS_107 ); /* No High PC */
        }
        break;
    case DW_W_STATIC:
        sym = va_arg( args, dw_sym_handle );
        DoReloc( sym, 0  );
        break;
    case DW_W_LABEL:
        bck = va_arg( args, dw_sym_handle );
        DoLblReloc( (bck_info *)bck, 0 );
        break;
    case DW_W_SEGMENT:
        sym = va_arg( args, dw_sym_handle );
        DoSegReloc( sym );
        break;
    case DW_W_LABEL_SEG:
        bck = va_arg( args, dw_sym_handle );
        DoSegLblReloc( (bck_info *)bck );
        break;
    case DW_W_LOC_RANGE:
        low =  va_arg( args, loc_range* );
        high = va_arg( args, loc_range* );
        DoLblReloc( low->bck, low->disp );
        DoLblReloc( high->bck, high->disp );
        break;
    case DW_W_DEFAULT_FUNCTION:
        break;
    case DW_W_ARANGE_ADDR:
        DoLblReloc( ARange, 0 );
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        if( _IsntTargetModel( FLAT_MODEL ) ) {
            DoSegLblReloc( ARange );
        }
#endif
        break;
    case DW_W_UNIT_SIZE:
        UnitSize->segment = curr->seg;
        UnitSize->offset =  AskBigLocation();
        DataBytes( sizeof( uint_32 ), (byte *)&zero );
        break;
    case DW_W_SECTION_POS:
        section = va_arg( args, uint );
        DoSectOffset( section );
        break;
    case DW_W_EXT_REF:
      {
        long_offset disp;

        sym = va_arg( args, dw_sym_handle );
        disp = va_arg( args, dw_addr_offset );
        DoReloc( sym, disp );
//      DFFEPtrRef( (sym_handle )sym, disp );
        break;
      }
    default:
        abort();
        break;
    }
    SetOP( old );
}


static void *CLIAlloc( size_t size ) {

    void        *p;

    p = CGAlloc( size );
    return( p );
}


static void CLIFree( void *p ) {

    CGFree( p );
}

static bck_info  *MakeLabel( void ){
    bck_info            *bck;

    bck = BENewBack( NULL );
    bck->seg = AskOP();
    return( bck );
}

extern  void    DFInitDbgInfo(){
/******************************/
/* called after ObjInit */
    CurrFNo = 0;
    CcuDef = FALSE;
    Client = NULL;
}
#define MAX_LANG 4
struct lang_map{
     uint       lang;
     char       name[10];
};

static struct lang_map LangNames[MAX_LANG] = {
    {DWLANG_C,       "C"},
    {DWLANG_CPP,     "CPP"},
    {DWLANG_FORTRAN, "FORTRAN"},
    {DWLANG_FORTRAN, "FORTRAN77"},
};

static int SetLang( void ){
    int     ret;
    char    *name;
    int     index;

    ret = DWLANG_C;
    name =  FEAuxInfo( NULL, SOURCE_LANGUAGE );
    for( index = 0; index < MAX_LANG; ++index ){
        if( strcmp( name, LangNames[index].name ) == 0 ){
            ret = LangNames[index].lang;
            break;
        }
    }
    return( ret );
}

static  void    InitSegBck( void ){
/*********************************/
    dw_sectnum   i;
    seg_id       old;
    bck_info    *bck;

    for( i = DW_DEBUG_INFO; i < DW_DEBUG_MAX; ++i ){
        old = SetOP( DwarfSegs[i].seg );
        bck = MakeLabel();
        bck->seg = DwarfSegs[i].seg;
        DwarfSegs[i].bck = bck;
        DataLabel( bck->lbl );
        SetOP( old );
    }
}

static  void    InitLineSegBck( void ){
/*************************************/
    seg_id       old;
    bck_info    *bck;

    old = SetOP( DwarfSegs[DW_DEBUG_LINE].seg );
    bck = MakeLabel();
    bck->seg = DwarfSegs[DW_DEBUG_LINE].seg;
    DwarfSegs[DW_DEBUG_LINE].bck = bck;
    DataLabel( bck->lbl );
    SetOP( old );
}

static  void    FiniSegBck( void ){
/**********************************/
    int         i;
    bck_info    *bck;

    for( i = DW_DEBUG_INFO; i < DW_DEBUG_MAX; ++i ){
        bck = DwarfSegs[i].bck;
        BEFreeBack( bck );
    }
}

static  void    FiniLineSegBck( void ){
/**************************************/
    bck_info    *bck;

    bck = DwarfSegs[DW_DEBUG_LINE].bck;
    BEFreeBack( bck );
}

extern  void    DFSymRange( sym_handle sym, offset size ){
/*********************************************************/
    // I don't see what this is good for. The aranges for any
    // comdat symbols will be taken care of by DFSegRange().
    // Running this code may produce overlapping aranges that
    // confuse the hell out of the debugger. However, not running
    // this may cause debug information to be missing... call it
    // a FIXME

    bck_info    *bck;

    if( !_IsModel( DBG_LOCALS | DBG_TYPES ) )return;
    bck = FEBack( sym );
    ARange = bck;
    DWAddress( Client, size );
}

extern  void    DFSegRange( void ){
/****************************/
/* do arange for the current segment */
    bck_info    *bck;
    offset      off;
    offset      size;

    if( !_IsModel( DBG_LOCALS | DBG_TYPES ) )return;
    size = AskMaxSize();
    if( size > 0 ){
        bck = MakeLabel();
        off = AskLocation();
        SetLocation( 0 );
        DataLabel( bck->lbl );
        SetLocation( off );
        ARange = bck;
        DWAddress( Client, size );
        BEFreeBack( bck );
    }
}

extern  void    DFBegCCU( seg_id code, dw_sym_handle dbg_pch ){
/**************************************/
// Call when codeseg hase been defined
    dw_cu_info          cu;
    bck_info           *bck;
    seg_id              old;
    type_def           *tipe_addr;

    if( !_IsModel( DBG_LOCALS | DBG_TYPES ) ){
        return;
    }
    if( CcuDef ){
        cu.source_filename = FEAuxInfo( NULL, SOURCE_NAME );
        cu.directory = "";
        cu.dbg_pch   = dbg_pch;
        cu.inc_list = NULL;
        cu.inc_list_len = 0;
        old = SetOP( code );
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        if( _IsTargetModel( FLAT_MODEL ) ) {
            bck = MakeLabel();
            OutLabel( bck->lbl );
            Pc_Low = bck;
            Pc_High = MakeLabel();
            // Emitting DW_AT_low_pc and DW_AT_high_pc is valid *only* if the
            // compilation unit's code is in a single contiguous block (see
            // DWARF 2, section 3.1).
            // I don't know how to find out at the time of this call if there's
            // only one code segment or not, hence these attributes are always
            // disabled. The low/high pc attribs should probably be handled by
            // the linker.
            cu.flags = FALSE;
            cu.segment_size = 0;
        }else{
            cu.flags = FALSE;
            Pc_Low = NULL;
            Pc_High = NULL;
            cu.segment_size = 2;
        }
#else
        bck = MakeLabel();
        OutLabel( bck->lbl );
        Pc_Low = bck;
        Pc_High = MakeLabel();
        cu.flags = TRUE;
        cu.segment_size = 0;
#endif
        SetOP( old );
        Comp_High = Pc_High;
        tipe_addr = TypeAddress( TY_NEAR_POINTER );
        cu.offset_size = tipe_addr->length;
        switch( GetMemModel() ){
            case 'h':
                cu.model = DW_MODEL_HUGE;
                break;
            case 'l':
                cu.model = DW_MODEL_LARGE;
                break;
            case 'f':
                cu.model = DW_MODEL_FLAT;
                break;
            case 's':
                cu.model = DW_MODEL_SMALL;
                break;
            default:
                cu.model = DW_MODEL_NONE;
                break;
        }
        DWBeginCompileUnit( Client, &cu );
        if( cu.flags ){
            BEFreeBack( bck );
        }
    }else{
        CcuDef = TRUE;
    }
}

extern  void    DFObjInitInfo( void ) {
/*****************************************************/
/* called by objinit to init segments and dwarf writing library */
    static const dw_funcs cli_funcs = {
        CLIReloc,
        CLIWrite,
        CLISeek,
        CLITell,
        CLIAlloc,
        CLIFree
    };
    dw_init_info    info;
    sym_handle      abbrev_sym;
    sym_handle      debug_pch;
    fe_attr         attr;

    if( !_IsModel( DBG_LOCALS | DBG_TYPES ) ){
        return;
    }
    info.language = DWLANG_C;
    info.compiler_options = DW_CM_DEBUGGER;
    info.abbrev_sym = 0;
    info.producer_name = "WATCOM";
    info.language = SetLang();
    if( setjmp( info.exception_handler ) == 0 ) {
        info.funcs = cli_funcs;
        InitSegBck(); // start each seg with a ref label
        if( _IsModel( DBG_PREDEF ) ) {
            abbrev_sym = FEAuxInfo( NULL, DBG_PREDEF_SYM );
            info.abbrev_sym = (dw_sym_handle)abbrev_sym;
            attr = FEAttr( abbrev_sym );
            if( (attr & FE_IMPORT) ) {
                info.compiler_options |= DW_CM_ABBREV_PRE;
            }else{
                bck_info *bck;
                seg_id    old;

                info.compiler_options |= DW_CM_ABBREV_GEN;
                bck = FEBack( abbrev_sym ); // dump out export label
                bck->seg = DwarfSegs[DW_DEBUG_ABBREV].seg;
                old = SetOP( DwarfSegs[DW_DEBUG_ABBREV].seg );
                DataLabel( bck->lbl );
                SetOP( old );
            }
        }
        debug_pch = FEAuxInfo( NULL, DBG_PCH_SYM );
        if( debug_pch != NULL ){
            attr = FEAttr( debug_pch );
            if( !(attr & FE_IMPORT) ) {
                bck_info *bck;
                seg_id    old;

                bck = FEBack( debug_pch );
                bck->seg = DwarfSegs[DW_DEBUG_INFO].seg;
                old = SetOP( DwarfSegs[DW_DEBUG_INFO].seg );
                DataLabel( bck->lbl );
                SetOP( old );
                debug_pch = NULL;
            }
        }
        Client = DWInit( &info );
        if( Client == NULL ) {
            Zoiks( ZOIKS_107 ); /* Bad */
        }
        DFBegCCU( AskCodeSeg(), (dw_sym_handle)debug_pch );
    } else {
        Zoiks( ZOIKS_107 ); /* Big Error */
    }
}

extern  void    DFObjLineInitInfo( void ) {
/*****************************************************/
/* called by objinit to init segments and dwarf writing library */
    static const dw_funcs cli_funcs = {
        CLIReloc,
        CLIWrite,
        CLISeek,
        CLITell,
        CLIAlloc,
        CLIFree
    };
    dw_init_info    info;
    dw_cu_info      cu;
    type_def       *tipe_addr;

    info.language = DWLANG_C;
    info.compiler_options = DW_CM_DEBUGGER;
    info.abbrev_sym = 0;
    info.producer_name = "WATCOM";
    info.language = SetLang();
    if( setjmp( info.exception_handler ) == 0 ) {
        info.funcs = cli_funcs;
        InitLineSegBck(); // start each seg with a ref label
        Client = DWInit( &info );
        if( Client == NULL ) {
            Zoiks( ZOIKS_107 ); /* Bad */
        }
        cu.source_filename = FEAuxInfo( NULL, SOURCE_NAME );
        cu.directory = "";
        cu.dbg_pch   = 0;
        cu.inc_list = NULL;
        cu.inc_list_len = 0;
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        if( _IsTargetModel( FLAT_MODEL ) ) {
            cu.flags = TRUE;
            cu.segment_size = 0;
        }else{
            cu.flags = FALSE;
            cu.segment_size = 2;
        }
#else
        cu.flags = TRUE;
        cu.segment_size = 0;
#endif
        tipe_addr = TypeAddress( TY_NEAR_POINTER );
        cu.offset_size = tipe_addr->length;
        switch( GetMemModel() ){
            case 'h':
                cu.model = DW_MODEL_HUGE;
                break;
            case 'l':
                cu.model = DW_MODEL_LARGE;
                break;
            case 'f':
                cu.model = DW_MODEL_FLAT;
                break;
            case 's':
                cu.model = DW_MODEL_SMALL;
                break;
            default:
                cu.model = DW_MODEL_NONE;
                break;
        }
        DWInitDebugLine( Client, &cu );
    } else {
        Zoiks( ZOIKS_107 ); /* Big Error */
    }
}


extern pointer _CGAPI DFClient( void ) {
/*** return the client handle ***/
    return( Client );
}
//TODO: maybe this should be some sort of call back
extern void _CGAPI DFDwarfLocal( pointer client, pointer locid, sym_handle sym ){
/*** add to location expr where local sym is ***********************************/
    name        *tmp;
    type_length offset;

    tmp = DeAlias( AllocUserTemp( sym, XX ) );
    offset = NewBase( tmp );
    DWLocOp( client, locid, DW_LOC_fbreg, offset );

}

extern  void    DFFiniDbgInfo() {
}


extern  void    DFObjFiniDbgInfo( offset codesize ) {
/******************************/
    seg_id              old;
    offset              here;
    bck_info           *bck;

    if( !_IsModel( DBG_LOCALS | DBG_TYPES ) )return;
    bck = Comp_High;
    if( bck != NULL ){
        old = SetOP( AskCodeSeg() );
        OutLabel( bck->lbl );
        SetOP( old );
        BEFreeBack( bck );
        Comp_High = NULL;
    }
    DWEndCompileUnit( Client );
    DWFini( Client );
    old = SetOP( UnitSize->segment );
    here = AskLocation();
    SetLocation( UnitSize->offset );
    DataLong( codesize );
    SetLocation( here );
    SetOP( old );
    FiniSegBck();
}

extern  void    DFObjLineFiniDbgInfo( void ) {
/********************************************/

    DWFiniDebugLine( Client );
    DWFini( Client );
    FiniLineSegBck();
}

extern void     DFLineNum( cue_state *state, offset lc ){
/*******************************************************/
    char *fname;

    if( NeedBaseSet() ){
        bck_info    *bck;

        bck = MakeLabel();
        OutLabel( bck->lbl );
        DWLineAddr( Client, (dw_sym_handle)bck, lc );
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        if( !_IsTargetModel( FLAT_MODEL ) ) {
            DWLineSeg( Client, (dw_sym_handle)bck );
        }
#endif
        BEFreeBack( bck );
    }
    if( state->fno != CurrFNo ){
        fname = SrcFNoFind( state->fno );
        DWSetFile( Client, fname );
        CurrFNo = state->fno;
    }
    DWLineNum( Client, DW_LN_STMT, state->line, state->col, lc );
}


#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
static  dw_loc_handle   SegLoc( sym_handle sym ){
/************************************************/
    dw_loc_id       locid;
    dw_loc_handle   df_loc;

    locid = DWLocInit( Client );
    DWLocSegment( Client, locid, (dw_sym_handle)sym );
    df_loc = DWLocFini( Client, locid );
    return( df_loc );
}
#endif
extern  void    DFGenStatic( sym_handle sym, dbg_loc loc ) {
/*******************************************************************/
    uint            flags;
    fe_attr         attr;
    char           *name;
    dw_loc_handle   dw_loc;
    dw_loc_handle   dw_segloc;
    dw_handle       obj;
    dbg_type        dbtype;

    attr = FEAttr( sym );
    if( attr & FE_GLOBAL ){
        flags = DW_FLAG_GLOBAL;
    }else{
        flags = 0;
    }
    name = FEName( sym );
    if( attr & FE_STATIC ){
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        if( _IsTargetModel( FLAT_MODEL ) ) {
            dw_segloc = NULL;
        }else{
            dw_segloc = SegLoc( sym );
        }
#else
       dw_segloc = NULL;
#endif
    }else{
        dw_segloc = NULL;
    }
    dbtype = FEDbgType( sym ); /* causes name side effects */
    dw_loc = DBGLoc2DF( loc );
    obj = DWVariable( Client, dbtype, dw_loc,
                0, dw_segloc, name, 0, flags );
    if( attr &  FE_GLOBAL ){
        name = FEName( sym );
        DWPubname( Client, obj, name );
    }
    if( dw_loc != NULL ){
        DWLocTrash( Client, dw_loc );
    }
    if( dw_segloc != NULL ){
        DWLocTrash( Client, dw_segloc );
    }
}

extern  void    DFTypedef( char *nm, dbg_type tipe ){
/*** emit a user typedef ***************************/
     DWTypedef( Client, tipe, nm, 0, 0 );
}

static  void    GenRetSym( dbg_loc loc, dbg_type tipe ) {
/*******************************************************************/
    dw_loc_handle   dw_loc;

    dw_loc = DBGLoc2DF( loc );
    if( dw_loc != NULL ){
        DWVariable( Client, tipe, dw_loc,
                   0, NULL, ".return", 0, DW_FLAG_ARTIFICIAL );
        DWLocTrash( Client, dw_loc );
    }
}
static void    SymParm( sym_handle sym, dw_loc_handle loc,
                                        dw_loc_handle entry ) {
/*******************************************************************/
    fe_attr         attr;
    char           *name;
    dw_handle       obj;
    dbg_type        dbtype;

    attr = FEAttr( sym );
    dbtype = FEDbgType( sym ); /* causes FEName side effects */
    name = FEName( sym );
    obj = DWFormalParameter( Client, dbtype, loc,
                entry,  name, DW_DEFAULT_NONE );
}

/**/
/* Coming out of optimizer queue*/
/**/

#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
static dw_loc_handle  RetLoc( uint_32 ret_offset ){
/**** make a loc for return address *************/
    dw_loc_id       locid;
    dw_loc_handle   df_loc;

    locid = DWLocInit( Client );
    DWLocOp( Client, locid, DW_LOC_fbreg, ret_offset );
    df_loc = DWLocFini( Client, locid );
    return( df_loc );
}

static dw_loc_handle  FrameLoc( void ){
/**** make a loc for frame  address *************/
    uint            dsp;
    dw_loc_id       locid;
    dw_loc_handle   df_loc;

    locid = DWLocInit( Client );
    dsp = DFDisplayReg();
    DWLocReg( Client, locid, dsp );
    df_loc = DWLocFini( Client, locid );
    return( df_loc );
}
#endif
static dw_loc_id StkLoc( uint_32 stk_offset, dw_loc_id locid ){
/**** make a loc for stack  address *************/
    uint            stk;

    stk = DFStkReg();
    DWLocOp( Client, locid, DW_LOC_breg, stk, stk_offset );
    return( locid );
}

static  dbg_local *UnLinkLoc( dbg_local **owner, sym_handle sym ) {
/********************************************/
// unlink dbg_local with sym from owner

    dbg_local           *curr;

    while( (curr = *owner) != NULL ) {
        if( curr->sym == sym ){
            *owner = curr->link;
            break;
        }
        owner = &(*owner)->link;
    }
    return( curr );
}


static  void GenParmLoc( dbg_local   *parm,
                    dbg_local   **locals ){
    dbg_local      *alt;
    dw_loc_handle   dw_loc;
    dw_loc_handle   dw_entry;
    dw_loc_id       df_locid;

    df_locid = DWLocInit( Client );
    if( parm->loc->class == LOC_CONST_4 ){
        df_locid = StkLoc( parm->loc->u.val, df_locid );
    }else{
        df_locid = DBGLoc2DFCont( parm->loc, df_locid );
    }
    alt = UnLinkLoc( locals, parm->sym );
    if( alt != NULL ){
        dbg_loc       alt_loc;
        dbg_loc       *alt_lnk;

        alt_loc = alt->loc;      // skip down to loc base
        alt_lnk = &alt_loc->next;
        if( *alt_lnk != NULL ){
            for(;;){
                alt_loc = *alt_lnk;
                if( alt_loc->next == NULL )break;
                alt_lnk = &alt_loc->next;
            }
            *alt_lnk = NULL;   // don't use base
            df_locid = DBGLoc2DFCont( alt->loc, df_locid );
            *alt_lnk = alt_loc;
        }
        dw_loc = DBGLoc2DF( alt->loc );
        DBLocFini( alt->loc );
        CGFree( alt );
    }else{
        dw_loc = DBGLoc2DF( NULL );
    }
    dw_entry = DWLocFini( Client, df_locid );
    SymParm( parm->sym, dw_loc, dw_entry );
    DWLocTrash( Client, dw_loc );
    DWLocTrash( Client, dw_entry );
}

#if _TARGET & _TARG_IAPX86
static int  DW_PTR_TYPE_FAR  = DW_PTR_TYPE_FAR16;
#elif _TARGET & _TARG_80386
static int  DW_PTR_TYPE_FAR  = DW_PTR_TYPE_FAR32;
#endif

extern  void    DFProEnd( dbg_rtn *rtn, offset lc ) {
/****************************************************/
    sym_handle          sym;
    dbg_type            tipe;
    fe_attr             attr;
    char               *name;
    call_class          *class_ptr;
    uint                flags;
    dw_loc_handle       dw_retloc;
    dw_loc_handle       dw_frameloc;
    dw_loc_handle       dw_segloc;
    dbg_local           *parm;
    dbg_local           *junk;
    dw_handle           obj;
    bck_info            *bck;
    sym_access          *access;

    lc = lc;
    sym = AskForLblSym( CurrProc->label );
    tipe = FEDbgRetType( sym );
    class_ptr = FEAuxInfo( FEAuxInfo( sym, AUX_LOOKUP ), CALL_CLASS );
    flags = 0;
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
    if( *class_ptr & FAR_CALL ) {
        flags |= DW_PTR_TYPE_FAR;
    }
#endif
    attr = FEAttr( sym );
    name = FEName( sym );
    if( attr & FE_GLOBAL ){
        flags |= DW_FLAG_GLOBAL;
    }else{
        flags |= DW_SUB_STATIC;
    }
    if( attr & FE_COMPILER ){
        flags |= DW_FLAG_ARTIFICIAL;
    }
    flags |= DW_FLAG_PROTOTYPED;
    access = FEAuxInfo( sym,DBG_SYM_ACCESS );
    if( access != NULL ){
        if( *access == SYM_ACC_PUBLIC ){
            flags |= DW_FLAG_PUBLIC;
        }else if( *access == SYM_ACC_PROTECTED ){
            flags |= DW_FLAG_PROTECTED;
        }else if( *access == SYM_ACC_PRIVATE ){
            flags |= DW_FLAG_PRIVATE;
        }
    }
    DBLocFini( rtn->obj_loc );
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
    dw_retloc = RetLoc( rtn->ret_offset );
    dw_frameloc = FrameLoc();
    if( _IsTargetModel( FLAT_MODEL ) ) {
        dw_segloc = NULL;
    }else{
        dw_segloc = SegLoc( sym );
    }
#else
    dw_retloc = NULL;
    dw_frameloc = NULL;
    dw_segloc = NULL;
#endif
    bck = FEBack( sym );
    rtn->end_lbl = MakeLabel();
    Pc_Low  = bck;
    Pc_High = rtn->end_lbl;
    obj = DWBeginSubroutine( Client, 0, tipe, dw_retloc,
                     dw_frameloc, NULL, rtn->obj_type,
                     dw_segloc, name, rtn->pro_size, flags );
    if( attr &  FE_GLOBAL ){
        if( rtn->obj_type != DBG_NIL_TYPE ) {
            name = FEAuxInfo( sym, CLASS_APPENDED_NAME );
        }else{
            name = FEName( sym );
        }
        DWPubname( Client, obj, name );
    }
    if( dw_retloc != NULL ){
        DWLocTrash( Client, dw_retloc );
    }
    if( dw_frameloc != NULL ){
        DWLocTrash( Client, dw_frameloc );
    }
    if( dw_segloc != NULL ){
        DWLocTrash( Client, dw_segloc );
    }
    parm = rtn->parms;
    while( parm != NULL ) {  /* flush these suckers */
        if( parm->sym != NULL ){
            GenParmLoc( parm, &rtn->blk->locals );
        }
        DBLocFini( parm->loc );
        junk = parm;
        parm = parm->link;
        CGFree( junk );
    }
    if( rtn->reeturn != NULL ){
        GenRetSym( rtn->reeturn, tipe );
        DBLocFini( rtn->reeturn );
    }
    DFBlkBeg( rtn->blk, lc );
//   DumpLocals( rtn->blk->locals );
}

extern  void    DFBlkBeg( dbg_block *blk, offset lc ) {
/****************************************************/
    bck_info    *bck;

    lc = lc;
    bck = MakeLabel();
    blk->end_lbl = MakeLabel();
    OutLabel( bck->lbl );
    Pc_Low  = bck;
    Pc_High = blk->end_lbl;
    DWBeginLexicalBlock( Client, NULL, NULL );
    BEFreeBack( bck );
    DumpLocals( blk->locals );
}

extern  void    DFBlkEnd( dbg_block *blk, offset lc ) {
/****************************************************/
    bck_info    *bck;

    lc = lc;
    bck = blk->end_lbl;
    OutLabel( bck->lbl );
    BEFreeBack( bck );
    DWEndLexicalBlock( Client );
}

extern  void    DFEpiBeg( dbg_rtn *rtn, offset lc ) {
/****************************************************/
    DFBlkEnd( rtn->blk, lc );
}

extern  void    DFRtnEnd( dbg_rtn *rtn, offset lc ) {
/****************************************************/
    bck_info            *bck;

    lc = 0;
    bck = rtn->end_lbl;
    OutLabel( bck->lbl );
    BEFreeBack( bck );
    DWEndSubroutine( Client );
}



static  void    DumpLocals( dbg_local *local ) {
/***********************************************/
    dbg_local   *junk;
    dbg_type    tipe;

    while( local != NULL ) {
        switch( local->kind ){
        case DBG_SYM_VAR:
            DFGenStatic( local->sym, local->loc );
            DBLocFini( local->loc );
            break;
        case DBG_SYM_TYPE:
            tipe = FEDbgType( local->sym );
            break;
        case DBG_SYM_TYPEDEF:
            tipe = FEDbgType( local->sym );
            DFTypedef( FEName( local->sym ), tipe );
            break;
        }
        junk = local;
        local = local->link;
        CGFree( junk );
    }
}

