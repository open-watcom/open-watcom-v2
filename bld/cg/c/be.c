/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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


// (jww) 93/12/13 -- add initialization of type aliases

#include "_cgstd.h"
#include <stdarg.h>
#include "_cg.h"
#include "bckdef.h"
#include "typclass.h"
#include "types.h"
#include "_cfloat.h"
#include "cgaux.h"
#include "model.h"
#include "cgstub.h"
#include "cgmem.h"
#include "utils.h"
#include "stubdata.h"
#include "bckptr.h"
#include "feprotos.h"


extern  int             TempId;
extern  uint_16         TypeIdx;

extern  char            *ACopyOf(const char *);
extern  char            *Tipe(cg_type );
extern  char            *Label(l *);
extern  void            Action(char *,... );
extern  void            Code(char *,... );
extern  segment_id      SetFile(segment_id );
extern  void            CGError(const char *,... );
extern  void            VerBack(b *);
extern  int             FCreate(char *);
extern  void            FShut(int);
extern  int             FStdOut();
extern  char            *EnvVar(char *);

extern  void            DumpTree(pointer);
extern  void            exit(int);
extern  void            VDefLabel(l *);
extern  void            InitDbgInfo();
extern  void            FiniDbgInfo();


extern  cg_init_info    BEInit( cg_switches cg_data, cg_target_switches tg_data,
                                uint opt_size, proc_revision proc ) {
//===================================================================

    cg_init_info        cg_info;
    int                 i;

    Model = cg_data;
    if( EnvVar( "ACTION_STDOUT" ) ) {
        Actions = FStdOut();
    } else {
        Actions = FCreate( "ACTIONS" );
    }
    ++FilesOpen;
    Action( "BEInit switches==" );
    Action( "%h %h", cg_data, tg_data );
    Action( ", opt_size==%d, proc_rev=%h%n", opt_size, proc );
    SymDebug = 0;
    TypDebug = 0;
    if( cg_data & CGSW_GEN_DBG_LOCALS ) {
        SymDebug = FCreate( "DBGSYM" );
        ++FilesOpen;
    }
    if( cg_data & CGSW_GEN_DBG_TYPES ) {
        TypDebug = FCreate( "DBGTYP" );
        ++FilesOpen;
    }
    TypeIdx = 0;
    InitDbgInfo();
    TypeInit();
    TypeDef( TY_DEFAULT, 0 );
    Inlines = NULL;
    NodeList = NULL;
    AutoList = NULL;
    LblList = NULL;
    BackList = NULL;
    TempList = NULL;
    StaticList = NULL;
    LabelId = 0;
    NodeId = 0;
    BackId = 0;
    TempId = 0;
    SelId = 0;
    SegDefs = NULL;
    CGState = S_INIT;
    InProc = NULL;
    FilesOpen = 0;
    for( i = 0; i < MAX_SEG; ++i ) {
        Locs[i] = 0;
        Files[i].hdl = 0;
        Files[i].name = NULL;
        Files[i].exists = false;
        SegOk[i] = false;
    }
    CurSeg = 0;
    Out = Files[CurSeg].hdl;
    CodeSeg = -1;
    TargTypeInit();
    Action( "BEInit%n" );
    cg_info.revision = II_REVISION;
#if _TARGET & _TARG_8086
    cg_info.target = II_TARG_8086;
#elif _TARGET & _TARG_80386
    cg_info.target = II_TARG_80386;
#elif _TARGET & _TARG_AXP
    cg_info.target = II_TARG_AXP;
#elif _TARGET & _TARG_PPC
    cg_info.target = II_TARG_PPC;
#elif _TARGET & _TARG_MIPS
    cg_info.target = II_TARG_MIPS;
#else
    #error UNKNOWN TARGET
#endif
    return( cg_info );
}
extern  void    BECloseFiles()
{
    int i;

    if( Actions != FStdOut() ) {
        FShut( Actions );
    }
    if( SymDebug != 0 ) {
        FShut( SymDebug );
    }
    if( TypDebug != 0 ) {
        FShut( TypDebug );
    }
    for( i = 0; i < MAX_SEG; ++i ) {
        if( Files[i].hdl != 0 ) {
            FShut( Files[i].hdl );
        }
    }
}
extern  void    BEFini() {
//========================

    b   *bk;
    l   *lb;

    for( bk = BackList; bk != NULL; bk = bk->n ) {
        Action("UnFreed back handle for %s%n", FEName( bk->s ) );
    }
    if( BackList != NULL ) {
        CGError("UnFreed back handles!" );
    }
    for( lb = LblList; lb != NULL; lb = lb->n ) {
        VDefLabel( lb );
    }
    Action( "BEFini%n" );
    FiniDbgInfo();
    TypeFini();
    BECloseFiles();
}
extern  l       *NewLabel() {
//===========================

    l   *nl;
    nl = CGAlloc( sizeof( l ) );
    nl->n = LblList;
    nl->ddef = 0;
    nl->dref = 0;
    nl->cref = NULL;
    nl->cdef = NULL;
    LblList = nl;
    nl->i = ++LabelId;
    return(nl);
}
extern  l       *BENewLabel() {
//=============================

    l   *nl;
    nl = NewLabel();
    Action( "BENewLabel() -> %s%n", Label(nl) );
    return(nl);
}
extern  void    BEFiniLabel(l *lb) {
//==================================

    l   **o;
    Action( "BEFiniLabel" );
    Action( "( %s )%n", Label(lb) );
    if( InProc ) {
        o = &LblList;
        while( *o != lb ) {
            o = (l**)*o;
        }
        *o = lb->n;
        VDefLabel( lb );
        CGFree( lb );
    } else {
        CGError( "BEFiniLabel must be called between CGProcDecl and CGReturn%n" );
    }
}
extern  void    BEDefType( cg_type t, uint algn, uint_32 l )
//==========================================================
{
    algn=algn;

    Action( "BEDefType" );
    TypeDef( t,l );
    Action( "( %s, %d, %l )%n", Tipe(t), algn, l );
}
extern  void    BEAliasType( cg_type t1, cg_type t2 ) {
//=====================================================

    Action( "BEAliasType" );
    TypeAlias(t1,t2);
    Action( "( %s, %s )%n", Tipe(t1), Tipe(t2) );
}
extern uint_32  BETypeLength( cg_type t ) {
//=========================================

    Action( "BETypeLength" );
    Action( "( %s ) -> %l%n", Tipe(t), TypeAddress( t )->length );
    return( TypeAddress( t )->length );
}
extern  uint    BETypeAlign( cg_type t ) {
//========================================
    t=t;
    Action( "BETypeAlign" );
    CGError( "Not yet implemented" );
    return( 1 );
}
extern  void    *BEPatch() {
//==========================
    Action( "BEPatch()%n" );
    return( NULL );
}
extern  void    BEPatchInteger( void *hdl, int_32 val ) {
//=====================================================
    Action( "BEPatchInteger( %p, %l )%n", hdl, val );
}
extern  void    BEFiniPatch( void *hdl ) {
//========================================
    Action( "BEFiniPatch( %p )%n", hdl );
}

