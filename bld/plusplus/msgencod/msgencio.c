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


// MSGENCIO -- i/o interface for MSGENCOD.C
//
// 91/10/03 -- J.W.Welch        -- defined

#include "msgencod.h"
#include "watlibio.h"

static FILE_DEFN *c_file;       // file: generated C file
static FILE_DEFN *h_file;       // file: generated H file


static int c_io_err(            // SIGNAL I/O ERROR ON C FILE IF REQD
    int retn )                  // - return from file operation
{
    IF( retn )
        retn = 0;
    ELSE
        retn = msgerr( "I/O error writing C file" );
    ENDIF
    return( retn );
}


static int h_io_err(            // SIGNAL I/O ERROR ON H FILE IF REQD
    int retn )                  // - return from file operation
{
    IF( retn )
        retn = 0;
    ELSE
        retn = msgerr( "I/O error writing H file" );
    ENDIF
    return( retn );
}


static int io_open(             // OPEN AN OUTPUT FILE
    const char *fname,          // - file name
    FILE_DEFN **fdef,           // - addr( addr( file definition ) )
    const char *dflt )          // - default extension
{
    int retn;                   // - return: 0 ==> ok
    char dev[ _MAX_DRIVE ];     // - drive
    char dir[ _MAX_DIR   ];     // - directory
    char fnm[ _MAX_FNAME ];     // - file name
    char ext[ _MAX_EXT   ];     // - extension
    char pth[ _MAX_PATH  ];     // - full name

    _splitpath( fname, dev, dir, fnm, ext );
    IF( ext[0] == '\0' )
        _makepath( pth, dev, dir, fnm, dflt );
        fname = pth;
    ENDIF
    *fdef = io_open_write_file( fname );
    IF( *fdef == NULL )
        retn = msgerr_list( "Cannot open ", fname, NULL );
    ELSE
        retn = 0;
    ENDIF
    return( retn );
}


static int io_close(            // CLOSE AN OUTPUT FILE
    FILE_DEFN *fdef )           // - file definition
{
    int retn;                   // - return: 0 ==> ok

    IF( io_close_write_file( fdef ) )
        retn = 0;
    ELSE
        retn = msgerr( "I/O error closing output file" );
    ENDIF
    return( retn );
}


int write_c_str(                // WRITE STRING TO C FILE
    const char *str )           // - string
{
    return( c_io_err( io_write_string( c_file, str ) ) );
}


int write_c_chr(                // WRITE EOR ON C FILE
    char chr )                  // - character
{
    return( c_io_err( io_write_char( c_file, chr ) ) );
}


int write_c_comma(              // WRITE ", " ON C FILE
    void )
{
    return( write_c_str( ", " ) );
}


int write_c_eol(                // WRITE EOR ON C FILE
    void )
{
    return( c_io_err( io_write_eor( c_file ) ) );
}


static int write_c_str_list(    // WRITE VARIABLE LIST OF STRINGS, LIST
    char **pstr )               // - pointer to list
{
    int retn;                   // - return: 0 ==> ok

    GUESS :: NO_ERRS
        WHILE( *pstr != NULL )
            retn = write_c_str( *pstr++ );
            QUITIF( retn != 0 ) :: NO_ERRS
        ENDWHILE
    ENDGUESS
    return( retn );
}


int write_c_strs(               // WRITE VARIABLE LIST OF STRINGS
    char *str, ... )            // - strings (NULL delimited)
{
    return( write_c_str_list( &str ) );
}


int write_c_strs_eol(           // WRITE VARIABLE LIST OF STRINGS
    char *str, ... )            // - strings (NULL delimited)
{
    int retn;                   // - return: 0 ==> ok

    GUESS :: NO_ERRS
        retn = write_c_str_list( &str );
        QUITIF( retn != 0 );
        retn = write_c_eol();
    ENDGUESS
    return( retn );
}


int write_c_line(               // WRITE STRING, EOR TO C FILE
    const char *str )           // - string
{
    int retn;                   // - return: 0 ==> ok

    GUESS :: NO_ERRS
        retn = write_c_str( str );
        QUITIF( retn != 0 );
        retn = write_c_eol();
    ENDGUESS
    return( retn );
}


int open_c_file(                // OPEN C FILE
    const char *fname )         // - file name
{
    return( io_open( fname, &c_file, ".c" ) );
}


int close_c_file(               // CLOSE C FILE
    void )
{
    return( io_close( c_file ) );
}


int write_h_str(                // WRITE STRING TO H FILE
    const char *str )           // - string
{
    return( h_io_err( io_write_string( h_file, str ) ) );
}


int write_h_eol(                // WRITE EOR ON H FILE
    void )
{
    return( h_io_err( io_write_eor( h_file ) ) );
}


int write_h_line(               // WRITE STRING, EOR TO H FILE
    const char *str )           // - string
{
    int retn;                   // - return: 0 ==> ok

    GUESS :: NO_ERRS
        retn = write_h_str( str );
        QUITIF( retn != 0 );
        retn = write_h_eol();
    ENDGUESS
    return( retn );
}


int open_h_file(                // OPEN H FILE
    const char *fname )         // - file name
{
    return( io_open( fname, &h_file, ".h" ) );
}


int close_h_file(               // CLOSE H FILE
    void )
{
    return( io_close( h_file ) );
}
