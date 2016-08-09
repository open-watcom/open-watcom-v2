/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
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


#include "cgstd.h"
#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdlib.h>
#include "coderep.h"
#include "cgdefs.h"
#include "cgmem.h"
#include "zoiks.h"
#include "cgauxinf.h"
#include "dw.h"
#include "dwarf.h"
#include "dfdbg.h"
#include "data.h"
#include "types.h"
#include "objout.h"
#include "dbsyms.h"
#include "dfsyms.h"
#include "dfsupp.h"
#include "targetdb.h"
#include "namelist.h"
#include "cgprotos.h"
#include "feprotos.h"


extern  void            DoBigBckPtr(back_handle,offset);
extern  void            DataLong( unsigned_32 );
extern  void            DataBytes(unsigned,const void *);
extern  void            IterBytes( offset len, byte pat );
extern  void            DataLabel( label_handle );
extern  void            DoBigLblPtr(cg_sym_handle);
extern dw_loc_handle    DBGLoc2DF( dbg_loc loc );
extern dw_loc_id        DBGLoc2DFCont( dbg_loc loc, dw_loc_id df_locid );
extern void             DFFEPtrRef( cg_sym_handle sym );

extern  void            DFBlkBeg( dbg_block *blk, offset lc );
static  void            DumpLocals( dbg_local *local );

dw_client               Client;
static short            CurrFNo;
static bool             CcuDef;


sect_info DwarfSegs[DW_DEBUG_MAX];

static back_handle Pc_High;
static back_handle Pc_Low;
static back_handle Comp_High;
static back_handle ARange;

static void CLIWrite( dw_sectnum sect, const void *block, size_t size )
/*********************************************************************/
{
    sect_info           *curr;
    segment_id          old;
//    long_offset         off;

    curr = &DwarfSegs[sect];
    old = SetOP( curr->seg );
//    off = AskBigLocation();
    DataBytes( size, block );
    SetOP( old );
}

static long CLITell( dw_sectnum sect )
/************************************/
{
    sect_info           *curr;
    long_offset         off;
    segment_id          old;

   curr = &DwarfSegs[sect];
   old = SetOP( curr->seg );
   off = AskBigLocation();
   SetOP( old );
   return( off );
}

static void CLISeek( dw_sectnum sect, long offs, uint type )
/**********************************************************/
{
    sect_info           *curr;
    long_offset         from;
    segment_id          old;

    from = 0;
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
    FEPtr( (cg_sym_handle)sym, ptr_type, disp );
}


static void DoSegReloc( dw_sym_handle sym ){
/*************************************/

    FEPtrBase( (cg_sym_handle)sym );
}

static void DoLblReloc( back_handle bck, int disp ){
/**********************************/
    type_def        *ptr_type;
    segment_id      id;

    id = AskSegID( bck, CG_BACK );
    ptr_type = TypeAddress( TY_NEAR_POINTER );
    BackPtr( bck, id, disp, ptr_type );
}

static void DoSegLblReloc( back_handle bck ){
/**********************************/
    segment_id      id;

    id = AskSegID( bck, CG_BACK );
    BackPtrBase( bck, id );
}

static void DoSectOffset( dw_sectnum section  ){
/**********************************/
    back_handle bck;
    long        pos;
    segment_id  id;

    pos = CLITell( section );
    bck = DwarfSegs[section].bck;
    id = DwarfSegs[section].seg;
    BackBigOffset( bck, id, pos );

}

typedef struct {
    segment_id  segment;
    long_offset offset;
} big_patch_handle;

static big_patch_handle UnitSize[1];

typedef struct {
    back_handle  bck;
    int_32       disp;
}loc_range;

