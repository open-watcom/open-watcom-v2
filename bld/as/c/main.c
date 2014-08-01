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
* Description:  Assembler mainline.
*
****************************************************************************/


#include "as.h"
#include <setjmp.h>
#include "preproc.h"
#include "clibint.h"

extern bool     OptionsInit( int argc, char *argv[] );
extern void     OptionsFini( void );
extern void     OptionsPPDefine( void );
extern int      yyparse( void );
extern void     AsLexerFini( void );

extern bool     DoReport;
extern int      CurrLineno;

jmp_buf         AsmParse;
int             ExitStatus = EXIT_SUCCESS;


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
        while( *argv ) {
            fname = MakeAsmFilename( *argv );
            if( PP_Init( fname, PPFLAG_ASM_COMMENT | PPFLAG_EMIT_LINE, NULL ) != 0 ) {
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
                        DoReport = TRUE;
                        if( !yyparse() ) {
                            CurrLineno--;    // This is the total # of lines
                            ObjRelocsFini(); // Must be done before ErrorReport
                                             // and other finis
                        } else {
                            DoReport = FALSE;
                        }
                    } else { // AbortParse() was invoked
                        DoReport = FALSE;
                    }
                    ErrorReport();
                    AsLexerFini();
                    ObjFini();
                }
                DirFini();
                InsFini();
                SymFini();
            }
            PP_Fini();
            ++argv;
        }
    }
    OptionsFini();
    AsMsgFini();
    MemFini();
    return( ExitStatus );
}
