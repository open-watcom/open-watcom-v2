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
* Description:  Built-in builder commands.
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/stat.h>
#if defined( __UNIX__ ) || defined( __WATCOMC__ )
    #include <utime.h>
    #include <fnmatch.h>
#else
    #include <sys/utime.h>
#endif
#ifdef __UNIX__
    #include <unistd.h>
    #include <dirent.h>
#else
    #include <direct.h>
    #include <dos.h>
#endif
#ifdef __NT__
    #include <windows.h>
#endif
#include "watcom.h"
#include "builder.h"
#include "pmake.h"
#include "wio.h"
#include "memutils.h"
#include "pathgrp2.h"

#include "clibext.h"


#define WILD_METAS      "*?"

#ifdef __UNIX__
    #define MASK_ALL_ITEMS              "*"
    #define ENTRY_INVALID(e)            IsDotOrDotDot(e->d_name)
    #define ENTRY_NOMATCH(n,e)          (fnmatch(n, e->d_name, FNM_PATHNAME | FNM_NOESCAPE) == FNM_NOMATCH)
    #define ENTRY_SUBDIR(n,e)           chk_is_dir(n)
    #define ENTRY_RDONLY(n,e)           (access( n, W_OK ) == -1 && errno == EACCES)
    /* Linux has (strangely) no 'archive' attribute, compare modification times */
    #define ENTRY_CHANGED1(n1,n2)       !chk_same_time(n1, n2)
    #define ENTRY_CHANGED2(n1,e1,n2)    !chk_same_time(n1, n2)
#else
    #define MASK_ALL_ITEMS              "*.*"
    #define ENTRY_INVALID(e)            (IsDotOrDotDot(e->d_name) || (e->d_attr & _A_VOLID))
    #define ENTRY_NOMATCH(n,e)          (fnmatch(n, e->d_name, FNM_PATHNAME | FNM_NOESCAPE | FNM_IGNORECASE) == FNM_NOMATCH)
    #define ENTRY_SUBDIR(n,e)           (e->d_attr & _A_SUBDIR)
    #define ENTRY_RDONLY(n,e)           (e->d_attr & _A_RDONLY)
    #define ENTRY_CHANGED1(n1,n2)       (access(n2, R_OK) == -1 || chk_is_archived(n1))
    #define ENTRY_CHANGED2(n1,e1,n2)    (access(n2, R_OK) == -1 || (e1->d_attr & _A_ARCH) != 0)
#endif

#define COPY_BUFF_SIZE  (32 * 1024)

#ifdef __NT__
typedef DWORD           fattrs;
#else
typedef unsigned        fattrs;
#endif

typedef struct struct_copy {
    struct struct_copy  *next;
    char                src[_MAX_PATH];
    char                dst[_MAX_PATH];
} struct_copy;

typedef struct dd {
    struct dd   *next;
//    char        attr;
    char        name[1];
} iolist;

char            tmp_buf[MAX_LINE];

static int      rm_rflag;
static int      rm_fflag;
static int      rm_sflag;

static int RecursiveRM( const char *dir );

#ifdef __UNIX__
static bool chk_is_dir( const char *name )
{
    struct stat     s;

    return( stat( name, &s ) == 0 && S_ISDIR( s.st_mode ) );
}

static bool chk_same_time( const char *name1, const char *name2 )
{
    struct stat     s1;
    struct stat     s2;

    return( stat( name1, &s1 ) == 0 && stat( name2, &s2 ) == 0 && ( s1.st_mtime == s2.st_mtime ) );
}
#else
static bool chk_is_archived( const char *name )
{
    fattrs      attr;

#if defined( __NT__ )
    attr = GetFileAttributes( name );
    return( attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_ARCHIVE) );
#else
    return( _dos_getfileattr( name, &attr ) == 0 && (attr & _A_ARCH) );
#endif
}
#endif

#define BSIZE   256
#define SCREEN  79

static const char Equals[] = "========================================"\
                             "========================================";

static void LogDir( const char *dir )
{
    char        tbuff[BSIZE];
    size_t      equals;
    size_t      bufflen;
    const char  *eq;
    struct tm   *tm;
    time_t      ttime;

    ttime = time( NULL );
    tm = localtime( &ttime );
    strftime( tbuff, BSIZE, "%H:%M:%S", tm );
    strcat( tbuff, " " );
    strcat( tbuff, dir );
    bufflen = strlen( tbuff );
    equals = ( SCREEN - 2 - bufflen ) / 2;
    if( bufflen > SCREEN - 4 ) {
        equals = 1;
    }
    eq = &Equals[ ( sizeof( Equals ) - 1 ) - equals];
    Log( false, "%s %s %s%s\n", eq, tbuff, eq, ( bufflen & 1 ) ? "" : "=" );
}

