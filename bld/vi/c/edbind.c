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
* Description:  VI editor bind utility.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "wio.h"
#include "watcom.h"
#include "bool.h"
#include "banner.h"
#include "bnddata.h"
#include "pathgrp2.h"
#include "myio.h"

#include "clibext.h"


#define SKIP_SPACES(s)  while( isspace( *s ) ) s++

#define MAX_LINE_LEN    1024
#define COPY_SIZE       (0x8000 - 512)  /* QNX read/write size limitation */
#define MAX_DATA_FILES  255
#define MAX_BIND_DATA   65000
#define FILE_BUFF_SIZE  COPY_SIZE

char        *dats[MAX_DATA_FILES];

bind_size   FileCount;
bool        sflag = false;
bool        qflag = false;
char        _bf[] = "edbind.dat";
char        *bindfile = _bf;

static void Banner( void )
{
    if( qflag ) {
        return;
    }
    printf( banner1w( "Editor Bind Utility", _EDBIND_VERSION_ ) "\n" );
    printf( banner2 "\n" );
    printf( banner2a( 1984 ) "\n" );
    printf( banner3 "\n" );
    printf( banner3a "\n" );
}

/*
 * Abort - made a boo-boo
 */
static void Abort( char *str, ... )
{
    va_list     args;

    va_start( args, str );
    vprintf( str, args );
    va_end( args );
    printf( "\n" );
    exit( 1 );

} /* Abort */

/*
 * MyPrintf - do a printf
 */
static void MyPrintf( char *str, ... )
{
    va_list     args;

    if( !qflag ) {
        va_start( args, str );
        vprintf( str, args );
        va_end( args );
    }

} /* MyPrintf */

static int copy_file( FILE *src, FILE *dst, unsigned long tocopy )
{
    char            *copy;
    unsigned        size;
    int             rc;

    rc = 0;
    copy = malloc( COPY_SIZE );
    if( copy == NULL ) {
        printf( "Out of Memory\n" );
        rc = 1;
    } else {
        size = COPY_SIZE;
        while( tocopy > 0 ) {
            if( size > tocopy )
                size = (unsigned)tocopy;
            if( fread( copy, 1, size, src ) != size ) {
                printf( "Read error" );
                rc = 1;
                break;
            }
            if( fwrite( copy, 1, size, dst ) != size ) {
                printf( "Write error" );
                rc = 1;
                break;
            }
            tocopy -= size;
        }
        free( copy );
    }
    return( rc );
}

/*
 * AddDataToEXE - tack data to end of an EXE
 */
static void AddDataToEXE( char *exe, char *data, bind_size data_len, unsigned long tocopy )
{
    FILE            *fp;
    FILE            *newfp;
    char            buff[sizeof( MAGIC_COOKIE ) + sizeof( bind_size )];
    int             rc;

    /*
     * get files
     */
    fp = fopen( exe, "rb" );
    if( fp == NULL ) {
        Abort( "Fatal error opening \"%s\"", exe );
    }
    newfp = tmpfile();
    if( newfp == NULL ) {
        fclose( fp );
        Abort( "Fatal error opening temporary file" );
    }

    /*
     * get trailer
     */
    if( fseek( fp, - (long)sizeof( buff ), SEEK_END ) ) {
        fclose( fp );
        Abort( "Initial seek error on \"%s\"", exe );
    }
    if( fread( buff, 1, sizeof( buff ), fp ) != sizeof( buff ) ) {
        fclose( fp );
        Abort( "Read error on \"%s\"", exe );
    }

    /*
     * if trailer is one of ours, then set back to overwrite data;
     * else just set to write at end of file
     */
    if( memcmp( buff, MAGIC_COOKIE, sizeof( MAGIC_COOKIE ) ) ) {
        if( sflag ) {
            fclose( fp );
            Abort( "\"%s\" does not contain configuration data!", exe );
        }
    } else {
        tocopy -= sizeof( buff ) + *((bind_size *)( buff + sizeof( MAGIC_COOKIE ) ));
    }
    if( fseek( fp, 0, SEEK_SET ) ) {
        fclose( fp );
        Abort( "Seek error on \"%s\"", exe );
    }

    /*
     * copy crap
     */
    rc = copy_file( fp, newfp, tocopy );

    fclose( fp );

    if( rc ) {
        fclose( newfp );
        exit( 1 );
    }

    /*
     * write out data and new trailer
     */
    if( !sflag ) {
        if( fwrite( data, 1, data_len, newfp ) != data_len ) {
            Abort( "write 1 error on \"%s\"", exe );
        }
        memcpy( buff, MAGIC_COOKIE, sizeof( MAGIC_COOKIE ) );
        *((bind_size *)( buff + sizeof( MAGIC_COOKIE ) )) = data_len;
        if( fwrite( buff, 1, sizeof( buff ), newfp ) != sizeof( buff ) ) {
            Abort( "write 2 error on \"%s\"", exe );
        }
    }
    fp = fopen( exe, "wb" );
    if( fp == NULL ) {
        Abort( "Fatal error opening \"%s\"", exe );
    }
    tocopy = ftell( newfp );
    rewind( newfp );
    copy_file( newfp, fp, tocopy );
    fclose( fp );
    fclose( newfp );

} /* AddDataToEXE */

