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
* Description:  Diagnostics routines (errors/warnings/notes, listings)
*
****************************************************************************/


#if defined( _STANDALONE_ )

#include "asmglob.h"
#include <stdarg.h>
#include "directiv.h"
#include "asminput.h"
#include "fatal.h"
#include "errout.h"

extern void             MsgPrintf( int resourceid ); // don't use this
extern int              trademark( void );

void                    OpenErrFile( void );
void                    print_include_file_nesting_structure( void );

//    WngLvls[level] // warning levels associated with warning messages
//    CompFlags.errout_redirected

/* globals to this module */
#define ErrLimit Options.error_limit
#define ErrCount Options.error_count
#define WngCount Options.warning_count
#define WngLevel Options.warning_level

static bool             Errfile_Written = false;
static FILE             *ErrFile = NULL;

static void             AsmSuicide( void );
static void             PutMsg( FILE *fp, char *prefix, int msgnum, va_list args );
static void             PrtMsg1( char *prefix, int msgnum, va_list args1, va_list args2 );

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
    if( !Options.int_debug ) return;

    va_start( args, format );
    vprintf( format, args );
    va_end( args );
}
#endif

void AsmNote( int level, int msgnum, ... )
/****************************************/
{
    va_list args1, args2;

    if( level <= WngLevel ) {
        va_start( args1, msgnum );
        va_start( args2, msgnum );

        PrtMsg1( "Note!", msgnum, args1, args2 );
        va_end( args1 );
        va_end( args2 );
    }
}

void AsmErr( int msgnum, ... )
/****************************/
{
    va_list args1, args2;

#ifdef DEBUG_OUT
    DebugCurrLine();
#endif
    va_start( args1, msgnum );
    va_start( args2, msgnum );
    if( ErrLimit == -1  ||  ErrCount < ErrLimit ) {
        PrtMsg1( "Error!", msgnum, args1, args2 );
        va_end( args1 );
        va_end( args2 );
        ++ErrCount;
        print_include_file_nesting_structure();
    } else {
        PrtMsg1( "", ERR_TOO_MANY_ERRORS, args1, args2 );
        AsmSuicide();
    }
}

void AsmWarn( int level, int msgnum, ... )
/****************************************/
{
    va_list args1, args2;

    if( level <= WngLevel ) {
#ifdef DEBUG_OUT
        DebugCurrLine();
#endif
        va_start( args1, msgnum );
        va_start( args2, msgnum );
        if( !Options.warning_error ) {
            PrtMsg1( "Warning!", msgnum, args1, args2 );
            ++WngCount;
        } else {
            PrtMsg1( "Error!", msgnum, args1, args2 );
            ++ErrCount;
        }
        va_end( args1 );
        va_end( args2 );
        print_include_file_nesting_structure();
    }
}

static void PrtMsg1( char *prefix, int msgnum, va_list args1, va_list args2 )
/***************************************************************************/
// print standard WASM messages, no WOMP
{
    if( !Options.banner_printed ) {
        Options.banner_printed = true;
        trademark();
    }
    if( ErrFile == NULL ) OpenErrFile();
    PutMsg( errout, prefix, msgnum, args1 );
    fflush( errout );                       /* 27-feb-90 */
    if( ErrFile ) {
        Errfile_Written = true;
        PutMsg( ErrFile, prefix, msgnum, args2 );
    }
}

void PrtMsg( int msgnum, ... )
/****************************/
// print messages from WOMP !!!
{
    va_list args1;

    if( !Options.banner_printed ) {
        Options.banner_printed = true;
        trademark();
    }
    if( ErrFile == NULL )
        OpenErrFile();
    va_start( args1, msgnum );
    PutMsg( errout, "Warning!", msgnum, args1 );
    fflush( errout );
}

void DelErrFile( void )
/*********************/
{
    // fixme if( CompFlags.errout_redirected ) return;
    remove( AsmFiles.fname[ERR] );
}

void OpenErrFile( void )
/**********************/
{
//    if( !isatty( fileno( errout ) ) ) return;
    if( AsmFiles.fname[ERR] != NULL ) {
        ErrFile = fopen( AsmFiles.fname[ERR], "w" );
    }
}

void LstMsg( const char *format, ... )
/************************************/
{
    va_list     args;

    if( AsmFiles.file[LST] ) {
        va_start( args, format );
        vfprintf( AsmFiles.file[LST], format, args );
        va_end( args );
    }
}

void OpenLstFile( void )
/**********************/
{
    if( AsmFiles.fname[LST] != NULL && Options.write_listing ) {
        AsmFiles.file[LST] = fopen( AsmFiles.fname[LST], "w" );
    }
}

static void PutMsg( FILE *fp, char *prefix, int msgnum, va_list args )
/********************************************************************/
{
    const FNAME     *fname;
    char            msgbuf[MAX_MESSAGE_SIZE];

    if( fp != NULL ) {
        fname = get_curr_srcfile();
        if( LineNumber != 0 ) {
            if( fname != NULL ) {
                fprintf( fp, "%s(%lu): ", fname->name, LineNumber );
            }
        }
        fprintf( fp, "%s %c%03d: ", prefix, *prefix, msgnum );
        // CGetMsg( msgbuf, msgnum );
        MsgGet( msgnum, msgbuf );
        vfprintf( fp, msgbuf, args );
        fprintf( fp, "\n" );
    }
}

static void AsmSuicide( void )
/****************************/
{
    AsmShutDown();
    exit( 1 );
}

void PrintStats( void )
/*********************/
{
    printf( "%s: ", ModuleInfo.srcfile->name );
    printf( "%lu lines, ", LineNumber );
    printf( "%u warnings, ", WngCount );
    printf( "%u errors\n", ErrCount );
#ifdef DEBUG_OUT
    printf( "%u passes\n", Parse_Pass + 1 );
#endif
    fflush( stdout );                   /* 27-feb-90 for QNX */
}
#endif
