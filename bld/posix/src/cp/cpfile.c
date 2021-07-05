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
* Description:  Performs enhanced Unix cp file I/O.
*
****************************************************************************/


#include "cp.h"
#include <conio.h>
#include <errno.h>
#include "fnutils.h"
#include "filerx.h"

#include "clibext.h"


#define _osmode_REALMODE()  (_osmode == DOS_MODE)

#if defined( _M_IX86 )
extern long DosGetFullPath( char __FAR *org, char __FAR *real );
#if defined( _M_I86 )
#pragma aux DosGetFullPath = \
        "push ds"       \
        "mov  si,ax"    \
        "mov  ds,dx"    \
        "mov  ah,60H"   \
        "int 21h"       \
        "sbb  dx,dx"    \
        "pop  ds"       \
    __parm      [__dx __ax] [__es __di] \
    __value     [__dx __ax] \
    __modify    [__si]
#else
#pragma aux DosGetFullPath = \
        "push ds"       \
        "mov  esi,eax"  \
        "mov  ds,dx"    \
        "mov  ah,60H"   \
        "int 21h"       \
        "sbb  edx,edx"  \
        "mov  dx,ax"    \
        "pop  ds"       \
    __parm      [__dx __eax] [__es __edi] \
    __value     [__edx] \
    __modify    [__esi]
#endif
#endif


#if defined( __OS2__ ) && !defined( _M_I86 )

// copy by OS

#else

/*
 * osSameFile - OS specific same file test
 */
static int osSameFile( char *dest, char *src )
{
    char        fullsrc[_MAX_PATH];
    char        fulldest[_MAX_PATH];
#ifdef __NT__
    char *fp;

    GetFullPathName( dest, _MAX_PATH, fulldest, &fp );
    GetFullPathName( src, _MAX_PATH, fullsrc, &fp );
    return( stricmp( fulldest, fullsrc ) == 0 );
#elif defined( __DOS__ )
    DosGetFullPath( dest, fulldest );
    DosGetFullPath( src, fullsrc );
    return( stricmp( fulldest, fullsrc ) == 0 );
#elif defined( __OS2__ )
    if( _osmode_REALMODE() ) {
        DosGetFullPath( dest, fulldest );
        DosGetFullPath( src, fullsrc );
        return( stricmp( fulldest, fullsrc ) == 0 );
    }
    return( true );
#else
    return( true );
#endif

} /* osSameFile */

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
    return( false );

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

    DIR                 *dirp;
    struct dirent       *dire;
    void                *crx = NULL;
    char                *new_source_tail;
    char                *new_dest_tail;

    pathCopy( source_head, source_tail, "*.*" );

    dirp = opendir( source_head );
    if( dirp == NULL ) {
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
    while( (dire = readdir( dirp )) != NULL ) {
        /*
         * set up file name, then try to copy it
         */
        FNameLower( dire->d_name );
        if( rxflag ) {
            if( !FileMatch( crx, dire->d_name ) ) {
                continue;
            }
        } else {
            if( !FileMatchNoRx( dire->d_name, source_wild ) ) {
                continue;
            }
        }
        new_source_tail = pathCopy( source_head, source_tail, dire->d_name );
        new_dest_tail = pathCopy( dest_head, dest_tail, dire->d_name );

        if( dire->d_attr & _A_SUBDIR ) {
            if( IsDotOrDotDot( dire->d_name ) ) {
                continue;
            }

            if( rflag ) {
                int     rc;

                rc = mkdir( dest_head );
                if( rc == 0 ) {
                    DirCnt++;
                }
                if( !sflag ) {
                    if( rc ) {
                        PrintALineThenDrop( "directory %s already exists", dest_head );
                    } else {
                        PrintALineThenDrop( "created new directory %s", dest_head );
                    }
                }
                new_dest_tail = pathCopy( dest_head, new_dest_tail, FILESEPSTR );
                new_source_tail = pathCopy( source_head, new_source_tail, FILESEPSTR );
                recursiveCp( source_head, new_source_tail,
                    rxflag ? "*" : "*.*",
                    dest_head, new_dest_tail );
            }

        } else {

            CopyOneFile( dest_head, source_head );

        }

    }
    closedir( dirp );
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
    fattrs      srcattr;

    /*
     * first, check if source exists
     */
    if( stat( src,&stat_s ) == - 1 ) {
        DropPrintALine("file \"%s\" not found.",src );
        return;
    }
#ifdef __NT__
    srcattr = GetFileAttributes( src );

    /*
     * check if the archive bit is set; if not, go back
     */
    if( aflag ) {
        if( (srcattr & FILE_ATTRIBUTE_ARCHIVE) == 0 ) {
            return;
        }
    }
#else
    _dos_getfileattr( src, &srcattr );

    /*
     * check if the archive bit is set; if not, go back
     */
    if( aflag ) {
        if( (srcattr & _A_ARCH) == 0 ) {
            return;
        }
    }
#endif

    /*
     * if destination exists, make sure we can overwrite it
     */
    if( stat( dest,&stat_d ) != -1 ) {

#if defined( __OS2__ ) && !defined( _M_I86 )

// copy by OS

#else
        if( sameFile( dest, &stat_d, src, &stat_s ) ) {
            DropPrintALine( "%s and %s the same file, copy failed",src,dest );
            return;
        }
#endif

        if( (stat_d.st_mode & S_IWRITE) == 0 ) {
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
                i = getch();
            }
            DropALine();
            if( i == 'n' ) {
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
#ifdef __NT__
        SetFileAttributes( src, srcattr & ~FILE_ATTRIBUTE_ARCHIVE );
#else
        _dos_setfileattr( src, srcattr & ~_A_ARCH );
#endif
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

#if defined( __OS2__ ) && !defined( _M_I86 )
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
