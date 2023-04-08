/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include "plusplus.h"
#include <ctype.h>
#ifdef __WATCOMC__
    #include <malloc.h>     /* necessary for _heapmin() */
#endif
#include "memmgr.h"
#include "idedll.h"
#include "ideentry.h"
#include "cgmisc.h"
#include "fmtsym.h"

#include "clibext.h"


#ifndef NDEBUG
// #undef DbgVerify
// #define DbgVerify(c,m) if( !(c) ) printIDE( "** IDEDLL **" m )
#endif

typedef uint_16 OptionSize;     // size of option written to file
typedef char    NUMBER_STR[8];  // number string


#if 0


// MEMORY-ALLOCATION INTERFACE

#ifndef NDEBUG
    #define allocMem( p ) CMemAlloc( p )
#else
static void* allocMem           // ALLOCATE MEMORY
    ( size_t size )             // - size
{
    return( CMemAlloc( size ) );
}
#endif


#ifndef NDEBUG
    #define freeMem( p ) CMemFree( p )
#else
static void freeMem             // FREE MEMORY
    ( void* old )               // - allocated memory
{
    CMemFree( old );
}
#endif


static void* reallocMem         // RE-ALLOCATE MEMORY
    ( void* old                 // - old memory
    , size_t old_size
    , size_t size )             // - new size
{
    void* new_blk = allocMem( size );
    DbgVerify( size > old_size, "reallocMem -- sizes messed" );
    memcpy( new_blk, old, old_size );
    freeMem( old );
    return( new_blk );
}


// PRINTING INTERFACE

static struct                   // printing control information
{   char* buffer;               // - buffer
    size_t size_buf;            // - size of buffer
    size_t index;               // - index for next character
} printCtl;


static void printIDE            // CALL IDE FOR PRINTING
    ( char const *line )        // - print line
{
    IDEFN( PrintMessage )( CompInfo.idehdl, line );
    // we are ignoring return for now
}


// Must be called after C++ memory-manager has been initialized
//
void IdePrintInit               // INITIALIZE PRINTING
    ( void )
{
    printCtl.index = 0;
    printCtl.size_buf = 128;
    printCtl.buffer = allocMem( printCtl.size_buf );
    DbgVerify( NULL != printCtl.buffer, "printInit -- alloc failure" );
}


void IdePrintFini               // COMPLETE PRINTING
    ( void )
{
    if( NULL != printCtl.buffer ) {
        freeMem( printCtl.buffer );
        printCtl.buffer = NULL;
    }
}


static void printChar           // PRINT A CHARACTER
    ( char chr )                // - to be printed
{
    if( chr == '\n' ) {
        printCtl.buffer[printCtl.index] = '\0';
        printIDE( printCtl.buffer );
        printCtl.index = 0;
    } else {
        printCtl.buffer[printCtl.index] = chr;
        ++ printCtl.index;
        if( printCtl.index >= printCtl.size_buf ) {
            unsigned old_size = printCtl.size_buf;
            printCtl.size_buf += 32;
            printCtl.buffer = reallocMem( printCtl.buffer
                                        , old_size
                                        , printCtl.size_buf );
        }
    }
}


static void printString         // PRINT A STRING
    ( char const *str )         // - string to be printed
{
    for( ; ; ++str ) {
        char chr = *str;
        if( '\0' == chr )
            break;
        printChar( chr );
    }
}


static void printLine           // PRINT A LINE
    ( char const *line )        // - line to be printed
{
    printString( line );
    printChar( '\n' );
}


#endif


// IDE INTERFACE


unsigned IDEAPI IDEGetVersion( void )
/************************************
 * GET IDE VERSION
 */
{
    return( IDE_CUR_DLL_VER );
}


IDEBool IDEAPI IDEInitDLL   // DLL INITIALIZATION
    ( IDECBHdl cbhdl        // - handle for this instantiation
    , IDECallBacks *cb      // - call backs into IDE
    , IDEDllHdl *hdl )      // - uninitialized info
{
    CompInfo.idehdl = cbhdl;
    CompInfo.idecbs = cb;
    *hdl = (IDEDllHdl)cbhdl;
    return( false );
}


