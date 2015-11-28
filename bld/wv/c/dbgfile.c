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


#define CHK_DIR_SEP(c,i)    ((c) != '\0' && ((c) == (i)->path_separator[0] || (c) == (i)->path_separator[1]))
#define CHK_DRV_SEP(c,i)    ((c) != '\0' && (c) == (i)->drv_separator)

#define CHECK_PATH_SEP(c,i) (CHK_DIR_SEP((c),i) || CHK_DRV_SEP((c),i))
#define CHECK_PATH_ABS(p,i) (CHK_DIR_SEP((p)[0],i) || (p)[0] != '\0' && CHK_DRV_SEP((p)[1],i) && CHK_DIR_SEP((p)[2],i))

static char_ring *LclPath;

#define LOC_ESCAPE      '@'
#define REMOTE_LOC      'r'
#define LOCAL_LOC       'l'
#define REMOTE_IND      0x8000
#define MAX_OPENS       100
#define MAX_ERRORS      10

static sys_handle       SysHandles[MAX_OPENS];
static sys_error        SysErrors[MAX_ERRORS];
static error_idx        ErrRover;
static error_idx        LastErr;

handle PathOpen( const char *name, unsigned name_len, const char *ext );

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

static handle FindFreeHandle( void )
{
    handle      i;

    for( i = 0; i < MAX_OPENS; ++i ) {
        if( SysHandles[i] == NIL_SYS_HANDLE ) return( i );
    }
    return( NIL_HANDLE );
}

unsigned ReadStream( handle h, void *b, unsigned l )
{
    sys_handle  sys;

    sys = SysHandles[h & ~REMOTE_IND];
    if( h & REMOTE_IND ) {
        return( RemoteRead( sys, b, l ) );
    } else {
        return( LocalRead( sys, b, l ) );
    }
}

unsigned ReadText( handle h, void *b, unsigned l )
{
    return( ReadStream( h, b, l ) );
}

unsigned WriteStream( handle h, const void *b, unsigned l)
{
    sys_handle  sys;

    sys = SysHandles[h & ~REMOTE_IND];
    if( h & REMOTE_IND ) {
        return( RemoteWrite( sys, b, l ) );
    } else {
        return( LocalWrite( sys, b, l ) );
    }
}

unsigned WriteNL( handle h )
{
    char    *nl;

    if( h & REMOTE_IND ) {
        nl = RemFile.newline;
    } else {
        nl = LclFile.newline;
    }
    return( WriteStream( h, nl, (nl[1] != NULLCHAR) ? 2 : 1 ) );
}

unsigned WriteText( handle h, const void *b, unsigned len )
{
    len = WriteStream( h, b, len );
    WriteNL( h );
    return( len );   /* not including the newline sequence */
}

unsigned long SeekStream( handle h, long p, seek_method m )
{
    sys_handle  sys;

    sys = SysHandles[h & ~REMOTE_IND];
    if( h & REMOTE_IND ) {
        return( RemoteSeek( sys, p, m ) );
    } else {
        return( LocalSeek( sys, p, m ) );
    }
}

handle FileOpen( const char *name, open_access o )
{
    sys_handle  sys;
    handle      h;

    if( o & OP_SEARCH ) {
        return( PathOpen( name, strlen( name ), "" ) );
    }
    name = FileLoc( name, &o );
    h = FindFreeHandle();
    if( h == NIL_HANDLE ) return( NIL_HANDLE );
    if( o & OP_REMOTE ) {
        h |= REMOTE_IND;
        sys = RemoteOpen( name, o );
    } else {
        sys = LocalOpen( name, o );
    }
    if( sys == NIL_SYS_HANDLE ) return( NIL_HANDLE );
    SysHandles[ h & ~REMOTE_IND ] = sys;
    if( o & OP_APPEND ) SeekStream( h, 0, DIO_SEEK_END );
    return( h );
}

