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
* Description:  Built-in builder commands.
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>
#ifdef __UNIX__
    #include <utime.h>
    #include <unistd.h>
    #include <dirent.h>
#else
    #include <sys/utime.h>
    #include <direct.h>
    #include <dos.h>
#endif
#include "watcom.h"
#include "builder.h"
#include "pmake.h"

#ifdef __UNIX__

static int __fnmatch( char *pattern, char *string )
/*************************************************/
{
    char    *p;
    int     len;
    int     star_char;
    int     i;

    /*
     * check pattern section with wildcard characters
     */
    star_char = 0;
    while( ( *pattern == '*' ) || ( *pattern == '?' ) ) {
        if( *pattern == '?' ) {
            if( *string == 0 ) {
                return( 0 );
            }
            string++;
        } else {
            star_char = 1;
        }
        pattern++;
    }
    if( *pattern == 0 ) {
        if( ( *string == 0 ) || star_char ) {
            return( 1 );
        } else {
            return( 0 );
        }
    }
    /*
     * check pattern section with exact match
     * ( all characters except wildcards )
     */
    p = pattern;
    len = 0;
    do {
        if( star_char ) {
            if( string[ len ] == 0 ) {
                return( 0 );
            }
            len++;
        } else {
            if( *pattern != *string ) {
                return( 0 );
            }
            string++;
        }
        pattern++;
    } while( *pattern && ( *pattern != '*' ) && ( *pattern != '?' ) );
    if( star_char == 0 ) {
        /*
         * match is OK, try next pattern section
         */
        return( __fnmatch( pattern, string ) );
    } else {
        /*
         * star pattern section, try locate exact match
         */
        while( *string ) {
            if( *p == *string ) {
                for( i = 1; i < len; i++ ) {
                    if( *( p + i ) != *( string + i ) ) {
                        break;
                    }
                }
                if( i == len ) {
                    /*
                     * if rest doesn't match, find next occurence
                     */
                    if( __fnmatch( pattern, string + len ) ) {
                        return( 1 );
                    }
                }
            }
            string++;
        }
        return( 0 );
    }
}

#endif

static void LogDir( char *dir )
{
    Log( FALSE, "%s", LogDirEquals( dir ) );
}

static int ProcSet( char *cmd )
{
    char        *var;
    char        *rep;

    var = cmd;
    rep = strchr( cmd, '=' );
    if( rep == NULL )
        return( 1 );
    *rep++ = '\0';
    if( *rep == '\0' ) {
        rep = NULL;     /* Delete the environment variable! */
    }
#ifdef __WATCOMC__
    /* We don't have unsetenv(), but our setenv() is extended vs. POSIX */
    if( rep == NULL ) {
        setenv( var, NULL, 1 );
        return( 0 );
    } else
        return( setenv( var, rep, 1 ) );
#else
    if( rep == NULL ) {
        unsetenv( var );
        return( 0 );
    } else
        return( setenv( var, rep, 1 ) );
#endif
}

void ResetArchives( copy_entry *list )
{
    copy_entry  *next;
#ifndef __UNIX__
    unsigned    attr;
#endif

    while( list != NULL ) {
        next = list->next;
#ifndef __UNIX__
        if( _dos_getfileattr( list->src, &attr ) == 0 ) {
            _dos_setfileattr( list->src, attr & ~_A_ARCH );
        }
#endif
        free( list );
        list = next;
    }
}