void IDEAPI IDEFiniDLL   // DLL COMPLETION
    ( IDEDllHdl hdl )           // - handle
{
    /* unused parameters */ (void)hdl;

    DbgVerify( hdl == CompInfo.idehdl, "FiniDLL -- handle mismatch" );
}

static void fillInputOutput( char *input, char *output )
{
    size_t  len;                // - length of string
    char    *p;

    input[0] = '\0';
    output[0] = '\0';
    if( !CompFlags.ide_cmd_line_has_files ) {
        p = input + 1;
        if( ! IDEFN( GetInfo )( CompInfo.idehdl, IDE_GET_SOURCE_FILE, (IDEGetInfoWParam)NULL, (IDEGetInfoLParam)&p ) ) {
            input[0] = '"';
            len = strlen( &input[1] );
            input[1 + len] = '"';
            input[1 + len + 1] = '\0';
        }
        p = output + 5;
        if( ! IDEFN( GetInfo )( CompInfo.idehdl, IDE_GET_TARGET_FILE, (IDEGetInfoWParam)NULL, (IDEGetInfoLParam)&p ) ) {
            output[0] = '-';
            output[1] = 'f';
            output[2] = 'o';
            output[3] = '=';
            output[4] = '"';
            len = strlen( &output[5] );
            output[5 + len] = '"';
            output[5 + len + 1] = '\0';
        }
    }
}

static void initDLLInfo( DLL_DATA *data )
{
    data->print_str = NULL;
    data->print_chr = NULL;
    data->print_line = NULL;
    data->cmd_line = NULL;
    data->argc = 0;
    data->argv = NULL;
}



int IDEAPI IDERunYourSelf       // COMPILE A PROGRAM
    ( IDEDllHdl hdl             // - handle for this instantiation
    , const char* opts          // - options
    , IDEBool* fatal_error )    // - addr[ fatality indication ]
{
    DLL_DATA dllinfo;                   // - information passed to DLL
    char input[1 + _MAX_PATH + 1];      // - input file name ("<fname>")
    char output[4 + 1 + _MAX_PATH + 1]; //- output file name (-fo="<fname>")

    /* unused parameters */ (void)hdl;

    DbgVerify( hdl == CompInfo.idehdl, "RunYourSelf -- handle mismatch" );
    TBreak();   // clear any pending IDEStopRunning's
    initDLLInfo( &dllinfo );
    dllinfo.cmd_line = (char*)opts;
    fillInputOutput( input, output );
    WppCompile( &dllinfo, input, output );
    *fatal_error = (IDEBool)CompFlags.fatal_error;
    return( CompFlags.compile_failed );
}


int IDEAPI IDERunYourSelfArgv(  // COMPILE A PROGRAM (ARGV ARGS)
    IDEDllHdl hdl,              // - handle for this instantiation
    int argc,                   // - # of arguments
    char **argv,                // - argument vector
    IDEBool* fatal_error )      // - addr[ fatality indication ]
{
    DLL_DATA dllinfo;                   // - information passed to DLL
    char input[1 + _MAX_PATH + 1];      // - input file name ("<fname>")
    char output[4 + 1 + _MAX_PATH + 1]; //- output file name (-fo="<fname>")

    /* unused parameters */ (void)hdl;

    DbgVerify( hdl == CompInfo.idehdl, "RunYourSelf -- handle mismatch" );
    TBreak();   // clear any pending IDEStopRunning's
    initDLLInfo( &dllinfo );
    dllinfo.argc = argc;
    dllinfo.argv = argv;
    fillInputOutput( input, output );
    WppCompile( &dllinfo, input, output );
    *fatal_error = (IDEBool)CompFlags.fatal_error;
    return( CompFlags.compile_failed );
}

void IDEAPI IDEStopRunning( void )
{
    CauseTBreak();
}

void IDEAPI IDEFreeHeap( void )
{
#ifdef __WATCOMC__
    _heapmin();
#endif
}

#if 0
// HELP Interface

IDEBool IDEAPI IDEProvideHelp // PROVIDE HELP INFORMATION
    ( IDEDllHdl hdl             // - handle for this instantiation
    , char const* msg )         // - message
{
    hdl = hdl;
    DbgVerify( hdl == CompInfo.idehdl, "ProvideHelp -- handle mismatch" );
    msg = msg;
    return( true );
}
#endif


