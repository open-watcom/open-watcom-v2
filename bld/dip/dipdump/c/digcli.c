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
* Description:  Minimal implementation of DIG client routines.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include "bool.h"
#include "wio.h"
#include "iopath.h"
#include "digtypes.h"
#include "digcli.h"
#include "digld.h"
#include "pathlist.h"
#include "dipdump.h"


#if 0
# define dprintf(a)         do { printf a; } while( 0 )
#else
# define dprintf(a)         do {} while( 0 )
#endif

#if defined( __UNIX__ ) || defined( __DOS__ )
typedef struct char_ring {
    struct char_ring    *next;
    char                name[1];
} char_ring;

static char   *FilePathList = NULL;
#endif

void *DIGCLIENTRY( Alloc )( size_t amount )
{
    void    *ptr = malloc( amount );

    dprintf(( "DIGCliAlloc: amount=%#x -> %p\n", (unsigned)amount, ptr ));
    return( ptr );
}

void *DIGCLIENTRY( Realloc )( void *p, size_t amount )
{
    void    *ptr = realloc( p, amount);

    dprintf(( "DIGCliRealloc: p=%p amount=%3x -> %p\n", p, (unsigned)amount, ptr ));
    return( ptr );
}

void DIGCLIENTRY( Free )( void *p )
{
    dprintf(( "DIGCliFree: p=%p\n", p ));
    free( p );
}

dig_fhandle DIGCLIENTRY( Open )( char const *name, dig_open mode )
{
    int     fd;
    int     flgs;

    dprintf(( "DIGCliOpen: name=%p:{%s} mode=%#x\n", name, name, mode ));

    /* convert flags. */
    switch( mode & (DIG_READ | DIG_WRITE) ) {
    case DIG_READ:
        flgs = O_RDONLY;
        break;
    case DIG_WRITE:
        flgs = O_WRONLY;
        break;
    case DIG_WRITE | DIG_READ:
        flgs = O_RDWR;
        break;
    default:
        return( DIG_NIL_HANDLE );
    }
#ifdef O_BINARY
    flgs |= O_BINARY;
#endif
    if( mode & DIG_CREATE )
        flgs |= O_CREAT;
    if( mode & DIG_TRUNC )
        flgs |= O_TRUNC;
    if( mode & DIG_APPEND )
        flgs |= O_APPEND;
    /* (ignore the remaining flags) */

    fd = open( name, flgs, PMODE_RWX );

    dprintf(( "DIGCliOpen: returns %d\n", fd ));
    if( fd == -1 )
        return( DIG_NIL_HANDLE );
    return( DIG_PH2FID( fd ) );
}

unsigned long DIGCLIENTRY( Seek )( dig_fhandle fid, unsigned long p, dig_seek k )
{
    int     whence;
    long    off;

    switch( k ) {
    case DIG_ORG:   whence = SEEK_SET; break;
    case DIG_CUR:   whence = SEEK_CUR; break;
    case DIG_END:   whence = SEEK_END; break;
    default:
        dprintf(( "DIGCliSeek: h=%d p=%ld k=%d -> -1\n", DIG_FID2PH( fid ), p, k ));
        return( DIG_SEEK_ERROR );
    }

    off = lseek( DIG_FID2PH( fid ), p, whence );
    dprintf(( "DIGCliSeek: h=%d p=%ld k=%d -> %ld\n", DIG_FID2PH( fid ), p, k, off ));
    return( off );
}

size_t DIGCLIENTRY( Read )( dig_fhandle fid, void *b , size_t s )
{
    size_t      rc;

    rc = read( DIG_FID2PH( fid ), b, s );
    dprintf(( "DIGCliRead: h=%d b=%p s=%d -> %d\n", DIG_FID2PH( fid ), b, (unsigned)s, (unsigned)rc ));
    return( rc );
}

size_t DIGCLIENTRY( Write )( dig_fhandle fid, const void *b, size_t s )
{
    size_t      rc;

    rc = write( DIG_FID2PH( fid ), b, s );
    dprintf(( "DIGCliWrite: h=%d b=%p s=%d -> %d\n", DIG_FID2PH( fid ), b, (unsigned)s, (unsigned)rc ));
    return( rc );
}

void DIGCLIENTRY( Close )( dig_fhandle fid )
{
    dprintf(( "DIGCliClose: h=%d\n", DIG_FID2PH( fid ) ));
    if( close( DIG_FID2PH( fid ) ) ) {
        dprintf(( "DIGCliClose: h=%d failed!!\n", DIG_FID2PH( fid ) ));
    }
}