/*
 * GetFromEnv - get file name from environment
 */
static void GetFromEnv( const char *what, char *path )
{
    _searchenv( what, "EDPATH", path );
    if( path[0] != '\0' ) {
        return;
    }
    _searchenv( what, "PATH", path );

} /* GetFromEnv */

/*
 * GetFromEnvAndOpen - search env and fopen a file
 */
static FILE *GetFromEnvAndOpen( const char *inpath )
{
    char tmppath[_MAX_PATH];

    GetFromEnv( inpath, tmppath );
    if( tmppath[0] != '\0' ) {
        MyPrintf( " %s...", tmppath );
        return( fopen( tmppath, "r" ) );
    }
    return( NULL );

} /* GetFromEnvAndOpen */

/*
 * Usage - dump the usage message
 */
#if defined( __WATCOMC__ )
    #pragma aux Usage __aborts
#endif
static void Usage( char *msg )
{
    if( msg != NULL ) {
        printf( "%s\n", msg );
    }
    printf( "Usage: edbind [-?sq] [-d<datfile>] <exename>\n" );
    if( msg == NULL ) {
        printf( "\t<exename>\t     executable to add editor data to\n" );
        printf( "\tOptions -?:\t     display this message\n" );
        printf( "\t\t-s:\t     strip info from executable\n" );
        printf( "\t\t-q:\t     run quietly\n" );
        printf( "\t\t-d<datfile>: specify data file other than edbind.dat\n" );
    }
    exit( 1 );

} /* Usage */

/*
 * MyAlloc - allocate memory, failing if cannot
 */
static void *MyAlloc( size_t size )
{
    void        *tmp;

    tmp = malloc( size );
    if( tmp == NULL ) {
        Abort( "Out of Memory!" );
    }
    return( tmp );

} /* MyAlloc */