static copy_entry *BuildList( char *src, char *dst, bool test_abit )
{
    copy_entry          *head;
    copy_entry          *curr;
    copy_entry          **owner;
    char                *end;
    char                buff[_MAX_PATH2];
    char                full[_MAX_PATH];
    char                srcdir[_MAX_PATH];
    char                *drive;
    char                *dir;
    char                *fn;
    char                *ext;
    DIR                 *directory;
    struct dirent       *dent;
#ifndef __UNIX__
    FILE                *fp;
    unsigned            attr;
#else
    struct stat         statsrc, statdst;
    int                 dstrc, srcrc;
    char                pattern[_MAX_PATH];
#endif

    strcpy( srcdir, src );
    end = &dst[strlen( dst ) - 1];
    while( end[0] == ' ' || end[0] == '\t' ) {
        --end;
    }
    end[1] = '\0';
    if( strchr( srcdir, '*' ) == NULL && strchr( srcdir, '?' ) == NULL ) {
        /* no wild cards */
        head = Alloc( sizeof( *head ) );
        head->next = NULL;
        _fullpath( head->src, srcdir, sizeof( head->src ) );
        switch( *end ) {
        case '\\':
        case '/':
            /* need to append source file name */
            _splitpath2( srcdir, buff, &drive, &dir, &fn, &ext );
            _makepath( full, NULL, dst, fn, ext );
            _fullpath( head->dst, full, sizeof( head->dst ) );
            break;
        default:
            _fullpath( head->dst, dst, sizeof( head->dst ) );
            break;
        }
        if( test_abit ) {
#ifndef __UNIX__
            fp = fopen( head->dst, "rb" );
            if( fp != NULL )
                fclose( fp );
            _dos_getfileattr( head->src, &attr );
            if( !( attr & _A_ARCH ) && fp != NULL ) {
                /* file hasn't changed */
                free( head );
                head = NULL;
            }
#else
            /* Linux has (strangely) no 'archive' attribute, compare modification times */
            dstrc = stat( head->dst, &statdst );
            srcrc = stat( head->src, &statsrc );
            if( (dstrc != -1) && (srcrc != -1) && (statdst.st_mtime == statsrc.st_mtime) ) {
                free( head );
                head = NULL;
            }
#endif
        }
        return( head );
    }
#ifdef __UNIX__
    _splitpath2( srcdir, buff, &drive, &dir, &fn, &ext );
    _makepath( srcdir, drive, dir, NULL, NULL );
    _makepath( pattern, NULL, NULL, fn, ext );
#endif
    directory = opendir( srcdir );
    if( directory == NULL ) {
        Log( FALSE, "Can not open source directory '%s': %s\n", srcdir, strerror( errno ) );
        return( NULL );
    }
    head = NULL;
    owner = &head;
    for( ;; ) {
        dent = readdir( directory );
        if( dent == NULL )
            break;
#ifdef __UNIX__
        {
            struct stat buf;
            size_t len = strlen( srcdir );

            if( __fnmatch( pattern, dent->d_name ) == 0 )
                continue;

            strcat( srcdir, dent->d_name );
            stat( srcdir, &buf );
            srcdir[len] = '\0';
            if( S_ISDIR( buf.st_mode ) )
                continue;
        }
#else
        if( dent->d_attr & ( _A_SUBDIR | _A_VOLID ) )
            continue;
#endif
        curr = Alloc( sizeof( *curr ) );
        curr->next = NULL;
        _splitpath2( srcdir, buff, &drive, &dir, &fn, &ext );
        _makepath( full, drive, dir, dent->d_name, NULL );
        _fullpath( curr->src, full, sizeof( curr->src ) );
        strcpy( full, dst );
        switch( *end ) {
        case '\\':
        case '/':
            strcat( full, dent->d_name );
            break;
        }
        _fullpath( curr->dst, full, sizeof( curr->dst ) );
        if( test_abit ) {
#ifndef __UNIX__
            fp = fopen( curr->dst, "rb" );
            if( fp != NULL )
                fclose( fp );
            if( !(dent->d_attr & _A_ARCH) && fp != NULL ) {
                /* file hasn't changed */
                free( curr );
                continue;
            }
#else
            /* Linux has (strangely) no 'archive' attribute, compare modification times */
            dstrc = stat( curr->dst, &statdst );
            srcrc = stat( curr->src, &statsrc );
            if( (dstrc != -1) && (srcrc != -1) && (statdst.st_mtime == statsrc.st_mtime) ) {
                free( curr );
                continue;
            }
#endif
        }
        *owner = curr;
        owner = &curr->next;
    }
    return( head );
}

