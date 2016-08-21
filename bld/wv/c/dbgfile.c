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
* Description:  Local and remote file access routines.
*
****************************************************************************/


#include <stdlib.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include <process.h>
#endif
#include "wio.h"
#include "dbgdefn.h"
#if !defined( BUILD_RFX )
#include "dbgdata.h"
#include "dbglit.h"
#include "dui.h"
#else
#include "rfxdata.h"
#endif
#include "dbgmem.h"
#include "dbgio.h"
#include "digio.h"
#include "strutil.h"
#include "filelcl.h"
#include "filermt.h"
#include "diptypes.h"
#include "dbgsrc.h"
#include "remfile.h"
#include "dbgerr.h"

#include "clibext.h"


#define CHK_DIR_SEP(c,i)    ((c) != NULLCHAR && ((c) == (i)->path_separator[0] || (c) == (i)->path_separator[1]))
#define CHK_DRV_SEP(c,i)    ((c) != NULLCHAR && (c) == (i)->drv_separator)

#define CHECK_PATH_SEP(c,i) (CHK_DIR_SEP((c),i) || CHK_DRV_SEP((c),i))
#define CHECK_PATH_ABS(p,i) (CHK_DIR_SEP((p)[0],i) || (p)[0] != NULLCHAR && CHK_DRV_SEP((p)[1],i) && CHK_DIR_SEP((p)[2],i))

static char_ring *LclPath;

#define LOC_ESCAPE      '@'
#define REMOTE_LOC      'r'
#define LOCAL_LOC       'l'
#define REMOTE_IND      0x8000
#define MAX_OPENS       100
#define MAX_ERRORS      10

#define SYSHANDLE(h)    SysHandles[h & ~REMOTE_IND]
#define SYSERROR(e)     SysErrors[(e & ~REMOTE_IND) - 1]
#define ISREMOTE(x)     ((x & REMOTE_IND) != 0)
#define SETREMOTE(x)    x |= REMOTE_IND

static sys_handle       SysHandles[MAX_OPENS];
static sys_error        SysErrors[MAX_ERRORS];
static error_handle     ErrRover;
static error_handle     LastErr;

const char  *RealFName( char const *name, open_access *loc )
{
    *loc &= ~(OP_REMOTE|OP_LOCAL);
    if( name[0] == LOC_ESCAPE ) {
        if( (name[1] | 0x20) == REMOTE_LOC ) {
            *loc |= OP_REMOTE;
            name += 2;
        } else if( (name[1] | 0x20) == LOCAL_LOC ) {
            *loc |= OP_LOCAL;
            name += 2;
        } else if( name[1] == LOC_ESCAPE ) {
            name += 1;
        }
    }
    return( name );
}

static open_access DefaultLoc( open_access loc )
{
    if( (loc & (OP_REMOTE|OP_LOCAL) ) == 0 ) {
        if( _IsOn( SW_REMOTE_FILES ) ) {
            loc |= OP_REMOTE;
        } else {
            loc |= OP_LOCAL;
        }
    }
    if( (loc & OP_REMOTE) && !HaveRemoteFiles() ) {
        loc &= ~OP_REMOTE;
        loc |= OP_LOCAL;
    }
    return( loc );
}

const char  *FileLoc( char const *name, open_access *loc )
{
    open_access ind;

    ind = 0;
    name = RealFName( name, &ind );
    if( ind != 0 ) {
        *loc &= ~(OP_LOCAL|OP_REMOTE);
        *loc |= ind;
    }
    *loc = DefaultLoc( *loc );
    return( name );
}


static file_components *PathInfo( char const *path, open_access loc )
{
    file_components     *info;

    FileLoc( path, &loc );
    if( loc & OP_LOCAL ) {
        info = &LclFile;
    } else {
        info = &RemFile;
    }
    return( info );
}

static file_handle FindFreeHandle( void )
{
    file_handle     fh;

    for( fh = 0; fh < MAX_OPENS; ++fh ) {
        if( SYSHANDLE( fh ) == NIL_SYS_HANDLE ) {
            return( fh );
        }
    }
    return( NIL_HANDLE );
}

size_t ReadStream( file_handle fh, void *b, size_t l )
{
    sys_handle  sys;

    sys = SYSHANDLE( fh );
    if( ISREMOTE( fh ) ) {
        return( RemoteRead( sys, b, l ) );
    } else {
        return( LocalRead( sys, b, l ) );
    }
}

size_t ReadText( file_handle fh, void *b, size_t l )
{
    return( ReadStream( fh, b, l ) );
}

