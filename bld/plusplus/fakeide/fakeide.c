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


// FAKEIDE -- fake IDE driver
//
// 95/01/25 -- J.W.Welch        Defined
// 95/02/28 -- Jim Randall      Updated for new idedll.h

#include <conio.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "idedll.h"
#include "consread.h"

#define FAKE_HDL (1024+3)
#define OK 0
#define FAILED 1

#define _Msg( m ) puts( "*** IDE ERROR *** " m )
#define _Verify( c, m ) if( !(c) ) _Msg( m )


//-------------------------------------------------------------------
// General Support
//-------------------------------------------------------------------

static char source_file[ _MAX_PATH ];   // source file
static char target_file[ _MAX_PATH ];   // target file
static char options[ 256 ];             // options


static char const * scanBlanks  // SCAN OVER BLANKS
    ( char const * scan )       // - scanner
{
    for( ; *scan == ' '; ++scan );
    return scan;
}


static void scanString          // SCAN A STRING
    ( char const * scan         // - scanner
    , char * tgt )              // - target
{
    for( ; *scan != '\0'; *tgt = *scan, ++ scan, ++tgt );
    for( ; *--tgt == ' '; );
    tgt[1] = '\0';
}


static IDEBool getString           // COPY A STRING
    ( char* tgt                 // - target
    , char const * src )        // - source
{
    strcpy( tgt, src );
    return OK;
}


static char const * fmtBool     // FORMAT A BOOL VALUE
    ( IDEBool val )             // - value
{
    char const *retn;           // - return string

    if( val == OK ) {
        retn = "TRUE";
    } else {
        retn = "FALSE";
    }
    return retn;
}


//-------------------------------------------------------------------
// Option Save/Load Support
//-------------------------------------------------------------------

static char opt_data[256];      // saved options
static unsigned opt_written;    // amount written
static unsigned opt_read;       // amount read


static int optionsWriter        // WRITE FUN (PASSED TO DLL)
    ( void *cookie              // - magic
    , void *data                // - data to be written
    , int size )                // - size
{
    _Verify( cookie == &opt_written, "optionsWriter -- bad cookie" );
    if( sizeof( opt_data ) >= opt_written + size ) {
        memcpy( &opt_data[ opt_written ], data, size );
        opt_written += size;
    } else {
        size = 0;
    }
    return size;
}


static void saveOptions         // SAVE OPTIONS
    ( void )
{
    IDEBool retn;               // - return

    opt_written = 0;
    retn = IDESaveOptions( FAKE_HDL, options, &optionsWriter, &opt_written );
    _Verify( retn == OK, "saveOptions -- failed" );
}


static int optionsReader        // READ FUN (PASSED TO DLL)
    ( void *cookie              // - magic
    , void *data                // - data to be read
    , int size )                // - size
{
    _Verify( cookie == &opt_read, "optionsReader -- bad cookie" );
    if( opt_written >= opt_read + size ) {
        memcpy( data, &opt_data[ opt_read ], size );
        opt_read += size;
    } else {
        size = 0;
    }
    return size;
}


static void loadOptions         // LOAD OPTIONS
    ( void )
{
    IDEBool retn;               // - return
    void *loaded;               // - loaded options

    opt_read = 0;
    retn = IDELoadOptions( FAKE_HDL
                      , &loaded
                      , &optionsReader
                      , &opt_read );
    _Verify( retn == OK, "loadOptions -- failed" );
    if( retn == OK ) {
        // IDE would free the option much later
        // we just zap loaded stuff into our area and free it now
        strcpy( options, loaded );
        IDEFreeOptions( FAKE_HDL, loaded );
    }
}


typedef struct {                // saved options:
    unsigned char s1;           // - low-order size(1)
    unsigned char s2;           // - low_order size(2)
    char data[254];             // - data
} SAVED_OPTS;


static void dumpOptions         // DUMP SAVED OPTIONS
    ( void )
{
    SAVED_OPTS* saved = (SAVED_OPTS*)opt_data;
    unsigned size = saved->s2 * 256 + saved->s1;
    char buffer[8];
    unsigned index;

    utoa( size, buffer, 10 );
    fputs( "SAVED OP: " , stdout );
    fputs( buffer, stdout );
    fflush( stdout );
    putch( ' ' );
    for( index = 0; index < size; ++index ) {
        putch( saved->data[ index ] );
    }
    puts( "" );
}


//-------------------------------------------------------------------
// Memory Support
//-------------------------------------------------------------------


static void *allocMem           // ALLOCATE MEMORY
    ( IDECBHdl hdl              // - handle
    , unsigned long size )      // - size to be allocated
{
    _Verify( hdl == FAKE_HDL, "reAllocMem -- bad handle" );
    return malloc( size );
}


static void freeMem             // FREE MEMORY
    ( IDECBHdl hdl              // - handle
    , void *ptr )               // - allocated memory
{
    _Verify( hdl == FAKE_HDL, "reAllocMem -- bad handle" );
    if( NULL != ptr ) {
        free( ptr );
    }
}


static void *reAllocMem         // RE-ALLOCATE MEMORY
    ( IDECBHdl hdl              // - handle
    , void *ptr                 // - allocated memory
    , unsigned long size )      // - size to be allocated
{
    _Verify( hdl == FAKE_HDL, "reAllocMem -- bad handle" );
    return realloc( ptr, size );
}


//-------------------------------------------------------------------
// Printing Support
//-------------------------------------------------------------------