static int mkdir_nested( char *path )
/***********************************/
{
#ifdef __UNIX__
    struct stat sb;
#else
    unsigned    attr;
#endif
    char        pathname[ FILENAME_MAX ];
    char        *p;
    char        *end;

    p = pathname;
    strncpy( pathname, path, FILENAME_MAX );
    end = pathname + strlen( pathname );

#ifndef __UNIX__
    /* special case for drive letters */
    if( p[0] && p[1] == ':' ) {
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
#ifdef __UNIX__
        if( stat( pathname, &sb ) == -1 ) {
#else
        if( _dos_getfileattr( pathname, &attr ) != 0 ) {
#endif
            int rc;

#ifdef __UNIX__
            rc = mkdir( pathname, S_IRWXU | S_IRWXG | S_IRWXO );
#else
            rc = mkdir( pathname );
#endif
            if( rc != 0 ) {
                Log( FALSE, "Can not create directory '%s': %s\n", pathname, strerror( errno ) );
                return( -1 );
            }
        } else {
            /* make sure it really is a directory */
#ifdef __UNIX__
            if( !S_ISDIR( sb.st_mode ) ) {
#else
            if( (attr & _A_SUBDIR) == 0 ) {
#endif
                Log( FALSE, "Can not create directory '%s': file with the same name already exists\n", pathname );
                return( -1 );
            }
        }
        /* put back the path separator - forward slash always works */
        *p++ = '/';
    }
    return( 0 );
}

static int ProcOneCopy( char *src, char *dst, bool cond_copy )
{
    FILE            *sp;
    FILE            *dp;
    unsigned        len;
    unsigned        out;
    struct stat     srcbuf;
    struct utimbuf  dstbuf;
    static char     buff[32 * 1024];

    sp = fopen( src, "rb" );
    if( sp == NULL ) {
        if( cond_copy ) {
            return( 0 );    // Quietly ignore missing source
        } else {
            Log( FALSE, "Can not open '%s' for reading: %s\n", src, strerror( errno ) );
            return( 1 );
        }
    }
    dp = fopen( dst, "wb" );
    if( dp == NULL ) {
        char *end1, *end2, *end;

        strcpy( buff, dst );
        end1 = strrchr( buff, '/' );
        end2 = strrchr( buff, '\\' );
        if( end1 && end2 ) {
            if( end1 > end2 )
                end = end1;
            else
                end = end2;
        } else if( end1 ) {
            end = end1;
        } else {
            end = end2;
        }
        if( end ) {
            end[0] = 0;
            mkdir_nested( buff );
            dp = fopen( dst, "wb" );
        }
        if( !dp ) {
            Log( FALSE, "Can not open '%s' for writing: %s\n", dst, strerror( errno ) );
            fclose( sp );
            return( 1 );
        }
    }
    Log( FALSE, "Copying '%s' to '%s'...\n", src, dst );
    for( ;; ) {
        len = fread( buff, 1, sizeof( buff ), sp );
        if( len == 0 )
            break;
        if( ferror( sp ) ) {
            Log( FALSE, "Error reading '%s': %s\n", src, strerror( errno ) );
            fclose( sp );
            fclose( dp );
            return( 1 );
        }
        out = fwrite( buff, 1, len, dp );
        if( ferror( dp ) ) {
            Log( FALSE, "Error writing '%s': %s\n", dst, strerror( errno ) );
            fclose( sp );
            fclose( dp );
            return( 1 );
        }
        if( out != len ) {
            Log( FALSE, "Error writing '%s': Disk full\n", dst );
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

static int ProcCopy( char *cmd, bool test_abit, bool cond_copy )
{
    char        *src;
    char        *dst;
    copy_entry  *list;
    copy_entry  *next;
    int         res;

    src = cmd;
    dst = strchr( src, ' ' );
    if( dst == NULL ) {
        dst = strchr( src, '\t' );
        if( dst == NULL ) {
            Log( FALSE, "Missing destination parameter\n" );
            return( 1 );
        }
    }
    *dst = '\0';
    dst = SkipBlanks( dst + 1 );
    if( *dst == '\0' ) {
        Log( FALSE, "Missing destination parameter\n" );
        return( 1 );
    }
    list = BuildList( src, dst, test_abit );
    for( ;; ) {
        if( list == NULL )
            break;
        res = ProcOneCopy( list->src, list->dst, cond_copy );
        if( res != 0 ) {
            while( list != NULL ) {
                next = list->next;
                free( list );
                list = next;
            }
            return( res );
        }
        next = list->next;
        if( test_abit ) {
            list->next = IncludeStk->reset_abit;
            IncludeStk->reset_abit = list;
        } else {
            free( list );
        }
        list = next;
    }
    return( 0 );
}

static int ProcMkdir( char *cmd )
{
    return( mkdir_nested( cmd ) );
}

void PMakeOutput( char *str )
{
    Log( FALSE, "%s\n", str );
}

static int DoPMake( pmake_data *data )
{
    pmake_list  *curr;
    int         res;
    char        cmd[256];

    for( curr = data->dir_list; curr != NULL; curr = curr->next ) {
        res = SysChdir( curr->dir_name );
        if( res != 0 ) {
            if( data->ignore_err ) {
                Log( FALSE, "non-zero return: %d\n", res );
                continue;
            } else {
                return( res );
            }
        }
        getcwd( IncludeStk->cwd, sizeof( IncludeStk->cwd ) );
        if( data->display )
            LogDir( IncludeStk->cwd );
        PMakeCommand( data, cmd );
        res = SysRunCommand( cmd );
        if( res != 0 ) {
            if( data->ignore_err ) {
                Log( FALSE, "non-zero return: %d\n", res );
            } else {
                return( res );
            }
        }
    }
    return( 0 );
}

static int ProcPMake( char *cmd, bool ignore_errors )
{
    pmake_data  *data;
    int         res;
    char        save[_MAX_PATH];

    data = PMakeBuild( cmd );
    if( data == NULL )
        return( 1 );
    if( data->want_help || data->signaled ) {
        PMakeCleanup( data );
        return( 2 );
    }
    strcpy( save, IncludeStk->cwd );
    data->ignore_err = ignore_errors;
    res = DoPMake( data );
    SysChdir( save );
    getcwd( IncludeStk->cwd, sizeof( IncludeStk->cwd ) );
    PMakeCleanup( data );
    return( res );
}

int RunIt( char *cmd, bool ignore_errors )
{
    int     res;

    #define BUILTIN( b )        \
        (strnicmp( cmd, b, sizeof( b ) - 1 ) == 0 && cmd[sizeof(b)-1] == ' ')
    res = 0;
    if( BUILTIN( "CD" ) ) {
        res = SysChdir( SkipBlanks( cmd + sizeof( "CD" ) ) );
        if( res == 0 ) {
            getcwd( IncludeStk->cwd, sizeof( IncludeStk->cwd ) );
        }
    } else if( BUILTIN( "CDSAY" ) ) {
        res = SysChdir( SkipBlanks( cmd + sizeof( "CDSAY" ) ) );
        if( res == 0 ) {
            getcwd( IncludeStk->cwd, sizeof( IncludeStk->cwd ) );
            LogDir( IncludeStk->cwd );
        }
    } else if( BUILTIN( "SET" ) ) {
        res = ProcSet( SkipBlanks( cmd + sizeof( "SET" ) ) );
    } else if( BUILTIN( "ECHO" ) ) {
        Log( Quiet, "%s\n", SkipBlanks( cmd + sizeof( "ECHO" ) ) );
    } else if( BUILTIN( "ERROR" ) ) {
        Log( Quiet, "%s\n", SkipBlanks( cmd + sizeof( "ERROR" ) ) );
        res = 1;
    } else if( BUILTIN( "COPY" ) ) {
        res = ProcCopy( SkipBlanks( cmd + sizeof( "COPY" ) ), FALSE, FALSE );
    } else if( BUILTIN( "ACOPY" ) ) {
        res = ProcCopy( SkipBlanks( cmd + sizeof( "ACOPY" ) ), TRUE, FALSE );
    } else if( BUILTIN( "CCOPY" ) ) {
        res = ProcCopy( SkipBlanks( cmd + sizeof( "CCOPY" ) ), FALSE, TRUE );
    } else if( BUILTIN( "ACCOPY" ) ) {
        res = ProcCopy( SkipBlanks( cmd + sizeof( "ACCOPY" ) ), TRUE, TRUE );
    } else if( BUILTIN( "MKDIR" ) ) {
        res = ProcMkdir( SkipBlanks( cmd + sizeof( "MKDIR" ) ) );
    } else if( BUILTIN( "PMAKE" ) ) {
        res = ProcPMake( SkipBlanks( cmd + sizeof( "PMAKE" ) ), ignore_errors );
    } else {
        res = SysRunCommand( cmd );
    }
    return( res );
}
