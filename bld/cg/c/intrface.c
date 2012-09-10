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
* Description:  Code generator public interface.
*
****************************************************************************/


#include <ctype.h>
#include "standard.h"
#include "coderep.h"
#include "hostsys.h"
#include "cgdefs.h"
#include "model.h"
#include "seldef.h"
#include "memcheck.h"
#include "cgmem.h"
#include "tree.h"
#include "addrname.h"
#include "cfloat.h"
#include "offset.h"
#include "ptrint.h"
#include "zoiks.h"
#include "cgaux.h"
#include "types.h"
#include "feprotos.h"
#include "cgprotos.h"
#include "rtclass.h"
#include "optlbl.h"

#ifndef NDEBUG
#include "echoapi.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#endif

#include "bldins.h"

extern  void            FEPtr(sym_handle,type_def*,offset);
extern  seg_id          AskOP(void);
extern  void            InitBlip(void);
extern  void            FiniBlip(void);
extern  void            InitWeights(uint);
extern  void            CGMemInit(void);
extern  void            InitSegDefs(void);
extern  void            InitDbgInfo(void);
extern  void            TInit(void);
extern  bool            CGOpenf(void);
extern  void            InitCG(void);
extern  void            SillyMemLimit(void);
extern  void            FiniCG(void);
extern  void            AbortCG(void);
extern  void            FiniDbgInfo(void);
extern  void            TFini(void);
extern  void            CGMemFini(void);
extern  seg_id          SetOP(seg_id);
extern  void            FlushOP(seg_id);
extern  bool            AskSegROM(segment_id);
extern  void            DefSegment(seg_id,seg_attr,char*,uint,bool);
extern  void            BGFiniLabel(label_handle);
extern  seg_id          AskBackSeg(void);
extern  bool            AskSegBlank(seg_id);
extern  void            TellNoSymbol(label_handle);
extern  void            BGProcDecl(sym_handle,type_def*);
extern  void            BGParmDecl(sym_handle,type_def*);
extern  void            BGAutoDecl(sym_handle,type_def*);
extern  tn              TGLeaf(an);
extern  tn              TGTmpLeaf(an);
extern  tn              TGReLeaf(an);
extern  tn              TGLVAssign(tn,tn,type_def*);
extern  tn              TGAssign(tn,tn,type_def*);
extern  tn              TGPostGets(cg_op,tn,tn,type_def*);
extern  tn              TGLVPreGets(cg_op,tn,tn,type_def*);
extern  tn              TGPreGets(cg_op,tn,tn,type_def*);
extern  tn              TGTrash(tn);
extern  tn              TGBinary(cg_op,tn,tn,type_def*);
extern  tn              TGUnary(cg_op,tn,type_def*);
extern  tn              TGIndex(tn,tn,type_def*,type_def*);
extern  tn              TGInitCall(tn,type_def*,sym_handle);
extern  tn              TGAddParm(tn,tn,type_def*);
extern  tn              TGCall(tn);
extern  tn              TGCompare(cg_op,tn,tn,type_def*);
extern  tn              TGFlow(cg_op,tn,tn);
extern  tn              TGQuestion(tn,tn,tn,type_def*);
extern  tn              TGWarp(tn,label_handle,tn);
extern  void            TGControl(cg_op,tn,label_handle);
extern  void            TG3WayControl(tn,label_handle,label_handle,label_handle);
extern  select_node     *BGSelInit(void);
extern  void            BGSelCase(select_node*,label_handle,signed_32);
extern  void            BGSelRange(select_node*,signed_32,signed_32,label_handle);
extern  void            BGSelOther(select_node*,label_handle);
extern  an              TGen(tn,type_def*);
extern  void            BGSelect(select_node*,an,cg_switch_type);
extern  an              TGReturn(tn,type_def*);
extern  void            BGReturn(an,type_def*);
extern  an              BGSave(an);
extern  cg_type         TGType(tn);
extern  btn             TGBitMask(tn,byte,byte,type_def*);
extern  tn              TGVolatile(tn);
extern  tn              TGAttr( tn, cg_sym_attr );
extern  tn              TGAlign( tn, uint );
extern  void            DGBlip(void);
extern  void            DataLabel(label_handle);
extern  void            BackPtr(bck_info*,seg_id,offset,type_def*);
extern  uint            Length(char*);
extern  type_class_def  TypeClass(type_def*);
extern  void            DataBytes(unsigned_32,byte*);
extern  void            IterBytes(offset,byte);
extern  void            SetLocation(offset);
extern  void            IncLocation(offset);
extern  offset          AskLocation(void);
extern  seg_id          AskCodeSeg(void);
extern  seg_id          AskAltCodeSeg(void);
extern  bool            BGInInline(void);
extern  void            BGParmInline(sym_handle,type_def*);
extern  void            BGRetInline(an,type_def*);
extern  void            BGProcInline(sym_handle,type_def*);
extern  void            TellObjNewProc(sym_handle);
extern  tn              TGCallback( cg_callback, callback_handle );
extern  patch_handle    BGNewPatch(void);
extern  cg_name         BGPatchNode( patch_handle, type_def * );
extern  void            BGPatchInteger( patch_handle, signed_32 );
extern  void            BGFiniPatch( patch_handle );
extern  bool            AskSegBlank( seg_id );
extern  an              MakeConst( cfloat *, type_def * );
extern  void            DataAlign( unsigned_32 );
extern  pointer         SafeRecurse( pointer (* rtn)( pointer ), pointer arg );