static IDEBool printMessage     // PRINT A MESSAGE
    ( IDECBHdl hdl              // - handle
    , const char *message )     // - message
{
    ErrorInfo err_info;         // - error information

    _Verify( hdl == FAKE_HDL, "printMessage -- bad handle" );
    puts( message );
    if( ! IDEParseMessage( FAKE_HDL, message, &err_info ) ) {
        char number[8];
        if( err_info.flags & ERRINFO_FILENAME ) {
            fputs( "info: File: ", stdout ); puts( err_info.filename );
        }
        if( err_info.flags & ERRINFO_LINENUM ) {
            utoa( err_info.linenum, number, 10 );
            fputs( "info: Line: ", stdout ); puts( number );
        }
        if( err_info.flags & ERRINFO_COLUMN ) {
            utoa( err_info.col, number, 10 );
            fputs( "info: Column: ", stdout ); puts( number );
        }
        if( err_info.flags & ERRINFO_HELPINDEX ) {
            utoa( err_info.help_index, number, 10 );
            fputs( "info: Help: ", stdout ); puts( number );
        }
    }
    return OK;
}


//-------------------------------------------------------------------
// Macro Expansion
//-------------------------------------------------------------------

static unsigned expandMacro     // EXPAND A MACRO
    ( IDECBHdl hdl              // - handle
    , char *fmtstr              // - input string
    , char *buf                 // - output buffer
    , unsigned bufsize )        // - output size
{
    size_t fmt_size;            // - size to be formatted

    _Verify( hdl == FAKE_HDL, "expandMacro -- bad handle" );
    fmt_size = strlen( fmtstr ) + 1;
    if( fmt_size <= bufsize ) {
        memcpy( buf, fmtstr, fmt_size );
    }
    return fmt_size;
}


//-------------------------------------------------------------------
// Information Request
//-------------------------------------------------------------------

static IDEBool getInfo          // GET INFORMATION
    ( IDECBHdl hdl              // - handle
    , IDEInfoType type          // - request type
    , unsigned long wparam      // - param(1)
    , unsigned long lparam )    // - param(2)
{
    IDEBool retn;               // - return

    _Verify( hdl == FAKE_HDL, "getInfo -- bad handle" );
    switch( type ) {
      case IDE_GET_RUN_OPTIONS :
        _Msg( "getInfo -- GET_RUN_OPTIONS request not implemented" );
        retn = FAILED;
        break;
      case IDE_GET_SOURCE_FILE :
        retn = getString( (char*)lparam, source_file );
        break;
      case IDE_GET_TARGET_FILE :
        retn = getString( (char*)lparam, target_file );
        break;
      default :
        _Msg( "getInfo -- bad request" );
        retn = FAILED;
        break;
    }
    return retn;
}


//-------------------------------------------------------------------
// DLL Data
//-------------------------------------------------------------------

static IDEDllInfo dll_info;     // DLL information

static IDECallBacks callbacks = // DLL call-backs
{   NULL    // &runBatch
,   &printMessage
,   &getInfo
,   &expandMacro
,   &allocMem
,   &freeMem
,   &reAllocMem
//,   NULL    // &openFile
//,   NULL    // &readFile
//,   NULL    // &writeFile
//,   NULL    // &closeFile
//,   NULL    // &seekFile
//,   NULL    // &eraseFile
};

//-------------------------------------------------------------------
// DLL DRIVER
//-------------------------------------------------------------------

int main()
{
    char cmd_line[82];          // - command line
    char const * cmd;           // - scanner
    char code;                  // - current code
    IDEBool fatal_error;        // - fatality indicator
    IDEBool retn;               // - return code

    retn = OK;
    fatal_error = 0;
    _Verify( IDE_CUR_DLL_VER == IDEGetVersion(), "invalid DLL version" );
    IDEInitDLL( FAKE_HDL, &callbacks, &dll_info );
    puts( "\n" );
    for( ; ; ) {
        ConsoleReadPrefixed( cmd_line, sizeof( cmd_line ), "FAKEIDE>" );
        cmd = scanBlanks( cmd_line );
        code = *cmd;
        cmd = scanBlanks( cmd + 1 );
        switch( code ) {
          case '\0' :
            continue;
          case 'q' :
            break;
          case 's' :
            scanString( cmd, source_file );
            continue;
          case 't' :
            scanString( cmd, target_file );
            continue;
          case 'o' :
            scanString( cmd, options );
            continue;
          case 'c' :
            retn = IDERunYourSelf( FAKE_HDL, options, &fatal_error );
            continue;
          case 'r' :
            loadOptions();
            continue;
          case 'w' :
            saveOptions();
            continue;
          case '?' :
            puts( "" );
            fputs( "OPTIONS: " , stdout ); puts( options );
            fputs( "SOURCE:  " , stdout ); puts( source_file );
            fputs( "TARGET:  " , stdout ); puts( target_file );
            fputs( "RETURN:  " , stdout ); puts( fmtBool( retn ) );
            fputs( "FATAL:   " , stdout ); puts( fmtBool( fatal_error ) );
            dumpOptions();
            puts( "" );
            puts( "Enter 'h' for help" );
            puts( "" );
            continue;
          case 'h' :
            puts( "" );
            puts( "o options      [ set compiler options]" );
            puts( "s source file  [ set source file]" );
            puts( "t target file  [ set target file]" );
            puts( "c              [ do a compile]" );
            puts( "w              [ write current options ]" );
            puts( "r              [ read written options ]" );
            puts( "" );
            puts( "?              [ dump IDE data ]" );
            puts( "q              [ quit ]" );
            puts( "h              [ display this message ]" );
            puts( "" );
            continue;
          default :
            _Msg( "undecipherable crap entered" );
            continue;
        }
        break;
    }
    IDEFiniDLL( FAKE_HDL );
    return 0;
}
