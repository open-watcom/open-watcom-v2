/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Code generator public interface.
*
****************************************************************************/


#include "_cgstd.h"
#include <ctype.h>
#include "coderep.h"
#include "seldef.h"
#include "memcheck.h"
#include "cgmem.h"
#include "tree.h"
#include "cfloat.h"
#include "zoiks.h"
#include "cgauxinf.h"
#include "data.h"
#include "types.h"
#include "bldins.h"
#include "utils.h"
#include "objout.h"
#include "treeprot.h"
#include "makeaddr.h"
#include "patch.h"
#include "dbsyms.h"
#include "objio.h"
#include "blips.h"
#include "bldsel.h"
#include "intrface.h"
#include "opttell.h"
#include "typemap.h"
#include "bldcall.h"
#include "generate.h"
#include "bckptr.h"
#include "inline.h"
#include "memout.h"
#include "memmgt.h"
#include "bgcall.h"
#include "savings.h"
#include "regsave.h"
#include "feprotos.h"
#include "cgprotos.h"

#ifndef NDEBUG
#include "echoapi.h"
#include <stdlib.h>
#endif


#ifdef QNX_FLAKEY
unsigned        OrigModel;
#endif

#define MAX_BCK_INFO    1000    // number of bck_info's per carve block

typedef union uback_info {
    bck_info    bck;
    union uback_info    *link;
} uback_info;

typedef struct bck_info_block {
    struct bck_info_block       *next;
    uback_info                  bck_infos[MAX_BCK_INFO];
} bck_info_block;

uback_info      *BckInfoHead;           // linked list of available bck_info's
bck_info_block  *BckInfoCarveHead;      // list of big blocks of bck_info's


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%                                              %%*/
/*%   Module interface                           %%*/
/*%                                              %%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

static  bool            memStarted = false;

void _CGAPI     BEMemInit( void )
/*******************************/
{
    cf_callbacks cf_rtns = { CGAlloc, CGFree };

    BckInfoHead = NULL;
    BckInfoCarveHead = NULL;
    InitBlip();
    CGMemInit();
    CFInit( &cf_rtns );
    memStarted = true;
}

cg_init_info _CGAPI     BEInitCg( cg_switches switches,
                                    cg_target_switches platform,
                                    uint optsize,
                                    proc_revision proc )
/**************************************************************/
{
    cg_init_info        info;

    OptForSize = (byte)optsize;
    CGProcessorVersion = proc; /* so _CPULevel works */
#if _TARGET & _TARG_8086
    /* if it ain't a 386 or better, FS and GS aren't there */
    if( !_CPULevel( CPU_386 ) ) {
        switches &= ~(FLOATING_FS | FLOATING_GS);
    }
#elif  _TARGET & _TARG_80386
    if( !_CPULevel( CPU_386 ) ) {
        SET_CPU( proc, CPU_386 );
    }
    proc &= ~FPU_EMU;   /* don't need funny fixups for 386 */
#endif
    Model = switches;
    TargetModel = platform;
#ifdef QNX_FLAKEY
    OrigModel = switches;
#endif
#ifndef NDEBUG
    EchoAPIInit();
    EchoAPI( "BEInit( %x, %x, %i, %x )\n", switches, platform, optsize, proc );
    // must be after Model is set and before InitDBGInfo call
#endif
    CGProcessorVersion = proc;
    InitWeights( optsize );
    InitSegDefs();
    InitDbgInfo();
    TypeInit();
    TInit();
    if( !CGOpenf() ) {
        info.success = 0;
    } else {
        info.success = 1;
        info.version.is_large = true;
#if _TARGET & _TARG_8086
        info.version.target = II_TARG_8086;
#elif _TARGET & _TARG_80386
        info.version.target = II_TARG_80386;
#elif _TARGET & _TARG_370
        info.version.target = II_TARG_370; /* NYI -- for now */
#elif _TARGET & _TARG_AXP
        info.version.target = II_TARG_AXP;
#endif
        info.version.revision = II_REVISION;
    }
    return( info );
}

cg_init_info _CGAPI     BEInit( cg_switches switches,
                                cg_target_switches platform,
                                uint optsize, proc_revision proc )
/****************************************************************/
{
    BEMemInit();
    return( BEInitCg( switches, platform, optsize, proc ) );
}

void _CGAPI     BEStart( void )
/*****************************/
{
#ifndef NDEBUG
    EchoAPI( "BEStart()\n" );
#endif
    InitCG();
}

void _CGAPI     BEStop( void )
/****************************/
{
#ifndef NDEBUG
    EchoAPI( "BEStop()\n" );
#endif
    FiniCG();
}

void _CGAPI     BEAbort( void )
/*****************************/
{
#ifndef NDEBUG
    EchoAPI( "BEAbort()\n" );
#endif
    AbortCG();
}

pointer _CGAPI  BEMemAlloc( unsigned size )
/*****************************************/
{
    return( CGAlloc( size ) );
}

void _CGAPI     BEMemFree( pointer ptr )
/**************************************/
{
    CGFree( ptr );
}

void _CGAPI     BEMemFini( void )
/*******************************/
{
    CFFini();
    CGMemFini();
}

static void FreeBckInfoCarveBlocks( void )
{
    bck_info_block      *carve_block;

    for( ; (carve_block = BckInfoCarveHead) != NULL; ) {
        BckInfoCarveHead = carve_block->next;
        CGFree( carve_block );
    }
    BckInfoHead = NULL;
}

void _CGAPI     BEFiniCg( void )
/******************************/
{
#ifndef NDEBUG
    EchoAPI( "BEFiniCg()\n" );
    EchoAPIFini();
#endif
    FreeBckInfoCarveBlocks();
    FiniDbgInfo();
    TypeFini();
    TFini();
    FiniBlip();
}

void _CGAPI     BEFini( void )
/****************************/
{
    BEFiniCg();
    BEMemFini();
}

bool _CGAPI     BEMoreMem( void )
/*******************************/
{
    return( _MemCheck( 1 ) );
}

segment_id _CGAPI   BEGetSeg( void )
/**********************************/
{
#ifndef NDEBUG
    segment_id  segid;

    EchoAPI( "BEGetSeg()" );
    segid = AskOP();
    return EchoAPIHexReturn( segid );
#else
    return( AskOP() );
#endif
}

