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
* Description:  Performs enhanced Unix cp file I/O.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <conio.h>
#include <errno.h>
#include <dos.h>
#include <direct.h>
#if defined( __OS2__ ) && defined( __386__ )
#define  INCL_DOSFILEMGR
#define  INCL_DOSERRORS
#define  INCL_DOSMISC
#include <os2.h>
#elif defined( __NT__ )
#include <windows.h>
#endif
#include "watcom.h"
#include "cp.h"
#include "fnutils.h"
#include "clibext.h"
#include "filerx.h"

#if defined( __OS2__ ) && defined( _M_I86 ) || defined( __DOS__ )
extern long DosGetFullPath( char __FAR *org, char __FAR *real );
#pragma aux DosGetFullPath = \
        "push   ds" \
        "mov    si,ax" \
        "mov    ds,dx" \
        "mov    ah,60H" \
        "int    21h" \
        "sbb    dx,dx" \
        "pop    ds" \
        parm [dx ax] [es di] value [dx ax] modify[si];

/*
 * dosSameFile - DOS specific same file test
 */
int dosSameFile( char *dest, char *src )
{
#define strcmp(p1,p2)           _inline_strcmp(p1,p2)
extern int _inline_strcmp( char *, char * );
    char        fullsrc[_MAX_PATH];
    char        fulldest[_MAX_PATH];

    DosGetFullPath( dest, fulldest );
    DosGetFullPath( src, fullsrc );
    if( !strcmp( fulldest, fullsrc ) ) {
        return( TRUE );
    }
    return( FALSE );

} /* dosSameFile */

/*
 * osSameFile - OS/2 specific same file test
 */
static int osSameFile( char *dest, char *src )
{
    if( _osmode == DOS_MODE ) {
        return( dosSameFile( dest, src ) );
    }
    src = src;
    dest = dest;
    return( TRUE );

} /* osSameFile */

#elif defined( __NT__ )
/*
 * osSameFile - NT specific same file test
 */
static int osSameFile( char *dest, char *src )
{
    char *fp;
    char        fullsrc[MAX_PATH];
    char        fulldest[MAX_PATH];

    GetFullPathName( dest, MAX_PATH, fulldest, &fp );
    GetFullPathName( src, MAX_PATH, fullsrc, &fp );
    if( !stricmp( fulldest, fullsrc ) ) {
        return( TRUE );
    }
    return( FALSE );

} /* osSameFile */

#endif

#if !( defined( __OS2__ ) && defined( __386__ ) )
/*
 * sameFile - test if two files are the same
 */
static int sameFile( char *dest, struct stat *ds, char *src, struct stat *ss )
{
    if( ds->st_size == ss->st_size ) {
        if( ds->st_atime == ss->st_atime ) {
            if( ds->st_dev == ss->st_dev ) {
                if( ds->st_mode == ss->st_mode ) {
                    return( osSameFile( dest, src ) );
                }
            }
        }
    }
    return( FALSE );

} /* sameFile */
#endif


/*
 * head is a buffer of size _MAX_PATH.
 * tail points at null terminator of head.
 * to_append is a string that is to be appended to head.
 */
static char *pathCopy( char *head, char *tail, const char *to_append ) {

    size_t      len;

    len = strlen( to_append );
    if( ( tail + len ) - head >= _MAX_PATH ) {
        *tail = 0;
        Die( "Path too long: %s%s\n", head, to_append );
    }
    return( (char *)memcpy( tail, to_append, len + 1 ) + len );
}


/*
 * recursiveCp - copy a file to a directory (recursively entered)
 *
 * source_head  points to a buffer of size _MAX_PATH that ends with a ':'
 *              or a FILESEP.
 * source_tail  points to the null terminator of source_head.
 * source_wild  is the filename/pattern to append to source_head to get the
 *              names of the file(s) to copy.
 * dest_head    points to a buffer of size _MAX_PATH that ends with a ':'
 *              or a FILESEP
 * dest_tail    points to the null terminator of dest_head.
 *
 * Note that the buffers source_head/dest_head are passed down the
 * recursion to save stack space.
 */
