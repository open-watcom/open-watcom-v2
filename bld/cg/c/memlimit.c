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

#include "cg.h"
#include "cgaux.h"
#include "bckdef.h"
#include "model.h"
#include "hostsys.h"
#include "targsys.h"
#include "ptrint.h"
#include "feprotos.h"
#include "cfloat.h"

static    pointer_int   MemLimit;
static    bool          IckyWicky;

extern    int           InOptimizer;
extern    pointer       MemStart;
extern    pointer       MemFinish;
extern    pointer_int   FrlSize;

extern  bool            InsFrlFree();
extern  bool            InstrFrlFree();
extern  bool            NameFrlFree();
extern  bool            ConfFrlFree();
extern  bool            RegTreeFrlFree();
extern  bool            ScoreFrlFree();
extern  bool            RegTreeFrlFree();
extern  bool            AddrFrlFree();
extern  bool            SchedFrlFree();
extern  bool            ShrinkQueue(pointer_int);
extern  seg_id          SetOP(seg_id);
extern  seg_id          AskCodeSeg();
extern  bool            HaveCodeSeg();
extern  pointer_int     MemInUse();
extern  pointer_int     MemSize();
extern  bool            TreeFrlFree();
extern  bool            FreeObjCache();
#ifdef __DOS__
extern  void            MemAddBlock(pointer);
#endif


static  bool    FlushSomeOpt( pointer_int size ) {
/************************************************/

    seg_id      old;
    bool        freed;

    if( InOptimizer == 0 && HaveCodeSeg() ) {
        old = SetOP( AskCodeSeg() );
        freed = ShrinkQueue( size );
        SetOP( old );
        if( _IsntModel( NO_OPTIMIZATION ) && IckyWicky == FALSE ) {
            IckyWicky = TRUE;
            FEMessage( MSG_PEEPHOLE_FLUSHED, NULL );
        }
    } else {
        freed = FALSE;
    }
    return( freed );
}


static  bool    ChkMemLimit( pointer_int limit ) {
/************************************************/

    if( _IsModel( MEMORY_LOW_FAILS ) ) return( FALSE );
    if( MemInUse() - FrlSize <= limit ) return( FALSE );
    FlushSomeOpt( MemInUse() - limit - FrlSize );
    if( MemInUse() - FrlSize <= limit ) return( FALSE );
    return( TRUE );
}


extern  void    CalcMemLimit() {
/******************************/

    pointer_int size;
#ifdef __DOS__
    pointer     extra;

    FrlSize = 0;
    extra = FEAuxInfo( NULL, FREE_SEGMENT );
    while( extra != NULL ) {
        MemAddBlock( extra );
        extra = FEAuxInfo( extra, FREE_SEGMENT );
    }
#endif
    size = MemSize();
    MemLimit = size - size / 4;
    IckyWicky = FALSE;
}


extern  void    FlushOpt() {
/**************************/

    FlushSomeOpt( -1 );
}


extern  bool    MemCritical() {
/*****************************/

    return( ChkMemLimit( MemLimit - MemLimit/4 ) );
}


extern  bool    MemLow() {
/************************/

    return( ChkMemLimit( MemLimit ) );
}


extern  void    BlowAwayFreeLists() {
/***********************************/

    AddrFrlFree();
    ScoreFrlFree();
    TreeFrlFree();
    RegTreeFrlFree();
    InsFrlFree();
    InstrFrlFree();
    CFFrlFree();
    ConfFrlFree();
    NameFrlFree();
    SchedFrlFree();
}


extern  bool    MemCheck( int size ) {
/************************************/

    if( FEMoreMem( size ) ) return( TRUE );
    if( FreeObjCache() ) return( TRUE );
    if( ScoreFrlFree() ) return( TRUE );
    if( AddrFrlFree() ) return( TRUE );
    if( RegTreeFrlFree() ) return( TRUE );
    if( CFFrlFree() ) return( TRUE );
    if( InsFrlFree() ) return( TRUE );
    if( TreeFrlFree() ) return( TRUE );
    if( ConfFrlFree() ) return( TRUE );
    if( NameFrlFree() ) return( TRUE );
    if( InstrFrlFree() ) return( TRUE );
    if( SchedFrlFree() ) return( TRUE );
    if( FlushSomeOpt( size ) ) return( TRUE );
    return( FALSE );
}