static int ProcSet( const char *cmd )
{
    char        *rep;
    size_t      len;

    rep = strchr( cmd, '=' );
    if( rep == NULL )
        return( 1 );
    len = rep - cmd;
    strncpy( tmp_buf, cmd, len );
    tmp_buf[len] = '\0';
    /* Our setenv() is extended vs. POSIX, check for blank value is not necessary */
    /* Watcom implementation is non-conforming to POSIX, return value is incorrect in some cases */
    rep++;
    if( *rep == '\0' ) {
#if defined( __WATCOMC__ ) && ( __WATCOMC__ < 1300 )
        setenv( tmp_buf, NULL, 1 );
#else
        /* Delete the environment variable! */
        unsetenv( tmp_buf );
#endif
        return( 0 );
    }
    return( setenv( tmp_buf, rep, 1 ) );
}

void ResetArchives( copy_entry list )
{
    copy_entry  next;
#if defined( __UNIX__ )
#else
    fattrs      attr;
#endif

    while( list != NULL ) {
        next = list->next;
#if defined( __UNIX__ )
#elif defined( __NT__ )
        attr = GetFileAttributes( list->src );
        if( attr != INVALID_FILE_ATTRIBUTES ) {
            SetFileAttributes( list->src, attr & ~FILE_ATTRIBUTE_ARCHIVE );
        }
#else
        if( _dos_getfileattr( list->src, &attr ) == 0 ) {
            _dos_setfileattr( list->src, attr & ~_A_ARCH );
        }
#endif
        MFree( list );
        list = next;
    }
}

static int IsDotOrDotDot( const char *fname )
{
    /* return 1 if fname is "." or "..", 0 otherwise */
    return( fname[0] == '.' && ( fname[1] == 0 || fname[1] == '.' && fname[2] == 0 ) );
}

static copy_entry *add_copy_entry( copy_entry *list, char *src, char *dst )
{
    copy_entry  entry;

    entry = MAlloc( sizeof( *entry ) );
    entry->next = NULL;
    strcpy( entry->src, src );
    strcpy( entry->dst, dst );
    *list = entry;
    return( &entry->next );
}