#ifdef QNX_FLAKEY
unsigned long   OrigModel;
#endif

extern    byte  OptForSize;

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

static  bool            memStarted = FALSE;

extern  void _CGAPI     BEMemInit( void )
/***************************************/
{
    cf_callbacks cf_rtns = { CGAlloc, CGFree };

    BckInfoHead = NULL;
    BckInfoCarveHead = NULL;
    InitBlip();
    CGMemInit();
    CFInit( &cf_rtns );
    memStarted = TRUE;
}

extern  cg_init_info _CGAPI     BEInitCg( cg_switches switches,
                                                cg_target_switches platform,
                                                uint optsize,
                                                proc_revision proc )
/**************************************************************************/
{
    cg_init_info        info;

    OptForSize = optsize;
    CGProcessorVersion = proc; /* so _CPULevel works */
    #if  _TARGET & _TARG_80386
        if( !_CPULevel( CPU_386 ) ) {
            SET_CPU( proc, CPU_386 );
        }
        proc &= ~FPU_EMU;   /* don't need funny fixups for 386 */
    #elif _TARGET & _TARG_IAPX86
        /* if it ain't a 386 or better, FS and GS aren't there */
        if( !_CPULevel( CPU_386 ) ) {
            switches &= ~(FLOATING_FS | FLOATING_GS);
        }
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
        info.version.is_large = TRUE;
#if _TARGET & _TARG_80386
        info.version.target = II_TARG_80386;
#elif _TARGET & _TARG_IAPX86
        info.version.target = II_TARG_8086;
#elif _TARGET & _TARG_370
        info.version.target = II_TARG_370; /* NYI -- for now */
#elif _TARGET & _TARG_AXP
        info.version.target = II_TARG_AXP;
#endif
        info.version.revision = II_REVISION;
    }
    return( info );
}

extern  cg_init_info _CGAPI     BEInit( cg_switches switches,
                                        cg_target_switches platform,
                                        uint optsize, proc_revision proc )
/************************************************************************/
{
    BEMemInit();
    return( BEInitCg( switches, platform, optsize, proc ) );
}

extern  void _CGAPI     BEStart( void )
/*************************************/
{
#ifndef NDEBUG
    EchoAPI( "BEStart()\n" );
#endif
    InitCG();
}

extern  void _CGAPI     BEStop( void )
/************************************/
{
#ifndef NDEBUG
    EchoAPI( "BEStop()\n" );
#endif
    FiniCG();
}

extern  void _CGAPI     BEAbort( void )
/*************************************/
{
#ifndef NDEBUG
    EchoAPI( "BEAbort()\n" );
#endif
    AbortCG();
}

extern  pointer _CGAPI  BEMemAlloc( unsigned size )
/*********************************************************/
{
    return( CGAlloc( size ) );
}

extern  void _CGAPI     BEMemFree( pointer ptr )
/******************************************************/
{
    CGFree( ptr );
}

extern  void _CGAPI     BEMemFini( void )
/***************************************/
{
    CFFini();
    CGMemFini();
}

static void FreeBckInfoCarveBlocks( void )
{
    bck_info_block      *carve_block;

    for( ; carve_block = BckInfoCarveHead; ) {
        BckInfoCarveHead = carve_block->next;
        CGFree( carve_block );
    }
    BckInfoHead = NULL;
}

extern  void _CGAPI     BEFiniCg( void )
/**************************************/
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

extern  void _CGAPI     BEFini( void )
/************************************/
{
    BEFiniCg();
    BEMemFini();
}

extern  bool _CGAPI     BEMoreMem( void )
/***************************************/
{
    return( _MemCheck( 1 ) );
}

extern  segment_id _CGAPI       BESetSeg( segment_id seg )
/********************************************************/
{
#ifndef NDEBUG
    EchoAPI( "BESetSeg( %x )", seg );
    seg = SetOP(seg);
    return EchoAPIHexReturn( seg );
#else
    return( SetOP( seg ) );
#endif
}

