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


#include <windows.h>
#include "penwin.h"
#include "penwoem.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include "winext.h"
#include "stubs.h"

#pragma aux BackPatch_penwin parm [ax];
extern LPVOID FAR BackPatch_penwin( char *str );

static UINT (FAR PASCAL *penwinEnumSymbols)(LPSYG, WORD, ENUMPROC, LPVOID);
static BOOL (FAR PASCAL *penwinExecuteGesture)(HWND, SYV, LPRCRESULT);
static VOID (FAR PASCAL *penwinFirstSymbolFromGraph)(LPSYG, LPSYV, int, int FAR *);
static int  (FAR PASCAL *penwinGetSymbolCount)(LPSYG);
static int  (FAR PASCAL *penwinGetSymbolMaxLength)(LPSYG);
static BOOL (FAR PASCAL *penwinTrainContext)(LPRCRESULT, LPSYE, int, LPSYC, int);

/*
 * GetSYGAlias - alias pointer fields in an SYG structure
 */
static void GetSYGAlias( LPSYG orig, SYG _FAR *save )
{
    LPVOID      pdata1,pdata2;

    pdata1 = save->lpsye = orig->lpsye;
    pdata2 = save->lpsyc = orig->lpsyc;
    GetAlias( &pdata1 );
    GetAlias( &pdata2 );
    orig->lpsye = pdata1;
    orig->lpsyc = pdata2;

} /* GetSYGAlias */

/*
 * ReleaseSYGAlias - release aliased fields in an SYG structure
 */
static void ReleaseSYGAlias( LPSYG orig, SYG _FAR *save )
{
    ReleaseAlias( save->lpsye, orig->lpsye );
    ReleaseAlias( save->lpsyc, orig->lpsyc );
    orig->lpsye = save->lpsye;
    orig->lpsyc = save->lpsyc;

} /* ReleaseSYGAlias */

/*
 * GetRCRESULTAlias - alias pointer fields in an RCRESULT structure
 */
static void GetRCRESULTAlias( LPRCRESULT orig, RCRESULT _FAR *save )
{
    LPVOID      pdata1,pdata2;

    pdata1 = save->lpsyv = orig->lpsyv;
    pdata2 = save->lprc = orig->lprc;
    GetAlias( &pdata1 );
    GetAlias( &pdata2 );
    orig->lpsyv = pdata1;
    orig->lprc = pdata2;
    GetSYGAlias( &orig->syg, &save->syg );

} /* GetRCRESULTAlias */

/*
 * ReleaseRCRESULTAlias - release aliased fields in an RCRESULT structure
 */
static void ReleaseRCRESULTAlias( LPRCRESULT orig, RCRESULT _FAR *save )
{
    ReleaseAlias( orig->lpsyv, save->lpsyv );
    ReleaseAlias( orig->lprc, save->lprc );
    ReleaseSYGAlias( &orig->syg, &save->syg );
    orig->lpsyv = save->lpsyv;
    orig->lprc = save->lprc;

} /* ReleaseRCRESULTAlias */

/*
 * __EnumSymbols - cover for penwin function EnumSymbols
 */
UINT FAR PASCAL __EnumSymbols(LPSYG lpsyg, WORD max, ENUMPROC proc, LPVOID d)
{
    UINT        rc;
    SYG         syg;

    if( penwinEnumSymbols == NULL ) {
        penwinEnumSymbols = BackPatch_penwin( "EnumSymbols" );
        if( penwinEnumSymbols == NULL ) return( 0 );
    }

    GetSYGAlias( lpsyg, &syg );
    rc = penwinEnumSymbols( lpsyg, max, proc, d );
    ReleaseSYGAlias( lpsyg, &syg );

    return( rc );

} /* __EnumSymbols */

/*
 * __ExecuteGesture - cover for penwin function ExecuteGesture
 */
BOOL FAR PASCAL __ExecuteGesture(HWND hwnd, SYV syv, LPRCRESULT lprcresult )
{
    BOOL        rc;
    RCRESULT    rcr;

    if( penwinExecuteGesture == NULL ) {
        penwinExecuteGesture = BackPatch_penwin( "ExecuteGesture" );
        if( penwinExecuteGesture == NULL ) return( 0 );
    }

    GetRCRESULTAlias( lprcresult, &rcr );
    rc = penwinExecuteGesture( hwnd, syv, lprcresult );
    ReleaseRCRESULTAlias( lprcresult, &rcr );

    return( rc );

} /* __ExecuteGesture */

/*
 * __FirstSymbolFromGraph - cover for penwin function FirstSymbolFromGraph
 */
VOID FAR PASCAL __FirstSymbolFromGraph(LPSYG lpsyg, LPSYV lpsyv, int max,
                        int FAR *lpcsyv )
{
    SYG syg;

    if( penwinFirstSymbolFromGraph == NULL ) {
        penwinFirstSymbolFromGraph = BackPatch_penwin( "FirstSymbolFromGraph" );
        if( penwinFirstSymbolFromGraph == NULL ) return;
    }

    GetSYGAlias( lpsyg, &syg );
    penwinFirstSymbolFromGraph( lpsyg, lpsyv, max, lpcsyv );
    ReleaseSYGAlias( lpsyg, &syg );

} /* __FirstSymbolFromGraph */

/*
 * __GetSymbolCount - cover for penwin function GetSymbolCount
 */
int FAR PASCAL __GetSymbolCount( LPSYG lpsyg )
{
    SYG syg;
    int rc;

    if( penwinGetSymbolCount == NULL ) {
        penwinGetSymbolCount = BackPatch_penwin( "GetSymbolCount" );
        if( penwinGetSymbolCount == NULL ) return( 0 );
    }

    GetSYGAlias( lpsyg, &syg );
    rc = penwinGetSymbolCount( lpsyg );
    ReleaseSYGAlias( lpsyg, &syg );

    return( rc );

} /* __GetSymbolCount */

/*
 * __GetSymbolMaxLength - cover for penwin function GetSymbolMaxLength
 */
int FAR PASCAL __GetSymbolMaxLength( LPSYG lpsyg )
{
    int rc;
    SYG syg;

    if( penwinGetSymbolMaxLength == NULL ) {
        penwinGetSymbolMaxLength = BackPatch_penwin( "GetSymbolMaxLength" );
        if( penwinGetSymbolMaxLength == NULL ) return( 0 );
    }

    GetSYGAlias( lpsyg, &syg );
    rc = penwinGetSymbolMaxLength( lpsyg );
    ReleaseSYGAlias( lpsyg, &syg );

    return( rc );

} /* __GetSymbolMaxLength */

/*
 * __TrainContext - cover for penwin function TrainContext
 */
BOOL FAR PASCAL __TrainContext( LPRCRESULT lprcresult, LPSYE lpsye, int csye,
                LPSYC lpsyc, int csyc )
{
    BOOL        rc;
    RCRESULT    rcr;

    if( penwinTrainContext == NULL ) {
        penwinTrainContext = BackPatch_penwin( "TrainContext" );
        if( penwinTrainContext == NULL ) return( 0 );
    }

    GetRCRESULTAlias( lprcresult, &rcr );
    rc = penwinTrainContext( lprcresult, lpsye, csye, lpsyc, csyc );
    ReleaseRCRESULTAlias( lprcresult, &rcr );

    return( rc );

} /* __TrainContext */