segment_id _CGAPI   BESetSeg( segment_id segid )
/**********************************************/
{
#ifndef NDEBUG
    EchoAPI( "BESetSeg( %x )", segid );
    segid = SetOP( segid );
    return EchoAPIHexReturn( segid );
#else
    return( SetOP( segid ) );
#endif
}

void _CGAPI BEDefSeg( segment_id segid, seg_attr attr, cchar_ptr str, uint algn )
/*******************************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "BEDefSeg( %x, %x, %c, %i )\n", segid, attr, str, algn );
#endif
    DefSegment( segid, attr, str, algn, false );
}

void _CGAPI     BEFlushSeg( segment_id segid )
/********************************************/
{
#ifndef NDEBUG
    EchoAPI( "BEFlushSeg( %x )\n", segid );
#endif
    FlushOP( segid );
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%                                              %%*/
/*%   Handle creation/deletion                   %%*/
/*%                                              %%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

label_handle _CGAPI BENewLabel( void )
/************************************/
{
#ifndef NDEBUG
    label_handle    retn;

    EchoAPI( "BENewLabel()" );
    retn = AskForNewLabel();
    EchoAPI( " -> %L\n", retn );
    hdlAdd( LABEL_HANDLE, retn );
    return retn;
#else
    return( AskForNewLabel() );
#endif
}

void _CGAPI     BEFiniLabel( label_handle lbl )
/*********************************************/
{
#ifndef NDEBUG
    EchoAPI( "BEFiniLabel( %L )\n", lbl );
    hdlExists( LABEL_HANDLE, lbl );
    hdlUseOnce( LABEL_HANDLE, lbl );
#endif

    BGFiniLabel( lbl );
}


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%                                              %%*/
/*%   Miscellaneous stuff                        %%*/
/*%                                              %%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


unsigned_32 _CGAPI      BEUnrollCount( unsigned_32 unroll_count )
/***************************************************************/
{
#ifndef NDEBUG
    unsigned_32 retn;
    EchoAPI( "BEUnrollCount( %i )", unroll_count );
    retn = BGUnrollCount( unroll_count );
    return EchoAPIIntReturn( retn );
#else
    return( BGUnrollCount( unroll_count ) );
#endif
}


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%                                              %%*/
/*%   Tree patch creation/use/deletion           %%*/
/*%                                              %%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

patch_handle _CGAPI     BEPatch( void )
/*************************************/
{
#ifndef NDEBUG
    patch_handle patch;

    EchoAPI( "BEPatch()" );
    patch = BGNewPatch();
    EchoAPI( " -> %P\n", patch );
    hdlAdd( PATCH_HANDLE, patch );
    return( patch );
#else
    return( BGNewPatch() );
#endif
}

cg_name _CGAPI          CGPatchNode( patch_handle patch, cg_type tipe )
/*********************************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGPatchNode( %P, %t )", patch, tipe );
    hdlExists( PATCH_HANDLE, patch );
    retn = BGPatchNode( patch, TypeAddress( tipe ) );
    hdlAdd( CG_NAMES, retn );
    return EchoAPICgnameReturn( retn );
#else
    return( BGPatchNode( patch, TypeAddress( tipe ) ) );
#endif
}

void _CGAPI     BEPatchInteger( patch_handle patch, signed_32 value )
/*******************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "BEPatchInteger( %P, %x )\n", patch, value );
    hdlExists( PATCH_HANDLE, patch );
#endif
    BGPatchInteger( patch, value );
}

void _CGAPI     BEFiniPatch( patch_handle patch )
/***********************************************/
{
#ifndef NDEBUG
    EchoAPI( "BEFiniPatch( %P )\n", patch );
    hdlUseOnce( PATCH_HANDLE, patch );
#endif

    BGFiniPatch( patch );
}


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%                                              %%*/
/*%   Back Handles                               %%*/
/*%                                              %%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

static  pointer                 NewBackReturn = NULL;

pointer cg_internal LkAddBack( cg_sym_handle sym, pointer curr_back )
/*******************************************************************/
{
    back_handle bck;

    NewBackReturn = TO_FAKE_BACK( curr_back );
    bck = FEBack( sym );
    NewBackReturn = NULL;
    return( TO_REAL_BACK( bck ) );
}

back_handle cg_internal SymBack( cg_sym_handle sym )
/**************************************************/
{
    back_handle bck;

    bck = FEBack( sym );
    return( TO_REAL_BACK( bck ) );
}

static void AllocMoreBckInfo( void )
/**********************************/
{
    uback_info          *p;
    bck_info_block      *carve_block;
    int                 i;

    carve_block = CGAlloc( sizeof( bck_info_block ) );
    if( carve_block != NULL ) {
        carve_block->next = BckInfoCarveHead;   // link into big list
        BckInfoCarveHead = carve_block;
        p = &carve_block->bck_infos[0];
        BckInfoHead = p;
        for( i = 0; i < (MAX_BCK_INFO - 1); i++ ) {
            p->link = p + 1;
            ++p;
        }
        p->link = NULL;
    }
}

back_handle _CGAPI      BENewBack( cg_sym_handle sym )
/****************************************************/
{
    back_handle bck;

#ifndef NDEBUG
    EchoAPI( "BENewBack( %s )", sym );
#endif
    if( NewBackReturn == FAKE_NULL ) {
#ifndef NDEBUG
        EchoAPI( " -> %B\n", NULL );
#endif
        return( NULL );
    }
    if( NewBackReturn != NULL ) {
#ifndef NDEBUG
    EchoAPI( " -> %B\n", NewBackReturn );
#endif
        return( NewBackReturn );
    }
//    bck = CGAlloc( sizeof( bck_info ) );
    if( BckInfoHead == NULL ) {
        AllocMoreBckInfo();
    }
    bck = &BckInfoHead->bck;
    BckInfoHead = BckInfoHead->link;
    bck->lbl = AskForLabel( sym );
    bck->imp = NOT_IMPORTED;
    bck->imp_alt = NOT_IMPORTED;
    if( sym == 0 ) {
        bck->segid = AskBackSeg();
    } else {
        bck->segid = FESegID( sym );
    }
    if( !IS_REAL_BACK( bck ) )
        _Zoiks( ZOIKS_067 );
#ifndef NDEBUG
    EchoAPI( " -> %B\n", bck );
#endif
    return( bck );
}

void _CGAPI     BEFiniBack( back_handle bck )
/*******************************************/
{
#ifndef NDEBUG
    EchoAPI( "BEFiniBack( %L )\n", bck );
#endif
    if( IS_REAL_BACK( bck ) ) {
        TellNoSymbol( bck->lbl );
    }
}

void _CGAPI     BEFreeBack( back_handle bck )
/*******************************************/
{
#ifndef NDEBUG
    EchoAPI( "BEFreeBack( %L )\n", bck );
#endif
    if( IS_REAL_BACK( bck ) ) {
//      CGFree( bck );
        uback_info      *p;

        p = (uback_info *)bck;
        p->link = BckInfoHead;
        BckInfoHead = p;
    }
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%                                              %%*/
/*%   Typing information                         %%*/
/*%                                              %%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void _CGAPI     BEDefType( cg_type what, uint align, unsigned_32 len )
/********************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "BEDefType( %t, %x, %i )\n", what, align, len );
#endif
    TypeDef( what, len, align );
}

void _CGAPI     BEAliasType( cg_type what, cg_type to )
/*****************************************************/
{
#ifndef NDEBUG
    EchoAPI( "BEAliasType( %t, %t )\n", what, to );
#endif
    TypeAlias( what, to );
}

unsigned_32 _CGAPI      BETypeLength( cg_type tipe )
/**************************************************/
{
#ifndef NDEBUG
    unsigned_32 retn;
    EchoAPI( "BETypeLength( %t )\n", tipe );
    retn = TypeLength( tipe) ;
    return EchoAPIIntReturn( retn );
#else
    return( TypeLength( tipe ) );
#endif
}

uint _CGAPI     BETypeAlign( cg_type tipe )
/*****************************************/
{
#ifndef NDEBUG
    uint retn;
    EchoAPI( "BETypeAlign( %t )\n", tipe );
    retn = 1;
    return EchoAPIIntReturn( retn );
#else
    /* unused parameters */ (void)tipe;

    return( 1 );
#endif
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%                                              %%*/
/*%   Procedure/parms/autos declaration          %%*/
/*%                                              %%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void _CGAPI     CGProcDecl( cg_sym_handle sym, cg_type tipe )
/***********************************************************/
{
#ifndef NDEBUG
    EchoAPI( "\n" );
    EchoAPI( "CGProcDecl( %s, %t )\n", sym, tipe );
#endif
    if( BGInInline() ) {
        BGProcInline( sym, TypeAddress( tipe ) );
    } else {
        TellObjNewProc( sym );
        BGProcDecl( sym, TypeAddress( tipe ) );
    }
}

void _CGAPI     CGParmDecl( cg_sym_handle sym, cg_type tipe )
/***********************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGParmDecl( %s, %t )\n", sym, tipe );
#endif
    if( BGInInline() ) {
        BGParmInline( sym, TypeAddress( tipe ) );
    } else {
        BGParmDecl( sym, TypeAddress( tipe ) );
    }
}

label_handle _CGAPI CGLastParm( void )
/************************************/
{
    label_handle    top;

#ifndef NDEBUG
    EchoAPI( "CGLastParm()\n" );
#endif
    top = NULL;
    if( _IsntModel( NO_OPTIMIZATION ) ) {
        if( !BGInInline() ) {
            top = BENewLabel();
            BGGenCtrl( O_LABEL, NULL, top, true );
        }
    }
    return( top );
}

void _CGAPI     CGAutoDecl( cg_sym_handle sym, cg_type tipe )
/***********************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGAutoDecl( %s, %t )\n", sym, tipe );
#endif
    BGAutoDecl( sym, TypeAddress( tipe ) );
}

void _CGAPI     CGReturn( cg_name name, cg_type tipe )
/****************************************************/
{
    type_def    *new_tipe;
    an          retv;

#ifndef NDEBUG
    EchoAPI( "CGReturn( %n, %t )\n\n", name, tipe );
    if( NULL != name ) {
        hdlUseOnce( CG_NAMES, name );
        hdlAllUsed( CG_NAMES );
    }
#endif
    new_tipe = TypeAddress( tipe );
    retv = NULL;
    if( name != NULL ) {
        retv = TGReturn( name, new_tipe ); /* special TGen()*/
    }
    if( BGInInline() ) {
        BGRetInline( retv, new_tipe );
    } else {
        BGReturn( retv, new_tipe );
    }
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%                                              %%*/
/*%   Code generator actions                     %%*/
/*%                                              %%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**/
/* Leaf nodes*/
/**/

cg_name _CGAPI CGVarargsBasePtr( cg_type tipe )
/*********************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGVarargsBasePtr( %t )", tipe );
    retn = TGLeaf( BGVarargsBasePtr( TypeAddress( tipe ) ) );
    hdlAdd( CG_NAMES, retn );
    return EchoAPICgnameReturn( retn );
#else
    return( TGLeaf( BGVarargsBasePtr( TypeAddress( tipe ) ) ) );
#endif
}

cg_name _CGAPI CGInteger( signed_32 val, cg_type tipe )
/*****************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGInteger( %i, %t )", val, tipe );
    verifyNotUserType( tipe );
    retn = TGLeaf( BGInteger( val, TypeAddress( tipe ) ) );
    hdlAdd( CG_NAMES, retn );
    return EchoAPICgnameReturn( retn );
#else
    return( TGLeaf( BGInteger( val, TypeAddress( tipe ) ) ) );
#endif
}

cg_name _CGAPI CGInt64( signed_64 val, cg_type tipe )
/***************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGInt64( %x %x, %t )"
           , val.u._32[I64LO32]
           , val.u._32[I64HI32]
           , tipe );
    verifyNotUserType( tipe );
    retn = TGLeaf( BGInt64( val, TypeAddress( tipe ) ) );
    hdlAdd( CG_NAMES, retn );
    return EchoAPICgnameReturn( retn );
#else
    return( TGLeaf( BGInt64( val, TypeAddress( tipe ) ) ) );
#endif
}

cg_name _CGAPI CGFloat( cchar_ptr num, cg_type tipe )
/***************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGFloat( %c, %t )", num, tipe );
    verifyNotUserType( tipe );
    retn = TGLeaf( BGFloat( num, TypeAddress( tipe ) ) );
    hdlAdd( CG_NAMES, retn );
    return EchoAPICgnameReturn( retn );
#else
    return( TGLeaf( BGFloat( num, TypeAddress( tipe ) ) ) );
#endif
}

#if 0
cg_name _CGAPI CGBigInt( float_handle f, cg_type tipe )
/*****************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGBigInt( %x, %t )", f, tipe );
    verifyNotUserType( tipe );
    retn = TGLeaf( MakeConst( f, TypeAddress( tipe ) ) );
    hdlAdd( CG_NAMES, retn );
    return EchoAPICgnameReturn( retn );
#else
    return( TGLeaf( MakeConst( f, TypeAddress( tipe ) ) ) );
#endif
}
#endif

cg_name _CGAPI CGFEName( cg_sym_handle sym, cg_type tipe )
/********************************************************/
{
    cg_name     leaf;


    if( (FEAttr( sym ) & FE_DLLIMPORT) && ( FindAuxInfoSym( sym, CALL_BYTES ) == NULL ) ) {
        leaf = TGLeaf( BGName( CG_FE, sym, TypeAddress( TY_POINTER ) ) );
#ifndef NDEBUG
        EchoAPI( "CGFEName( %s, %t ) declspec(dllimport)", sym, tipe );
        hdlAdd( CG_NAMES, leaf );
#endif
        leaf = CGUnary( O_POINTS, leaf, TY_POINTER );
    } else {
        leaf = TGLeaf( BGName( CG_FE, sym, TypeAddress( tipe ) ) );
#ifndef NDEBUG
        EchoAPI( "CGFEName( %s, %t )", sym, tipe );
        hdlAdd( CG_NAMES, leaf );
#endif
    }
#ifndef NDEBUG
    return EchoAPICgnameReturn( leaf );
#else
    return( leaf );
#endif
}

cg_name _CGAPI CGBackName( back_handle bck, cg_type tipe )
/********************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGBackName( %B, %t )", bck, tipe );
#endif

    if( !IS_REAL_BACK( bck ) )
        _Zoiks( ZOIKS_068 );

#ifndef NDEBUG
    retn = TGLeaf( BGName( CG_BACK, bck, TypeAddress( tipe ) ) );
    hdlAdd( CG_NAMES, retn );
    return EchoAPICgnameReturn( retn );
#else
    return( TGLeaf( BGName( CG_BACK, bck, TypeAddress( tipe ) ) ) );
#endif
}

cg_name _CGAPI CGTempName( temp_handle temp, cg_type tipe )
/*********************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGTempName( %T, %t )", temp, tipe );
    retn = TGLeaf( BGTempName( (name *)temp, TypeAddress( TY_POINTER ) ) );
    hdlAdd( CG_NAMES, retn );
    return EchoAPICgnameReturn( retn );
#else
    /* unused parameters */ (void)tipe;

    return( TGLeaf( BGTempName( (name *)temp, TypeAddress( TY_POINTER ) ) ) );
#endif
}

temp_handle _CGAPI      CGTemp( cg_type tipe )
/********************************************/
{
#ifndef NDEBUG
    temp_handle     retn;

    EchoAPI( "CGTemp( %t )", tipe );
    retn = (temp_handle)BGGlobalTemp( TypeAddress( tipe ) );
    return EchoAPITempHandleReturn( retn );
#else
    return( (temp_handle)BGGlobalTemp( TypeAddress( tipe ) ) );
#endif
}

/**/
/* Assignment operations*/
/**/

cg_name _CGAPI CGAssign( cg_name dest, cg_name source, cg_type tipe )
/*******************************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGAssign( %n, %n, %t )", dest, source, tipe );
    hdlUseOnce( CG_NAMES, dest );
    hdlUseOnce( CG_NAMES, source );
    retn = TGAssign( dest, source, TypeAddress( tipe ) );
    hdlAddBinary( CG_NAMES, retn, dest, source );
    return EchoAPICgnameReturn( retn );
#else
    return( TGAssign( dest, source, TypeAddress( tipe ) ) );
#endif
}


cg_name _CGAPI CGLVAssign( cg_name dest, cg_name source, cg_type tipe )
/*********************************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGLVAssign( %n, %n, %t )", dest, source, tipe );
    hdlUseOnce( CG_NAMES, dest );
    hdlUseOnce( CG_NAMES, source );
    retn = TGLVAssign( dest, source, TypeAddress( tipe ) );
    hdlAddBinary( CG_NAMES, retn, dest, source );
    return EchoAPICgnameReturn( retn );
#else
    return( TGLVAssign( dest, source, TypeAddress( tipe ) ) );
#endif
}


cg_name _CGAPI CGPostGets( cg_op op, cg_name dest, cg_name src, cg_type tipe )
/****************************************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGPostGets( %o, %n, %n, %t )", op, dest, src, tipe );
    hdlUseOnce( CG_NAMES, dest );
    hdlUseOnce( CG_NAMES, src );
    retn = TGPostGets( op, dest, src, TypeAddress( tipe ) );
    hdlAddBinary( CG_NAMES, retn, dest, src );
    return EchoAPICgnameReturn( retn );
#else
    return( TGPostGets( op, dest, src, TypeAddress( tipe ) ) );
#endif
}

cg_name _CGAPI CGPreGets( cg_op op, cg_name dest, cg_name src, cg_type tipe )
/***************************************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGPreGets( %o, %n, %n, %t )", op, dest, src, tipe );
    hdlUseOnce( CG_NAMES, dest );
    hdlUseOnce( CG_NAMES, src );
    retn = TGPreGets( op, dest, src, TypeAddress( tipe ) );
    hdlAddBinary( CG_NAMES, retn, dest, src );
    return EchoAPICgnameReturn( retn );
#else
    return( TGPreGets( op, dest, src, TypeAddress( tipe ) ) );
#endif
}

cg_name _CGAPI CGLVPreGets( cg_op op, cg_name dest, cg_name src, cg_type tipe )
/*****************************************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGLVPreGets( %o, %n, %n, %t )", op, dest, src, tipe );
    hdlUseOnce( CG_NAMES, dest );
    hdlUseOnce( CG_NAMES, src );
    retn = TGLVPreGets( op, dest, src, TypeAddress( tipe ) );
    hdlAddBinary( CG_NAMES, retn, dest, src );
    return EchoAPICgnameReturn( retn );
#else
    return( TGLVPreGets( op, dest, src, TypeAddress( tipe ) ) );
#endif
}

/**/
/* Arithmetic/logical operations*/
/**/

cg_name _CGAPI CGBinary( cg_op op, cg_name name1, cg_name name2, cg_type tipe )
/*****************************************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGBinary( %o, %n, %n, %t )", op, name1, name2, tipe );
    hdlUseOnce( CG_NAMES, name1 );
    hdlUseOnce( CG_NAMES, name2 );
    if( op != O_COMMA ) verifyNotUserType( tipe );
#endif
    if( op == O_COMMA ) {
        name1 = TGTrash( name1 );
    } else if( op == O_SIDE_EFFECT ) {
        name2 = TGTrash( name2 );
    }
#ifndef NDEBUG
    retn = TGBinary( op, name1, name2, TypeAddress(tipe) );
    hdlAddBinary( CG_NAMES, retn, name1, name2 );
    return EchoAPICgnameReturn( retn );
#else
    return( TGBinary( op, name1, name2, TypeAddress( tipe ) ) );
#endif
}

cg_name _CGAPI CGUnary( cg_op op, cg_name name, cg_type tipe )
/************************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGUnary( %o, %n, %t )", op, name, tipe );
    hdlUseOnce( CG_NAMES, name );
    if( op != O_POINTS ) verifyNotUserType( tipe );
    retn = TGUnary( op, name, TypeAddress( tipe ) );
    hdlAddUnary( CG_NAMES, retn, name );
    return EchoAPICgnameReturn( retn );
#else
    return( TGUnary( op, name, TypeAddress( tipe ) ) );
#endif
}

cg_name _CGAPI CGIndex( cg_name name, cg_name by, cg_type tipe, cg_type ptipe )
/*****************************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGIndex( %n, %n, %t, %t )", name, by, tipe, ptipe );
#endif
    return( TGIndex( name, by, TypeAddress( tipe ), TypeAddress( ptipe ) ) );
}

/**/
/* Routine calling*/
/**/

call_handle _CGAPI  CGInitCall( cg_name name, cg_type tipe, cg_sym_handle sym )
/*****************************************************************************/
{
#ifndef NDEBUG
    call_handle  call;

    EchoAPI( "CGInitCall( %n, %t, %s )", name, tipe, sym );
    hdlUseOnce( CG_NAMES, name );
    call = TGInitCall( name, TypeAddress( tipe ), sym );
    hdlAddUnary( CG_NAMES, call, name );
    return( EchoAPICallHandleReturn( call ) );
#else
    return( TGInitCall( name, TypeAddress( tipe ), sym ) );
#endif
}

void _CGAPI     CGAddParm( call_handle call, cg_name name, cg_type tipe )
/***********************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGAddParm( %C, %n, %t )\n", call, name, tipe );
    hdlExists( CG_NAMES, call );
    hdlUseOnce( CG_NAMES, name );
#endif
    TGAddParm( call, name, TypeAddress( tipe ) );
}

cg_name _CGAPI CGCall( call_handle call )
/***************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGCall( %C )\n", call );
    hdlUseOnce( CG_NAMES, call );
    retn = TGCall( call );
    hdlAddUnary( CG_NAMES, retn, call );
    return( retn );
#else
    return( TGCall( call ) );
#endif
}

/**/
/* Comparison/short-circuit operations*/
/**/

cg_name _CGAPI CGCompare( cg_op op, cg_name name1, cg_name name2, cg_type tipe )
/******************************************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGCompare( %o, %n, %n, %t )", op, name1, name2, tipe );
    hdlUseOnce( CG_NAMES, name1 );
    hdlUseOnce( CG_NAMES, name2 );
    retn = TGCompare( op, name1, name2, TypeAddress( tipe ) );
    hdlAddBinary( CG_NAMES, retn, name1, name2 );
    return EchoAPICgnameReturn( retn );
#else
    return( TGCompare( op, name1, name2, TypeAddress( tipe ) ) );
#endif
}

cg_name _CGAPI CGFlow( cg_op op, cg_name name1, cg_name name2 )
/*************************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGFlow( %o, %n, %n )", op, name1, name2 );
    hdlUseOnce( CG_NAMES, name1 );
    if( NULL != name2 ) {
        hdlUseOnce( CG_NAMES, name2 );
    }
    retn = TGFlow( op, name1, name2 );
    hdlAddBinary( CG_NAMES, retn, name1, name2 );
    return EchoAPICgnameReturn( retn );
#else
    return( TGFlow( op, name1, name2 ) );
#endif
}

/**/
/* Control flow operations*/
/**/

cg_name _CGAPI CGChoose( cg_name sel, cg_name n1, cg_name n2, cg_type tipe )
/**************************************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGChoose( %n, %n, %n, %t )", sel, n1, n2, tipe );
    hdlUseOnce( CG_NAMES, sel );
    hdlUseOnce( CG_NAMES, n1 );
    hdlUseOnce( CG_NAMES, n2 );
    retn = TGQuestion( sel, n1, n2, TypeAddress( tipe ) );
    hdlAddTernary( CG_NAMES, retn, sel, n1, n2 );
    return EchoAPICgnameReturn( retn );
#else
    return( TGQuestion( sel, n1, n2, TypeAddress( tipe ) ) );
#endif
}

cg_name _CGAPI CGWarp( cg_name before, label_handle label, cg_name after )
/************************************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGWarp( %n, %L, %n )", before, label, after );
    if( before != NULL ) { // Fortran calls with NULL first parm
        hdlUseOnce( CG_NAMES, before );
    }
    hdlUseOnce( CG_NAMES, after );
    retn = TGWarp( before, label, after );
    hdlAddUnary( CG_NAMES, retn, before );
    return EchoAPICgnameReturn( retn );
#else
    return( TGWarp( before, label, after ) );
#endif
}


cg_name _CGAPI  CGCallback( cg_callback func, callback_handle parm )
/******************************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGCallback( %x, %x )", func, parm );
    retn = TGCallback( func, parm );
    hdlAdd( CG_NAMES, retn );
    EchoAPI( " -> %n\n", retn );
    return retn;
#else
    return( TGCallback( func, parm ) );
#endif
}


void _CGAPI     CG3WayControl( cg_name expr, label_handle lt,
                               label_handle eq, label_handle gt )
/***************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CG3WayControl( %n, %L, %L, %L )\n", expr, lt, eq, gt );
#endif
    TG3WayControl( expr, lt, eq, gt );  /* special TGen()*/
}

void _CGAPI     CGControl( cg_op op, cg_name expr, label_handle lbl )
/*******************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGControl( %o, %n, %L )\n", op, expr, lbl );
    if( NULL != expr ) {
        hdlUseOnce( CG_NAMES, expr );
        hdlAllUsed( CG_NAMES );
    }
    if( lbl != 0 ) {
        hdlExists( LABEL_HANDLE, lbl );
    }
#endif
    TGControl( op, expr, lbl );  /* special TGen()*/
}

void _CGAPI     CGBigLabel( back_handle bck )
/*******************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGBigLabel( %B )\n", bck );
#endif
    BGBigLabel( bck );
}

void _CGAPI     CGBigGoto( label_handle lbl, int level )
/******************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGBigGoto( %L, %i )\n", lbl, level );
#endif
    BGBigGoto( lbl, level );
}

sel_handle _CGAPI       CGSelInit( void )
/***************************************/
{
#ifndef NDEBUG
    sel_handle  retn;

    EchoAPI( "CGSelInit()" );
    retn = BGSelInit();
    hdlAdd( SEL_HANDLE, retn );
    return EchoAPISelHandleReturn( retn );
#else
    return( BGSelInit() );
#endif
}

void _CGAPI     CGSelCase( sel_handle s, label_handle lbl, signed_32 val )
/************************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGSelCase( %S, %L, %i )\n", s, lbl, val );
    hdlExists( SEL_HANDLE, s );
    hdlExists( LABEL_HANDLE, lbl );
#endif
    BGSelCase( s, lbl, val );
}

void _CGAPI     CGSelRange( sel_handle s, signed_32 lo,
                            signed_32 hi, label_handle lbl )
/**********************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGSelRange( %S, %L, %i, %i )\n", s, lbl, lo, hi );
    hdlExists( SEL_HANDLE, s );
    hdlExists( LABEL_HANDLE, lbl );
#endif
    BGSelRange( s, lo, hi, lbl );
}

void _CGAPI     CGSelOther( sel_handle s, label_handle lbl )
/**********************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGSelOther( %S, %L )\n", s, lbl );
    hdlExists( SEL_HANDLE, s );
    hdlExists( LABEL_HANDLE, lbl );
#endif
    BGSelOther( s, lbl );
}

void _CGAPI     CGSelectRestricted( sel_handle s, cg_name expr, cg_switch_type allowed )
/**************************************************************************************/
{
    an  selv;

    selv = TGen( expr, TypeAddress( TY_DEFAULT ) );
    BGSelect( s, selv, allowed );
}

void _CGAPI     CGSelect( sel_handle s, cg_name expr )
/****************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGSelect( %S, %n )\n", s, expr );
    hdlExists( SEL_HANDLE, s );
    hdlUseOnce( CG_NAMES, expr );
    hdlUseOnce( SEL_HANDLE, s );
#endif

    CGSelectRestricted( s, expr, CG_SWITCH_ALL );
}

/**/
/* Misc. operations*/
/**/

cg_name _CGAPI CGEval( cg_name name )
/***********************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGEval( %n )", name );
    hdlUseOnce( CG_NAMES, name );
    retn = TGTmpLeaf( TGen( name, TypeAddress( TY_DEFAULT ) ) );
    hdlAddUnary( CG_NAMES, retn, name );
    return EchoAPICgnameReturn( retn );
#else
    return( TGTmpLeaf( TGen( name, TypeAddress( TY_DEFAULT ) ) ) );
#endif
}

void _CGAPI     CGTrash( cg_name name )
/*************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGTrash( %n )\n", name );
    hdlUseOnce( CG_NAMES, name );
    hdlAllUsed( CG_NAMES );
#endif
    BGTrash( TGen( TGTrash( name ), TypeAddress( TY_DEFAULT ) ) );
}

void _CGAPI     CGDone( cg_name name )
/************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGDone( %n )\n", name );
    hdlUseOnce( CG_NAMES, name );
    hdlAllUsed( CG_NAMES );
#endif
    BGTrash( TGen( TGTrash( name ), TypeAddress( TY_DEFAULT ) ) );
    BGStartBlock();
}

cg_type _CGAPI CGType( cg_name name )
/***********************************/
{
#ifndef NDEBUG
    cg_type     retn;

    EchoAPI( "CGType( %n )", name );
    retn = TGType( name );
    return EchoAPICgtypeReturn( retn );
#else
    return( TGType( name ) );
#endif
}

cg_name _CGAPI CGBitMask( cg_name left, byte start, byte len, cg_type tipe )
/**************************************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGBitMask( %n, %x, %x, %t )", left, start, len, tipe );
    hdlUseOnce( CG_NAMES, left );
    retn = TGBitMask( left, start, len, TypeAddress( tipe ) );
    hdlAddUnary( CG_NAMES, retn, left );
    return EchoAPICgnameReturn( retn );
#else
    return( TGBitMask( left, start, len, TypeAddress( tipe ) ) );
#endif
}

cg_name _CGAPI CGVolatile( cg_name name )
/***************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGVolatile( %n )", name );
    hdlUseOnce( CG_NAMES, name );
    retn = TGVolatile( name );
    hdlAddUnary( CG_NAMES, retn, name );
    return EchoAPICgnameReturn( retn );
#else
    return( TGVolatile( name ) );
#endif
}

cg_name _CGAPI CGAttr( cg_name name, cg_sym_attr attr )
/*****************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGAttr( %n, %i )", name, attr );
    hdlUseOnce( CG_NAMES, name );
    retn = TGAttr( name, attr );
    hdlAddUnary( CG_NAMES, retn, name );
    return EchoAPICgnameReturn( retn );
#else
    return( TGAttr( name, attr ) );
#endif
}

#if 0
cg_name _CGAPI CGAlign( cg_name name, uint alignment )
/****************************************************/
{
#ifndef NDEBUG
    cg_name     retn;

    EchoAPI( "CGAlign( %n, %i )", name, alignment );
    hdlUseOnce( CG_NAMES, name );
    retn = TGAlign( name, alignment );
    hdlAddUnary( CG_NAMES, retn, name );
    return EchoAPICgnameReturn( retn );
#else
    return( TGAlign( name, alignment ) );
#endif
}
#endif

static  cg_name CGDuplicateArray[2];

cg_name * _CGAPI CGDuplicate( cg_name name )
/******************************************/
{
    an          addr;
#ifndef NDEBUG
    cg_name     *retn;

    EchoAPI( "CGDuplicate( %n )", name );
    hdlExists( CG_NAMES, name );
#endif

    addr = TGen( name, TypeAddress( TY_DEFAULT ) );
    CGDuplicateArray[0] = TGReLeaf( BGCopy( addr ) );
    CGDuplicateArray[1] = TGReLeaf( addr );

#ifndef NDEBUG
    retn = CGDuplicateArray;
    EchoAPI( " -> %n %n\n", retn[0], retn[1] );
    hdlAdd( CG_NAMES, retn[0] );
    hdlAdd( CG_NAMES, retn[1] );
    return retn;
#else
    return( CGDuplicateArray );
#endif
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%                                              %%*/
/*%   Data generator actions                     %%*/
/*%                                              %%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void _CGAPI     DGLabel( back_handle bck )
/****************************************/
{
#ifndef NDEBUG
    EchoAPI( "DGLabel( %B )\n", bck );
#endif
    if( !IS_REAL_BACK( bck ) )
        _Zoiks( ZOIKS_068 );
    DGBlip();
    DataLabel( bck->lbl );
    bck->segid = AskOP();
}

void _CGAPI     DGBackPtr( back_handle bck, segment_id segid,
                            signed_32 offset, cg_type tipe )
/***********************************************************/
{
#ifndef NDEBUG
    EchoAPI( "DGBackPtr( %B, %S, %i, %t )\n", bck, segid, offset, tipe );
#endif
    if( !IS_REAL_BACK( bck ) )
        _Zoiks( ZOIKS_068 );
    DGBlip();
    BackPtr( bck, segid, offset, TypeAddress( tipe ) );
}

void _CGAPI DGFEPtr( cg_sym_handle sym, cg_type tipe, signed_32 offset )
/**********************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "DGFEPtr( %S, %t, %i )\n", sym, tipe, offset );
#endif
    DGBlip();
    FEPtr( sym, TypeAddress( tipe ), offset );
}

void _CGAPI     DGBytes( unsigned_32 len, const void *src )
/*********************************************************/
{
#ifndef NDEBUG
    EchoAPI( "DGBytes( %x, %x )\n", len, src );
#endif
    DGBlip();
    DataBytes( len, src );
}

void _CGAPI     DGInteger( unsigned_32 value, cg_type tipe )
/**********************************************************/
{
    type_length len;
    int         i;
    byte        buff[6];
    byte        *form;

#ifndef NDEBUG
    EchoAPI( "DGInteger( %i, %t )\n", value, tipe );
#endif
    len = TypeLength( tipe );
    i = 0;
#if _TARG_MEMORY & _TARG_LOW_FIRST
    form = buff;
    while( i < len ) {
        *form++ = value & 0xff;
        value /= 256;
        ++i;
    }
    form = buff;
#else
    form = buff + 6;
    while( i < len ) {
        *--form = value & 0xff;
        value /= 256;
        ++i;
    }
#endif
    DGBytes( len, form );
}

void _CGAPI     DGInteger64( unsigned_64 value, cg_type tipe )
/************************************************************/
{
    type_length len;
    union{
        unsigned_32 vall;
        unsigned_64 val;
        byte        buff[8];
    } data;
    byte        *form;

#if ( ( _TARG_MEMORY & _TARG_LOW_FIRST ) == 0 ) == defined( __BIG_ENDIAN__ )
    data.val = value;
#else
    {  // reverse them
        union{
            unsigned_64 val;
            byte        buff[8];
        }temp;
        int  i;
        temp.val = value;
        for( i = 0; i <= 7; ++i ) {
            data.buff[i] = temp.buff[7 - i];
        }
    }
#endif
#ifndef NDEBUG
// fix this up when we get printf support for int64
    EchoAPI( "DGInteger64( %x %x, %t )\n"
           , data.val.u._32[0]
           , data.val.u._32[1]
           , tipe );
#endif
    form = data.buff;
    len = TypeLength( tipe );
    DGBytes( len, form );
}

void _CGAPI     DGFloat( cchar_ptr value, cg_type tipe )
/******************************************************/
{
    float_handle    cf;
    flt             buff;

#ifndef NDEBUG
    EchoAPI( "DGFloat( %c, %t )\n", value, tipe );
#endif
    cf = CFCnvSF( value );
    CFCnvTarget( cf, &buff, TypeLength( tipe ) );
    CFFree( cf );
    DGBytes( TypeLength( tipe ), &buff );
}

void _CGAPI     DGIBytes( unsigned_32 len, byte pat )
/***************************************************/
{
#ifndef NDEBUG
    EchoAPI( "DGIBytes( %x, %x )\n", len, pat );
#endif
    DGBlip();
    IterBytes( len, pat );
}

void _CGAPI     DGChar( char value )
/**********************************/
{
#ifndef NDEBUG
    EchoAPI( "DGChar( %x )\n", value );
#endif
    DGIBytes( 1, value );
}

void _CGAPI DGString( cchar_ptr value, uint len )
/***********************************************/
{
#ifndef NDEBUG
    char        data[40];
    unsigned    slen = len;
    char        *d = data;
    char        *dt = &data[sizeof( data ) - 1];
    const char  *s = value;
    char        *hex = "0123456789abcdef";
    char        c;

    for( ;; ) {
        assert( d <= dt );
        if( slen == 0 )
            break;
        --slen;
        c = *s++;
        if( !iscntrl(c) && isascii(c)) {
            if(( d + (1+1)) >= dt )
                break;
            *d++ = c;
        } else {
            if(( d + (4+1) ) >= dt )
                break;
            *d++ = '\\';
            *d++ = 'x';
            *d++ = hex[( c >> 4 ) & 0x0f];
            *d++ = hex[( c >> 0 ) & 0x0f];
        }
    }
    *d = '\0';
    assert( strlen( data ) < sizeof( data ) );
    if( slen != 0 ) {
        EchoAPI( "DGString( \"%c...\", %x )\n", data, len );
    } else {
        EchoAPI( "DGString( \"%c\", %x )\n", data, len );
    }
#endif

#if 1
    DGBytes( len, value );
#else
    // this code is only useful if we do ASCII<->EBCDIC translation
    // but since we don't do conversions; it's only slow
    while( len > 0 ) {
        DGChar( *value++ );
        --len;
    }
#endif
}

void _CGAPI     DGUBytes( unsigned_32 len )
/*****************************************/
{
#ifndef NDEBUG
    EchoAPI( "DGUBytes( %x )\n", len );
#endif
    DGBlip();
    IncLocation( len );
}

void _CGAPI     DGAlign( uint align )
/***********************************/
{
#ifndef NDEBUG
    EchoAPI( "DGAlign( %i )\n", align );
#endif
    DGBlip();
    DataAlign( align );
}


unsigned_32 _CGAPI  DGSeek( unsigned_32 where )
/*********************************************/
{
    uint        old;

#ifndef NDEBUG
    EchoAPI( "DGSeek( %x )", where );
#endif

    DGBlip();
    old = AskLocation();
    SetLocation( where );

#ifndef NDEBUG
    return( EchoAPIHexReturn( old ) );
#else
    return( old );
#endif
}

unsigned_32 _CGAPI      DGTell( void )
/************************************/
{
#ifndef NDEBUG
    unsigned_32     retn;

    EchoAPI( "DGTell()" );
    retn = AskLocation();
    return( EchoAPIIntReturn( retn ) );
#else
    return( AskLocation() );
#endif
}


unsigned_32 _CGAPI      DGBackTell( back_handle bck )
/***************************************************/
{
#ifndef NDEBUG
    unsigned_32 retn;
    EchoAPI( "DGBackTell( %B )", bck );
#endif

    if( !IS_REAL_BACK( bck ) )
        _Zoiks( ZOIKS_068 );

#ifndef NDEBUG
    retn = AskAddress( bck->lbl );
    return EchoAPIIntReturn( retn );
#else
    return( AskAddress( bck->lbl ) );
#endif
}


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%                                              %%*/
/*%   Internal routines                          %%*/
/*%                                              %%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

void _CGAPI     DGCFloat( pointer cf, cg_type tipe )
/**************************************************/
{
    flt        buff;

#ifndef NDEBUG
    EchoAPI( "DGCFloat( %x, %t )\n", cf, tipe );
#endif
    CFCnvTarget( cf, &buff, TypeLength( tipe ) );
    DGBytes( TypeLength( tipe ), &buff );
}

const char * cg_internal AskName( pointer hdl, cg_class class )
/*************************************************************/
{
    switch( class ) {
    case CG_FE:
        return( FEName( hdl ) );
    case CG_LBL:
        return( "(LBL)" );
    case CG_CLB:
        return( "(CLB)" );
    case CG_BACK:
        return( "(BCK)" );
    case CG_TBL:
        return( "(TBL)" );
    case CG_VTB:
        return( "(VTB)" );
    default:
        _Zoiks( ZOIKS_130 );
        return( "" );
    }
}

label_handle cg_internal AskForSymLabel( pointer hdl, cg_class class )
/********************************************************************/
{
    switch( class ) {
    case CG_FE:
        return( FEBack( hdl )->lbl );
    case CG_LBL:
        return( (label_handle)hdl );
    case CG_CLB:
        return( (label_handle)hdl );
    case CG_BACK:
        return( ((back_handle)hdl)->lbl );
    case CG_TBL:
        return( ((tbl_control*)hdl)->lbl );
    case CG_VTB:
        return( ((tbl_control*)hdl)->value_lbl );
    default:
        _Zoiks( ZOIKS_130 );
        return( NULL );
    }
}

import_handle   cg_internal AskImportHandle( cg_sym_handle sym )
/**************************************************************/
{
    return( FEBack( sym )->imp );
}

void    cg_internal TellImportHandle( cg_sym_handle sym, import_handle imphdl )
/*****************************************************************************/
{
    FEBack( sym )->imp = imphdl;
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%                                              %%*/
/*%   CFloat routines                            %%*/
/*%                                              %%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


char * _CGAPI   BFCnvFS( float_handle cf, char_ptr buff, int buff_len )
/*********************************************************************/
{
    return( CFCnvFS( cf, buff, buff_len ) );
}

float_handle _CGAPI     BFCnvSF( cchar_ptr start )
/************************************************/
{
    return( CFCnvSF( start ) );
}

void _CGAPI     BFCnvTarget( float_handle cf, pointer buff, int class )
/*********************************************************************/
{
    CFCnvTarget( cf, (flt *)buff, class );
}

float_handle _CGAPI     BFMul( float_handle c1, float_handle c2 )
/***************************************************************/
{
    return( CFMul( c1, c2 ) );
}

float_handle _CGAPI     BFAdd( float_handle c1, float_handle c2 )
/***************************************************************/
{
    return( CFAdd( c1, c2 ) );
}

float_handle _CGAPI     BFDiv( float_handle c1, float_handle c2 )
/***************************************************************/
{
    return( CFDiv( c1, c2 ) );
}

float_handle _CGAPI     BFSub( float_handle c1, float_handle c2 )
/***************************************************************/
{
    return( CFSub( c1, c2 ) );
}

void _CGAPI             BFNegate( float_handle c1 )
/*************************************************/
{
     CFNegate( c1 );
}

float_handle _CGAPI     BFTrunc( float_handle c1 )
/************************************************/
{
     return( CFTrunc( c1 ) );
}

float_handle _CGAPI     BFCopy( float_handle c1 )
/***********************************************/
{
     return( CFCopy( c1 ) );
}

int _CGAPI              BFSign( float_handle c1 )
/***********************************************/
{
     return( CFTest( c1 ) );
}

float_handle _CGAPI     BFCnvIF( int data )
/*****************************************/
{
    return( CFCnvIF( data ) );
}

float_handle _CGAPI     BFCnvUF( uint data )
/******************************************/
{
    return( CFCnvUF( data ) );
}

signed_32 _CGAPI        BFCnvF32( float_handle f )
/************************************************/
{
    return( CFCnvF32( f ) );
}

int _CGAPI              BFCmp( float_handle l, float_handle r )
/*************************************************************/
{
    return( CFCompare( l, r ) );
}

void _CGAPI             BFFree( float_handle cf )
/***********************************************/
{
    CFFree( cf );
}
