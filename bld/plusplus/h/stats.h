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


#ifndef __STATS_H__
#define __STATS_H__

// STATS.H -- statistical information
//
// 91/06/04 -- J.W.Welch        -- defined

#if ! defined( global )
    #define global extern
#endif

                                // DATA
global unsigned SrcLineCount;   // - # of lines in primary source file
global unsigned IncLineCount;   // - # of lines in all included files
global unsigned ErrCount;       // - total # of errors encountered
global unsigned WngCount;       // - total # of warnings encountered


#ifdef XTRA_RPT

#define ExtraRptCtr( ctr ) static int ctr;
#define ExtraRptSpace( ctr ) static int ctr;
#define ExtraRptAddtoCtr( tgt, val ) ((tgt) += (val))
#define ExtraRptDecrementCtr( ctr ) (--(ctr))
#define ExtraRptIncrementCtr( ctr ) (++(ctr))
#define ExtraRptHighWater( ctr, hiwater ) if( hiwater < ctr ) hiwater = ctr;
#define ExtraRptZeroCtr( ctr ) ((ctr) = 0)
#define ExtraRptZeroSpace( ctr ) ((ctr) = 0)
#define ExtraRptRegisterSpace( a, b ) ExtraRptRegisterCtr( a, b )
#define ExtraRptSpaceAdd( ctr, size ) ((ctr) += (size))
#define ExtraRptSpaceSubtract( ctr, size ) ((ctr) -= (size))
#define ExtraRptMaximum( ctr, mx ) (((mx) = ((ctr)>(mx)) ? (ctr):(mx)),(ctr) = 0)
#define ExtraRptTabIncr( table, r, c ) ++ table[r][c];
#define ExtraRptTabAdd( table, r, c, a ) table[r][c] += (a);
#define ExtraRptTabSub( table, r, c, a ) table[r][c] -= (a);
#define ExtraRptTable( table, r, c ) static int table[r][c];

void ExtraRptRegisterAvg(       // REGISTER AVERAGING
    int *a_total,               // - addr[ total ]
    int *a_count,               // - addr[ count ]
    const char *rpt_line )      // - report line
;
void ExtraRptRegisterCtr(       // REGISTER A COUNTER
    int *a_ctr,                 // - addr( counter )
    const char *rpt_line )      // - report line
;
void ExtraRptRegisterMax(       // REGISTER A MAXIMUM
    int *a_ctr,                 // - addr( counter )
    const char *rpt_line )      // - report line
;
void ExtraRptRegisterTab(       // REGISTER TABLE
    char const *title,          // - title
    char const * const *row_labels, //- row labels
    int *table,                 // - table
    unsigned rows,              // - # rows
    unsigned cols )             // - # columns
;
void ExtraRptSymDefn(           // REPORT SYMBOL DEFINITION IN PRIMARY SOURCE
    SYMBOL sym )
;
void ExtraRptSymUsage(          // REPORT SYMBOL USAGE FROM PRIMARY SOURCE
    SYMBOL sym )
;

#else

#define ExtraRptRegisterAvg( a, b, c ) ;
#define ExtraRptRegisterCtr( a, b ) ;
#define ExtraRptRegisterMax( a, b ) ;
#define ExtraRptRegisterSpace( a, b ) ;
#define ExtraRptRegisterTab( a, b, c, d, e );
#define ExtraRptHighWater( a, b ) ;
#define ExtraRptAddtoCtr( a, b ) ;
#define ExtraRptDecrementCtr( a ) ;
#define ExtraRptIncrementCtr( a ) ;
#define ExtraRptCtr( a ) ;
#define ExtraRptSpace( a ) ;
#define ExtraRptZeroCtr( a ) ;
#define ExtraRptZeroSpace( a ) ;
#define ExtraRptSpaceAdd( a, b ) ;
#define ExtraRptSpaceSubtract( a, b ) ;
#define ExtraRptMaximum( a, b ) ;
#define ExtraRptTabIncr( table, r, c )
#define ExtraRptTable( table, r, c )
#define ExtraRptSymDefn( s )
#define ExtraRptSymUsage( s )
#define ExtraRptTabAdd( table, r, c, a );
#define ExtraRptTabSub( table, r, c, a ) ;

#endif

#endif