int main( int argc, char *argv[] )
{
    char                *data = NULL;
    char                *buff2, *buff3;
    char                *buffn, *buffs;
    char                *ptr;
    int                 j, k;
    size_t              len, arg_len;
    bind_size           fi;
    FILE                *fp;
    struct stat         fs;
    pgroup2             pg;
    char                path[_MAX_PATH];
    char                tmppath[_MAX_PATH];
    bind_size           data_len;
    bind_size           len1;
    bind_size           lines;
    bind_size           *index;
    bind_size           *entries;

    for( j = argc - 1; j > 0; --j ) {
        if( argv[j][0] == '/' || argv[j][0] == '-' ) {
            arg_len = strlen( argv[j] );
            for( len = 1; len < arg_len; len++ ) {
                switch( argv[j][len] ) {
                case 's': sflag = true;
                    break;
                case 'q': qflag = true;
                    break;
                case 'd':
                    bindfile = &argv[j][len + 1];
                    len = arg_len;
                    break;
                case '?':
                    Banner();
                    Usage( NULL );
                default:
                    Banner();
                    Usage( "Invalid option" );
                }
            }
            for( k = j; k < argc; k++ ) {
                argv[k]= argv[k + 1];
            }
            argc--;
        }
    }
    Banner();

    /*
     * now, check for null file name
     */
    if( argc < 2 ) {
        Usage( "No executable to bind" );
    }
    _splitpath2( argv[1], pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    if( pg.ext[0] == '\0' )
        pg.ext = "exe";
    _makepath( path, pg.drive, pg.dir, pg.fname, pg.ext );
    if( stat( path, &fs ) == -1 ) {
        Abort( "Could not find executable \"%s\"", path );
    }

    data_len = 0;
    if( !sflag ) {

        data = MyAlloc( MAX_BIND_DATA );
        buff3 = MyAlloc( MAX_LINE_LEN );

        /*
         * read in all data files
         */
        MyPrintf( "Getting data files from" );
        fp = GetFromEnvAndOpen( bindfile );
        MyPrintf( "\n" );
        if( fp == NULL ) {
            Abort( "Could not open %s", bindfile );
        }
        while( (ptr = myfgets( buff3, MAX_LINE_LEN, fp )) != NULL ) {
            SKIP_SPACES( ptr );
            if( ptr[0] == '\0' || ptr[0] == '#' ) {
                continue;
            }
            dats[FileCount] = MyAlloc( strlen( ptr ) + 1 );
            strcpy( dats[FileCount], ptr );
            FileCount++;
            if( FileCount >= MAX_DATA_FILES ) {
                Abort( "Too many files to bind!" );
            }
        }
        fclose( fp );

        buffn = data;

        *(bind_size *)buffn = FileCount;
        buffn += sizeof( bind_size );
        data_len += sizeof( bind_size );
        buffs = buffn;
        buffn += sizeof( bind_size );
        data_len += sizeof( bind_size );
        len1 = 1;
        for( fi = 0; fi < FileCount; fi++ ) {
            _splitpath2( dats[fi], pg.buffer, NULL, NULL, &pg.fname, &pg.ext );
            _makepath( tmppath, NULL, NULL, pg.fname, pg.ext );
            len = strlen( tmppath ) + 1;
            memcpy( buffn, tmppath, len );
            buffn += len;
            len1 += (bind_size)len;
        }
        *buffn++ = '\0';                /* trailing zero */
        *(bind_size *)buffs = len1;     /* size of token list */
        data_len += len1;
        index = (bind_size *)buffn;
        buffn += FileCount * sizeof( bind_size );
        entries = (bind_size *)buffn;
        buffn += FileCount * sizeof( bind_size );
        data_len += FileCount * ( sizeof( bind_size ) + sizeof( bind_size ) );

        buff2 = MyAlloc( FILE_BUFF_SIZE );
        for( fi = 0; fi < FileCount; fi++ ) {
            MyPrintf( "Loading" );
            fp = GetFromEnvAndOpen( dats[fi] );
            if( fp == NULL ) {
                Abort( "\nLoad of %s failed!", dats[fi] );
            }
            free( dats[fi] );
            setvbuf( fp, buff2, _IOFBF, FILE_BUFF_SIZE );
            index[fi] = data_len;
            lines = 0;
            len1 = 0;
            while( (ptr = myfgets( buff3, MAX_LINE_LEN, fp )) != NULL ) {
                SKIP_SPACES( ptr );
                if( ptr[0] == '\0' || ptr[0] == '#' ) {
                    continue;
                }
                len = strlen( ptr );
                if( len > 255 )
                    len = 255;
                *buffn++ = (char)len;
                memcpy( buffn, ptr, len );
                buffn += len;
                len1 += (bind_size)( len + 1 );
                lines++;
            }
            fclose( fp );
            data_len += len1;
            entries[fi] = lines;
            MyPrintf( "Added %d lines (%d bytes)\n", lines, len1 );
        }
        free( buff2 );
        free( buff3 );

        AddDataToEXE( path, data, data_len, fs.st_size );

        free( data );
    }

    if( !sflag ) {
        MyPrintf( "Added %d bytes to \"%s\"\n", data_len, path );
    } else {
        MyPrintf( "\"%s\" has been stripped of configuration information\n", path );
    }
    return( 0 );

} /* main */
