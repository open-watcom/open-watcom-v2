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


#include <ctype.h>
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
#include "digld.h"
#include "strutil.h"
#include "filelcl.h"
#include "filermt.h"
#include "diptypes.h"
#include "dbgsrc.h"
#include "remfile.h"
#include "dbgerr.h"
#include "digcli.h"

#include "clibext.h"


#define CHK_DIR_SEP(c,i)    ((c) != NULLCHAR && ((c) == (i)->path_separator[0] || (c) == (i)->path_separator[1]))
#define CHK_DRV_SEP(c,i)    ((c) != NULLCHAR && (c) == (i)->drv_separator)
#define CHK_PATH_SEP(c,i)   ((c) != NULLCHAR && ((c) == (i)->drv_separator || (c) == (i)->path_separator[0] || (c) == (i)->path_separator[1]))

#define CHECK_PATH_ABS(p,i) (CHK_DIR_SEP((p)[0],i) || (p)[0] != NULLCHAR && CHK_DRV_SEP((p)[1],i) && CHK_DIR_SEP((p)[2],i))

#define LOC_ESCAPE      '@'
#define REMOTE_LOC      'r'
#define LOCAL_LOC       'l'
#define REMOTE_IND      0x8000
#define MAX_OPENS       100
#define MAX_ERRORS      10

#define SYSHANDLE(sh)   SysHandles[sh & ~REMOTE_IND]
#define SYSERROR(e)     SysErrors[(e & ~REMOTE_IND) - 1]
#define ISREMOTE(x)     ((x & REMOTE_IND) != 0)
#define SETREMOTE(x)    x |= REMOTE_IND

static char_ring        *LclPath;

static sys_handle       SysHandles[MAX_OPENS];
static sys_error        SysErrors[MAX_ERRORS];
static error_handle     ErrRover;
static error_handle     LastErr;

const char  *RealFName( char const *name, obj_attrs *oattrs )
{
    *oattrs &= ~(OP_REMOTE | OP_LOCAL);
    if( name[0] == LOC_ESCAPE ) {
        if( tolower( name[1] ) == REMOTE_LOC ) {
            *oattrs |= OP_REMOTE;
            name += 2;
        } else if( tolower( name[1] ) == LOCAL_LOC ) {
            *oattrs |= OP_LOCAL;
            name += 2;
        } else if( name[1] == LOC_ESCAPE ) {
            name += 1;
        }
    }
    return( name );
}

static obj_attrs DefaultLoc( obj_attrs oattrs )
{
    if( (oattrs & (OP_REMOTE | OP_LOCAL) ) == 0 ) {
        if( _IsOn( SW_REMOTE_FILES ) ) {
            oattrs |= OP_REMOTE;
        } else {
            oattrs |= OP_LOCAL;
        }
    }
    if( (oattrs & OP_REMOTE) && !HaveRemoteFiles() ) {
        oattrs &= ~OP_REMOTE;
        oattrs |= OP_LOCAL;
    }
    return( oattrs );
}

const char  *FileLoc( char const *name, obj_attrs *oattrs )
{
    obj_attrs   oattrs_file;

    oattrs_file = 0;
    name = RealFName( name, &oattrs_file );
    if( oattrs_file ) {
        *oattrs &= ~(OP_LOCAL | OP_REMOTE);
        *oattrs |= oattrs_file;
    }
    *oattrs = DefaultLoc( *oattrs );
    return( name );
}


static const file_components *PathInfo( char const *path, obj_attrs oattrs )
{
    const file_components   *info;

    FileLoc( path, &oattrs );
    if( oattrs & OP_LOCAL ) {
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
        if( IS_SYSHANDLE_NULL( SYSHANDLE( fh ) ) ) {
            return( fh );
        }
    }
    return( NIL_HANDLE );
}

size_t ReadStream( file_handle fh, void *b, size_t l )
{
    sys_handle  sh;

    sh = SYSHANDLE( fh );
    if( ISREMOTE( fh ) ) {
        return( RemoteRead( sh, b, l ) );
    } else {
        return( LocalRead( sh, b, l ) );
    }
}

size_t ReadText( file_handle fh, void *b, size_t l )
{
    return( ReadStream( fh, b, l ) );
}

size_t WriteStream( file_handle fh, const void *b, size_t l )
{
    sys_handle  sh;

    sh = SYSHANDLE( fh );
    if( ISREMOTE( fh ) ) {
        return( RemoteWrite( sh, b, l ) );
    } else {
        return( LocalWrite( sh, b, l ) );
    }
}