error_idx FileClose( handle h )
{
    sys_handle  sys;

    sys = SysHandles[h & ~REMOTE_IND];
    SysHandles[h & ~REMOTE_IND] = NIL_SYS_HANDLE;
    if( h & REMOTE_IND ) {
        return( RemoteClose( sys ) );
    } else {
        return( LocalClose( sys ) );
    }
}


error_idx FileRemove( char const *name, open_access loc )
{
    name = FileLoc( name, &loc );
    if( loc & OP_REMOTE ) {
        return( RemoteErase( name ) );
    } else {
        return( LocalErase( name ) );
    }
}

void WriteToPgmScreen( const void *buff, unsigned len )
{
#if !defined( BUILD_RFX )
    DUIWndUser();
#endif
    RemoteWriteConsole( buff, len );
}

open_access FileHandleInfo( handle h )
{
    if( h & REMOTE_IND ) return( OP_REMOTE );
    return( OP_LOCAL );
}

char *SysErrMsg( error_idx code, char *buff )
{
    sys_error   sys;

    sys = SysErrors[(code & ~REMOTE_IND) - 1];
    if( code & REMOTE_IND ) {
        RemoteErrMsg( sys, buff );
    } else {
        LocalErrMsg( sys, buff );
    }
    return( &buff[strlen( buff )] );
}

error_idx StashErrCode( sys_error sys, open_access loc )
{
    error_idx   code;

    if( sys == 0 )
        return( 0 );
    if( ++ErrRover >= MAX_ERRORS )
        ErrRover = 0;
    code = ErrRover;
    SysErrors[code] = sys;
    ++code;
    if( loc & OP_REMOTE )
        code |= REMOTE_IND;
    LastErr = code;
    return( code );
}

/* for RFX */
error_idx GetLastErr( void )
{
    return( LastErr );
}

/* for RFX */
sys_error GetSystemErrCode( error_idx code )
{
    if( code == 0 )
        return( 0 );
    return( SysErrors[(code & ~REMOTE_IND) - 1] );
}