static void CLIReloc( dw_sectnum sect, dw_relocs reloc_type, ... ){
/******************************************************/
    static uint_32 const    zero  = 0;
    sect_info               *curr;
    dw_sym_handle           sym;
    dw_sym_handle           bck;
    loc_range               *low;
    loc_range               *high;
    dw_sectnum              section;
    va_list                 args;
    segment_id              old;
//    long_offset             off;

    va_start( args, reloc_type );
    curr = &DwarfSegs[sect];
    old = SetOP( curr->seg );
//    off = AskBigLocation();
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
        DoLblReloc( (back_handle)bck, 0 );
        break;
    case DW_W_SEGMENT:
        sym = va_arg( args, dw_sym_handle );
        DoSegReloc( sym );
        break;
    case DW_W_LABEL_SEG:
        bck = va_arg( args, dw_sym_handle );
        DoSegLblReloc( (back_handle)bck );
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
#if _TARGET & ( _TARG_IAPX86 | _TARG_80386 )
        if( _IsntTargetModel( FLAT_MODEL ) ) {
            DoSegLblReloc( ARange );
        }
#endif
        break;
    case DW_W_UNIT_SIZE:
        UnitSize->segment = curr->seg;
        UnitSize->offset =  AskBigLocation();
        DataBytes( sizeof( zero ), &zero );
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
//      DFFEPtrRef( (cg_sym_handle)sym, disp );
        break;
      }
    default:
        va_end( args );
        abort();
        break;
    }
    SetOP( old );
    va_end( args );
}


static void *CLIAlloc( size_t size ) {

    void        *p;

    p = CGAlloc( size );
    return( p );
}


static void CLIFree( void *p ) {

    CGFree( p );
}

static back_handle  MakeLabel( void ) {
    back_handle bck;

    bck = BENewBack( NULL );
    bck->seg = AskOP();
    return( bck );
}

extern  void    DFInitDbgInfo( void )
/***********************************/
/* called after ObjInit */
{
    CurrFNo = 0;
    CcuDef = false;
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
    dw_sectnum  i;
    segment_id  old;
    back_handle bck;

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
    segment_id  old;
    back_handle bck;

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
    back_handle bck;

    for( i = DW_DEBUG_INFO; i < DW_DEBUG_MAX; ++i ){
        bck = DwarfSegs[i].bck;
        BEFreeBack( bck );
    }
}

static  void    FiniLineSegBck( void ){
/**************************************/
    back_handle bck;

    bck = DwarfSegs[DW_DEBUG_LINE].bck;
    BEFreeBack( bck );
}

extern  void    DFSymRange( cg_sym_handle sym, offset size ){
/*********************************************************/
    // I don't see what this is good for. The aranges for any
    // comdat symbols will be taken care of by DFSegRange().
    // Running this code may produce overlapping aranges that
    // confuse the hell out of the debugger. However, not running
    // this may cause debug information to be missing... call it
    // a FIXME

    if( _IsModel( DBG_LOCALS | DBG_TYPES ) ) {
        ARange = FEBack( sym );
        DWAddress( Client, size );
    }
}

