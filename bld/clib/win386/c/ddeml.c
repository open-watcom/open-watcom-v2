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


#include "variety.h"
#include "cover.h"

WORD PASCAL _Cover_DdeInitialize( LPDWORD pidInst, PFNCALLBACK p,
                                DWORD afCmd, DWORD ulRes)
{
    return( DdeInitialize( pidInst,
        (PFNCALLBACK) SetProc( (FARPROC)p, GETPROC_DDEMLCALLBACK ),
                afCmd, ulRes ) );
}

HDDEDATA PASCAL _Cover_DdeClientTransaction( LPBYTE lpvdata,
                                             DWORD cbdata,
                                             HCONV hconv,
                                             HSZ hszitem,
                                             WORD ufmt,
                                             WORD utype,
                                             DWORD utimeout,
                                             LPDWORD lpuresult )
{
    HDDEDATA    rc;
    DWORD       pdata;
    DWORD       presult;

    pdata = (DWORD)lpvdata;
    if( cbdata != -1L ) {
        if( pdata != 0 ) {
            pdata = AllocAlias16( lpvdata );
        }
    }
    presult = (DWORD)lpuresult;
    if( presult != 0 ) {
        presult = AllocAlias16( lpuresult );
    }
    rc = _16DdeClientTransaction( (LPBYTE)pdata, cbdata, hconv, hszitem,
                                  ufmt, utype, utimeout, (LPDWORD)presult );
    if( cbdata != -1L ) {
        if( pdata != 0 ) {
            FreeAlias16( pdata );
        }
    }
    if( presult != 0 ) {
        FreeAlias16( presult );
    }
    return( rc );
}