static int BuildList( char *src, char *dst, bool test_abit, bool cond_copy, copy_entry *list )
{
    char                *dst_end;
    pgroup2             pg;
    char                full[_MAX_PATH];
    DIR                 *dirp;
    struct dirent       *dire;
#ifdef __UNIX__
    char                pattern[_MAX_PATH];
#endif
    int                 rc;
    char                entry_src[_MAX_PATH];
    char                entry_dst[_MAX_PATH];

    *list = NULL;
    dst_end = &dst[strlen( dst ) - 1];
    while( IS_BLANK( dst_end[0] ) ) {
        --dst_end;
    }
    dst_end[1] = '\0';
    if( strpbrk( src, WILD_METAS ) == NULL ) {
        /* no wild cards */
        _fullpath( entry_src, src, sizeof( entry_src ) );
        switch( *dst_end ) {
        case '\\':
        case '/':
            /* need to append source file name */
            _splitpath2( src, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
            _makepath( full, NULL, dst, pg.fname, pg.ext );
            _fullpath( entry_dst, full, sizeof( entry_dst ) );
            break;
        default:
            _fullpath( entry_dst, dst, sizeof( entry_dst ) );
            break;
        }
        if( !test_abit || ENTRY_CHANGED1( entry_src, entry_dst ) ) {
            add_copy_entry( list, entry_src, entry_dst );
        }
        return( 0 );
    }
    _splitpath2( src, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
#ifdef __UNIX__
    _makepath( src, pg.drive, pg.dir, NULL, NULL );
    _makepath( pattern, NULL, NULL, pg.fname, pg.ext );
    if( src[0] == '\0' ) {
        dirp = opendir( "." );
    } else {
        dirp = opendir( src );
    }
#else
    if( pg.fname[0] == '*' && pg.fname[1] == '\0' && pg.ext[0] == '\0' )
        strcat( src, ".*" );
    dirp = opendir( src );
#endif
    rc = 1;
    if( dirp == NULL ) {
        if( !cond_copy ) {
            Log( false, "Can not open source directory '%s': %s\n", src, strerror( errno ) );
        }
    } else {
#ifdef __UNIX__
        char *src_end = src + strlen( src );
#endif
        while( (dire = readdir( dirp )) != NULL ) {
            if( ENTRY_INVALID( dire ) )
                continue;
            rc = 0;
#ifdef __UNIX__
            if( ENTRY_NOMATCH( pattern, dire ) )
                continue;
            strcpy( src_end, dire->d_name );
#endif
            if( ENTRY_SUBDIR( src, dire ) )
                continue;
            _makepath( full, pg.drive, pg.dir, dire->d_name, NULL );
            _fullpath( entry_src, full, sizeof( entry_src ) );
            strcpy( full, dst );
            switch( *dst_end ) {
            case '\\':
            case '/':
                strcat( full, dire->d_name );
                break;
            }
            _fullpath( entry_dst, full, sizeof( entry_dst ) );
            if( !test_abit || ENTRY_CHANGED2( entry_src, dire, entry_dst ) ) {
                list = add_copy_entry( list, entry_src, entry_dst );
            }
        }
        closedir( dirp );
    }
    if( cond_copy ) {
        return( 0 );
    }
    return( rc );
}

static int mkdir_nested( const char *path )
/*****************************************/
{
    struct stat sb;
    char        pathname[ FILENAME_MAX ];
    char        *p;
    char        *end;

    p = pathname;
    strncpy( pathname, path, FILENAME_MAX );
    end = pathname + strlen( pathname );

#ifndef __UNIX__
    /* special case for drive letters */
    if( p[0] != '\0' && p[1] == ':' ) {
        p += 2;
    }
#endif
    /* skip initial path separator if present */
    if( (p[0] == '/') || (p[0] == '\\') )
        ++p;

    /* find the next path component */
    while( p < end ) {
        while( (p < end) && (*p != '/') && (*p != '\\') )
            ++p;
        *p = '\0';

        /* check if pathname exists */
        if( stat( pathname, &sb ) == -1 ) {
            int rc;

#ifdef __UNIX__
            rc = mkdir( pathname, S_IRWXU | S_IRWXG | S_IRWXO );
#else
            rc = mkdir( pathname );
#endif
            if( rc != 0 ) {
                Log( false, "Can not create directory '%s': %s\n", pathname, strerror( errno ) );
                return( -1 );
            }
        } else if( !S_ISDIR( sb.st_mode ) ) {   /* make sure it really is a directory */
            Log( false, "Can not create directory '%s': file with the same name already exists\n", pathname );
            return( -1 );
        }
        /* put back the path separator - forward slash always works */
        *p++ = '/';
    }
    return( 0 );
}

static int ProcOneCopy( const char *src, char *dst, bool cond_copy, char *copy_buff )
{
    FILE            *sp;
    FILE            *dp;
    size_t          len;
    size_t          out;
    struct stat     srcbuf;
    struct utimbuf  dstbuf;

    sp = fopen( src, "rb" );
    if( sp == NULL ) {
        if( cond_copy ) {
            return( 0 );    // Quietly ignore missing source
        } else {
            Log( false, "Can not open '%s' for reading: %s\n", src, strerror( errno ) );
            return( 1 );
        }
    }
    dp = fopen( dst, "wb" );
    if( dp == NULL ) {
        len = strlen( dst );
        while( len-- > 0 ) {
            char c = dst[len];
            if( c == '/' || c == '\\' ) {
                dst[len] = '\0';
                mkdir_nested( dst );
                dst[len] = c;
                dp = fopen( dst, "wb" );
                break;
            }
        }
        if( dp == NULL ) {
            Log( false, "Can not open '%s' for writing: %s\n", dst, strerror( errno ) );
            fclose( sp );
            return( 1 );
        }
    }
    Log( Quiet, "Copying '%s' to '%s'...\n", src, dst );
    while( (len = fread( copy_buff, 1, COPY_BUFF_SIZE, sp )) != 0 ) {
        if( ferror( sp ) ) {
            Log( false, "Error reading '%s': %s\n", src, strerror( errno ) );
            fclose( sp );
            fclose( dp );
            return( 1 );
        }
        out = fwrite( copy_buff, 1, len, dp );
        if( ferror( dp ) ) {
            Log( false, "Error writing '%s': %s\n", dst, strerror( errno ) );
            fclose( sp );
            fclose( dp );
            return( 1 );
        }
        if( out != len ) {
            Log( false, "Error writing '%s': Disk full\n", dst );
            fclose( sp );
            fclose( dp );
            return( 1 );
        }
    }
    fclose( sp );
    fclose( dp );

    /* make real copy, set the date back */
    stat( src, &srcbuf );
    dstbuf.actime = srcbuf.st_atime;
    dstbuf.modtime = srcbuf.st_mtime;
    utime( dst, &dstbuf );
#ifdef __UNIX__
    /* copy permissions: mostly necessary for the "x" bit */
    // some files is copied from the source tree with the read-only permission
    // for next run we need the write permission for the current user as minimum
    chmod( dst, srcbuf.st_mode | S_IWUSR );
#endif
    return( 0 );
}

static int ProcCopy( const char *cmd, bool test_abit, bool cond_copy, bool ignore_errors )
{
    char        *p;
    copy_entry  list;
    copy_entry  next;
    int         res;
    char        src[_MAX_PATH];
    char        dst[_MAX_PATH];

    p = src;
    for( ; *cmd != '\0'; cmd++ ) {
        if( IS_BLANK( *cmd ) ) {
            SKIP_BLANKS( cmd );
            break;
        }
        *p++ = *cmd;
    }
    *p = '\0';
    if( *cmd == '\0' ) {
        Log( false, "Missing parameter\n" );
        return( 1 );
    }
    strncpy( dst, cmd, _MAX_PATH - 1 );
    dst[_MAX_PATH - 1] = '\0';
    res = BuildList( src, dst, test_abit, cond_copy, &list );
    if( res == 0 && list != NULL ) {
        char    *copy_buff = MAlloc( COPY_BUFF_SIZE );
        for( ; list != NULL; list = next ) {
            next = list->next;
            if( res == 0 || ignore_errors ) {
                int     rc;

                rc = ProcOneCopy( list->src, list->dst, cond_copy, copy_buff );
                if( rc != 0 ) {
                    res = rc;
#ifndef __UNIX__
                } else if( test_abit ) {
                    list->next = GetArchive();
                    SetArchive( list );
                    continue;
#endif
                }
            }
            MFree( list );
        }
        MFree( copy_buff );
    }
    return( res );
}

static int ProcMkdir( const char *cmd )
{
    return( mkdir_nested( cmd ) );
}

void PMakeOutput( const char *str )
{
    Log( false, "%s\n", str );
}

static int DoPMake( pmake_data *data )
{
    pmake_list  *curr;
    int         res;
    char        cmd[PMAKE_COMMAND_SIZE];
    int         rc = 0;

    for( curr = data->dir_list; curr != NULL; curr = curr->next ) {
        res = SysChdir( curr->dir_name );
        if( res != 0 ) {
            if( !data->ignore_errors ) {
                return( res );
            }
            Log( false, "'cd %s' non-zero return: %d\n", curr->dir_name, res );
            rc = res;
            continue;
        }
        SetIncludeCWD();
        if( data->display )
            LogDir( GetIncludeCWD() );
        PMakeCommand( data, cmd );
        res = SysRunCommand( cmd );
        if( res != 0 ) {
            if( !data->ignore_errors ) {
                return( res );
            }
            Log( false, "'%s' non-zero return: %d\n", cmd, res );
            rc = res;
        }
    }
    return( rc );
}

static int ProcPMake( const char *cmd, bool ignore_errors )
{
    pmake_data  pmake;
    int         res;
    char        save[_MAX_PATH];

    res = -1;
    if( PMakeBuild( &pmake, cmd ) != NULL ) {
        if( !pmake.want_help && !pmake.signaled ) {
            pmake.ignore_errors = ignore_errors;
            strcpy( save, GetIncludeCWD() );
            res = DoPMake( &pmake );
            SysChdir( save );
            SetIncludeCWD();
        }
        PMakeCleanup( &pmake );
    }
    return( res );
}

static int remove_item( const char *name, bool dir )
{
    int         rc;
    char        *err_msg;
    char        *inf_msg;

    if( dir ) {
        err_msg = "Unable to delete directory %s\n";
        inf_msg = "Directory %s deleted\n";
        rc = rmdir( name );
    } else {
        err_msg = "Unable to delete file %s\n";
        inf_msg = "File %s deleted\n";
        rc = remove( name );
    }
    if( rm_fflag && rc != 0 && errno == EACCES ) {
        rc = chmod( name, PMODE_RW );
        if( rc == 0 ) {
            if( dir ) {
                rc = rmdir( name );
            } else {
                rc = remove( name );
            }
        }
    }
    if( rm_fflag && rc != 0 && errno == ENOENT ) {
        rc = 0;
    }
    if( rc != 0 ) {
        rc = errno;
        Log( false, err_msg, name );
        return( rc );
    } else if( !rm_sflag ) {
        Log( false, inf_msg, name );
    }
    return( 0 );
}

/* DoRM - perform RM on a specified file */
static int DoRM( const char *f )
{
    iolist              *tmp;
    iolist              *dhead = NULL;
    iolist              *dtail = NULL;
    char                fpath[_MAX_PATH];
    char                fname[_MAX_PATH];
    char                *fpathend;
    size_t              i;
    size_t              j;
    size_t              len;
    DIR                 *dirp;
    struct dirent       *dire;
    int                 rc;
    int                 retval = 0;

    /* separate file name to path and file name parts */
    len = strlen( f );
    for( i = len; i > 0; --i ) {
        char ch = f[i - 1];
        if( ch == '/' || ch == '\\' || ch == ':' ) {
            break;
        }
    }
    j = i;
    /* if no path then use current directory */
    if( i == 0 ) {
        fpath[i++] = '.';
        fpath[i++] = '/';
    } else {
        memcpy( fpath, f, i );
    }
    fpathend = fpath + i;
    *fpathend = '\0';
#ifdef __UNIX__
    memcpy( fname, f + j, len - j + 1 );
#else
    if( strcmp( f + j, MASK_ALL_ITEMS ) == 0 ) {
        fname[0] = '*';
        fname[1] = '\0';
    } else {
        memcpy( fname, f + j, len - j + 1 );
    }
#endif
    dirp = opendir( fpath );
    if( dirp == NULL ) {
        Log( false, "File (%s) not found.\n", f );
        return( ENOENT );
    }

    while( (dire = readdir( dirp )) != NULL ) {
        if( ENTRY_INVALID( dire ) )
            continue;
        if( ENTRY_NOMATCH( fname, dire ) )
            continue;
        /* set up file name, then try to delete it */
        len = strlen( dire->d_name );
        memcpy( fpathend, dire->d_name, len );
        fpathend[len] = '\0';
        if( ENTRY_SUBDIR( fpath, dire ) ) {
            /* process a directory */
            if( rm_rflag ) {
                /* build directory list */
                len += i + 1;
                tmp = MAlloc( offsetof( iolist, name ) + len );
                tmp->next = NULL;
                if( dtail == NULL ) {
                    dhead = tmp;
                } else {
                    dtail->next = tmp;
                }
                dtail = tmp;
                memcpy( tmp->name, fpath, len );
            } else {
                Log( false, "%s is a directory, use -r\n", fpath );
                retval = EACCES;
            }
        } else if( !rm_fflag && ENTRY_RDONLY( fpath, dire ) ) {
            Log( false, "%s is read-only, use -f\n", fpath );
            retval = EACCES;
        } else {
            rc = remove_item( fpath, false );
            if( rc != 0 ) {
                retval = rc;
            }
        }
    }
    closedir( dirp );
    /* process any directories found */
    for( tmp = dhead; tmp != NULL; tmp = dhead ) {
        dhead = tmp->next;
        rc = RecursiveRM( tmp->name );
        if( rc != 0 ) {
            retval = rc;
        }
        MFree( tmp );
    }
    return( retval );
}

/* RecursiveRM - do an RM recursively on all files */
static int RecursiveRM( const char *dir )
{
    int         rc;
    int         rc2;
    char        fname[_MAX_PATH];

    /* purge the files */
    strcpy( fname, dir );
    strcat( fname, "/" MASK_ALL_ITEMS );
    rc = DoRM( fname );
    /* purge the directory */
    rc2 = remove_item( dir, true );
    if( rc == 0 )
        rc = rc2;
    return( rc );
}

static const char *GetString( const char *cmd, char *buffer )
{
    char        c;
    char        quotechar;

    while( isspace( *cmd ) )
        ++cmd;
    if( *cmd == '\0' )
        return( NULL );
    quotechar = ( *cmd == '"' ) ? *cmd++ : '\0';
    for( ; (c = *cmd) != '\0'; ++cmd ) {
        if( c == quotechar ) {
            ++cmd;
            break;
        }
        *buffer++ = c;
    }
    *buffer = '\0';
    return( cmd );
}

static int ProcRm( const char *cmd )
{
    char    buffer[_MAX_PATH];
    int     retval = 0;

    /* gather options */
    rm_rflag = false;
    rm_fflag = false;
    rm_sflag = true;
    for( ;; ) {
        while( isspace( *cmd ) )
            ++cmd;
        if( *cmd != '-' )
            break;
        ++cmd;
        while( isalpha( *cmd ) ) {
            switch( *cmd++ ) {
            case 'f':
                rm_fflag = true;
                break;
            case 'R':
            case 'r':
                rm_rflag = true;
                break;
            case 'v':
                rm_sflag = false;
                break;
            default:
                return( 1 );
            }
        }
    }

    if( rm_rflag ) {
        /* process -r option */
        while( (cmd = GetString( cmd, buffer )) != NULL ) {
            if( strcmp( buffer, MASK_ALL_ITEMS ) == 0 ) {
                int rc = RecursiveRM( "." );
                if( rc != 0 ) {
                    retval = rc;
                }
            } else if( strpbrk( buffer, WILD_METAS ) != NULL ) {
                // wild cards is not processed for directories
                continue;
            } else {
                struct stat buf;
                if( stat( buffer, &buf ) == 0 ) {
                    if( S_ISDIR( buf.st_mode ) ) {
                        int rc = RecursiveRM( buffer );
                        if( rc != 0 ) {
                            retval = rc;
                        }
                    } else {
                        int rc = DoRM( buffer );
                        if( rc != 0 ) {
                            retval = rc;
                        }
                    }
                }
            }
        }
    } else {
        /* run through all specified files */
        while( (cmd = GetString( cmd, buffer )) != NULL ) {
            int rc = DoRM( buffer );
            if( rc != 0 ) {
                retval = rc;
            }
        }
    }
    return( retval );
}

int RunIt( const char *cmd, bool ignore_errors, bool *res_nolog )
{
    int     res;

    #define BUILTIN( c, b )     (strnicmp( (c), b, sizeof( b ) - 1 ) == 0 && (c)[sizeof( b ) - 1] == ' ')
    #define SKIP_CMD( c, b )    SkipBlanks( c + sizeof( b ) )

    *res_nolog = false;
    if( BUILTIN( cmd, "CD" ) ) {
        res = SysChdir( SKIP_CMD( cmd, "CD" ) );
        if( res == 0 ) {
            SetIncludeCWD();
        }
    } else if( BUILTIN( cmd, "CDSAY" ) ) {
        res = SysChdir( SKIP_CMD( cmd, "CDSAY" ) );
        if( res == 0 ) {
            SetIncludeCWD();
            LogDir( GetIncludeCWD() );
        }
    } else if( BUILTIN( cmd, "SET" ) ) {
        res = ProcSet( SKIP_CMD( cmd, "SET" ) );
    } else if( BUILTIN( cmd, "ECHO" ) ) {
        Log( Quiet, "%s\n", SKIP_CMD( cmd, "ECHO" ) );
        res = 0;
    } else if( BUILTIN( cmd, "ERROR" ) ) {
        Log( Quiet, "%s\n", SKIP_CMD( cmd, "ERROR" ) );
        res = 1;
    } else if( BUILTIN( cmd, "COPY" ) ) {
        res = ProcCopy( SKIP_CMD( cmd, "COPY" ), false, false, ignore_errors );
        *res_nolog = true;
    } else if( BUILTIN( cmd, "ACOPY" ) ) {
        res = ProcCopy( SKIP_CMD( cmd, "ACOPY" ), true, false, ignore_errors );
        *res_nolog = true;
    } else if( BUILTIN( cmd, "CCOPY" ) ) {
        res = ProcCopy( SKIP_CMD( cmd, "CCOPY" ), false, true, ignore_errors );
        *res_nolog = true;
    } else if( BUILTIN( cmd, "ACCOPY" ) ) {
        res = ProcCopy( SKIP_CMD( cmd, "ACCOPY" ), true, true, ignore_errors );
        *res_nolog = true;
    } else if( BUILTIN( cmd, "MKDIR" ) ) {
        res = ProcMkdir( SKIP_CMD( cmd, "MKDIR" ) );
    } else if( BUILTIN( cmd, "PMAKE" ) ) {
        res = ProcPMake( SKIP_CMD( cmd, "PMAKE" ), ignore_errors );
        *res_nolog = ignore_errors;
    } else if( BUILTIN( cmd, "RM" ) ) {
        res = ProcRm( SKIP_CMD( cmd, "RM" ) );
    } else if( cmd[0] == '!' ) {
        res = SysRunCommand( SkipBlanks( cmd + 1 ) );
    } else {
        res = SysRunCommand( cmd );
    }
    return( res );
}