size_t WriteNL( file_handle fh )
{
    const char      *nl;

    if( ISREMOTE( fh ) ) {
        nl = RemFile.line_eol;
    } else {
        nl = LclFile.line_eol;
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
    sys_handle  sh;

    sh = SYSHANDLE( fh );
    if( ISREMOTE( fh ) ) {
        return( RemoteSeek( sh, p, m ) );
    } else {
        return( LocalSeek( sh, p, m ) );
    }
}

file_handle FileOpen( const char *name, obj_attrs oattrs )
{
    sys_handle  sh;
    file_handle fh;

    if( oattrs & OP_SEARCH ) {
        return( PathOpen( name, strlen( name ), "" ) );
    }
    name = FileLoc( name, &oattrs );
    fh = FindFreeHandle();
    if( fh == NIL_HANDLE )
        return( NIL_HANDLE );
    if( oattrs & OP_REMOTE ) {
        SETREMOTE( fh );
        sh = RemoteOpen( name, oattrs );
    } else {
        sh = LocalOpen( name, oattrs );
    }
    if( IS_SYSHANDLE_NULL( sh ) )
        return( NIL_HANDLE );
    SYSHANDLE( fh ) = sh;
    if( oattrs & OP_APPEND )
        SeekStream( fh, 0, DIO_SEEK_END );
    return( fh );
}

error_handle FileClose( file_handle fh )
{
    sys_handle  sh;

    sh = SYSHANDLE( fh );
    SET_SYSHANDLE_NULL( SYSHANDLE( fh ) );
    if( ISREMOTE( fh ) ) {
        return( RemoteClose( sh ) );
    } else {
        return( LocalClose( sh ) );
    }
}


error_handle FileRemove( char const *name, obj_attrs oattrs )
{
    name = FileLoc( name, &oattrs );
    if( oattrs & OP_REMOTE ) {
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

obj_attrs FileHandleInfo( file_handle fh )
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

error_handle StashErrCode( sys_error syserr, obj_attrs oattrs )
{
    error_handle    errh;

    if( syserr == 0 )
        return( 0 );
    if( ++ErrRover >= MAX_ERRORS )
        ErrRover = 0;
    errh = ErrRover + 1;
    SYSERROR( errh ) = syserr;
    if( oattrs & OP_REMOTE )
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
    const file_components   *info;
    const char              *p;
    obj_attrs               oattrs;

    oattrs = 0;
    p = RealFName( path, &oattrs );
    info = PathInfo( p, oattrs );
    if( strlen( p ) == 0 )
        return( false );
    return( CHECK_PATH_ABS( p, info ) );
}

char *AppendPathDelim( char *path, obj_attrs oattrs )
{
    const file_components   *info;
    size_t                  len;
    char                    *end;

    info = PathInfo( path, oattrs );
    len = strlen( path );
    end = path + len;
    if( len == 0 || !CHK_PATH_SEP( end[-1], info ) ) {
        *end++ = info->path_separator[0];
    }
    return( end );
}

const char  *SkipPathInfo( char const *path, obj_attrs oattrs )
{
    const file_components   *info;
    const char              *name;
    char                    c;

    name = path;
    info = PathInfo( path, oattrs );
    while( (c = *path++) != NULLCHAR ) {
        if( CHK_PATH_SEP( c, info ) ) {
            name = path;
        }
    }
    return( name );
}


const char  *ExtPointer( char const *path, obj_attrs oattrs )
{
    const file_components   *info;
    const char              *p;
    const char              *end;
    char                    c;

    p = end = path + strlen( path );
    info = PathInfo( path, oattrs );
    for( ;; ) {
        c = *--p;
        if( p < path )
            return( end );
        if( CHK_PATH_SEP( c, info ) )
            return( end );
        if( c == info->ext_separator ) {
            return( p );
        }
    }
}


size_t MakeFileName( char *result, const char *name, const char *ext, obj_attrs oattrs )
{
    const file_components   *info;
    char                    *p;

    p = StrCopy( name, result );
    if( *ExtPointer( result, oattrs ) == NULLCHAR ) {
        info = PathInfo( name, oattrs );
        *p++ = info->ext_separator;
        p = StrCopy( ext, p );
    }
    return( p - result );
}

static size_t MakeNameWithPath( obj_attrs oattrs,
                                const char *path, size_t plen,
                                const char *name, size_t nlen, char *res )
{
    const file_components   *info;
    char                    *p;

    p = res;
    if( oattrs & OP_REMOTE ) {
        *p++ = LOC_ESCAPE;
        *p++ = REMOTE_LOC;
    } else if( oattrs & OP_LOCAL ) {
        *p++ = LOC_ESCAPE;
        *p++ = LOCAL_LOC;
    } else {
        oattrs = DefaultLoc( oattrs );
    }
    if( path != NULL ) {
        memcpy( p, path, plen );
        p += plen;
        if( oattrs & OP_LOCAL ) {
            info = &LclFile;
        } else {
            info = &RemFile;
        }
        if( plen > 0 && !CHK_PATH_SEP( p[-1], &LclFile ) ) {
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
    char                    buffer[TXT_LEN];
    char                    *p;
    const char              *p1;
    obj_attrs               oattrs;
    bool                    have_ext;
    bool                    have_path;
    const file_components   *file;
    file_handle             fh;
    char                    c;

    oattrs = 0;
    p1 = FileLoc( name, &oattrs );
    name_len -= p1 - name;
    name = p1;
    have_ext = false;
    have_path = false;
    if( force_local ) {
        oattrs &= ~OP_REMOTE;
        oattrs |= OP_LOCAL;
    }
    if( oattrs & OP_LOCAL ) {
        file = &LclFile;
    } else {
        file = &RemFile;
    }
    p = buffer;
    while( name_len-- > 0 ) {
        c = *name++;
        *p++ = c;
        if( CHK_PATH_SEP( c, file ) ) {
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
    if( oattrs & OP_REMOTE ) {
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
        p1 = RealFName( result, &oattrs );
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
static bool IsWritable( char const *name, obj_attrs oattrs )
{
    file_handle     fh;

    fh = FileOpen( name, oattrs | OP_READ );
    if( fh == NIL_HANDLE ) {
        fh = FileOpen( name, oattrs | OP_WRITE | OP_CREATE );
        if( fh != NIL_HANDLE ) {
            FileClose( fh );
            FileRemove( name, oattrs );
            return( true );
        }
    } else {
        FileClose( fh );
        fh = FileOpen( name, oattrs | OP_WRITE );
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
    obj_attrs   oattrs;

    oattrs = 0;
    src = RealFName( src, &oattrs );
    if( IsWritable( src, oattrs ) ) {
        StrCopy( src, dst );
        return( true );
    }
    name = SkipPathInfo( src, oattrs );
    nlen = strlen( name );
    if( DefaultLoc( oattrs ) & OP_LOCAL ) {
        plen = DUIEnvLkup( "HOME", buffer, sizeof( buffer ) );
        if( plen > 0 ) {
            MakeNameWithPath( oattrs, buffer, plen, name, nlen, dst );
            if( IsWritable( dst, oattrs ) ) {
                return( true );
            }
        }
    }
    MakeNameWithPath( oattrs, NULL, 0, name, nlen, dst );
    return( IsWritable( dst, oattrs ) );
}
#endif

void SysFileInit( void )
{
    file_handle     fh;

    for( fh = 0; fh < MAX_OPENS; ++fh ) {
        SET_SYSHANDLE_NULL( SYSHANDLE( fh ) );
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
static FILE *MakeNameWithPathOpen( const char *path, const char *name, size_t nlen, char *res, size_t rlen )
{
    char        *p;
    size_t      len;

    if( rlen < 2 ) {
        if( rlen == 1 )
            *res = NULLCHAR;
        return( NULL );
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
        if( !CHK_PATH_SEP( p[-1], &LclFile ) ) {
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
    return( fopen( res, "rb" ) );
}

FILE *DIGLoader( Open )( const char *name, size_t name_len, const char *ext, char *buff, size_t buff_size )
{
    char            buffer[TXT_LEN];
    char            *p;
    bool            have_ext;
    bool            have_path;
    char            c;
    char            dummy[TXT_LEN];
    FILE            *fp;
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
        if( CHK_PATH_SEP( c, &LclFile ) ) {
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
        fp = fopen( buffer, "rb" );
    } else {
        // check open file in current directory or in full path
        fp = MakeNameWithPathOpen( NULL, buffer, p - buffer, buff, buff_size );
        if( fp == NULL ) {
            // check open file in debugger directory list
            for( curr = LclPath; curr != NULL; curr = curr->next ) {
                fp = MakeNameWithPathOpen( curr->name, buffer, p - buffer, buff, buff_size );
                if( fp != NULL ) {
                    break;
                }
            }
        }
    }
    if( fp == NULL )
        strcpy( buff, buffer );
    return( fp );
}

int DIGLoader( Read )( FILE *fp, void *buff, size_t len )
{
    return( fread( buff, 1, len, fp ) != len );
}

int DIGLoader( Seek )( FILE *fp, unsigned long offs, dig_seek where )
{
    return( fseek( fp, offs, where ) );
}

int DIGLoader( Close )( FILE *fp )
{
    return( fclose( fp ) );
}
#endif
