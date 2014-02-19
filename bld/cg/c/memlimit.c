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
* Description:  Routines to deal with low memory conditions.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "targsys.h"
#include "data.h"
#include "cfloat.h"
#include "objout.h"
#include "feprotos.h"

static    pointer_int   MemLimit;
static    bool          IckyWicky;

extern    pointer       MemStart;
extern    pointer       MemFinish;

extern  bool            InsFrlFree( void );
extern  bool            InstrFrlFree( void );
extern  bool            NameFrlFree( void );
extern  bool            ConfFrlFree( void );
extern  bool            RegTreeFrlFree( void );
extern  bool            ScoreFrlFree( void );
extern  bool            RegTreeFrlFree( void );
extern  bool            AddrFrlFree( void );
extern  bool            SchedFrlFree( void );
extern  bool            ShrinkQueue( pointer_int );
extern  pointer_int     MemInUse( void );
extern  pointer_int     MemSize( void );
extern  bool            TreeFrlFree( void );
extern  bool            FreeObjCache( void );


static  bool    FlushSomeOpt( pointer_int size )
/**********************************************/
{
    segment_id  old;
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


static  bool    ChkMemLimit( pointer_int limit )
/**********************************************/
{
    if( _IsModel( MEMORY_LOW_FAILS ) ) return( FALSE );
    if( MemInUse() - FrlSize <= limit ) return( FALSE );
    FlushSomeOpt( MemInUse() - limit - FrlSize );
    if( MemInUse() - FrlSize <= limit ) return( FALSE );
    return( TRUE );
}


extern  void    CalcMemLimit( void )
/**********************************/
{
    pointer_int size;
    size = MemSize();
    MemLimit = size - size / 4;
    IckyWicky = FALSE;
}


extern  void    FlushOpt( void )
/******************************/
{
    FlushSomeOpt( (pointer_int)-1 );
}


extern  bool    MemCritical( void )
/*********************************/
{
    return( ChkMemLimit( MemLimit - MemLimit/4 ) );
}


extern  bool    MemLow( void )
/****************************/
{
    return( ChkMemLimit( MemLimit ) );
}


extern  void    BlowAwayFreeLists( void )
/***************************************/
{
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


extern  bool    MemCheck( size_t size )
/*************************************/
{
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
