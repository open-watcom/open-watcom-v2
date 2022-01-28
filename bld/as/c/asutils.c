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
* Description:  Miscellaneous 'as' helper functions.
*
****************************************************************************/


#include "as.h"
#include <stdarg.h>
#ifdef _STANDALONE_
#include <setjmp.h>
#include "banner.h"
#else
#include "asinline.h"
#endif
#if defined( __UNIX__ ) && defined( __WATCOMC__ )
  #if ( __WATCOMC__ < 1300 )
    // fix for OW 1.9
    #include <limits.h>
  #endif
#endif
#include "pathgrp2.h"

#include "clibext.h"


#ifdef _STANDALONE_

enum {
    MSG_USAGE_COUNT = 0
    #define pick(c,e,j) + 1
    #include "usage.gh"
    #undef pick
};

#if defined( INCL_MSGTEXT )
enum {
    MSG_USAGE_BASE = 0
    #define pick(c,e,j) + 1
    #include "as.msg"
    #undef pick
};
#endif

typedef enum {
    MSG_ERROR,
    MSG_WARNING
} msg_type;

extern int      CurrLineno;
extern char     *CurrFilename;
extern FILE     *ErrorFile;
extern jmp_buf  AsmParse;

unsigned        ErrorLimit = 20;        // default error limit
unsigned        WarningLevel = 1;       // default warning level = 1
unsigned        DebugLevel = 0;
bool            DoReport;
char            AsResBuffer[ MAX_RESOURCE_SIZE ];

static unsigned numErrors, numWarnings;

void Banner( void )
//*****************
{
    static bool printed = false;

    if( !printed ) {
        printed = true;
        if( _IsOption( BE_QUIET ) )
            return;
#if defined( AS_ALPHA )
        puts( banner1w( "Alpha AXP Assembler", _WASAXP_VERSION_ ) );
#elif defined( AS_PPC )
        puts( banner1w( "PowerPC Assembler", _WASPPC_VERSION_ ) );
#elif defined( AS_MIPS )
        puts( banner1w( "MIPS Assembler", _WASMIPS_VERSION_ ) );
#else
    #error unknown AS_CPU
#endif
        puts( banner2 );
        puts( banner2a( 1984 ) );
        puts( banner3 );
        puts( banner3a );
    }
}

void Usage( void )
//****************
{
    int         msg;

    if( !_IsOption( BE_QUIET ) ) {
        puts( "" );
    }
    for( msg = MSG_USAGE_BASE; msg < MSG_USAGE_BASE + MSG_USAGE_COUNT; msg++ ) {
        AsMsgGet( msg, AsResBuffer );
        puts( AsResBuffer );
    }
}

void AsOutMessage( FILE *fp, int resource_id, ... )
//*************************************************
// Simply stuffs out the message. No header added.
{
    va_list     args;

    va_start( args, resource_id );
    AsMsgGet( resource_id, AsResBuffer );
    vfprintf( fp, AsResBuffer, args );
    va_end( args );
}

static char *leadingMessage( msg_type type, char *buffer )
//********************************************************
{
    switch( type ) {
    case MSG_ERROR:
        AsMsgGet( AS_MSG_ERROR, buffer );
        break;
    case MSG_WARNING:
        AsMsgGet( AS_MSG_WARNING, buffer );
        break;
    default:
        assert( 0 );
        break;
    }
    return( buffer );
}

static void outMsg( FILE *fp, msg_type mtype, int resource_id, va_list *pargs )
//*****************************************************************************
{
    fprintf( fp, "%s(%d): ", CurrFilename, CurrLineno );
    fputs( leadingMessage( mtype, AsResBuffer ), fp );
    AsMsgGet( resource_id, AsResBuffer );
    if( pargs != NULL ) {
        vfprintf( fp, AsResBuffer, *pargs );
        fputc( '\n', fp );
    } else {
        fprintf( fp, "%s\n", AsResBuffer );
    }
}

static void abortMsg( void )
//**************************
{
    outMsg( stderr, MSG_ERROR, ABORT_PARSE, NULL );
    if( ErrorFile ) {
        outMsg( ErrorFile, MSG_ERROR, ABORT_PARSE, NULL );
    }
}

void AsError( int resource_id, ... )
//**********************************
{
    va_list     args;

    va_start( args, resource_id );
    outMsg( stderr, MSG_ERROR, resource_id, &args );
    va_end( args );
    if( ErrorFile ) {
        va_start( args, resource_id );
        outMsg( ErrorFile, MSG_ERROR, resource_id, &args );
        va_end( args );
    }
    numErrors++;
    if( numErrors >= ErrorLimit ) {
        abortMsg();
        AbortParse();
    }
}

void AsWarning( int resource_id, ... )
//************************************
{
    va_list     args;

    va_start( args, resource_id );
    outMsg( stderr, MSG_WARNING, resource_id, &args );
    va_end( args );
    if( ErrorFile ) {
        va_start( args, resource_id );
        outMsg( ErrorFile, MSG_WARNING, resource_id, &args );
        va_end( args );
    }
    numWarnings++;
}

bool ErrorsExceeding( unsigned count )
//************************************
{
    return( numErrors > count );
}

bool WarningsExceeding( unsigned count )
//**************************************
{
    return( numWarnings > count );
}

void ErrorReport( void )
//**********************
{
    if( !DoReport )
        return;
    if( _IsOption( BE_QUIET ) )
        return;
    AsMsgGet( AS_ERROR_REPORT, AsResBuffer );
    printf( AsResBuffer, CurrFilename, CurrLineno, numWarnings, numErrors );
}

void ErrorCountsReset( void )
//***************************
{
    numErrors = numWarnings = 0;
}

void AbortParse( void )
//*********************
{
    longjmp( AsmParse, 1 );
}

char *MakeAsmFilename( const char *orig_name )
//********************************************
{
    static char ret[_MAX_PATH];
    pgroup2     pg;

    _splitpath2( orig_name, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    if( pg.ext[0] == '\0' ) {   // extension is not specified
        pg.ext = "asm";
    }
    _makepath( ret, pg.drive, pg.dir, pg.fname, pg.ext );
    return( ret );
}

#ifdef AS_DEBUG_DUMP
void DebugPrintf( char *fmt, ... )
//********************************
{
    va_list     args;

    if( !_IsOption( DUMP_DEBUG_MSGS ) )
        return;
    va_start( args, fmt );
    vprintf( fmt, args );
    va_end( args );
    fflush( stdout );
}
#endif

#else   // !_STANDALONE_

char        AsResBuffer[ MAX_RESOURCE_SIZE ];

void AsError( int resource_id, ... )
//**********************************
{
    va_list     args;
    char        msg[ MAX_RESOURCE_SIZE ];

    va_start( args, resource_id );
    AsMsgGet( resource_id, AsResBuffer );
    vsprintf( msg, AsResBuffer, args );
    va_end( args );
    AsmError( msg );    // CC provides this
}

void AsWarning( int resource_id, ... )
//************************************
{
    va_list     args;
    char        msg[ MAX_RESOURCE_SIZE ];

    va_start( args, resource_id );
    AsMsgGet( resource_id, AsResBuffer );
    vsprintf( msg, AsResBuffer, args );
    va_end( args );
    AsmWarning( msg );  // CC provides this
}

#endif

// Functions common to both inline and standalone assemblers.

char *AsStrdup( const char *str )
//*******************************
{
    char    *mem;
    size_t  len;

    len = strlen( str ) + 1;
    mem = MemAlloc( len );
    memcpy( mem, str, len );
    return( mem );
}
