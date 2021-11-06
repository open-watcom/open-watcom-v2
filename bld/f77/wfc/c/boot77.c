/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
#include "inout.h"
#include "boot77.h"
#include "cle.h"
#include "fmacros.h"
#include "option.h"
#include "initopt.h"


static  void    InitComVars( void ) {
//=============================

    ProgSw    = PS_DONT_GENERATE; // so we get command line errors
    NumErrors = 0;
    NumWarns  = 0;
    NumExtens = 0;
    SrcRecNum = 0;
}


void    InitCompMain( void ) {
//======================

    InitComIO();
    InitComVars();
    FIncludePathInit();
    InitMacroProcessor();
    SetDefaultOpts();
    LastColumn = LAST_COL;
}


void    FiniCompMain( void ) {
//======================

    FiniMacroProcessor();
    FIncludePathFini();
    FiniProcCmd();
    FiniComIO();
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

static void    InitCompile( void ) {
//=====================

}


static void    FiniCompile( void ) {
//=====================

}


int     CompMain( void ) {
//===============================

    InitCompile();
    RetCode = ProcName();
    if( RetCode == _SUCCESSFUL ) {
        CLE();
        if( ( NumErrors != 0 ) && ( RetCode == _SUCCESSFUL ) ) {
            RetCode = _SYSRETCOD( NumErrors );
        }
    } else {
        ShowUsage();
        if( RetCode == _REQSYNTAX ) {
            // A specific request for syntax (wfc... ?) should return 0.
            RetCode = _SUCCESSFUL;
        }
    }
    FiniCompile();
    return( RetCode );
}


void ProcOpts( char **opt_array )
//===============================
{
    InitOptions();
    NewOptions = Options;
    for(;;) {
        if( *opt_array == NULL )
            break;
        CmdOption( *opt_array );
        ++opt_array;
    }
    Options = NewOptions;
}