void DIGCLIENTRY( Remove )( char const *name, dig_open mode )
{
    dprintf(( "DIGCliRemove: name=%p:{%s} mode=%#x\n", name, name, mode ));
    unlink( name );
    mode = mode;
}

unsigned DIGCLIENTRY( MachineData )( address addr, dig_info_type info_type,
                        dig_elen in_size,  const void *in,
                        dig_elen out_size, void *out )
{
    dprintf(( "DIGCliMachineData: \n" ));
    return( 0 ); /// @todo check this out out.
}

#if defined( __UNIX__ ) || defined( __DOS__ )

static char *addPath( char *old_list, const char *path_list )
/***********************************************************/
{
    size_t          len;
    size_t          old_len;
    char            *new_list;
    char            *p;

    new_list = old_list;
    if( path_list != NULL && *path_list != '\0' ) {
        len = strlen( path_list );
        if( old_list == NULL ) {
            p = new_list = malloc( len + 1 );
        } else {
            old_len = strlen( old_list );
            new_list = malloc( old_len + 1 + len + 1 );
            memcpy( new_list, old_list, old_len );
            free( old_list );
            p = new_list + old_len;
        }
        while( *path_list != '\0' ) {
            if( p != new_list )
                *p++ = PATH_LIST_SEP;
            path_list = GetPathElement( path_list, NULL, &p );
        }
        *p = '\0';
    }
    return( new_list );
}

static char *findFile( char *fullname, char *name, char *path_list )
/******************************************************************/
{
    int         fh;
    char        *p;
    char        c;

    fh = open( name, O_RDONLY | O_BINARY, S_IREAD );
    if( fh != -1 ) {
        close( fh );
        strcpy( fullname, name );
        return( fullname );
    }
    if( path_list != NULL ) {
        while( (c = *path_list) != '\0' ) {
            p = fullname;
            do {
                ++path_list;
                if( IS_PATH_LIST_SEP( c ) )
                    break;
                *p = c;
            } while( (c = *path_list) != '\0' );
            c = p[-1];
            if( !IS_PATH_SEP( c ) ) {
                *p++ = DIR_SEP;
            }
            strcat( p, name );
            fh = open( fullname, O_RDONLY | O_BINARY, S_IREAD );
            if( fh != -1 ) {
                close( fh );
                return( fullname );
            }
        }
    }
    return( NULL );
}

void PathInit( void )
/*******************/
{
    char        buff[_MAX_PATH];
    char        *p;

    if( _cmdname( buff ) != NULL ) {
#if defined( __UNIX__ )
        p = strrchr( buff, '/' );
#else
        p = strrchr( buff, '\\' );
#endif
        if( p != NULL ) {
            *p = '\0';
            FilePathList = addPath( FilePathList, buff );
        }
    }
    FilePathList = addPath( FilePathList, getenv( "PATH" ) );
#if defined(__UNIX__)
    FilePathList = addPath( FilePathList, getenv( "WD_PATH" ) );
    FilePathList = addPath( FilePathList, "/usr/watcom/wd" );
    FilePathList = addPath( FilePathList, "/opt/watcom/wd" );
#endif
}

void PathFini( void )
{
    free( FilePathList );
}

dig_fhandle DIGLoader( Open )( const char *name, size_t name_len, const char *ext, char *result, size_t max_result )
/******************************************************************************************************************/
{
    char        realname[ _MAX_PATH2 ];
    char        *filename;
    int         fd;

    /* unused parameters */ (void)max_result;

    memcpy( realname, name, name_len );
    realname[name_len] = '\0';
    if( ext != NULL && *ext != '\0' ) {
        _splitpath2( realname, result, NULL, NULL, &filename, NULL );
        _makepath( realname, NULL, NULL, filename, ext );
    }
    filename = findFile( result, realname, FilePathList );
    fd = -1;
    if( filename != NULL )
        fd = open( filename, O_RDONLY );
    if( fd == -1 )
        return( DIG_NIL_HANDLE );
    return( DIG_PH2FID( fd ) );
}

int DIGLoader( Read )( dig_fhandle fid, void *buff, unsigned len )
{
    unsigned read_len;
    read_len = read( DIG_FID2PH( fid ), buff, len );
printf("read: in: %x  out: %x\n", len, read_len);
    return( read_len != len );
}

int DIGLoader( Seek )( dig_fhandle fid, unsigned long offs, dig_seek where )
{
    unsigned long pos;

    pos = lseek( DIG_FID2PH( fid ), offs, where );
printf("seek: in: %lx  out: %lx\n", offs, pos);
    return( pos == -1L );
}

int DIGLoader( Close )( dig_fhandle fid )
{
    return( close( DIG_FID2PH( fid ) ) != 0 );
}

#endif