size_t WriteStream( file_handle fh, const void *b, size_t l )
{
    sys_handle  sys;

    sys = SYSHANDLE( fh );
    if( ISREMOTE( fh ) ) {
        return( RemoteWrite( sys, b, l ) );
    } else {
        return( LocalWrite( sys, b, l ) );
    }
}

size_t WriteNL( file_handle fh )
{
    char    *nl;

    if( ISREMOTE( fh ) ) {
        nl = RemFile.newline;
    } else {
        nl = LclFile.newline;
    }
    return( WriteStream( fh, nl, (nl[1] != NULLCHAR) ? 2 : 1 ) );
}

size_t WriteText( file_handle fh, const void *b, size_t len )
{
    len = WriteStream( fh, b, len );
    WriteNL( fh );
    return( len );   /* not including the newline sequence */
}

unsigned long SeekStream( file_handle fh, long p, seek_method m )
{
    sys_handle  sys;

    sys = SYSHANDLE( fh );
    if( ISREMOTE( fh ) ) {
        return( RemoteSeek( sys, p, m ) );
    } else {
        return( LocalSeek( sys, p, m ) );
    }
}

file_handle FileOpen( const char *name, open_access o )
{
    sys_handle  sys;
    file_handle fh;

    if( o & OP_SEARCH ) {
        return( PathOpen( name, strlen( name ), "" ) );
    }
    name = FileLoc( name, &o );
    fh = FindFreeHandle();
    if( fh == NIL_HANDLE )
        return( NIL_HANDLE );
    if( o & OP_REMOTE ) {
        SETREMOTE( fh );
        sys = RemoteOpen( name, o );
    } else {
        sys = LocalOpen( name, o );
    }
    if( sys == NIL_SYS_HANDLE )
        return( NIL_HANDLE );
    SYSHANDLE( fh ) = sys;
    if( o & OP_APPEND )
        SeekStream( fh, 0, DIO_SEEK_END );
    return( fh );
}

error_handle FileClose( file_handle fh )
{
    sys_handle  sys;

    sys = SYSHANDLE( fh );
    SYSHANDLE( fh ) = NIL_SYS_HANDLE;
    if( ISREMOTE( fh ) ) {
        return( RemoteClose( sys ) );
    } else {
        return( LocalClose( sys ) );
    }
}


error_handle FileRemove( char const *name, open_access loc )
{
    name = FileLoc( name, &loc );
    if( loc & OP_REMOTE ) {
        return( RemoteErase( name ) );
    } else {
        return( LocalErase( name ) );
    }
}

void WriteToPgmScreen( const void *buff, size_t len )
{
#if !defined( BUILD_RFX )
    DUIWndUser();
#endif
    RemoteWriteConsole( buff, len );
}

open_access FileHandleInfo( file_handle fh )
{
    if( ISREMOTE( fh ) )
        return( OP_REMOTE );
    return( OP_LOCAL );
}

char *SysErrMsg( error_handle errh, char *buff )
{
    sys_error   syserr;

    syserr = SYSERROR( errh );
    if( ISREMOTE( errh ) ) {
        RemoteErrMsg( syserr, buff );
    } else {
        LocalErrMsg( syserr, buff );
    }
    return( buff + strlen( buff ) );
}

error_handle StashErrCode( sys_error syserr, open_access loc )
{
    error_handle    errh;

    if( syserr == 0 )
        return( 0 );
    if( ++ErrRover >= MAX_ERRORS )
        ErrRover = 0;
    errh = ErrRover + 1;
    SYSERROR( errh ) = syserr;
    if( loc & OP_REMOTE )
        SETREMOTE( errh );
    LastErr = errh;
    return( errh );
}

/* for RFX */
error_handle GetLastErr( void )
{
    return( LastErr );
}

/* for RFX */
sys_error GetSystemErrCode( error_handle errh )
{
    if( errh == 0 )
        return( 0 );
    return( SYSERROR( errh ) );
}

/* for RFX */
sys_handle GetSystemHandle( file_handle fh )
{
    return( SYSHANDLE( fh ) );
}

bool IsAbsolutePath( const char *path )
{
    file_components     *info;
    const char          *p;
    open_access         loc;

    p = RealFName( path, &loc );
    info = PathInfo( p, loc );
    if( strlen( p ) == 0 )
        return( false );
    return( CHECK_PATH_ABS( p, info ) );
}

char *AppendPathDelim( char *path, open_access loc )
{
    file_components     *info;
    size_t              len;
    char                *end;

    info = PathInfo( path, loc );
    len = strlen( path );
    end = path + len;
    if( len == 0 || !CHECK_PATH_SEP( end[-1], info ) ) {
        *end++ = info->path_separator[0];
    }
    return( end );
}