/* for RFX */
sys_handle GetSystemHandle( handle h )
{
    return( SysHandles[ h & ~REMOTE_IND ] );
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
    unsigned            len;
    char                *end;

    info = PathInfo( path, loc );
    len = strlen( path );
    end = &path[len];
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
    for( ;; ) {
        c = *path++;
        if( c == NULLCHAR ) break;
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

    end = path + strlen( path );
    info = PathInfo( path, loc );
    p = end;
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


unsigned MakeFileName( char *result, const char *name, const char *ext, open_access loc )
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

static unsigned MakeNameWithPath( open_access loc,
                                const char *path, unsigned plen,
                                const char *name, unsigned nlen, char *res )
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


handle LclStringToFullName( const char *name, unsigned len, char *full )
{
    char_ring   *curr;
    handle      hndl;
    unsigned    plen;

    // check open file in current directory or in full path
    MakeNameWithPath( OP_LOCAL, NULL, 0, name, len, full );
    hndl = FileOpen( full, OP_READ );
    if( hndl != NIL_HANDLE )
        return( hndl );
    // check open file in debugger directory list
    for( curr = LclPath; curr != NULL; curr = curr->next ) {
        plen = strlen( curr->name );
        MakeNameWithPath( OP_LOCAL, curr->name, plen, name, len, full );
        hndl = FileOpen( full, OP_READ );
        if( hndl != NIL_HANDLE ) {
            return( hndl );
        }
    }
    return( NIL_HANDLE );
}


/*
 *
 */
static handle FullPathOpenInternal( const char *name, unsigned name_len, const char *ext,
                                    char *result, unsigned max_result, bool force_local )
{
    char        buffer[TXT_LEN];
    char        *p;
    const char  *p1;
    open_access loc;
    bool        have_ext;
    bool        have_path;
    file_components *file;
    handle      f;
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
        RemoteStringToFullName( false, buffer, result, max_result );
        f = FileOpen( result, OP_READ | OP_REMOTE );
    } else if( have_path ) {
        StrCopy( buffer, result );
        f = FileOpen( buffer, OP_READ );
    } else {
        f = LclStringToFullName( buffer, p - buffer, result );
    }
    if( f == NIL_HANDLE ) {
        strcpy( result, buffer );
    } else {
        p1 = RealFName( result, &loc );
        memmove( result, p1, strlen( p1 ) + 1 );
    }
    return( f );
}

handle FullPathOpen( const char *name, unsigned name_len, const char *ext, char *result, unsigned max_result )
{
    return( FullPathOpenInternal( name, name_len, ext, result, max_result, false ) );
}

handle LocalFullPathOpen( const char *name, unsigned name_len, const char *ext, char *result, unsigned max_result )
{
    return( FullPathOpenInternal( name, name_len, ext, result, max_result, true ) );
}

static handle PathOpenInternal( const char *name, unsigned name_len, const char *ext, bool force_local )
{
    char        result[TXT_LEN];

    if( force_local ) {
        return( LocalFullPathOpen( name, name_len, ext, result, TXT_LEN ) );
    } else {
        return( FullPathOpen( name, name_len, ext, result, TXT_LEN ) );
    }
}

handle PathOpen( const char *name, unsigned name_len, const char *ext )
{
    return( PathOpenInternal( name, name_len, ext, false ) );
}

handle LocalPathOpen( const char *name, unsigned name_len, const char *ext )
{
    return( PathOpenInternal( name, name_len, ext, true ) );
}

#if !defined( BUILD_RFX )
static bool IsWritable( char const *name, open_access loc )
{
    handle      h;

    h = FileOpen( name, OP_READ | loc );
    if( h == NIL_HANDLE ) {
        h = FileOpen( name, OP_WRITE | OP_CREATE | loc );
        if( h != NIL_HANDLE ) {
            FileClose( h );
            FileRemove( name, loc );
            return( true );
        }
    } else {
        FileClose( h );
        h = FileOpen( name, OP_WRITE | loc );
        if( h != NIL_HANDLE ) {
            FileClose( h );
            return( true );
        }
    }
    return( false );
}

bool FindWritable( char const *src, char *dst )
{
    char        buffer[TXT_LEN];
    unsigned    plen;
    unsigned    nlen;
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
            if( IsWritable( dst, loc ) ) return( true );
        }
    }
    MakeNameWithPath( loc, NULL, 0, name, nlen, dst );
    return( IsWritable( dst, loc ) );
}
#endif

void SysFileInit( void )
{
    unsigned    i;

    for( i = 0; i < MAX_OPENS; ++i ) {
        SysHandles[i] = NIL_SYS_HANDLE;
    }
    SysHandles[STD_IN ] = LocalHandleSys( STD_IN  );
    SysHandles[STD_OUT] = LocalHandleSys( STD_OUT );
    SysHandles[STD_ERR] = LocalHandleSys( STD_ERR );
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
    unsigned   len;
    char_ring  *new;

    // find end of list
    while( *owner != NULL )
        owner = &(*owner)->next;
    // add items to the end of list
    for( start = end = src;; ++end ) {
        if( *end == LclPathSep || *end == '\0' ) {
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
    unsigned    size;

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

#if defined( __DOS__ ) || defined( __LINUX__ )
dig_fhandle DIGPathOpen( const char *name, unsigned name_len, const char *ext, char *buff, unsigned buff_size )
{
    char        dummy[TXT_LEN];
    handle      f;

    if( buff == NULL ) {
        buff = dummy;
        buff_size = sizeof( dummy );
    }
    f = FullPathOpenInternal( name, name_len, ext, buff, buff_size, false );
    return( ( f == NIL_HANDLE ) ? DIG_NIL_HANDLE : f );
}

unsigned DIGPathClose( dig_fhandle h )
{
    return( FileClose( h ) );
}

long DIGGetSystemHandle( dig_fhandle h )
{
    return( SysHandles[h & ~REMOTE_IND] );
}
#endif