extern  void _CGAPI     BEDefSeg( segment_id id, seg_attr attr, char *str, uint algn )
/************************************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "BEDefSeg( %x, %x, %c, %i )\n", id, attr, str, algn );
#endif
    DefSegment( id, attr, str, algn, FALSE );
}

extern  void _CGAPI     BEFlushSeg( segment_id seg )
/**************************************************/
{
#ifndef NDEBUG
    EchoAPI( "BEFlushSeg( %x )\n", seg );
#endif
    FlushOP( seg );
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%                                              %%*/
/*%   Handle creation/deletion                   %%*/
/*%                                              %%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

extern  label_handle _CGAPI BENewLabel( void )
/********************************************/
{
#ifndef NDEBUG
    label_handle retn;

    EchoAPI( "BENewLabel()" );
    retn = AskForNewLabel();
    EchoAPI( " -> %L\n", retn );
    hdlAdd( LABEL_HANDLE, retn );
    return retn;
#else
    return( AskForNewLabel() );
#endif
}

extern  void _CGAPI     BEFiniLabel( label_handle lbl )
/*****************************************************/
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


extern  unsigned_32 _CGAPI      BEUnrollCount( unsigned_32 unroll_count )
/***********************************************************************/
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

extern  patch_handle _CGAPI     BEPatch( void )
/*********************************************/
{
#ifndef NDEBUG
    patch_handle retn;

    EchoAPI( "BEPatch()" );
    retn = BGNewPatch();
    EchoAPI( " -> %P\n", retn );
    hdlAdd( PATCH_HANDLE, retn );
    return retn;
#else
    return( BGNewPatch() );
#endif
}

extern  cg_name _CGAPI          CGPatchNode( patch_handle hdl, cg_type tipe )
/***************************************************************************/
{
#ifndef NDEBUG
    cg_name retn;
    EchoAPI( "CGPatchNode( %P, %t )", hdl, tipe );
    hdlExists( PATCH_HANDLE, hdl );
    retn = BGPatchNode( hdl, TypeAddress( tipe ) );
    hdlAdd( CG_NAMES, retn );
    return EchoAPICgnameReturn( retn );
#else
    return( BGPatchNode( hdl, TypeAddress( tipe ) ) );
#endif
}

extern  void _CGAPI     BEPatchInteger( patch_handle hdl, signed_32 value )
/*************************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "BEPatchInteger( %P, %x )\n", hdl, value );
    hdlExists( PATCH_HANDLE, hdl );
#endif
    BGPatchInteger( hdl, value );
}

extern  void _CGAPI     BEFiniPatch( patch_handle hdl )
/*****************************************************/
{
#ifndef NDEBUG
    EchoAPI( "BEFiniPatch( %P )\n", hdl );
    hdlUseOnce( PATCH_HANDLE, hdl );
#endif

    BGFiniPatch( hdl );
}


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%                                              %%*/
/*%   Back Handles                               %%*/
/*%                                              %%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#define PTR_INT( x )            (*(pointer_int*)&(x))
#define FAKE_BACK               ((pointer_int)1)
#define REAL_BACK( bck )        ( ( PTR_INT( bck ) & FAKE_BACK ) == 0 )
#define RETURN_NULL             (pointer)1
#define RETURN_NORMAL           NULL
static  pointer                 NewBackReturn = RETURN_NORMAL;


extern  pointer LkAddBack( sym_handle sym, pointer curr_back )
/************************************************************/
{
    bck_info    *bck;

    if( curr_back == NULL ) {
        NewBackReturn = RETURN_NULL;
    } else {
        NewBackReturn = (pointer)( PTR_INT( curr_back ) | FAKE_BACK );
    }
    bck = FEBack( sym );
    NewBackReturn = RETURN_NORMAL;
    return( (pointer)( PTR_INT( bck ) & ~FAKE_BACK ) );
}

extern pointer SymBack( pointer sym )
/***********************************/
{
    bck_info    *bck;

    bck = FEBack( sym );
    return( (pointer)( PTR_INT( bck ) & ~FAKE_BACK ) );
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
        for( i = 0; i < (MAX_BCK_INFO-1); i++ ) {
            p->link = p + 1;
            ++p;
        }
        p->link = NULL;
    }
}

