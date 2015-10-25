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
* Description:  Mainline for WATFOR-77.
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "cpopt.h"
#include "progsw.h"
#include "errcod.h"
#include "fmemmgr.h"
#include "cioconst.h"
#include "inout.h"
#include "boot77.h"
#include "cle.h"
#include "sdcio.h"
#include "ftextfun.h"
#include "sdcline.h"
#include "fmacros.h"
#include "option.h"
#include "initopt.h"

#if _CPU == 8086
#define _NAME_  "wfc"
#elif _CPU == 386
#define _NAME_  "wfc386"
#elif _CPU == _AXP
#define _NAME_  "wfcaxp"
#elif _CPU == _PPC
#define _NAME_  "wfcppc"
#else
#error Unknown System
#endif

extern  void            MsgBuffer(uint,char *,...);

extern  char            *UsageLines[];


void    InitCompMain( void ) {
//======================

    FMemInit();
    FIncludePathInit();
    InitCompile();
    ProgSw |= PS_FIRST_COMPILE;
    InitMacroProcessor();
    SetDefaultOpts();
    LastColumn = LAST_COL;
}


void    FiniCompMain( void ) {
//======================

    FiniMacroProcessor();
    FIncludePathFini();
    FMemFini();
}


int     CompMain( char *parm ) {
//==============================

    int         num;

    num = 0;
    for(;;) {
        parm = Batch( parm, num );
        if( parm == NULL ) break;
        Compile( parm );
        ++num;
    }
    return( RetCode );
}

static  int     ProcName( void ) {
//==========================

    int code;

    code = _SUCCESSFUL;
    if( *SrcName == NULLCHAR ) {
        code = _NOFILENAME;
    } else if( *SrcName == '?' ) {
        code = _REQSYNTAX;
    }
    return( code );
}

static  bool    ProcCmd( char *buffer ) {
//=======================================

    char        *opt_array[MAX_OPTIONS+1];

    RetCode = _BADCMDLINE;
    if( ParseCmdLine( &SrcName, &CmdPtr, opt_array, buffer ) != FALSE ) {
        RetCode = ProcName();
        if( RetCode == _SUCCESSFUL ) {
            ProcOpts( opt_array );
        }
    }
    return( RetCode == _SUCCESSFUL );
}


void    Compile( char *buffer ) {
//===============================

    InitCompile();
    if( ProcCmd( buffer ) != FALSE ) {
        // initialize permanent i/o buffers after memory has been
        // initialized
        InitMemIO();
        CLE();
        if( ( NumErrors != 0 ) && ( RetCode == _SUCCESSFUL ) ) {
            RetCode = _SYSRETCOD( NumErrors );
        }
    } else {
        ShowUsage();
        if( RetCode == _REQSYNTAX ) {
            // A specific request for syntax (WATFOR77 ?) should return 0.
            RetCode = _SUCCESSFUL;
        }
    }
    FiniComIO();
    FiniProcCmd();
    FiniCompile();
}


void    ShowUsage( void ) {
//===================

    char        buff[LIST_BUFF_SIZE+1];

    TOutBanner();
    TOutNL( "" );
    MsgBuffer( MS_USAGE_LINE, buff, _NAME_ );
    TOutNL( buff );
    TOutNL( "" );
    ShowOptions( buff );
}


char    *SkipBlanks( char *ptr ) {
//================================

    while( ( *ptr == ' ' ) || ( *ptr == '\f' ) || ( *ptr == '\t' ) ) {
        ptr++;
    }
    return( ptr );
}


static  void    InitComVars( void ) {
//=============================

    ProgSw    = PS_DONT_GENERATE; // so we get command line errors
    NumErrors = 0;
    NumWarns  = 0;
    NumExtens = 0;
    SrcRecNum = 0;
}


void    InitCompile( void ) {
//=====================

// Initialize compiler i/o before we process the command line so we can
// issue errors to the error file. i/o buffers are temporary until memory
// has been initialized.

    if( ProgSw & PS_FIRST_COMPILE ) return; // done in InitCompMain() 1st time
    InitComIO();
    InitComVars();
}


void    FiniCompile( void ) {
//=====================

    ProgSw &= ~PS_FIRST_COMPILE;
}


void    ProcOpts( char **opt_array ) {
//====================================

    InitOptions();
    NewOptions = Options;
    for(;;) {
        if( *opt_array == NULL ) break;
        CmdOption( *opt_array );
        ++opt_array;
    }
    Options = NewOptions;
}
