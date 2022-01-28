/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  FORTRAN compiler main line
*
****************************************************************************/


#include "ftnstd.h"
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include <process.h>
#endif
#include "global.h"
#include "cpopt.h"
#include "fcgbls.h"
#include "boot77.h"
#include "errutil.h"
#include "filescan.h"
#include "sdcline.h"
#include "fmacros.h"
#include "cioconst.h"
#include "inout.h"
#include "errcod.h"
#include "fmemmgr.h"

#include "clibext.h"


#if _CPU == 8086
#define WFC_NAME  "wfc"
#define WFC_ENV   "WFC"
#elif _CPU == 386
#define WFC_NAME  "wfc386"
#define WFC_ENV   "WFC386"
#elif _CPU == _AXP
#define WFC_NAME  "wfcaxp"
#define WFC_ENV   "WFCAXP"
#elif _CPU == _PPC
#define WFC_NAME  "wfcppc"
#define WFC_ENV   "WFCPPC"
#else
#error Unknown System
#endif

enum {
    MSG_USAGE_COUNT = 0
    #define pick(c,e,j) + 1
    #include "usage.gh"
    #undef pick
};

static char     CmdBuff[2*128];

#if defined( _M_IX86 )
    unsigned char   _8087   = 0;
    unsigned char   _real87 = 0;
#endif

void    ShowUsage( void ) {
//===================

    unsigned    msg;
    const char  *str;

    TOutBanner();
    TOutNL( "" );
    for( msg = MSG_USAGE_BASE; msg < MSG_USAGE_BASE + MSG_USAGE_COUNT; msg++ ) {
        str = GetMsg( msg );
        TOutNL( str );
    }
}

static void FInit( void )
{
    char        imageName[_MAX_PATH];

    FMemInit();
    _cmdname( imageName );
    ErrorInit( imageName );
}

static void FFini( void )
{
    ErrorFini();
    FMemErrors();
    FMemFini();
}

int     main( int argc, char *argv[] ) {
//======================================

// FORTRAN compiler main line.

    int         ret_code;
    char        *opts[MAX_OPTIONS+1];
    char        *p;

#if !defined( __WATCOMC__ )
    _argc = argc;
    _argv = argv;
#else
    /* unused parameters */ (void)argc; (void)argv;
#endif

    FInit();
#if defined( _M_IX86 )
    _real87 = _8087 = 0;
#endif
    p = getenv( WFC_ENV );
    if( p != NULL && *p != NULLCHAR ) {
        strcpy( CmdBuff, p );
        p = &CmdBuff[ strlen( p ) ];
        *p = ' ';
        ++p;
    } else {
        p = CmdBuff;
    }
    getcmd( p );
    ret_code = 0;
    InitCompMain();
    if( MainCmdLine( &SrcName, &CmdPtr, opts, CmdBuff ) ) {
        SrcExtn = SDSplitSrcExtn( SrcName );    // parse the file name in case we get
        ProcOpts( opts );                       // an error in ProcOpts() so error
        InitPredefinedMacros();                 // file can be created
        ret_code = CompMain();
    } else {
        ShowUsage();
    }
    FiniCompMain();
    FFini();
    return( ret_code );
}
