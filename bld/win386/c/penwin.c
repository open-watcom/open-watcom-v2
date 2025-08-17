/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Covers for Pen Windows functions.
*
****************************************************************************/


#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <windows.h>
#include "penwin.h"
#include "penwoem.h"
#include "winext.h"


extern LPVOID FAR BackPatch_penwin( char *str );
#pragma aux BackPatch_penwin __parm [__ax]

static UINT (FAR PASCAL *penwinEnumSymbols)(LPSYG, WORD, ENUMPROC, LPVOID);
static BOOL (FAR PASCAL *penwinExecuteGesture)(HWND, SYV, LPRCRESULT);
static VOID (FAR PASCAL *penwinFirstSymbolFromGraph)(LPSYG, LPSYV, int, int FAR *);
static int  (FAR PASCAL *penwinGetSymbolCount)(LPSYG);
static int  (FAR PASCAL *penwinGetSymbolMaxLength)(LPSYG);
static BOOL (FAR PASCAL *penwinTrainContext)(LPRCRESULT, LPSYE, int, LPSYC, int);

/*
 * __EnumSymbols - cover for penwin function EnumSymbols
 */
UINT FAR PASCAL __EnumSymbols(LPSYG lpsyg, WORD max, ENUMPROC proc, LPVOID d)
{
    UINT            rc;
    DWORD           odata1;
    DWORD           odata2;

    if( penwinEnumSymbols == NULL ) {
        penwinEnumSymbols = BackPatch_penwin( "EnumSymbols" );
        if( penwinEnumSymbols == NULL ) {
            return( 0 );
        }
    }

    odata1 = GETALIAS( &lpsyg->lpsye );
    odata2 = GETALIAS( &lpsyg->lpsyc );
    rc = penwinEnumSymbols( lpsyg, max, proc, d );
    RELEASEALIAS( odata2, &lpsyg->lpsyc );
    RELEASEALIAS( odata1, &lpsyg->lpsye );

    return( rc );

} /* __EnumSymbols */

/*
 * __ExecuteGesture - cover for penwin function ExecuteGesture
 */
BOOL FAR PASCAL __ExecuteGesture(HWND hwnd, SYV syv, LPRCRESULT lprcresult )
{
    BOOL            rc;
    DWORD           odata1;
    DWORD           odata2;
    DWORD           odata3;
    DWORD           odata4;

    if( penwinExecuteGesture == NULL ) {
        penwinExecuteGesture = BackPatch_penwin( "ExecuteGesture" );
        if( penwinExecuteGesture == NULL ) {
            return( 0 );
        }
    }
    odata1 = GETALIAS( &lprcresult->lpsyv );
    odata2 = GETALIAS( &lprcresult->lprc );
    odata3 = GETALIAS( &lprcresult->syg->lpsye );
    odata4 = GETALIAS( &lprcresult->syg->lpsyc );
    rc = penwinExecuteGesture( hwnd, syv, lprcresult );
    RELEASEALIAS( odata4, &lprcresult->syg->lpsyc );
    RELEASEALIAS( odata3, &lprcresult->syg->lpsye );
    RELEASEALIAS( odata2, &lprcresult->lprc );
    RELEASEALIAS( odata1, &lprcresult->lpsyv );

    return( rc );

} /* __ExecuteGesture */

/*
 * __FirstSymbolFromGraph - cover for penwin function FirstSymbolFromGraph
 */
VOID FAR PASCAL __FirstSymbolFromGraph(LPSYG lpsyg, LPSYV lpsyv, int max, int FAR *lpcsyv )
{
    DWORD           odata1;
    DWORD           odata2;

    if( penwinFirstSymbolFromGraph == NULL ) {
        penwinFirstSymbolFromGraph = BackPatch_penwin( "FirstSymbolFromGraph" );
        if( penwinFirstSymbolFromGraph == NULL ) {
            return;
        }
    }

    odata1 = GETALIAS( &lpsyg->lpsye );
    odata2 = GETALIAS( &lpsyg->lpsyc );
    penwinFirstSymbolFromGraph( lpsyg, lpsyv, max, lpcsyv );
    RELEASEALIAS( odata2, &lpsyg->lpsyc );
    RELEASEALIAS( odata1, &lpsyg->lpsye );

} /* __FirstSymbolFromGraph */

/*
 * __GetSymbolCount - cover for penwin function GetSymbolCount
 */
int FAR PASCAL __GetSymbolCount( LPSYG lpsyg )
{
    int             rc;
    DWORD           odata1;
    DWORD           odata2;

    if( penwinGetSymbolCount == NULL ) {
        penwinGetSymbolCount = BackPatch_penwin( "GetSymbolCount" );
        if( penwinGetSymbolCount == NULL ) {
            return( 0 );
        }
    }

    odata1 = GETALIAS( &lpsyg->lpsye );
    odata2 = GETALIAS( &lpsyg->lpsyc );
    rc = penwinGetSymbolCount( lpsyg );
    RELEASEALIAS( odata2, &lpsyg->lpsyc );
    RELEASEALIAS( odata1, &lpsyg->lpsye );

    return( rc );

} /* __GetSymbolCount */

/*
 * __GetSymbolMaxLength - cover for penwin function GetSymbolMaxLength
 */
int FAR PASCAL __GetSymbolMaxLength( LPSYG lpsyg )
{
    int             rc;
    DWORD           odata1;
    DWORD           odata2;

    if( penwinGetSymbolMaxLength == NULL ) {
        penwinGetSymbolMaxLength = BackPatch_penwin( "GetSymbolMaxLength" );
        if( penwinGetSymbolMaxLength == NULL ) {
            return( 0 );
        }
    }

    odata1 = GETALIAS( &lpsyg->lpsye );
    odata2 = GETALIAS( &lpsyg->lpsyc );
    rc = penwinGetSymbolMaxLength( lpsyg );
    RELEASEALIAS( odata2, &lpsyg->lpsyc );
    RELEASEALIAS( odata1, &lpsyg->lpsye );

    return( rc );

} /* __GetSymbolMaxLength */

/*
 * __TrainContext - cover for penwin function TrainContext
 */
BOOL FAR PASCAL __TrainContext( LPRCRESULT lprcresult, LPSYE lpsye, int csye, LPSYC lpsyc, int csyc )
{
    BOOL            rc;
    DWORD           odata1;
    DWORD           odata2;
    DWORD           odata3;
    DWORD           odata4;

    if( penwinTrainContext == NULL ) {
        penwinTrainContext = BackPatch_penwin( "TrainContext" );
        if( penwinTrainContext == NULL ) {
            return( 0 );
        }
    }

    odata1 = GETALIAS( &lprcresult->lpsyv );
    odata2 = GETALIAS( &lprcresult->lprc );
    odata3 = GETALIAS( &lprcresult->syg->lpsye );
    odata4 = GETALIAS( &lprcresult->syg->lpsyc );
    rc = penwinTrainContext( lprcresult, lpsye, csye, lpsyc, csyc );
    RELEASEALIAS( odata4, &lprcresult->syg->lpsyc );
    RELEASEALIAS( odata3, &lprcresult->syg->lpsye );
    RELEASEALIAS( odata2, &lprcresult->lprc );
    RELEASEALIAS( odata1, &lprcresult->lpsyv );

    return( rc );

} /* __TrainContext */
