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


#ifdef _WASM_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "watcom.h"
#include "asmerr.h"
#include "asmglob.h"

extern uint         LineNumber;
extern File_Info    AsmFiles;   // files information

extern void             MsgPrintf( int resourceid ); // don't use this
extern int              MsgGet( int resourceid, char *buffer );
extern int              trademark( void );
extern char             *get_curr_filename( void );

void print_include_file_nesting_structure( void );

#include <stdarg.h>

//    WngLvls[level] // warning levels associated with warning messages
//    CompFlags.errout_redirected

#ifdef __QNX__
#define errout stderr
#else
#define errout stdout
#endif

/* globals to this module */
#define ErrLimit Options.error_limit
#define ErrCount Options.error_count
#define WngCount Options.warning_count
#define WngLevel Options.warning_level
#define ErrLine LineNumber
#define __fprintf fprintf
#define __vfprintf vfprintf
#define __printf printf

int Errfile_Written = FALSE;
FILE *ErrFile = NULL;

void AsmError( int msgnum )
/*************************/
{
    AsmErr( msgnum );
}

#ifdef DEBUG_OUT
void DoDebugMsg( const char *format, ... )
/****************************************/
{
    va_list args;
    if( !Options.debug ) return;

    va_start( args, format );
    vprintf( format, args );
    va_end( args );
}
#endif

void AsmNote( int msgnum, ... )
/*****************************/
{
    va_list args1, args2;

    va_start( args1, msgnum );
    va_start( args2, msgnum );

    PrtMsg( "Note!", msgnum, args1, args2 );
    va_end( args1 );
    va_end( args2 );
}

void AsmErr( int msgnum, ... )
/****************************/
{
    va_list args1, args2;

    va_start( args1, msgnum );
    va_start( args2, msgnum );
    if( ErrLimit == (char)-1  ||  ErrCount < ErrLimit ) {
        PrtMsg( "Error!", msgnum, args1, args2 );
        va_end( args1 );
        va_end( args2 );
        ++ErrCount;
        print_include_file_nesting_structure();
    } else {
        PrtMsg( "", ERR_TOO_MANY_ERRORS, args1, args2 );
        AsmSuicide();
    }
}

void AsmWarn( int level, int msgnum, ... )
/****************************************/
{
    va_list args1, args2;

//    if( WngLvls[level] <= WngLevel )
    if( level <= WngLevel ) {
        va_start( args1, msgnum );
        va_start( args2, msgnum );
        if( !Options.warning_error ) {
            PrtMsg( "Warning!", msgnum, args1, args2 );
            ++WngCount;
        } else {
            PrtMsg( "Error!", msgnum, args1, args2 );
            ++ErrCount;
        }
        va_end( args1 );
        va_end( args2 );
    }
}

void PrtMsg( prefix, msgnum, args1, args2 )
/*****************************************/
register char *prefix;
register int msgnum;
va_list args1, args2;
{
    if( !Options.banner_printed ) {
        Options.banner_printed = TRUE;
        trademark();
    }
    if( ErrFile == NULL ) OpenErrFile();
    PutMsg( errout, prefix, msgnum, args1 );
    fflush( errout );                       /* 27-feb-90 */
    if( ErrFile ) {
        Errfile_Written = TRUE;
        PutMsg( ErrFile, prefix, msgnum, args2 );
    }
}

void DelErrFile() {
    // fixme if( CompFlags.errout_redirected ) return;
    remove( AsmFiles.fname[ERR] );
}

void OpenErrFile()
/****************/
{
    if( !isatty( fileno( errout ) ) ) return;
    if( AsmFiles.fname[ERR] != NULL ) {
        ErrFile = fopen( AsmFiles.fname[ERR], "w" );
    }
}

void PutMsg( FILE *fp, char *prefix, int msgnum, va_list args )
/*************************************************************/
{
    char *fname;
    unsigned line_num;
    char msgbuf[MAX_LINE_LEN];

    if( fp != NULL ) {
        fname = get_curr_filename();
        line_num = LineNumber;
        if( line_num != 0 ) {
            if( fname != NULL ) {
                __fprintf( fp, "%s(%u): ", fname, line_num );
            }
        }
        __fprintf( fp, "%s %c%03d: ", prefix, *prefix, msgnum );
        // CGetMsg( msgbuf, msgnum );
        MsgGet( msgnum, msgbuf );
        __vfprintf( fp, msgbuf, args );
        __fprintf( fp, "\n" );
    }
}

static void AsmSuicide()
{
    exit(1);
}

void PrintStats()
/***************/
{
    __printf( "%s: ", AsmFiles.fname[ASM] );
    __printf( "%u lines, ", LineNumber );
    __printf( "%u warnings, ", WngCount );
    __printf( "%u errors\n", ErrCount );
    fflush( stdout );                   /* 27-feb-90 for QNX */
}
#endif