const char  *SkipPathInfo( char const *path, open_access loc )
{
    file_components     *info;
    const char          *name;
    char                c;

    name = path;
    info = PathInfo( path, loc );
    while( (c = *path++) != NULLCHAR ) {
        if( CHECK_PATH_SEP( c, info ) ) {
            name = path;
        }
    }
    return( name );
}


const char  *ExtPointer( char const *path, open_access loc )
{
    file_components     *info;
    const char          *p;
    const char          *end;
    char                c;

    p = end = path + strlen( path );
    info = PathInfo( path, loc );
    for( ;; ) {
        c = *--p;
        if( p < path )
            return( end );
        if( CHECK_PATH_SEP( c, info ) )
            return( end );
        if( c == info->ext_separator ) {
            return( p );
        }
    }
}


size_t MakeFileName( char *result, const char *name, const char *ext, open_access loc )
{
    file_components     *info;
    char                *p;

    p = StrCopy( name, result );
    if( *ExtPointer( result, loc ) == NULLCHAR ) {
        info = PathInfo( name, loc );
        *p++ = info->ext_separator;
        p = StrCopy( ext, p );
    }
    return( p - result );
}

static size_t MakeNameWithPath( open_access loc,
                                const char *path, size_t plen,
                                const char *name, size_t nlen, char *res )
{
    file_components     *info;
    char                *p;

    p = res;
    if( loc & OP_REMOTE ) {
        *p++ = LOC_ESCAPE;
        *p++ = REMOTE_LOC;
    } else if( loc & OP_LOCAL ) {
        *p++ = LOC_ESCAPE;
        *p++ = LOCAL_LOC;
    } else {
        loc = DefaultLoc( loc );
    }
    if( path != NULL ) {
        memcpy( p, path, plen );
        p += plen;
        if( loc & OP_LOCAL ) {
            info = &LclFile;
        } else {
            info = &RemFile;
        }
        if( plen > 0 && !CHECK_PATH_SEP( p[-1], &LclFile ) ) {
            *p++ = info->path_separator[0];
        }
    }
    memcpy( p, name, nlen );
    p += nlen;
    *p = NULLCHAR;
    return( p - res );
}


file_handle LclStringToFullName( const char *name, size_t len, char *full )
{
    char_ring   *curr;
    file_handle fh;
    size_t      plen;

    // check open file in current directory or in full path
    MakeNameWithPath( OP_LOCAL, NULL, 0, name, len, full );
    fh = FileOpen( full, OP_READ );
    if( fh != NIL_HANDLE )
        return( fh );
    // check open file in debugger directory list
    for( curr = LclPath; curr != NULL; curr = curr->next ) {
        plen = strlen( curr->name );
        MakeNameWithPath( OP_LOCAL, curr->name, plen, name, len, full );
        fh = FileOpen( full, OP_READ );
        if( fh != NIL_HANDLE ) {
            return( fh );
        }
    }
    return( NIL_HANDLE );
}


/*
 *
 */
static file_handle FullPathOpenInternal( const char *name, size_t name_len, const char *ext,
                                    char *result, size_t max_result, bool force_local )
{
    char        buffer[TXT_LEN];
    char        *p;
    const char  *p1;
    open_access loc;
    bool        have_ext;
    bool        have_path;
    file_components *file;
    file_handle fh;
    char        c;

    loc = 0;
    p1 = FileLoc( name, &loc );
    name_len -= p1 - name;
    name = p1;
    have_ext = false;
    have_path = false;
    if( force_local ) {
        loc &= ~OP_REMOTE;
        loc |= OP_LOCAL;
    }
    if( loc & OP_LOCAL ) {
        file = &LclFile;
    } else {
        file = &RemFile;
    }
    p = buffer;
    while( name_len-- > 0 ) {
        c = *name++;
        *p++ = c;
        if( CHECK_PATH_SEP( c, file ) ) {
            have_ext = false;
            have_path = true;
        } else if( c == file->ext_separator ) {
            have_ext = true;
        }
    }
    if( !have_ext ) {
        *p++ = file->ext_separator;
        p = StrCopy( ext, p );
    }
    *p = NULLCHAR;
    if( loc & OP_REMOTE ) {
        RemoteStringToFullName( false, buffer, result, (trap_elen)max_result );
        fh = FileOpen( result, OP_READ | OP_REMOTE );
    } else if( have_path ) {
        StrCopy( buffer, result );
        fh = FileOpen( buffer, OP_READ );
    } else {
        fh = LclStringToFullName( buffer, p - buffer, result );
    }
    if( fh == NIL_HANDLE ) {
        strcpy( result, buffer );
    } else {
        p1 = RealFName( result, &loc );
        memmove( result, p1, strlen( p1 ) + 1 );
    }
    return( fh );
}