extern  back_handle _CGAPI      BENewBack( sym_handle sym )
/*********************************************************/
{
    bck_info            *bck;

#ifndef NDEBUG
    EchoAPI( "BENewBack( %s )", sym );
#endif
    if( NewBackReturn == RETURN_NULL ) {
#ifndef NDEBUG
    EchoAPI( " -> %B\n", NULL );
#endif
        return( NULL );
    }
    if( NewBackReturn != RETURN_NORMAL ) {
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
    if( sym == 0 ) {
        bck->seg = AskBackSeg();
   } else {
        bck->seg = FESegID( sym );
    }
    if( !REAL_BACK( bck ) ) _Zoiks( ZOIKS_067 );
#ifndef NDEBUG
    EchoAPI( " -> %B\n", bck );
#endif
    return( bck );
}

extern  void _CGAPI     BEFiniBack( bck_info *bck )
/*************************************************/
{
#ifndef NDEBUG
    EchoAPI( "BEFiniBack( %L )\n", bck );
#endif
    if( REAL_BACK( bck ) ) TellNoSymbol( bck->lbl );
}

extern  void _CGAPI     BEFreeBack( bck_info *bck )
/*************************************************/
{
#ifndef NDEBUG
    EchoAPI( "BEFreeBack( %L )\n", bck );
#endif
    if( REAL_BACK( bck ) ) {
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

extern  void _CGAPI     BEDefType( cg_type what, uint align, unsigned_32 len )
/****************************************************************************/
{
    align = align;
#ifndef NDEBUG
    EchoAPI( "BEDefType( %t, %x, %i )\n", what, align, len );
#endif
    TypeDef( what, len, align );
}

extern  void _CGAPI     BEAliasType( cg_type what, cg_type to )
/*************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "BEAliasType( %t, %t )\n", what, to );
#endif
    TypeAlias( what, to );
}

extern  unsigned_32 _CGAPI      BETypeLength( cg_type tipe )
/**********************************************************/
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

extern  uint _CGAPI     BETypeAlign( cg_type tipe )
/*************************************************/
{
#ifndef NDEBUG
    uint retn;
    EchoAPI( "BETypeAlign( %t )\n", tipe );
    retn = 1;
    return EchoAPIIntReturn( retn );
#else
    tipe = tipe;
    return( 1 );
#endif
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%                                              %%*/
/*%   Procedure/parms/autos declaration          %%*/
/*%                                              %%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

extern  void _CGAPI     CGProcDecl( pointer name, cg_type tipe )
/**************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "\n" );
    EchoAPI( "CGProcDecl( %s, %t )\n", name, tipe );
#endif
    if( BGInInline() ) {
        BGProcInline( name, TypeAddress( tipe ) );
    } else {
        TellObjNewProc( name );
        BGProcDecl( name, TypeAddress( tipe ) );
    }
}

extern  void _CGAPI     CGParmDecl( pointer name, cg_type tipe )
/**************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGParmDecl( %s, %t )\n", name, tipe );
#endif
    if( BGInInline() ) {
        BGParmInline( name, TypeAddress( tipe ) );
    } else {
        BGParmDecl( name, TypeAddress( tipe ) );
    }
}

extern label_handle _CGAPI CGLastParm( void )
/*******************************************/
{
    label_handle        top;

#ifndef NDEBUG
    EchoAPI( "CGLastParm()\n" );
#endif
    top = NULL;
    if( _IsntModel( NO_OPTIMIZATION ) ) {
        if( !BGInInline() ) {
            top = BENewLabel();
            BGGenCtrl( O_LABEL, NULL, top, TRUE );
        }
    }
    return( top );
}

extern  void _CGAPI     CGAutoDecl( pointer name, cg_type tipe )
/**************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGAutoDecl( %s, %t )\n", name, tipe );
#endif
    BGAutoDecl( name, TypeAddress( tipe ) );
}

extern  void _CGAPI     CGReturn( cg_name name, cg_type tipe )
/************************************************************/
{
    type_def    *new_tipe;

#ifndef NDEBUG
    EchoAPI( "CGReturn( %n, %t )\n\n", name, tipe );
    if( NULL != name ) {
        hdlUseOnce( CG_NAMES, name );
        hdlAllUsed( CG_NAMES );
    }
#endif
    new_tipe = TypeAddress( tipe );
    if( name != NULL ) {
        name = TGReturn( name, new_tipe ); /* special TGen()*/
    }
    if( BGInInline() ) {
        BGRetInline( name, new_tipe );
    } else {
        BGReturn( name, new_tipe );
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

extern  cg_name _CGAPI CGVarargsBasePtr( cg_type tipe )
/*****************************************************/
{
#ifndef NDEBUG
    tn      retn;

    EchoAPI( "CGVarargsBasePtr( %t )", tipe );
    retn = TGLeaf( BGVarargsBasePtr( TypeAddress( tipe ) ) );
    hdlAdd( CG_NAMES, retn );
    return EchoAPICgnameReturn( retn );
#else
    return( TGLeaf( BGVarargsBasePtr( TypeAddress( tipe ) ) ) );
#endif
}

extern  cg_name _CGAPI CGInteger( signed_32 val, cg_type tipe )
/*************************************************************/
{
#ifndef NDEBUG
    tn      retn;

    EchoAPI( "CGInteger( %i, %t )", val, tipe );
    verifyNotUserType( tipe );
    retn = TGLeaf( BGInteger( val, TypeAddress( tipe ) ) );
    hdlAdd( CG_NAMES, retn );
    return EchoAPICgnameReturn( retn );
#else
    return( TGLeaf( BGInteger( val, TypeAddress( tipe ) ) ) );
#endif
}

extern  cg_name _CGAPI CGInt64( signed_64 val, cg_type tipe )
/***********************************************************/
{
#ifndef NDEBUG
    tn retn;
    EchoAPI( "CGInt64( %x %x, %t )"
           , val.u._32[ I64LO32 ]
           , val.u._32[ I64HI32 ]
           , tipe );
    verifyNotUserType( tipe );
    retn = TGLeaf( BGInt64( val, TypeAddress( tipe ) ) );
    hdlAdd( CG_NAMES, retn );
    return EchoAPICgnameReturn( retn );
#else
    return( TGLeaf( BGInt64( val, TypeAddress( tipe ) ) ) );
#endif
}

extern  cg_name _CGAPI CGFloat( char *num, cg_type tipe )
/*******************************************************/
{
#ifndef NDEBUG
    tn      retn;

    EchoAPI( "CGFloat( %c, %t )", num, tipe );
    verifyNotUserType( tipe );
    retn = TGLeaf( BGFloat( num, TypeAddress( tipe ) ) );
    hdlAdd( CG_NAMES, retn );
    return EchoAPICgnameReturn( retn );
#else
    return( TGLeaf( BGFloat( num, TypeAddress( tipe ) ) ) );
#endif
}

extern  cg_name _CGAPI CGBigInt( float_handle f, cg_type tipe )
/*************************************************************/
{
#ifndef NDEBUG
    tn      retn;

    EchoAPI( "CGBigInt( %x, %t )", f, tipe );
    verifyNotUserType( tipe );
    retn = TGLeaf( MakeConst( f, TypeAddress( tipe ) ) );
    hdlAdd( CG_NAMES, retn );
    return EchoAPICgnameReturn( retn );
#else
    return( TGLeaf( MakeConst( f, TypeAddress( tipe ) ) ) );
#endif
}

extern  cg_name _CGAPI CGFEName( sym_handle sym, cg_type tipe )
/*************************************************************/
{
    cg_name     leaf;


    if( (FEAttr( sym ) & FE_DLLIMPORT )
     && ( FEAuxInfo( sym, CALL_BYTES ) == NULL ) ){
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

extern  cg_name _CGAPI CGBackName( bck_info *bck, cg_type tipe )
/**************************************************************/
{
#ifndef NDEBUG
    tn      retn;

    EchoAPI( "CGBackName( %B, %t )", bck, tipe );
#endif

    if( !REAL_BACK( bck ) ) _Zoiks( ZOIKS_068 );

#ifndef NDEBUG
    retn = TGLeaf( BGName( CG_BACK, bck, TypeAddress( tipe ) ) );
    hdlAdd( CG_NAMES, retn );
    return EchoAPICgnameReturn( retn );
#else
    return( TGLeaf( BGName( CG_BACK, bck, TypeAddress( tipe ) ) ) );
#endif
}

extern  cg_name _CGAPI CGTempName( temp_handle temp, cg_type tipe )
/*****************************************************************/
{
#ifndef NDEBUG
    tn      retn;

    EchoAPI( "CGTempName( %T, %t )", temp, tipe );
    retn = TGLeaf( BGTempName( temp, TypeAddress( TY_POINTER ) ) );
    hdlAdd( CG_NAMES, retn );
    return EchoAPICgnameReturn( retn );
#else
    tipe = tipe;
    return( TGLeaf( BGTempName( temp, TypeAddress( TY_POINTER ) ) ) );
#endif
}

extern  temp_handle _CGAPI      CGTemp( cg_type tipe )
/****************************************************/
{
#ifndef NDEBUG
    temp_handle     retn;

    EchoAPI( "CGTemp( %t )", tipe );
    retn = BGGlobalTemp( TypeAddress( tipe ) );
    return EchoAPITempHandleReturn( retn );
#else
    return( BGGlobalTemp( TypeAddress( tipe ) ) );
#endif
}

/**/
/* Assignment operations*/
/**/

extern  cg_name _CGAPI CGAssign( cg_name dest, cg_name source, cg_type tipe )
/***************************************************************************/
{
#ifndef NDEBUG
    tn      retn;

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


extern  cg_name _CGAPI CGLVAssign( cg_name dest, cg_name source, cg_type tipe )
/*****************************************************************************/
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


extern  cg_name _CGAPI CGPostGets( cg_op op, cg_name dest,
                            cg_name src, cg_type tipe )
/********************************************************/
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

extern  cg_name _CGAPI CGPreGets( cg_op op, cg_name dest, cg_name src, cg_type tipe )
/***********************************************************************************/
{
#ifndef NDEBUG
    tn      retn;

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

extern  cg_name _CGAPI CGLVPreGets( cg_op op, cg_name dest, cg_name src, cg_type tipe )
/*************************************************************************************/
{
#ifndef NDEBUG
    tn      retn;

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

extern  cg_name _CGAPI CGBinary( cg_op op, cg_name name1,
                          cg_name name2, cg_type tipe )
/*******************************************************/
{
#ifndef NDEBUG
    tn      retn;

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

extern  cg_name _CGAPI CGUnary( cg_op op, cg_name name, cg_type tipe )
/********************************************************************/
{
#ifndef NDEBUG
    tn      retn;

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

extern  cg_name _CGAPI CGIndex( cg_name name, cg_name by,
                         cg_type tipe, cg_type ptipe )
/*******************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGIndex( %n, %n, %t, %t )", name, by, tipe, ptipe );
#endif
    return( TGIndex( name, by, TypeAddress( tipe ), TypeAddress( ptipe ) ) );
}

/**/
/* Routine calling*/
/**/

extern  call_handle _CGAPI      CGInitCall( cg_name name, cg_type tipe,
                                            sym_handle aux_info )
/*********************************************************************/
{
#ifndef NDEBUG
    tn      retn;

    EchoAPI( "CGInitCall( %n, %t, %s )", name, tipe, aux_info );
    hdlUseOnce( CG_NAMES, name );
    retn = TGInitCall( name, TypeAddress( tipe ), aux_info );
    hdlAddUnary( CG_NAMES, retn, name );
    return EchoAPICallHandleReturn( retn );
#else
    return( TGInitCall( name, TypeAddress( tipe ), aux_info ) );
#endif
}

extern  void _CGAPI     CGAddParm( call_handle call, cg_name name, cg_type tipe )
/*******************************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGAddParm( %C, %n, %t )\n", call, name, tipe );
    hdlExists( CG_NAMES, call );
    hdlUseOnce( CG_NAMES, name );
#endif
    TGAddParm( call, name, TypeAddress( tipe ) );
}

extern  cg_name _CGAPI CGCall( call_handle call )
/***********************************************/
{
#ifndef NDEBUG
    tn      retn;

    EchoAPI( "CGCall( %C )\n", call );
    hdlUseOnce( CG_NAMES, call );
    retn = TGCall( call );
    hdlAddUnary( CG_NAMES, retn, call );
    return retn;
#else
    return( TGCall( call ) );
#endif
}

/**/
/* Comparison/short-circuit operations*/
/**/

extern  cg_name _CGAPI CGCompare( cg_op op, cg_name name1,
                           cg_name name2, cg_type tipe )
/********************************************************/
{
#ifndef NDEBUG
    tn      retn;

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

extern  cg_name _CGAPI CGFlow( cg_op op, cg_name name1, cg_name name2 )
/*********************************************************************/
{
#ifndef NDEBUG
    tn      retn;

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

extern  cg_name _CGAPI CGChoose( cg_name sel, cg_name n1, cg_name n2, cg_type tipe )
/**********************************************************************************/
{
#ifndef NDEBUG
    tn      retn;

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

extern  cg_name _CGAPI CGWarp( cg_name before, label_handle label, cg_name after )
/********************************************************************************/
{
#ifndef NDEBUG
    tn      retn;

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


extern  cg_name _CGAPI  CGCallback( cg_callback func, callback_handle parm )
/**************************************************************************/
{
#ifndef NDEBUG
    tn      retn;

    EchoAPI( "CGCallback( %x, %x )", func, parm );
    retn = TGCallback( func, parm );
    hdlAdd( CG_NAMES, retn );
    EchoAPI( " -> %n\n", retn );
    return retn;
#else
    return( TGCallback( func, parm ) );
#endif
}


extern  void _CGAPI     CG3WayControl( cg_name expr, label_handle lt,
                               label_handle eq, label_handle gt )
/*******************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CG3WayControl( %n, %L, %L, %L )\n", expr, lt, eq, gt );
#endif
    TG3WayControl( expr, lt, eq, gt );  /* special TGen()*/
}

extern  void _CGAPI     CGControl( cg_op op, cg_name expr, label_handle lbl )
/***************************************************************************/
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

extern  void _CGAPI     CGBigLabel( back_handle value )
/*****************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGBigLabel( %B )\n", value );
#endif
    BGBigLabel( value );
}

extern  void _CGAPI     CGBigGoto( label_handle value, int level )
/****************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGBigLabel( %L, %i )\n", value, level );
#endif
    BGBigGoto( value, level );
}

extern  sel_handle _CGAPI       CGSelInit( void )
/***********************************************/
{
#ifndef NDEBUG
    select_node     *retn;

    EchoAPI( "CGSelInit()" );
    retn = BGSelInit();
    hdlAdd( SEL_HANDLE, retn );
    return EchoAPISelHandleReturn( retn );
#else
    return( BGSelInit() );
#endif
}

extern  void _CGAPI     CGSelCase( sel_handle s, label_handle lbl, signed_32 val )
/********************************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGSelCase( %S, %L, %i )\n", s, lbl, val );
    hdlExists( SEL_HANDLE, s );
    hdlExists( LABEL_HANDLE, lbl );
#endif

    BGSelCase( s, lbl, val );
}

extern  void _CGAPI     CGSelRange( sel_handle s, signed_32 lo,
                            signed_32 hi, label_handle lbl )
/*************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGSelRange( %S, %L, %i, %i )\n", s, lbl, lo, hi );
    hdlExists( SEL_HANDLE, s );
    hdlExists( LABEL_HANDLE, lbl );
#endif
    BGSelRange( s, lo, hi, lbl );
}

extern  void _CGAPI     CGSelOther( sel_handle s, label_handle lbl )
/******************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGSelOther( %S, %L )\n", s, lbl );
    hdlExists( SEL_HANDLE, s );
    hdlExists( LABEL_HANDLE, lbl );
#endif
    BGSelOther( s, lbl );
}

extern  void _CGAPI     CGSelectRestricted( sel_handle s, cg_name expr, cg_switch_type allowed )
/**********************************************************************************************/
{
    expr = TGen( expr, TypeAddress( TY_DEFAULT ) );
    BGSelect( s, expr, allowed );
}

extern  void _CGAPI     CGSelect( sel_handle s, cg_name expr )
/************************************************************/
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

extern  cg_name _CGAPI CGEval( cg_name name )
/*******************************************/
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

extern  void _CGAPI     CGTrash( cg_name name )
/*********************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGTrash( %n )\n", name );
    hdlUseOnce( CG_NAMES, name );
    hdlAllUsed( CG_NAMES );
#endif
    BGTrash( TGen( TGTrash( name ), TypeAddress( TY_DEFAULT ) ) );
}

extern  void _CGAPI     CGDone( cg_name name )
/********************************************/
{
#ifndef NDEBUG
    EchoAPI( "CGDone( %n )\n", name );
    hdlUseOnce( CG_NAMES, name );
    hdlAllUsed( CG_NAMES );
#endif
    BGTrash( TGen( TGTrash( name ), TypeAddress( TY_DEFAULT ) ) );
    BGStartBlock();
}

extern  cg_type _CGAPI CGType( cg_name name )
/*******************************************/
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

extern  cg_name _CGAPI CGBitMask( cg_name left, byte start, byte len, cg_type tipe )
/**********************************************************************************/
{
#ifndef NDEBUG
    tn_btn      retn;

    EchoAPI( "CGBitMask( %n, %x, %x, %t )", left, start, len, tipe );
    hdlUseOnce( CG_NAMES, left );
    retn.b = TGBitMask( left, start, len, TypeAddress( tipe ) );
    hdlAddUnary( CG_NAMES, retn.t, left );
    return EchoAPICgnameReturn( retn.t );
#else
    return( TGBitMask( left, start, len, TypeAddress( tipe ) ) );
#endif
}

extern  cg_name _CGAPI CGVolatile( cg_name name )
/***********************************************/
{
#ifndef NDEBUG
    tn      retn;

    EchoAPI( "CGVolatile( %n )", name );
    hdlUseOnce( CG_NAMES, name );
    retn = TGVolatile( name );
    hdlAddUnary( CG_NAMES, retn, name );
    return EchoAPICgnameReturn( retn );
#else
    return( TGVolatile( name ) );
#endif
}

extern cg_name _CGAPI CGAttr( cg_name name, cg_sym_attr attr )
/************************************************************/
{
#ifndef NDEBUG
    tn      retn;

    EchoAPI( "CGAttr( %n, %i )", name, attr );
    hdlUseOnce( CG_NAMES, name );
    retn = TGAttr( name, attr );
    hdlAddUnary( CG_NAMES, retn, name );
    return EchoAPICgnameReturn( retn );
#else
    return( TGAttr( name, attr ) );
#endif
}

extern cg_name _CGAPI CGAlign( cg_name name, uint alignment )
/***********************************************************/
{
#ifndef NDEBUG
    tn      retn;

    EchoAPI( "CGAlign( %n, %i )", name, alignment );
    hdlUseOnce( CG_NAMES, name );
    retn = TGAlign( name, alignment );
    hdlAddUnary( CG_NAMES, retn, name );
    return EchoAPICgnameReturn( retn );
#else
    return( TGAlign( name, alignment ) );
#endif
}

static  cg_name CGDuplicateArray[ 2 ];

extern  cg_name * _CGAPI CGDuplicate( cg_name name )
/**************************************************/
{
    an          addr;
#ifndef NDEBUG
    cg_name     *retn;

    EchoAPI( "CGDuplicate( %n )", name );
    hdlExists( CG_NAMES, name );
#endif

    addr = TGen( name, TypeAddress( TY_DEFAULT ) );
    CGDuplicateArray[ 0 ] = TGReLeaf( BGCopy( addr ) );
    CGDuplicateArray[ 1 ] = TGReLeaf( addr );

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

extern  void _CGAPI     DGLabel( bck_info *bck )
/**********************************************/
{
#ifndef NDEBUG
    EchoAPI( "DGLabel( %B )\n", bck );
#endif
    if( !REAL_BACK( bck ) ) _Zoiks( ZOIKS_068 );
    DGBlip();
    DataLabel( bck->lbl );
    bck->seg = AskOP();
}

extern  void _CGAPI     DGBackPtr( bck_info *bck, segment_id seg,
                                   signed_32 offset, cg_type tipe )
/*****************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "DGBackPtr( %B, %S, %i, %t )\n", bck, seg, offset, tipe );
#endif
    if( !REAL_BACK( bck ) ) _Zoiks( ZOIKS_068 );
    DGBlip();
    BackPtr( bck, seg, offset, TypeAddress( tipe ) );
}

extern  void _CGAPI DGFEPtr( cg_sym_handle sym, cg_type tipe, signed_32 offset )
/******************************************************************************/
{
#ifndef NDEBUG
    EchoAPI( "DGFEPtr( %S, %t, %i )\n", sym, tipe, offset );
#endif
    DGBlip();
    FEPtr( sym, TypeAddress( tipe ), offset );
}

extern  void _CGAPI     DGBytes( unsigned_32 len, void *src )
/***********************************************************/
{
#ifndef NDEBUG
    EchoAPI( "DGBytes( %x, %x )\n", len, src );
#endif
    DGBlip();
    DataBytes( len, src );
}

extern  void _CGAPI     DGInteger( unsigned_32 value, cg_type tipe )
/******************************************************************/
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

extern  void _CGAPI     DGInteger64( unsigned_64 value, cg_type tipe )
/********************************************************************/
{
    type_length len;
    int         i;
    union{
        unsigned_32 vall;
        unsigned_64 val;
        byte        buff[8];
    } data;
    byte        *form;

    i = 0;
#if !( _TARG_MEMORY & _TARG_LOW_FIRST) == !(_HOST_MEMORY & _LOW_FIRST)
    data.val = value;
#else
    {  // reverse them
        union{
            unsigned_64 val;
            byte        buff[8];
        }temp;
        temp.val = value;
        while( i <= 7 ) {
            data.buf[i] = temp.buff[7-i];
            ++i;
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

extern  void _CGAPI     DGFloat( char *value, cg_type tipe )
/**********************************************************/
{
    pointer     cf;
    flt         buff;

#ifndef NDEBUG
    EchoAPI( "DGFloat( %c, %t )\n", value, tipe );
#endif
    cf = CFCnvSF( value, value + Length( value ) );
    CFCnvTarget( cf, &buff, TypeLength( tipe ) );
    CFFree( cf );
    DGBytes( TypeLength( tipe ), &buff );
}

extern  void _CGAPI     DGIBytes( unsigned_32 len, byte pat )
/***********************************************************/
{
#ifndef NDEBUG
    EchoAPI( "DGIBytes( %x, %x )\n", len, pat );
#endif
    DGBlip();
    IterBytes( len, pat );
}

extern  void _CGAPI     DGChar( char value )
/******************************************/
{
#ifndef NDEBUG
    EchoAPI( "DGChar( %x )\n", value );
#endif
    DGIBytes( 1, value );
}

extern  void _CGAPI     DGString( char *value, uint len )
/*******************************************************/
{
#ifndef NDEBUG
    char        data[40];
    unsigned    slen = len;
    char        *d = data;
    char        *dt = &data[sizeof(data)-1];
    char        *s = value;
    char        *hex = "0123456789abcdef";
    char        c;

    for( ;; ) {
        assert( d <= dt );
        if( slen == 0 ) break;
        --slen;
        c = *s++;
        if( !iscntrl(c) && isascii(c)) {
            if(( d + (1+1)) >= dt ) break;
            *d++ = c;
        } else {
            if(( d + (4+1) ) >= dt ) break;
            *d++ = '\\';
            *d++ = 'x';
            *d++ = hex[ ( c >> 4 ) & 0x0f ];
            *d++ = hex[ ( c >> 0 ) & 0x0f ];
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

extern  void _CGAPI     DGUBytes( unsigned_32 len )
/*************************************************/
{
#ifndef NDEBUG
    EchoAPI( "DGUBytes( %x )\n", len );
#endif
    DGBlip();
    IncLocation( len );
}

extern  void _CGAPI     DGAlign( uint align )
/*******************************************/
{
#ifndef NDEBUG
    EchoAPI( "DGAlign( %i )\n", align );
#endif
    DGBlip();
    DataAlign( align );
}


extern  unsigned_32 _CGAPI  DGSeek( unsigned_32 where )
/*****************************************************/
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

extern  unsigned_32 _CGAPI      DGTell( void )
/********************************************/
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


extern  unsigned_32 _CGAPI      DGBackTell( bck_info *bck )
/*********************************************************/
{
#ifndef NDEBUG
    unsigned_32 retn;
    EchoAPI( "DGBackTell( %B )", bck );
#endif

    if( !REAL_BACK( bck ) ) _Zoiks( ZOIKS_068 );

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

extern  void _CGAPI     DGCFloat( pointer cf, cg_type tipe )
/**********************************************************/
{
    flt        buff;

#ifndef NDEBUG
    EchoAPI( "DGCFloat( %x, %t )\n", cf, tipe );
#endif
    CFCnvTarget( cf, &buff, TypeLength( tipe ) );
    DGBytes( TypeLength( tipe ), &buff );
}

extern  char    *AskName( pointer hdl, cg_class class )
/*****************************************************/
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

extern  label_handle    AskForSymLabel( pointer hdl, cg_class class )
/*******************************************************************/
{
    switch( class ) {
    case CG_FE:
        return( FEBack( hdl )->lbl );
    case CG_LBL:
        return( (label_handle)hdl );
    case CG_CLB:
        return( (label_handle)hdl );
    case CG_BACK:
        return( ((bck_info*)hdl)->lbl );
    case CG_TBL:
        return( ((tbl_control*)hdl)->lbl );
    case CG_VTB:
        return( ((tbl_control*)hdl)->value_lbl );
    default:
        _Zoiks( ZOIKS_130 );
        return( NULL );
    }
}

extern  import_handle   AskImportHandle( sym_handle sym )
/*******************************************************/
{
    return( FEBack( sym )->imp );
}

extern  void    TellImportHandle( sym_handle sym, import_handle imp )
/*******************************************************************/
{
    FEBack( sym )->imp = imp;
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%                                              %%*/
/*%   CFloat routines                            %%*/
/*%                                              %%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


extern  char * _CGAPI             BFCnvFS( float_handle cf, char *buff, int buff_len )
/************************************************************************************/
{
    return( CFCnvFS( (cfloat *)cf, buff, buff_len ) );
}

extern  float_handle _CGAPI     BFCnvSF( char *start, char *end )
/***************************************************************/
{
    return( CFCnvSF( start, end ) );
}

extern  float_handle _CGAPI     BFMul( float_handle c1, float_handle c2 )
/***********************************************************************/
{
    return( CFMul( (cfloat *)c1, (cfloat *)c2 ) );
}

extern  float_handle _CGAPI     BFAdd( float_handle c1, float_handle c2 )
/***********************************************************************/
{
    return( CFAdd( (cfloat *)c1, (cfloat *)c2 ) );
}

extern  float_handle _CGAPI     BFDiv( float_handle c1, float_handle c2 )
/***********************************************************************/
{
    return( CFDiv( (cfloat *)c1, (cfloat *)c2 ) );
}

extern  float_handle _CGAPI     BFSub( float_handle c1, float_handle c2 )
/***********************************************************************/
{
    return( CFSub( (cfloat *)c1, (cfloat *)c2 ) );
}

extern  void _CGAPI             BFNegate( float_handle c1 )
/*********************************************************/
{
     CFNegate( (cfloat *) c1 );
}

extern  float_handle _CGAPI     BFTrunc( float_handle c1 )
/********************************************************/
{
     return( CFTrunc( (cfloat *) c1 ) );
}

extern  float_handle _CGAPI     BFCopy( float_handle c1 )
/*******************************************************/
{
     return( CFCopy( (cfloat *) c1 ) );
}

extern  int _CGAPI              BFSign( float_handle c1 )
/*******************************************************/
{
     return( CFTest((cfloat *) c1) );
}

extern  float_handle _CGAPI     BFCnvIF( int data )
/*************************************************/
{
    return( CFCnvIF( data ) );
}

extern  float_handle _CGAPI     BFCnvUF( uint data )
/**************************************************/
{
    return( CFCnvUF( data ) );
}

extern  signed_32 _CGAPI       BFCnvF32( float_handle f )
/*******************************************************/
{
    return( CFCnvF32( (cfloat*) f ) );
}

extern  int _CGAPI              BFCmp( float_handle l, float_handle r )
/*********************************************************************/
{
    return( CFCompare( (cfloat *)l, (cfloat *)r ) );
}

extern  void _CGAPI             BFFree( float_handle cf )
/*******************************************************/
{
    CFFree( (cfloat *)cf );
}

extern  pointer _CGAPI CGSafeRecurse( pointer rtn, pointer arg )
/**************************************************************/
{
    return( SafeRecurse( (pointer(*)(pointer))rtn, arg ) );
}

extern  void _CGAPI     DBSrcCue( uint fno, uint line, uint col );
extern  void _CGAPI     DBBegBlock( void );
extern  void _CGAPI     DBEndBlock( void );

#ifndef _CGDLL
extern  int _CGAPI              BEDLLLoad( char *name )
/*****************************************************/
{
    return( TRUE );
}

extern  void _CGAPI             BEDLLUnload() {
/*********************************************/
}
#endif
