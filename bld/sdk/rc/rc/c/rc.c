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


#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>

#include "types.h"

#include "wresall.h"
#include "global.h"
#include "rcmem.h"
#include "pass2.h"
#include "errors.h"
#include "rcio.h"
#include "yydriver.h"
#include "param.h"
#include "depend.h"
#include "ldstr.h"
#include "preproc.h"
#include "dbtable.h"
#ifdef DLL_COMPILE
#include "rcdll.h"
#endif

static bool CreatePreprocFile( void ) {
    int         hdl;
    bool        error;
    int         ch;
    char        ch1;
    int         len;

    error = FALSE;
    hdl = RcOpen( CmdLineParms.OutResFileName,
                O_WRONLY | O_TEXT | O_CREAT | O_TRUNC,
                S_IRWXU | S_IRWXG | S_IRWXO );
    if( hdl == -1 ) {
        RcError( ERR_CANT_OPEN_FILE, CmdLineParms.OutResFileName,
                        strerror( errno ) );
        error = TRUE;
    } else {
        ch = RcIoGetChar();
        while( ch != RC_EOF ) {
            ch1 = (char) ch;
            len = RcWrite( hdl, &ch1, 1 );
            if( len != 1 ) {
                RcError( ERR_WRITTING_FILE, CmdLineParms.OutResFileName,
                                strerror( errno ) );
                error = TRUE;
            }
            ch = RcIoGetChar();
        }
    }
    if( hdl != -1 ) RcClose( hdl );
    return( error );
}

static int Pass1( void )
/**********************/
{
    int     noerror;

    noerror = RcPass1IoInit();
    if (noerror) {
        if( !CmdLineParms.PreprocessOnly ) {
            SetDefLang();
            ParseInit();
            Parse();
            WriteDependencyRes();
        } else {
            CreatePreprocFile();
        }
        PP_Fini();
        RcPass1IoShutdown();
        noerror = !ErrorHasOccured;
    }

    return( noerror );
} /* Pass1 */

/* Please note that this function is vital to the resource editors. Thusly
 * any changes made to Pass2 should cause the notification of the
 * resource editor dude.
 */
static int Pass2( void )
/**********************/
{
    int     noerror;

    noerror = RcPass2IoInit();
    if (noerror) {
        switch( Pass2Info.OldFile.Type ) {
        case EXE_TYPE_NE:
            noerror = MergeResExeNE();
            break;
        case EXE_TYPE_PE:
            noerror = MergeResExePE();
            break;
        }
        RcPass2IoShutdown( noerror );
    }

    return( noerror );
} /* Pass2 */

#ifdef DLL_COMPILE
int Dllmain( int argc, char * argv[] )
#else
int main( int argc, char * argv[] )
#endif
/***************************************/
{
    bool    noerror;

#ifndef DLL_COMPILE
    RcMemInit();
#endif
    if( !InitRcMsgs( argv[0] ) ) return( 1 );

    noerror = ScanParams( argc, argv );
    if (!CmdLineParms.Quiet) {
        RcIoPrintBanner();
    }
    if (CmdLineParms.PrintHelp) {
        RcIoPrintHelp( argv[0] );
    }

    if (noerror && !CmdLineParms.Pass2Only) {
        noerror = Pass1();
    }
    if (noerror && !CmdLineParms.Pass1Only && !CmdLineParms.PreprocessOnly ) {
        noerror = Pass2();
    }

    FiniTable();
#ifndef DLL_COMPILE
    ScanParamShutdown();
    FiniRcMsgs();
    RcMemShutdown();
#endif

    if (noerror) {
        return( 0 );
    } else {
        return( 1 );
    }
} /* main */