file_handle FullPathOpen( const char *name, size_t name_len, const char *ext, char *result, size_t max_result )
{
    return( FullPathOpenInternal( name, name_len, ext, result, max_result, false ) );
}

file_handle LocalFullPathOpen( const char *name, size_t name_len, const char *ext, char *result, size_t max_result )
{
    return( FullPathOpenInternal( name, name_len, ext, result, max_result, true ) );
}

static file_handle PathOpenInternal( const char *name, size_t name_len, const char *ext, bool force_local )
{
    char        result[TXT_LEN];

    if( force_local ) {
        return( LocalFullPathOpen( name, name_len, ext, result, TXT_LEN ) );
    } else {
        return( FullPathOpen( name, name_len, ext, result, TXT_LEN ) );
    }
}

file_handle PathOpen( const char *name, size_t name_len, const char *ext )
{
    return( PathOpenInternal( name, name_len, ext, false ) );
}

file_handle LocalPathOpen( const char *name, size_t name_len, const char *ext )
{
    return( PathOpenInternal( name, name_len, ext, true ) );
}

#if !defined( BUILD_RFX )
static bool IsWritable( char const *name, open_access loc )
{
    file_handle     fh;

    fh = FileOpen( name, OP_READ | loc );
    if( fh == NIL_HANDLE ) {
        fh = FileOpen( name, OP_WRITE | OP_CREATE | loc );
        if( fh != NIL_HANDLE ) {
            FileClose( fh );
            FileRemove( name, loc );
            return( true );
        }
    } else {
        FileClose( fh );
        fh = FileOpen( name, OP_WRITE | loc );
        if( fh != NIL_HANDLE ) {
            FileClose( fh );
            return( true );
        }
    }
    return( false );
}

bool FindWritable( char const *src, char *dst )
{
    char        buffer[TXT_LEN];
    size_t      plen;
    size_t      nlen;
    const char  *name;
    open_access loc;

    loc = 0;
    src = RealFName( src, &loc );
    if( IsWritable( src, loc ) ) {
        StrCopy( src, dst );
        return( true );
    }
    name = SkipPathInfo( src, loc );
    nlen = strlen( name );
    if( DefaultLoc( loc ) & OP_LOCAL ) {
        plen = DUIEnvLkup( "HOME", buffer, sizeof( buffer ) );
        if( plen > 0 ) {
            MakeNameWithPath( loc, buffer, plen, name, nlen, dst );
            if( IsWritable( dst, loc ) ) {
                return( true );
            }
        }
    }
    MakeNameWithPath( loc, NULL, 0, name, nlen, dst );
    return( IsWritable( dst, loc ) );
}
#endif

void SysFileInit( void )
{
    file_handle     fh;

    for( fh = 0; fh < MAX_OPENS; ++fh ) {
        SYSHANDLE( fh ) = NIL_SYS_HANDLE;
    }
    SYSHANDLE( STD_IN ) = LocalHandleSys( STD_IN  );
    SYSHANDLE( STD_OUT ) = LocalHandleSys( STD_OUT );
    SYSHANDLE( STD_ERR ) = LocalHandleSys( STD_ERR );
}

#if !defined( BUILD_RFX )

void PathFini( void )
{
    FreeRing( LclPath );
}

/*
 * parsePathList -- parse string with list of path into separate pieces
 *                  and add it into local path ring
 */

static void parsePathList( char_ring **owner, char *src )
{
    char       *start, *end;
    size_t     len;
    char_ring  *new;

    // find end of list
    while( *owner != NULL )
        owner = &(*owner)->next;
    // add items to the end of list
    for( start = end = src; ; ++end ) {
        if( *end == LclPathSep || *end == NULLCHAR ) {
            while( *start == ' ' ) {
                ++start;
            }
            len = end - start;
            while( len > 0 && start[len - 1] == ' ' ) {
                --len;
            }
            _Alloc( new, sizeof( char_ring ) + len );
            if( new == NULL )
                break;
            *owner = new;
            owner = &new->next;
            new->next = NULL;
            memcpy( new->name, start, len );
            new->name[len] = NULLCHAR;
            if( *end == NULLCHAR )
                return;
            start = end + 1;
        }
    }
}

