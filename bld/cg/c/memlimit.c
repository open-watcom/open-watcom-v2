/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


#include "_cgstd.h"
#include "coderep.h"
#include "data.h"
#include "cfloat.h"
#include "objout.h"
#include "memcheck.h"
#include "memlimit.h"
#include "optmain.h"
#include "namelist.h"
#include "optmkins.h"
#include "makeins.h"
#include "conflict.h"
#include "inssched.h"
#include "memsydep.h"
#include "regtree.h"
#include "score.h"
#include "makeaddr.h"
#include "tree.h"
#include "treeprot.h"
#include "feprotos.h"


static    pointer_uint  MemLimit;
static    bool          IckyWicky;

static  bool    FlushSomeOpt( pointer_uint size )
/***********************************************/
{
    segment_id  old_segid;
    bool        freed;

    if( InOptimizer == 0 && HaveCodeSeg() ) {
        old_segid = SetOP( AskCodeSeg() );
        freed = ShrinkQueue( size );
        SetOP( old_segid );
        if( _IsntModel( NO_OPTIMIZATION ) && !IckyWicky ) {
            IckyWicky = true;
            FEMessage( MSG_PEEPHOLE_FLUSHED, NULL );
        }
    } else {
        freed = false;
    }
    return( freed );
}


static  bool    ChkMemLimit( pointer_uint limit )
/***********************************************/
{
    if( _IsModel( MEMORY_LOW_FAILS ) )
        return( false );
    if( MemInUse() - FrlSize <= limit )
        return( false );
    FlushSomeOpt( MemInUse() - limit - FrlSize );
    if( MemInUse() - FrlSize <= limit )
        return( false );
    return( true );
}


void    CalcMemLimit( void )
/**************************/
{
    pointer_uint    size;

    size = MemSize();
    MemLimit = size - size / 4;
    IckyWicky = false;
}


void    FlushOpt( void )
/**********************/
{
    FlushSomeOpt( (pointer_uint)-1 );
}


bool    MemCritical( void )
/*************************/
{
    return( ChkMemLimit( MemLimit - MemLimit/4 ) );
}


bool    MemLow( void )
/********************/
{
    return( ChkMemLimit( MemLimit ) );
}


void    BlowAwayFreeLists( void )
/*******************************/
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


bool    MemCheck( size_t size )
/*****************************/
{
    if( FEMoreMem( size ) )
        return( true );
    if( FreeObjCache() )
        return( true );
    if( ScoreFrlFree() )
        return( true );
    if( AddrFrlFree() )
        return( true );
    if( RegTreeFrlFree() )
        return( true );
    if( CFFrlFree() )
        return( true );
    if( InsFrlFree() )
        return( true );
    if( TreeFrlFree() )
        return( true );
    if( ConfFrlFree() )
        return( true );
    if( NameFrlFree() )
        return( true );
    if( InstrFrlFree() )
        return( true );
    if( SchedFrlFree() )
        return( true );
    if( FlushSomeOpt( size ) )
        return( true );
    return( false );
}