static void recursiveCp( char *source_head, char *source_tail,
    char *source_wild, char *dest_head, char *dest_tail )
{

    DIR                 *directory;
    struct dirent       *nextdirentry;
    void                *crx = NULL;
    char                *new_source_tail;
    char                *new_dest_tail;

    pathCopy( source_head, source_tail, "*.*" );

    directory = opendir( source_head );
    if( directory == NULL ) {
        DropPrintALine( "file \"%s\" not found", source_head );
        return;
    }

    if( rxflag ) {
        char *err = FileMatchInit( &crx, source_wild );
        if( err != NULL ) {
            Die( "\"%s\": %s\n", err, source_wild );
        }
    }

    /*
     * loop through all files
     */
    while( ( nextdirentry = readdir( directory ) ) != NULL ) {
        /*
         * set up file name, then try to copy it
         */
        FNameLower( nextdirentry->d_name );
        if( rxflag ) {
            if( !FileMatch( crx, nextdirentry->d_name ) ) {
                continue;
            }
        } else {
            if( !FileMatchNoRx( nextdirentry->d_name, source_wild ) ) {
                continue;
            }
        }
        new_source_tail = pathCopy( source_head, source_tail,
            nextdirentry->d_name );
        new_dest_tail = pathCopy( dest_head, dest_tail, nextdirentry->d_name );

        if( nextdirentry->d_attr & _A_SUBDIR ) {

            if( !IsDotOrDotDot( nextdirentry->d_name ) && rflag ) {
                int     rc;

                rc = mkdir( dest_head );
                if( !rc ) {
                    DirCnt++;
                }
                if( !sflag ) {
                    if( rc ) {
                        PrintALineThenDrop( "directory %s already exists",
                            dest_head );
                    } else {
                        PrintALineThenDrop( "created new directory %s",
                            dest_head );
                    }
                }
                new_dest_tail = pathCopy( dest_head, new_dest_tail,
                    FILESEPSTR );
                new_source_tail = pathCopy( source_head, new_source_tail,
                    FILESEPSTR );
                recursiveCp( source_head, new_source_tail,
                    rxflag ? "*" : "*.*",
                    dest_head, new_dest_tail );
            }

        } else {

            CopyOneFile( dest_head, source_head );

        }

    }
    closedir( directory );
    if( rxflag ) {
        FileMatchFini( crx );
    }

} /* DoCP */


/*
 * DoCP - copy a file to a directory
 */
void DoCP( char *f, char *dir )
{
    char                source_buf[ _MAX_PATH ];
    char                *source_tail;
    char                dest_buf[ _MAX_PATH ];
    char                *dest_tail;
    size_t              i;

    /*
     * get file path prefix
     */
    source_tail = source_buf;
    i = strlen( f );
    while( i > 0 ) {
        --i;
        if( f[i] == ':' || isFILESEP( f[i] ) ) {
            source_tail = (char *)memcpy( source_buf, f, i + 1 ) + i + 1;
            break;
        }
    }
    *source_tail = 0;
    strcpy( dest_buf, dir );
    dest_tail = dest_buf + strlen( dest_buf );
    recursiveCp( source_buf, source_tail, f + i + 1, dest_buf, dest_tail );

} /* DoCP */


/*
 * CopyOneFile  - copy one file to another
 */
void CopyOneFile( char *dest, char *src )
{


    struct stat stat_s,stat_d;
    int         i;
    unsigned    srcattr;

    /*
     * first, check if source exists
     */
    if( stat( src,&stat_s ) == - 1 ) {
        DropPrintALine("file \"%s\" not found.",src );
        return;
    }
    _dos_getfileattr( src, &srcattr );

    /*
     * check if the archive bit is set; if not, go back
     */
    if( aflag ) {
        if( !(srcattr & _A_ARCH) ) {
            return;
        }
    }

    /*
     * if destination exists, make sure we can overwrite it
     */
    if( stat( dest,&stat_d ) != -1 ) {

#if !( defined( __OS2__ ) && defined( __386__ ) )
        if( sameFile( dest, &stat_d, src, &stat_s ) ) {
            DropPrintALine( "%s and %s the same file, copy failed",src,dest );
            return;
        }
#endif

        if( !(stat_d.st_mode & S_IWRITE) ) {
            if( !fflag ) {
                DropPrintALine( "destination file %s is read only - use cp -f", dest);
                return;
            } else {
                chmod( dest, S_IWRITE | S_IREAD );
            }
        }

        if( iflag ) {
            PrintALine( "overwrite %s (y\\n)", dest );
            i = 0;
            while( i != 'y' && i != 'n' ) {
                i=getch();
            }
            DropALine();
            if( i=='n' ) {
                return;
            }
        }

    }

    /*
     * copy the file, and if it works, reset archive flag
     * of source (if needed)
     */
    if( !GrabFile( src, &stat_s, dest, srcattr ) ) {
        return;
    }

    if( aflag ) {
        _dos_setfileattr( src, srcattr & (~_A_ARCH) );
    }

} /* CopyOneFile */

/*
 * IOError - fatal I/O error encountered
 */
void IOError( int error )
{
    printf( "\nDOS error[%d] %s\n", error, strerror( error ) );
    exit( 1 );

} /* IOError */

#if defined( __OS2__ ) && defined( __386__ )
/*
 * OS2Error - fatal I/O error encountered
 */
void OS2Error( int error )
{
    #define MAX_MSG 128
    char buffer[MAX_MSG];
    unsigned long len;

    DosGetMessage( NULL, 0, buffer, MAX_MSG, error, "OSO001.MSG", &len );
    buffer[len] = '\0';
    printf( "%s", buffer );
    exit( 1 );

} /* OS2Error */
#endif