static void parseEnvVar( char_ring **owner, const char *name )
{
    char        *buff;
    size_t      size;

    size = DUIEnvLkup( name, NULL, 0 );
    if( size > 0 ) {
        ++size;
        _Alloc( buff, size );   /* allocate enough room for a very long PATH */
        if( buff == NULL ) {
            StartupErr( LIT_ENG( ERR_NO_MEMORY ) );
        }
        if( DUIEnvLkup( name, buff, size ) != 0 ) {
            parsePathList( owner, buff );
        }
        _Free( buff );
    }
}

#endif

void PathInit( void )
{
#if !defined( BUILD_RFX )
    char        cmd[_MAX_PATH];
    char        *p;

//    parsePathList( &LclPath, "." );
    parseEnvVar( &LclPath, "WD_PATH" );
    parseEnvVar( &LclPath, "HOME" );
    if( _cmdname( cmd ) != NULL ) {
        p = strrchr( cmd, LclFile.path_separator[0] );
        if( p != NULL ) {
            *p = NULLCHAR;
            /* look in the executable's directory */
            parsePathList( &LclPath, cmd );
            p = strrchr( cmd, LclFile.path_separator[0] );
            if( p != NULL ) {
                /* look in a sibling directory of where the executable is */
                memcpy( p + 1, "wd", 3 );
                parsePathList( &LclPath, cmd );
            }
        }
    }
  #if defined( __UNIX__ )
    /* look in "/opt/watcom/wd" */
    parsePathList( &LclPath, "/opt/watcom/wd" );
  #endif
    parseEnvVar( &LclPath, "PATH" );
#endif
}

#if defined( __DOS__ ) || defined( __UNIX__ )
static int MakeNameWithPathOpen( const char *path, const char *name, size_t nlen, char *res, size_t rlen )
{
    char        *p;
    size_t      len;

    if( rlen < 2 ) {
        if( rlen == 1 )
            *res = NULLCHAR;
        return( -1 );
    }
    p = res;
    --rlen;     // save space for terminator
    len = 0;
    if( path != NULL ) {
        len = strlen( path );
        if( len > rlen )
            len = rlen;
        memcpy( p, path, len );
        p += len;
        if( !CHECK_PATH_SEP( p[-1], &LclFile ) ) {
            if( len < rlen ) {
                *p++ = LclFile.path_separator[0];
                ++len;
            }
        }
    }
    if( nlen > 0 && len < rlen ) {
        if( len + nlen > rlen )
            nlen = rlen - len;
        memcpy( p, name, nlen );
        p += nlen;
    }
    *p = NULLCHAR;
    return( open( res, O_RDONLY ) );
}

dig_lhandle DIGLoadOpen( const char *name, size_t name_len, const char *ext, char *buff, size_t buff_size )
{
    char            buffer[TXT_LEN];
    char            *p;
    bool            have_ext;
    bool            have_path;
    char            c;
    char            dummy[TXT_LEN];
    int             fh;
    char_ring       *curr;

    if( buff == NULL ) {
        buff = dummy;
        buff_size = sizeof( dummy );
    }
    have_ext = false;
    have_path = false;
    p = buffer;
    while( name_len-- > 0 ) {
        c = *name++;
        *p++ = c;
        if( CHECK_PATH_SEP( c, &LclFile ) ) {
            have_ext = false;
            have_path = true;
        } else if( c == LclFile.ext_separator ) {
            have_ext = true;
        }
    }
    if( !have_ext ) {
        *p++ = LclFile.ext_separator;
        p = StrCopy( ext, p );
    }
    *p = NULLCHAR;
    if( have_path ) {
        StrCopy( buffer, buff );
        fh = open( buffer, O_RDONLY );
    } else {
        // check open file in current directory or in full path
        fh = MakeNameWithPathOpen( NULL, name, name_len, buff, buff_size );
        if( fh == -1 ) {
            // check open file in debugger directory list
            for( curr = LclPath; curr != NULL; curr = curr->next ) {
                fh = MakeNameWithPathOpen( curr->name, name, name_len, buff, buff_size );
                if( fh != -1 ) {
                    break;
                }
            }
        }
    }
    if( fh == -1 ) {
        strcpy( buff, buffer );
        return( DIG_NIL_LHANDLE );
    }
    return( fh );
}

int DIGLoadRead( dig_lhandle fh, void *buff, unsigned len )
{
    return( read( fh, buff, len ) != len );
}

int DIGLoadSeek( dig_lhandle fh, unsigned long offs, dig_seek where )
{
    return( lseek( fh, offs, where ) == -1L );
}

int DIGLoadClose( dig_lhandle lfh )
{
    return( close( lfh ) );
}
#endif
