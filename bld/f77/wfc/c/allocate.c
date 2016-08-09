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


//
// ALLOCATE     : ALLOCATE/DEALLOCATE statement processor
//

#include "ftnstd.h"
#include "global.h"
#include "errcod.h"
#include "namecod.h"
#include "opr.h"
#include "falloc.h"
#include "recog.h"
#include "insert.h"
#include "utility.h"
#include "proctbl.h"
#include "allocate.h"
#include "ioprockw.h"
#include "symtab.h"
#include "upscan.h"
#include "galloc.h"
#include "gsublist.h"


/* forward declarations */
static  void    AllocStat( void );
static  void    AllocLoc( void );
static  void    ChkStat( void );
static  void    ChkLoc( void );
static  void    DeallocStat( void );


static  char            *StatKW = { "STAT" };
static  char            *LocKW = { "LOCATION" };


void    CpAllocate( void )
{
// Process ALLOCATE statement.
//      ALLOCATE( arr([l:]u,...),...,[STAT=istat])
//          or
//      ALLOCATE( arr([l:]u,...),...,LOCATION=loc)
//          or
//      ALLOCATE( arr([l:]u,...),...,LOCATION=loc, [STAT=istat])

    sym_id      sym;

    StmtExtension( SP_STRUCTURED_EXT );
    if( RecTrmOpr() && RecNOpn() ) {
        AdvanceITPtr();
    }
    ReqOpenParen();
    GBegAllocate();
    for(;;) {
        if( ReqName( NAME_ARRAY ) ) {
            sym = LkSym();
            if( (sym->u.ns.flags & SY_CLASS) == SY_VARIABLE ) {
                if( (sym->u.ns.flags & SY_SUBSCRIPTED) && _Allocatable( sym ) &&
                    !( (sym->u.ns.u1.s.typ == FT_CHAR) && (sym->u.ns.xt.size == 0) ) ) {
                    AdvanceITPtr();
                    ReqOpenParen();
                    sym->u.ns.u1.s.xflags |= SY_DEFINED;
                    GAllocate( sym );
                } else if( (sym->u.ns.u1.s.typ == FT_CHAR) && (sym->u.ns.xt.size == 0)
                  && (sym->u.ns.flags & SY_SUBSCRIPTED) == 0 ) {
                    AdvanceITPtr();
                    ReqMul();
                    sym->u.ns.u1.s.xflags |= SY_ALLOCATABLE | SY_DEFINED;
                    GAllocateString( sym );
                } else {
                    IllName( sym );
                }
            }
        }
        AdvanceITPtr();
        if( !RecComma() ) {
            GAllocEOL();
            break;
        }
        if( RecNextOpr( OPR_EQU ) ) {
            GAllocEOL();
            if( RecKeyWord( LocKW ) ) {
                AllocLoc();
            }
            if( RecKeyWord( StatKW ) ) {
                AllocStat();
            }
            break;
        }
    }
    GEndAllocate();
    ReqCloseParen();
    ReqNOpn();
    AdvanceITPtr();
    ReqEOS();
}


static  void    AllocStat( void )
{
    ChkStat();
}


static  void    AllocLoc( void )
{
    ChkLoc();
}


void    DimArray( sym_id sym )
{
// Dimension an allocatable array.
// Called by GAllocate() so that system dependent code
// can control the order in which code gets generated.

    uint                subs;

    subs = 0;
    for(;;) {
        subs++;
        IntegerExpr();
        // Consider:
        //
        //      SUBROUTINE SAM
        //      DIMENSION A(:)
        //      ALLOCATE( A(10) )
        //      DEALLOCATE( A )
        //      ALLOCATE( A(2:20) )
        //      DEALLOCATE( A )
        //      ALLOCATE( A(10) )
        //      ...
        //
        // For the third ALLOCATE statement we must fill the low bound
        // with 1 (the 2nd ALLOCATE set the low bound to 2).  In general,
        // the first ALLOCATE must also set the low bound to 1 (consider
        // if SAM is called more than once).
        if( RecNextOpr( OPR_COL ) ) {
            if( !AError ) {
                GSLoBound( subs, sym );
            }
            AdvanceITPtr();
            IntegerExpr();
            if( !AError ) {
                GSHiBound( subs, sym );
            }
        } else {
            if( !AError ) {
                GSHiBoundLo1( subs, sym );
            }
        }
        AdvanceITPtr();
        if( !RecComma() ||
            ( subs == _DimCount( sym->u.ns.si.va.u.dim_ext->dim_flags ) ) ) break;
    }
    ReqCloseParen();
    ReqNOpn();
}


void    LoadSCB( sym_id sym )
{
// Dimension an allocatable character string
// Called by GAllocateString() so that system dependent code
// can control the order in which code gets generated.

    IntegerExpr();
    if( !AError ) {
        GSCBLength( sym );
    }
}


void    CpDeAllocate( void )
{
// Process DEALLOCATE statement.
//      DEALLOCATE( arr,...,[STAT=istat])

    sym_id      sym;

    StmtExtension( SP_STRUCTURED_EXT );
    if( RecTrmOpr() && RecNOpn() ) {
        AdvanceITPtr();
    }
    ReqOpenParen();
    GBegDeAllocate();
    for(;;) {
        if( ReqName( NAME_ARRAY ) ) {
            sym = LkSym();
            if( (sym->u.ns.flags & SY_CLASS) == SY_VARIABLE ) {
                if( (sym->u.ns.flags & SY_SUBSCRIPTED) && _Allocatable( sym ) ) {
                    GDeAllocate( sym );
                } else if( (sym->u.ns.u1.s.typ == FT_CHAR) && (sym->u.ns.xt.size == 0) ) {
                    sym->u.ns.u1.s.xflags |= SY_ALLOCATABLE;
                    GDeAllocateString( sym );
                } else {
                    IllName( sym );
                }
            }
        }
        AdvanceITPtr();
        if( !RecComma() ) {
            GAllocEOL();
            break;
        }
        if( RecKeyWord( StatKW ) && RecNextOpr( OPR_EQU ) ) {
            GAllocEOL();
            DeallocStat();
            break;
        }
    }
    GEndDeAllocate();
    ReqCloseParen();
    ReqNOpn();
    AdvanceITPtr();
    ReqEOS();
}


static  void    DeallocStat( void )
{
    ChkStat();
}


static  void    ChkStat( void )
{
    AdvanceITPtr();
    IntSubExpr();
    if( !AError ) {
        CkSize4();
        CkAssignOk();
        GAllocStat();
    }
    AdvanceITPtr();
}


static  void    ChkLoc( void )
{
    AdvanceITPtr();
    IntSubExpr();
    if( !AError ) {
        GAllocLoc();
    }
    AdvanceITPtr();
}