extern  void    DFSegRange( void ){
/****************************/
/* do arange for the current segment */
    back_handle bck;
    offset      off;
    offset      size;

    if( _IsModel( DBG_LOCALS | DBG_TYPES ) ) {
        size = AskMaxSize();
        if( size > 0 ) {
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
}

extern  void    DFBegCCU( segment_id code, dw_sym_handle dbg_pch )
/****************************************************************/
// Call when codeseg hase been defined
{
    dw_cu_info      cu;
    back_handle     bck;
    segment_id      old;
    type_def        *tipe_addr;

    if( _IsntModel( DBG_LOCALS | DBG_TYPES ) ) {
        return;
    }
    if( CcuDef ) {
        cu.source_filename = FEAuxInfo( NULL, SOURCE_NAME );
        cu.directory = "";
        cu.dbg_pch = dbg_pch;
        cu.inc_list = NULL;
        cu.inc_list_len = 0;
        old = SetOP( code );
#if _TARGET & ( _TARG_IAPX86 | _TARG_80386 )
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
            cu.flags = false;
            cu.segment_size = 0;
        } else {
            bck = NULL;
            cu.flags = false;
            Pc_Low = NULL;
            Pc_High = NULL;
            cu.segment_size = 2;
        }
#else
        bck = MakeLabel();
        OutLabel( bck->lbl );
        Pc_Low = bck;
        Pc_High = MakeLabel();
        cu.flags = true;
        cu.segment_size = 0;
#endif
        SetOP( old );
        Comp_High = Pc_High;
        tipe_addr = TypeAddress( TY_NEAR_POINTER );
        cu.offset_size = tipe_addr->length;
        switch( GetMemModel() ) {
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
        if( cu.flags ) {
            BEFreeBack( bck );
        }
    } else {
        CcuDef = true;
    }
}

static const char *SetDwarfProducer( void )
{
    const char  *name;

#if 0      // disable this feature for now, to have compatibility with OW 1.9
    name = (const char *)FEAuxInfo( NULL, DBG_DWARF_PRODUCER );
    if( name == NULL )
        name = "";
#else
    name = "";
#endif
    return( name );
}

extern  void    DFObjInitDbgInfo( void ) {
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
    cg_sym_handle   abbrev_sym;
    cg_sym_handle   debug_pch;
    fe_attr         attr;

    if( _IsntModel( DBG_LOCALS | DBG_TYPES ) ){
        return;
    }
    info.compiler_options = DW_CM_DEBUGGER;
    info.abbrev_sym = 0;
    info.producer_name = SetDwarfProducer();
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
                back_handle bck;
                segment_id  old;

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
            if( (attr & FE_IMPORT) == 0 ) {
                back_handle bck;
                segment_id  old;

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

extern  void    DFObjLineInitDbgInfo( void ) {
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
    info.producer_name = SetDwarfProducer();
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
        cu.dbg_pch = NULL;
        cu.inc_list = NULL;
        cu.inc_list_len = 0;
#if _TARGET & ( _TARG_IAPX86 | _TARG_80386 )
        if( _IsTargetModel( FLAT_MODEL ) ) {
            cu.flags = true;
            cu.segment_size = 0;
        }else{
            cu.flags = false;
            cu.segment_size = 2;
        }
#else
        cu.flags = true;
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
extern void _CGAPI DFDwarfLocal( pointer client, pointer locid, pointer sym ) {
/*** add to location expr where local sym is *********************************/
    name        *tmp;
    type_length offset;

    tmp = DeAlias( AllocUserTemp( sym, XX ) );
    offset = NewBase( tmp );
    DWLocOp( client, locid, DW_LOC_fbreg, offset );

}

extern  void    DFFiniDbgInfo( void )
{
}


extern  void    DFObjFiniDbgInfo( offset codesize ) {
/******************************/
    segment_id      old;
    offset          here;
    back_handle     bck;

    if( _IsModel( DBG_LOCALS | DBG_TYPES ) ) {
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
}

extern  void    DFObjLineFiniDbgInfo( void )
/******************************************/
{
    DWFiniDebugLine( Client );
    DWFini( Client );
    FiniLineSegBck();
}

extern void     DFLineNum( cue_state *state, offset lc ){
/*******************************************************/
    char *fname;

    if( NeedBaseSet() ){
        back_handle bck;

        bck = MakeLabel();
        OutLabel( bck->lbl );
        DWLineAddr( Client, (dw_sym_handle)bck, lc );
#if _TARGET & ( _TARG_IAPX86 | _TARG_80386 )
        if( _IsntTargetModel( FLAT_MODEL ) ) {
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


#if _TARGET & ( _TARG_IAPX86 | _TARG_80386 )
static  dw_loc_handle   SegLoc( cg_sym_handle sym ){
/************************************************/
    dw_loc_id       locid;
    dw_loc_handle   df_loc;

    locid = DWLocInit( Client );
    DWLocSegment( Client, locid, (dw_sym_handle)sym );
    df_loc = DWLocFini( Client, locid );
    return( df_loc );
}
#endif
extern  void    DFGenStatic( cg_sym_handle sym, dbg_loc loc ) {
/*******************************************************************/
    uint            flags;
    fe_attr         attr;
    const char      *name;
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
#if _TARGET & ( _TARG_IAPX86 | _TARG_80386 )
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

extern  void    DFTypedef( const char *nm, dbg_type tipe )
/*** emit a user typedef ********************************/
{
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
static void    SymParm( cg_sym_handle sym, dw_loc_handle loc,
                                        dw_loc_handle entry ) {
/*******************************************************************/
//    fe_attr         attr;
    const char      *name;
//    dw_handle       obj;
    dbg_type        dbtype;

//    attr = FEAttr( sym );
    dbtype = FEDbgType( sym ); /* causes FEName side effects */
    name = FEName( sym );
//    obj = DWFormalParameter( Client, dbtype, loc, entry,  name, DW_DEFAULT_NONE );
    DWFormalParameter( Client, dbtype, loc, entry,  name, DW_DEFAULT_NONE );
}

/**/
/* Coming out of optimizer queue*/
/**/

#if _TARGET & ( _TARG_IAPX86 | _TARG_80386 )
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

static  dbg_local *UnLinkLoc( dbg_local **owner, cg_sym_handle sym ) {
/********************************************/
// unlink dbg_local with sym from owner

    dbg_local           *curr;

    for( ; (curr = *owner) != NULL; owner = &(*owner)->link ) {
        if( curr->sym == sym ){
            *owner = curr->link;
            break;
        }
    }
    return( curr );
}


static  void GenParmLoc( dbg_local *parm, dbg_local **locals )
{
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
        dbg_loc     alt_loc;
        dbg_loc     *alt_lnk;

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
    cg_sym_handle       sym;
    dbg_type            tipe;
    fe_attr             attr;
    const char          *name;
    uint                flags;
    dw_loc_handle       dw_retloc;
    dw_loc_handle       dw_frameloc;
    dw_loc_handle       dw_segloc;
    dbg_local           *parm;
    dbg_local           *next;
    dw_handle           obj;
    sym_access          *access;

    lc = lc;
    sym = AskForLblSym( CurrProc->label );
    tipe = FEDbgRetType( sym );
    flags = 0;
#if _TARGET & ( _TARG_IAPX86 | _TARG_80386 )
    if( *(call_class *)FindAuxInfoSym( sym, CALL_CLASS ) & FAR_CALL ) {
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
#if _TARGET & ( _TARG_IAPX86 | _TARG_80386 )
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
    rtn->end_lbl = MakeLabel();
    Pc_Low  = FEBack( sym );
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
    for( parm = rtn->parms; parm != NULL; parm = next ) {
        next = parm->link;
        if( parm->sym != NULL ){
            GenParmLoc( parm, &rtn->rtn_blk->locals );
        }
        DBLocFini( parm->loc );
        CGFree( parm );
    }
    if( rtn->reeturn != NULL ){
        GenRetSym( rtn->reeturn, tipe );
        DBLocFini( rtn->reeturn );
    }
    DFBlkBeg( rtn->rtn_blk, lc );
//   DumpLocals( rtn->blk->locals );
}

extern  void    DFBlkBeg( dbg_block *blk, offset lc ) {
/****************************************************/
    back_handle bck;

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
    back_handle bck;

    lc = lc;
    bck = blk->end_lbl;
    OutLabel( bck->lbl );
    BEFreeBack( bck );
    DWEndLexicalBlock( Client );
}

extern  void    DFEpiBeg( dbg_rtn *rtn, offset lc )
/*************************************************/
{
    DFBlkEnd( rtn->rtn_blk, lc );
}

extern  void    DFRtnEnd( dbg_rtn *rtn, offset lc )
/*************************************************/
{
    back_handle bck;

    lc = 0;
    bck = rtn->end_lbl;
    OutLabel( bck->lbl );
    BEFreeBack( bck );
    DWEndSubroutine( Client );
}



static  void    DumpLocals( dbg_local *local )
/********************************************/
{
    dbg_local   *next;
    dbg_type    tipe;

    for( ; local != NULL; local = next ) {
        next = local->link;
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
        CGFree( local );
    }
}