static  pointer                 NewBackReturn = NULL;

extern  pointer LkAddBack( sym_handle sym, pointer curr_back ) {
/**************************************************************/

    b   *bk;

    NewBackReturn = TO_FAKE_BACK( curr_back );
    bk = FEBack( sym );
    NewBackReturn = NULL;
    return( TO_REAL_BACK( bk ) );
}

extern  b       *BENewBack(sym s) {
//=================================

    b   *bk;

    Action( "BENewBack" );
    if( NewBackReturn == FAKE_NULL ) {
        bk = NULL;
    } else if( NewBackReturn != NULL ) {
        bk = NewBackReturn;
    } else {
        bk = CGAlloc( sizeof( b  ));
        bk->n = BackList;
        bk->s = s;
        bk->loc = -1;
        bk->i = ++BackId;
        bk->lp = NewLabel();
        BackList = bk;
        if( !IS_REAL_BACK( bk ) ) {
            CGError( "Internal error - odd memory" );
        }
    }
    Action( "( %s ) -> %p%n", FEName( s ), bk );
    return(bk);
}
extern  void    BEFiniBack( b *bk ) {
//===================================

    Action( "BEFiniBack" );
    if( !IS_REAL_BACK( bk ) ) {
        Action( "( %s )%n", FEName( TO_REAL_BACK( bk )->s ) );
        return;
    }
    VerBack(bk);
    Action( "( %s [ %s ] )%n", FEName( bk->s ), Label( bk->lp ) );
}
extern  void    BEFreeBack( b *bk ) {
//===================================

    b   **o;
    Action( "BEFreeBack" );
    if( !IS_REAL_BACK( bk ) ) {
        Action( "( %s )%n", FEName( TO_REAL_BACK( bk )->s ) );
        return;
    }
    VerBack(bk);
    o = &BackList;
    while( *o != bk ) {
        o = (b**)*o;
    }
    *o = bk->n;
    Action( "( %s [ %s ] )%n", FEName( bk->s ), Label( bk->lp ) );
    CGFree( bk );
}

extern  void    BEStart() {
//=========================


    Action( "BEStart()%n" );
    if( CodeSeg != -1 ) {
        CurSeg = CodeSeg;
    }
}
extern  void    BEStop() {
//========================

    Action( "BEStop()%n" );
}
extern  void    BEAbort() {
//=========================

    Action( "BEAbort()%n" );
    CGError( "*****ABORT******%n" );
    Code(   "*****ABORT******%n" );
    Action( "*****ABORT******%n" );
    BEFini();
    exit( 2010 );
}
extern  segment_id      BESetSeg( segment_id segid ) {
//==================================================

    segment_id  old_segid;

    Action( "BESetSeg( %d )", segid );
    old_segid = SetFile( segid );
    Action( " -> %d%n", old_segid );
    return( old_segid );
}
extern  void    BEFlushSeg( segment_id segid ) {
//============================================

    Action( "BEFlushSeg( %d )%n", segid );
    if( segid == CodeSeg ) {
        CodeSeg = -1;
    }
    if( Files[segid].hdl != 0 ) {
        FShut( Files[segid].hdl );
    }
    Files[segid].hdl = 0;
    Files[segid].exists = false;
    SegOk[segid] = false;
    StaticList = NULL;
}

extern  void    BEDefSeg( segment_id segid, seg_attr attr, const char *str, uint algn ) {
//====================================================================================

    segdef      *new;

    Action( "BEDefSeg( %d, %h, %s, %d )%n", segid, attr, str, algn );
    new = CGAlloc( sizeof( segdef ) );
    new->next = SegDefs;
    SegDefs = new;
    new->id = segid;
    new->attr = attr;
    new->str = ACopyOf( str );
    if( attr & EXEC ) {
        CodeSeg = segid;
        CurSeg = segid;
    }
    if( SegOk[segid] ) {
        if( Files[segid].hdl != 0 ) {
            FShut( Files[segid].hdl );
        }
        CGError( "BEDefSeg called twice (id %d) with no intervening BEFlushSeg%n", segid );
    }
    Files[segid].name = new->str;
    Files[segid].hdl = 0;
    SegOk[segid] = true;
    Locs[segid] = 0;
}
extern  uint_32 BEUnrollCount( uint_32 c ) {
/**************************************************/
    return( c );
}
