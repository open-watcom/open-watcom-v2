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
* Description:  Miscellaneous 'as' helper functions.
*
****************************************************************************/


#include "as.h"
#include <stdarg.h>
#ifdef _STANDALONE_
#include <setjmp.h>
#include "banner.h"

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

extern void Banner( void ) {
//**************************

    static bool printed = FALSE;

    if( _IsOption( BE_QUIET ) || printed ) return;
#if defined( AS_ALPHA )
    puts( banner1w( "Alpha AXP Assembler", "0.8" ) );
#elif defined( AS_PPC )
    puts( banner1w( "PowerPC Assembler", "0.3" ) );
#elif defined( AS_MIPS )
    puts( banner1w( "MIPS Assembler", "0.1" ) );
#else
    #error unknown AS_CPU
#endif
    puts( banner2 );
    puts( banner2a( "1984" ) );
    puts( banner3 );
    puts( banner3a );
    printed = TRUE;
}

extern void Usage( void ) {
//*************************

    int         ctr;

    for( ctr = USAGE_1; ctr <= USAGE_LAST; ++ctr ) {
        AsMsgGet( ctr, AsResBuffer );
        puts( AsResBuffer );
    }
}

extern void AsOutMessage( FILE *fp, int resource_id, ... ) {
//**********************************************************
// Simply stuffs out the message. No header added.

    va_list     arglist;

    va_start( arglist, resource_id );
    AsMsgGet( resource_id, AsResBuffer );
    vfprintf( fp, AsResBuffer, arglist );
}

static char *leadingMessage( msg_type type, char *buffer ) {
//**********************************************************

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

static void outMsg( FILE *fp, msg_type mtype, int resource_id, va_list *arglist ) {
//*********************************************************************************

    fprintf( fp, "%s(%d): ", CurrFilename, CurrLineno );
    fputs( leadingMessage( mtype, AsResBuffer ), fp );
    AsMsgGet( resource_id, AsResBuffer );
    if( arglist ) {
        vfprintf( fp, AsResBuffer, *arglist );
    } else {
        fprintf( fp, AsResBuffer );
    }
    fputc( '\n', fp );
}

static void abortMsg( void ) {
//****************************

    outMsg( stderr, MSG_ERROR, ABORT_PARSE, NULL );
    if( ErrorFile ) {
        outMsg( ErrorFile, MSG_ERROR, ABORT_PARSE, NULL );
    }
}

extern void AsError( int resource_id, ... ) {
//*******************************************

    va_list     arglist;

    va_start( arglist, resource_id );
    outMsg( stderr, MSG_ERROR, resource_id, &arglist );
    va_end( arglist );
    if( ErrorFile ) {
        va_start( arglist, resource_id );
        outMsg( ErrorFile, MSG_ERROR, resource_id, &arglist );
        va_end( arglist );
    }
    numErrors++;
    if( numErrors >= ErrorLimit ) {
        abortMsg();
        AbortParse();
    }
}

extern void AsWarning( int resource_id, ... ) {
//*********************************************

    va_list     arglist;

    va_start( arglist, resource_id );
    outMsg( stderr, MSG_WARNING, resource_id, &arglist );
    va_end( arglist );
    if( ErrorFile ) {
        va_start( arglist, resource_id );
        outMsg( ErrorFile, MSG_WARNING, resource_id, &arglist );
        va_end( arglist );
    }
    numWarnings++;
}

extern bool ErrorsExceeding( unsigned count ) {
//*********************************************

    return( numErrors > count );
}

extern bool WarningsExceeding( unsigned count ) {
//***********************************************

    return( numWarnings > count );
}

extern void ErrorReport( void ) {
//*******************************

    if( !DoReport ) return;
    if( _IsOption( BE_QUIET ) ) return;
    AsMsgGet( AS_ERROR_REPORT, AsResBuffer );
    printf( AsResBuffer, CurrFilename, CurrLineno, numWarnings, numErrors );
}

extern void ErrorCountsReset( void ) {
//************************************

    numErrors = numWarnings = 0;
}

extern void AbortParse( void ) {
//******************************

    longjmp( AsmParse, 1 );
}

extern char *MakeAsmFilename( const char *orig_name ) {
//*****************************************************

    static char ret[ _MAX_PATH ];
    char        default_ext[] = "asm";
    char        drive[ _MAX_DRIVE ];
    char        dir[ _MAX_DIR ];
    char        fname[ _MAX_FNAME ];
    char        ext[ _MAX_EXT ];
    char        *asm_ext;

    _splitpath( orig_name, drive, dir, fname, ext );
    if( stricmp( ext, "" ) != 0 ) {     // extension is specified
        asm_ext = ext;
    } else {
        asm_ext = default_ext;
    }
    _makepath( ret, drive, dir, fname, asm_ext );
    return( ret );
}

#ifdef AS_DEBUG_DUMP
extern void DebugPrintf( char *fmt, ... ) {
//*****************************************

    va_list     args;

    if( !_IsOption( DUMP_DEBUG_MSGS ) ) return;
    va_start( args, fmt );
    vprintf( fmt, args );
    va_end( args );
    fflush( stdout );
}
#endif
#else   // !_STANDALONE_
extern void AsmError( char * );
extern void AsmWarning( char * );

char        AsResBuffer[ MAX_RESOURCE_SIZE ];

extern void AsError( int resource_id, ... ) {
//*******************************************

    va_list     arglist;
    char        msg[ MAX_RESOURCE_SIZE ];

    va_start( arglist, resource_id );
    AsMsgGet( resource_id, AsResBuffer );
    vsprintf( msg, AsResBuffer, arglist );
    va_end( arglist );
    AsmError( msg );    // CC provides this
}

extern void AsWarning( int resource_id, ... ) {
//*********************************************

    va_list     arglist;
    char        msg[ MAX_RESOURCE_SIZE ];

    va_start( arglist, resource_id );
    AsMsgGet( resource_id, AsResBuffer );
    vsprintf( msg, AsResBuffer, arglist );
    va_end( arglist );
    AsmWarning( msg );  // CC provides this
}

#endif

// Functions common to both inline and standalone assemblers.

extern char *AsStrdup( const char *str ) {
//****************************************

    char    *mem;
    int     len;

    len = strlen( str ) + 1;
    mem = MemAlloc( len );
    memcpy( mem, str, len );
    return( mem );
}
