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


#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <fcntl.h>
#include "bdiff.h"


#define MXFNAME         130
#define BUFSIZE         0x4000

static void Fatal( char *, char * );
static void Usage( void );

static char *Buffer;
static char *Usetext[] = {
    "Usage: BPCMT comment_file patch_file",
    NULL
};

static char TmpExt[] = "A";

void main( int argc, char *argv[] )
{
    size_t size, bufsize;
    char        outfile[_MAX_PATH], infile[_MAX_PATH];
    char        drive[_MAX_DRIVE], dir[_MAX_DIR];
    char        name[_MAX_FNAME], ext[_MAX_EXT];
    int         fpin, fpout, fpcmt;
    char        *pos;


    if( argc != 3 )  Usage();
    if( argv[1][0] == '?' && argv[1][1] == '\0' ) Usage();
    bufsize = BUFSIZE;
    while( ( Buffer = _allocate( bufsize ) ) == NULL ) {
        size = bufsize & (bufsize - 1);
        bufsize = size ? size : ( (bufsize << 1) | (bufsize << 2) );
        if( bufsize < MXFNAME )  Fatal( "Too low on memory", NULL );
    }

    fpcmt = open( argv[1], O_RDONLY | O_BINARY, 0 );
    if( fpcmt == -1 ) {
        Fatal( "Unable to open '*' to read", argv[2] );
    }

    strcpy( infile, argv[2] );
    _splitpath( infile, drive, dir, name, ext );
    for( ;; ) {
        _makepath( outfile, drive, dir, "__", TmpExt );
        if( access( outfile, 0 ) != 0 ) break;
        TmpExt[ 0 ]++;
        if( TmpExt[0] > 'Z' ) Fatal( "Cannot create temporary file", NULL );
    }

    /* initialize input file */
    fpin = open( infile, O_RDONLY | O_BINARY, 0 );
    if( fpin == -1 ) {
        Fatal( "Unable to open '*' to read", infile );
    }

    /* initialize output file */
    fpout = open( outfile, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0 );
    if( fpout == -1 ) {
        Fatal( "Unable to create output file '*'", outfile );
    }
    /* write new comment */
    write( fpout, PATCH_SIGNATURE, sizeof( PATCH_SIGNATURE ) - 1 );
    for( ;; ) {
        size = read( fpcmt, Buffer, bufsize );
        if( size == 0 ) break;
        pos = memchr( Buffer, EOF_CHAR, size );
        if( pos != NULL ) {
            size = pos - Buffer;
            lseek( fpcmt, 0L, SEEK_END ); /*cause it to quit next time round*/
        }
        write( fpout, Buffer, size );
    }
    /* strip old comment */
    for( ;; ) {
        size = read( fpin, Buffer, bufsize );
        pos = memchr( Buffer, EOF_CHAR, size );
        if( pos != NULL ) break;
    }
    size -= (pos - Buffer);
    if( size != 0 ) write( fpout, pos, size );
    /* transfer patch file */
    for( ;; ) {
        size = read( fpin, Buffer, bufsize );
        if( size == 0 ) break;
        write( fpout, Buffer, size );
    }
    close( fpin );
    close( fpout );
    close( fpcmt );

    if( remove( infile ) )        Fatal( "Cannot erase file '*'", infile );
    if( rename(outfile,infile) )  Fatal( "Cannot rename file '*'", outfile);
    exit( EXIT_SUCCESS );
}

static void Fatal( char *reason, char *insert )
/* the reason doesn't have to be good */
{
    while( *reason ) {
        if( *reason == '*' ) {
            fputs( insert, stdout );
        } else {
            fputc( *reason, stdout );
        }
        ++reason;
    }
    puts( "\nbpcmt aborted" );
    exit( EXIT_FAILURE );
}


static void Usage( void )
{
    char **text;

    text = Usetext;
    while( *text ) {
        puts( *text++ );
    }
    exit( EXIT_FAILURE );
}