#if 0
// MESSAGE Parsing Interface

typedef struct                  // PARSE INFORMATION
{   char const *scan;           // - scan position
    char *tgt;                  // - target pointer
    unsigned left;              // - amount left
} SCAN_INFO;


static IDEBool collectChar      // COLLECT A CHARACTER IF POSSIBLE
    ( SCAN_INFO* si )           // - scanning information
{
    IDEBool retn;               // - return: true ==> scanned file name

    if( si->left > 0 ) {
        *si->tgt++ = *si->scan++;
        -- si->left;
        retn = true;
    } else {
        retn = false;
    }
    return( retn );
}


static IDEBool collectIfChar    // COLLECT A SPECIFIC CHARACTER
    ( SCAN_INFO* si             // - scanning information
    , char reqd )               // - specific character
{
    IDEBool retn;               // - return: true ==> collected the character

    if( reqd == *si->scan ) {
        retn = collectChar( si );
    } else {
        retn = false;
    }
    return( retn );
}


static IDEBool collectNumber    // COLLECT A NUMBER
    ( SCAN_INFO* si             // - scanning information
    , NUMBER_STR numb )         // - a number string
{
    IDEBool retn;               // - return: true ==> have number

    si->tgt = numb;
    si->left = sizeof( NUMBER_STR ) - 1;
    retn = false;
    for( ; ; ) {
        if( ! isdigit( *si->scan ) )
            break;
        retn = collectChar( si );
        if( !retn ) {
            break;
        }
    }
    *si->tgt = '\0';
    return( retn );
}


static IDEBool mustBeChar       // SCAN PAST REQ'D CHARACTER
    ( SCAN_INFO* si             // - scanning information
    , char reqd )               // - required character
{
    IDEBool retn;               // - return: true ==> have number

    if( *si->scan == reqd ) {
        ++ si->scan;
        retn = true;
    } else {
        retn = false;
    }
    return( retn );
}


static IDEBool mustBeText       // SCAN PAST REQ'D TEXT
    ( SCAN_INFO* si             // - scanning information
    , char const * text )       // - the text
{
    IDEBool retn;               // - return: true ==> have number
    char const *scan;           // - scan position

    scan = si->scan;
    for( ; ; ) {
        if( *text == '\0' ) {
            si->scan = scan;
            retn = true;
            break;
        } else if( *text++ != *scan++ ) {
            retn = false;
            break;
        }
    }
    return( retn );
}


static IDEBool isFileNameChar   // TEST IF CHAR IS IN FILE NAME
    ( char chr )                // - candidate character
{
    IDEBool retn;               // - return: true ==> ok in file name

    retn = isalnum( chr );
    if( ! retn ) switch( chr ) {
        default :
          retn = false;
          break;
        case '_' :  // special chars taken from DOS 5.0 Manual
        case '^' :
        case '$' :
        case '~' :
        case '!' :
        case '#' :
        case '%' :
        case '&' :
        case '-' :
        case '{' :
        case '}' :
//      case '(' :  // not allowed ( we scan for "file(line)" )
        case ')' :
        case '@' :
        case '\'' :
        // missing code for accent grave
          retn = true;
          break;
    }
    return( retn );
}


static IDEBool parseFileChunk   // PARSE FILE CHUNK (BETWEEN \'S)
    ( SCAN_INFO* si )           // - scanning information
{
    IDEBool retn;               // - return: true ==> scanned file name
    IDEBool got_chunk;          // - true ==> got a chunk
    IDEBool got_dot;            // - true ==> got '.' separator

    for( got_dot = false, got_chunk = false; ; got_chunk = true ) {
        if( isFileNameChar( *si->scan ) ) {
            retn = collectChar( si );
            if( ! retn ) {
                break;
            }
        } else if( got_dot ) {
            retn = got_chunk;
            break;
        } else if( collectIfChar( si, '.' ) ) {
            got_dot = true;
        } else {
            retn = got_chunk;
            break;
        }
    }
    return( retn );
}


