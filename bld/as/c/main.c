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
* Description:  Assembler mainline.
*
****************************************************************************/


#include "as.h"
#include <setjmp.h>
#include "preproc.h"
#include "asparser.h"
#include "clibint.h"


extern bool     DoReport;
extern int      CurrLineno;

jmp_buf         AsmParse;
int             ExitStatus = EXIT_SUCCESS;

int PP_MBCharLen( const char *p )
/*******************************/
{
    /* unused parameters */ (void)p;

    return( 1 );
}

int main( int argc, char **argv )
//*******************************
{
    static char *fname;

#ifndef __WATCOMC__
    _argv = argv;
    _argc = argc;
#endif
    MemInit();
    if( !AsMsgInit() ) {
        return( EXIT_FAILURE );
    }
    if( argc == 1 ) {
        Banner();
        Usage();
    } else if( OptionsInit( --argc, ++argv ) ) {
        Banner();
        if( _IsOption( PRINT_HELP ) ) {
            Usage();
            *argv = NULL;
        } else if( !*argv ) {
            AsOutMessage( stderr, AS_MSG_ERROR );
            AsOutMessage( stderr, NO_FILENAME_SPECIFIED );
            fputc( '\n', stderr );
        }
        PP_Init( '#' );
        while( *argv != NULL ) {
            fname = MakeAsmFilename( *argv );
            if( PP_FileInit( fname, PPFLAG_ASM_COMMENT | PPFLAG_EMIT_LINE | PPFLAG_TRUNCATE_FILE_NAME, NULL ) != 0 ) {
                AsOutMessage( stderr, UNABLE_TO_OPEN, fname );
                fputc( '\n', stderr );
            } else {
                OptionsPPDefine();
                SymInit();
                InsInit();
                DirInit();
                if( ObjInit( fname ) ) {
                    if( setjmp( AsmParse ) == 0 ) {
                        ErrorCountsReset();
                        DoReport = true;
                        if( !yyparse() ) {
                            CurrLineno--;    // This is the total # of lines
                            ObjRelocsFini(); // Must be done before ErrorReport
                                             // and other finis
                        } else {
                            DoReport = false;
                        }
                    } else { // AbortParse() was invoked
                        DoReport = false;
                    }
                    ErrorReport();
                    AsLexerFini();
                    ObjFini();
                }
                DirFini();
                InsFini();
                SymFini();
            }
            PP_FileFini();
            ++argv;
        }
        PP_Fini();
    }
    OptionsFini();
    AsMsgFini();
    MemFini();
    return( ExitStatus );
}