static IDEBool parseFileName    // PARSE FILE NAME, IF POSSIBLE
    ( SCAN_INFO* si )           // - scanning information
{
    char const * scan;          // - scanner
    IDEBool retn;               // - return: true ==> scanned file name

    scan = si->scan;
    if( scan[0] != '\0' && scan[1] == ':' ) {
        collectChar( si );
        collectChar( si );
    }
    collectIfChar( si, '\\' );
    for( ; ; ) {
        if( ! parseFileChunk( si ) ) {
            retn = false;
            break;
        }
        scan = si->scan;
        if( ! collectIfChar( si, '\\' ) ) {
            *si->tgt = '\0';
            retn = true;
            break;
        }
    }
    return( retn );
}


IDEBool IDEAPI IDEParseMessage // PARSE A MESSAGE
    ( IDEDllHdl hdl             // - handle for this instantiation
    , char const* msg           // - message
    , ErrorInfo* err )          // - error information
{
    IDEBool retn;               // - return: true ==> failed
    SCAN_INFO scan_info;        // - scanning information
    NUMBER_STR number;          // - used for number scanning

    hdl = hdl;
    DbgVerify( hdl == CompInfo.idehdl, "ParseMessage -- handle mismatch" );
    scan_info.scan = msg;
    scan_info.tgt = err->filename;
    scan_info.left = sizeof( err->filename ) - 1;
    if( parseFileName( &scan_info )
     && mustBeChar( &scan_info, '(' )
     && collectNumber( &scan_info, number )
     && mustBeText( &scan_info, "): " )
      ) {
        err->linenum = atoi( number );
        err->flags = ERRINFO_FILENAME | ERRINFO_LINENUM;
        if( ( mustBeText( &scan_info, "Error! E" )
           || mustBeText( &scan_info, "Warning! W" )
           || mustBeText( &scan_info, "Note! N" )
            )
         && collectNumber( &scan_info, number )
         && mustBeText( &scan_info, ": " )
          ) {
            err->flags |= ERRINFO_HELPINDEX;
            err->help_index = atoi( number );
            mustBeChar( &scan_info, ' ' );
        }
        if( mustBeText( &scan_info, "(col " )
         && collectNumber( &scan_info, number )
         && mustBeText( &scan_info, ") " )
          ) {
            err->col = atoi( number );
            err->flags |= ERRINFO_COLUMN;
        }
        retn = false;
    } else {
        err->flags = 0;
        retn = true;
    }
    return( retn );
}
#endif

IDEBool IDEAPI IDEPassInitInfo( IDEDllHdl hdl, IDEInitInfo *info )
{
    /* unused parameters */ (void)hdl;

    DbgVerify( hdl == CompInfo.idehdl, "PassInitInfo -- handle mismatch" );
    if( info->ver < 2 ) {
        return( true );
    }
    if( info->ignore_env ) {
        CompFlags.ignore_environment = true;
        CompFlags.ignore_current_dir = true;
    }
    if( info->cmd_line_has_files ) {
        CompFlags.ide_cmd_line_has_files = true;
    }
    if( info->ver > 2 ) {
        if( info->console_output ) {
            CompFlags.ide_console_output = true;
        }
        if( info->ver > 3 ) {
            if( info->progress_messages ) {
                CompFlags.progress_messages = true;
            }
        }
    }
    return( false );
}


const char *CppGetEnv           // COVER FOR getenv
    ( char const *env_name )    // - environment variable name
{
    const char *env_val;        // - NULL or value of environment variable

    env_val = NULL;
    if( !CompFlags.ignore_environment ) {
        if( IDEFN( GetInfo )( CompInfo.idehdl, IDE_GET_ENV_VAR, (IDEGetInfoWParam)env_name, (IDEGetInfoLParam)&env_val ) ) {
            env_val = NULL;
        }
    }
    return( env_val );
}


void CppStartFuncMessage( SYMBOL sym )
/************************************/
{
    VBUF buff;

    DbgAssert( CompFlags.progress_messages );
    if( sym != NULL ) {
        IDEFN( ProgressMessage )( CompInfo.idehdl, FormatSymWithTypedefs( sym, &buff ) );
        VbufFree( &buff );
    }
}
