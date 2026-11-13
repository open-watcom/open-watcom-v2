/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  POSIX rm utility
*               Removes files and directories
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <dos.h>
#include <conio.h>
#ifdef __NT__
#include <windows.h>
#endif
#include "bool.h"
#include "wio.h"
#include "watcom.h"
#include "lineprt.h"
#include "misc.h"
#include "getopt.h"
#include "argvenv.h"
#include "fnutils.h"
#include "filerx.h"

#include "clibext.h"


static const char       *usageMsg[] = {
    "Usage: rm [-?firRsvX] [files]",
    "\tfiles       : files/directories to delete",
    "\tOptions: -? : print this list",
    "\t\t -f : force deletion of read-only files",
    "\t\t      and don't complain about non-existent files / dirs",
    "\t\t -i : inquire about each deletion",
    "\t\t -r : recursively delete all directories",
    "\t\t -R : recursively delete all directories",
    "\t\t -s : silent operation (default)",
    "\t\t -v : verbose operation",
    "\t\t -X : match files by regular expressions",
    NULL
};

bool                    rflag = false;
bool                    iflag = false;
bool                    fflag = false;
bool                    sflag = true;
bool                    rxflag = false;
int                     error_occured = 0;

typedef struct dd {
    struct dd           *next;
    char                name[1];
}                       iolist;

typedef struct rm_pattern {
    struct rm_pattern   *next;
    void                *crx;
    bool                matched;
    char                text[1];
}                       rm_pattern;

typedef struct rm_group {
    struct rm_group     *next;
    rm_pattern          *patterns;
    rm_pattern          *pattern_tail;
    char                path[1];
}                       rm_group;

static void DoRM( const char *f );
static void RecursiveRM( const char *dir );
static void SplitRMSpec( const char *spec, char *path, char *pattern );
static rm_group *FindRMGroup( rm_group *groups, const char *path );
static rm_group *AddRMGroup( rm_group **head, rm_group **tail, const char *path );
static void AddRMPattern( rm_group *group, const char *pattern );
static void RemoveFileEntry( const char *name, unsigned attr );
static void DoRMMasks( rm_group *group );
static void FreeRMGroups( rm_group *groups );

/* start of mainline */
int main( int argc, char *argv[] )
{
    int         i;
    int         ch;
    DIR         *dirp;
    bool        isdir;
    rm_group    *groups = NULL;
    rm_group    *group_tail = NULL;
    rm_group    *group;
    char        path[_MAX_PATH];
    char        pattern[_MAX_PATH];

    argv = ExpandEnv( &argc, argv, "RM" );

    /* process options */
    while( ( ch = GetOpt( &argc, argv, "firRsvX", usageMsg ) ) != -1 ) {
        switch( ch ) {
        case 'f': fflag  = true;  iflag = false; break;
        case 'i': iflag  = true;  fflag = false; break;
        case 'R':
        case 'r': rflag  = true;  break;
        case 's': sflag  = true;  break;
        case 'v': sflag  = false; break;
        case 'X': rxflag = true;  break;
        }
    }
    if( argc < 2 )
        Quit( usageMsg, "No filename/directory specified\n" );

    StartPrint();

    /* process -r option */
    if( rflag ) {
        for( i = 1; i < argc; i++ ) {
            if( strcmp( argv[i], rxflag ? "*" : "*.*" ) == 0 ) {
                RecursiveRM( "." );
            } else {
                if( FileNameWild( argv[i], rxflag ) ) {
                    dirp = NULL;
                } else {
                    dirp = opendir( argv[i] );
                    if( dirp != NULL ) {
                        isdir = ( (dirp->d_attr & _A_SUBDIR) != 0 );
                        closedir( dirp );
                        if( isdir ) {
                            RecursiveRM( argv[i] );
                        } else if( fflag ) {
                            RemoveFileEntry( argv[i], 0 );
                        } else {
                            PrintALineThenDrop( "%s is not a directory.", argv[i] );
                            error_occured = 1;
                        }
                    }
                }
                if( dirp == NULL ) {
                    if( !fflag ) {
                        PrintALineThenDrop( "Directory %s not found.", argv[i] );
                        error_occured = 1;
                    }
                }
            }
        }
    } else if( iflag ) {
        /* Preserve command-line order for interactive operation. */
        for( i = 1; i < argc; i++ ) {
            if( FileNameWild( argv[i], rxflag ) ) {
                DoRM( argv[i] );
            } else {
                RemoveFileEntry( argv[i], 0 );
            }
        }
    } else {
        /*
         * Group wildcard specifications by directory.  Each directory is
         * enumerated only once, regardless of the number of masks for it.
         */
        for( i = 1; i < argc; i++ ) {
            if( FileNameWild( argv[i], rxflag ) ) {
                if( strlen( argv[i] ) >= sizeof( path ) ) {
                    PrintALineThenDrop( "Path too long: %s", argv[i] );
                    error_occured = 1;
                    continue;
                }
                SplitRMSpec( argv[i], path, pattern );
                group = FindRMGroup( groups, path );
                if( group == NULL ) {
                    group = AddRMGroup( &groups, &group_tail, path );
                }
                AddRMPattern( group, pattern );
            } else {
                RemoveFileEntry( argv[i], 0 );
            }
        }
        for( group = groups; group != NULL; group = group->next ) {
            DoRMMasks( group );
        }
        FreeRMGroups( groups );
    }
    DropALine();

    EndPrint();

    MemFree( argv );

    return( error_occured );
}

static void SplitRMSpec( const char *spec, char *path, char *pattern )
{
    size_t      i;
    size_t      j;
    size_t      len;

    len = strlen( spec );
    for( i = len; i > 0; --i ) {
        if( spec[i - 1] == ':' || spec[i - 1] == '\\' || spec[i - 1] == '/' ) {
            break;
        }
    }
    j = i;
    if( i == 0 ) {
        path[i++] = '.';
        path[i++] = '/';
    } else {
        memcpy( path, spec, i );
    }
    path[i] = '\0';
    memcpy( pattern, spec + j, len - j + 1 );
}

static bool FNameEq( const char *name1, const char *name2 )
{
    int         ch1;
    int         ch2;

    for( ;; ) {
        ch1 = (unsigned char)*name1++;
        ch2 = (unsigned char)*name2++;

        /* DOS, OS/2 and Windows accept both directory separators. */
        if( ch1 == '/' || ch1 == '\\' )
            ch1 = '\\';
        if( ch2 == '/' || ch2 == '\\' )
            ch2 = '\\';

        ch1 = tolower( ch1 );
        ch2 = tolower( ch2 );
        if( ch1 != ch2 )
            return( false );
        if( ch1 == '\0' )
            return( true );
    }
}

static rm_group *FindRMGroup( rm_group *groups, const char *path )
{
    rm_group    *group;

    for( group = groups; group != NULL; group = group->next ) {
        if( FNameEq( group->path, path ) ) {
            return( group );
        }
    }
    return( NULL );
}

static rm_group *AddRMGroup( rm_group **head, rm_group **tail, const char *path )
{
    rm_group    *group;
    size_t      len;

    len = strlen( path ) + 1;
    group = MemAlloc( offsetof( rm_group, path ) + len );
    group->next = NULL;
    group->patterns = NULL;
    group->pattern_tail = NULL;
    memcpy( group->path, path, len );
    if( *tail == NULL ) {
        *head = group;
    } else {
        (*tail)->next = group;
    }
    *tail = group;
    return( group );
}

static void AddRMPattern( rm_group *group, const char *pattern )
{
    rm_pattern  *item;
    size_t      len;
    char        *err;

    len = strlen( pattern ) + 1;
    item = MemAlloc( offsetof( rm_pattern, text ) + len );
    item->next = NULL;
    item->crx = NULL;
    item->matched = false;
    memcpy( item->text, pattern, len );
    if( rxflag ) {
        err = FileMatchInit( &item->crx, item->text );
        if( err != NULL )
            Die( "\"%s\": %s\n", err, item->text );
    }
    if( group->pattern_tail == NULL ) {
        group->patterns = item;
    } else {
        group->pattern_tail->next = item;
    }
    group->pattern_tail = item;
}

static bool RMMatchAny( rm_pattern *patterns, const char *name )
{
    rm_pattern  *pattern;
    bool        matched = false;

    for( pattern = patterns; pattern != NULL; pattern = pattern->next ) {
        if( rxflag ? FileMatch( pattern->crx, name )
                   : FileMatchNoRx( name, pattern->text ) ) {
            pattern->matched = true;
            matched = true;
        }
    }
    return( matched );
}

static void RemoveFileEntry( const char *name, unsigned attr )
{
    int         answer;
    int         rc;

    if( fflag && (attr & _A_RDONLY) )
        chmod( name, PMODE_RW );

    if( iflag ) {
        PrintALine( "Delete %s (y/n)", name );
        while( ( answer = tolower( getch() ) ) != 'y' && answer != 'n' )
            ;
        DropALine();
        if( answer != 'y' )
            return;
    }
    rc = remove( name );
    if( rc != 0 && fflag && errno == EACCES ) {
        if( chmod( name, PMODE_RW ) == 0 )
            rc = remove( name );
    }
    if( rc != 0 && fflag && errno == ENOENT )
        return;
    if( rc != 0 ) {
        PrintALineThenDrop( "Unable to delete file %s", name );
        error_occured = 1;
    } else if( !sflag ) {
        PrintALine( "Deleting file %s", name );
    }
}

static void DoRMMasks( rm_group *group )
{
    char                fpath[_MAX_PATH];
    char                *fpathend;
    size_t              path_len;
    size_t              name_len;
    DIR                 *dirp;
    struct dirent       *dire;
    rm_pattern          *pattern;

    path_len = strlen( group->path );
    if( path_len >= sizeof( fpath ) ) {
        PrintALineThenDrop( "Path too long: %s", group->path );
        error_occured = 1;
        return;
    }
    memcpy( fpath, group->path, path_len + 1 );
    fpathend = fpath + path_len;
    dirp = opendir( group->path );
    if( dirp == NULL ) {
        if( !fflag ) {
            for( pattern = group->patterns; pattern != NULL; pattern = pattern->next ) {
                PrintALineThenDrop( "File (%s%s) not found.", group->path, pattern->text );
            }
            error_occured = 1;
        }
        return;
    }

    while( (dire = readdir( dirp )) != NULL ) {
        FNameLower( dire->d_name );
        if( IsDotOrDotDot( dire->d_name ) )
            continue;
        if( !RMMatchAny( group->patterns, dire->d_name ) )
            continue;
        name_len = strlen( dire->d_name );
        if( path_len + name_len >= sizeof( fpath ) ) {
            PrintALineThenDrop( "Path too long: %s%s", group->path, dire->d_name );
            error_occured = 1;
            continue;
        }
        memcpy( fpathend, dire->d_name, name_len + 1 );
        if( dire->d_attr & _A_SUBDIR ) {
            PrintALineThenDrop( "%s is a directory, use -r", fpath );
            error_occured = 1;
        } else if( (dire->d_attr & _A_RDONLY) && !fflag ) {
            PrintALineThenDrop( "%s is read-only, use -f", fpath );
            error_occured = 1;
        } else {
            RemoveFileEntry( fpath, dire->d_attr );
        }
    }
    closedir( dirp );

    if( !fflag ) {
        for( pattern = group->patterns; pattern != NULL; pattern = pattern->next ) {
            if( !pattern->matched ) {
                PrintALineThenDrop( "File (%s%s) not found.", group->path, pattern->text );
                error_occured = 1;
            }
        }
    }
}

static void FreeRMGroups( rm_group *groups )
{
    rm_group    *next_group;
    rm_pattern  *pattern;
    rm_pattern  *next_pattern;

    while( groups != NULL ) {
        next_group = groups->next;
        for( pattern = groups->patterns; pattern != NULL; pattern = next_pattern ) {
            next_pattern = pattern->next;
            if( pattern->crx != NULL )
                FileMatchFini( pattern->crx );
            MemFree( pattern );
        }
        MemFree( groups );
        groups = next_group;
    }
}

/* DoRM - perform RM on a specified file */
static void DoRM( const char *f )
{
    iolist              *tmp;
    iolist              *dhead = NULL;
    iolist              *dtail = NULL;
    char                fpath[_MAX_PATH];
    char                tmppath[_MAX_PATH];
    char                *fpathend;
    size_t              i;
    size_t              len;
    size_t              prefix_len;
    DIR                 *dirp;
    struct dirent       *dire;
    char                wild[_MAX_PATH];
    char                *err;
    void                *crx = NULL;
    bool                found = false;

    len = strlen( f );
    if( len >= sizeof( fpath ) ) {
        PrintALineThenDrop( "Path too long: %s", f );
        error_occured = 1;
        return;
    }

    /* get file path prefix */
    for( i = len; i > 0; --i ) {
        if( f[i - 1] == ':' || f[i - 1] == '\\' || f[i - 1] == '/' )
            break;
    }
    prefix_len = i;
    if( prefix_len != 0 )
        memcpy( fpath, f, prefix_len );
    fpath[prefix_len] = '\0';

    dirp = opendir( FileMatchDirAll( f, tmppath, wild ) );
    if( dirp == NULL ) {
        if( !fflag ) {
            PrintALineThenDrop( "File (%s) not found.", f );
            error_occured = 1;
        }
        return;
    }

    if( rxflag ) {
        err = FileMatchInit( &crx, wild );
        if( err != NULL )
            Die( "\"%s\": %s\n", err, wild );
    }

    memcpy( tmppath, fpath, prefix_len + 1 );
    fpathend = tmppath + prefix_len;
    while( (dire = readdir( dirp )) != NULL ) {
        FNameLower( dire->d_name );
        if( rxflag ) {
            if( !FileMatch( crx, dire->d_name ) )
                continue;
        } else {
            if( !FileMatchNoRx( dire->d_name, wild ) )
                continue;
        }
        if( (dire->d_attr & _A_SUBDIR) && IsDotOrDotDot( dire->d_name ) )
            continue;
        found = true;
        len = strlen( dire->d_name );
        if( prefix_len + len >= sizeof( tmppath ) ) {
            PrintALineThenDrop( "Path too long: %s%s", fpath, dire->d_name );
            error_occured = 1;
            continue;
        }
        memcpy( fpathend, dire->d_name, len + 1 );
        if( dire->d_attr & _A_SUBDIR ) {
            if( rflag ) {
                len += prefix_len + 1;
                tmp = MemAlloc( offsetof( iolist, name ) + len );
                tmp->next = NULL;
                if( dtail == NULL ) {
                    dhead = tmp;
                } else {
                    dtail->next = tmp;
                }
                dtail = tmp;
                memcpy( tmp->name, tmppath, len );
            } else {
                PrintALineThenDrop( "%s is a directory, use -r", tmppath );
                error_occured = 1;
            }
        } else if( (dire->d_attr & _A_RDONLY) && !fflag ) {
            PrintALineThenDrop( "%s is read-only, use -f", tmppath );
            error_occured = 1;
        } else {
            RemoveFileEntry( tmppath, dire->d_attr );
        }
    }
    closedir( dirp );
    if( crx != NULL )
        FileMatchFini( crx );

    if( !found && !fflag && !rflag ) {
        PrintALineThenDrop( "File (%s) not found.", f );
        error_occured = 1;
    }

    /* process any directories found */
    for( tmp = dhead; tmp != NULL; tmp = dhead ) {
        dhead = tmp->next;
        RecursiveRM( tmp->name );
        MemFree( tmp );
    }
}

/* DoRMdir - perform RM on a specified directory */
static void DoRMdir( const char *dir )
{
    int         rc;

    if( ( rc = rmdir( dir ) ) == -1 ) {
#ifdef __NT__
        DWORD       attribute;

        attribute = GetFileAttributes( dir );
        if( attribute != INVALID_FILE_ATTRIBUTES && (attribute & FILE_ATTRIBUTE_READONLY) ) {
#else
        unsigned    attribute = 0;

        if( _dos_getfileattr( dir, &attribute ) == 0 && (attribute & _A_RDONLY) ) {
#endif
            if( fflag ) {
#ifdef __NT__
                if( SetFileAttributes( dir, attribute & ~FILE_ATTRIBUTE_READONLY ) )
                    rc = rmdir( dir );
#else
                if( _dos_setfileattr( dir, attribute & ~_A_RDONLY ) == 0 )
                    rc = rmdir( dir );
#endif
            } else {
                PrintALineThenDrop( "Directory %s is read-only, use -f", dir );
                error_occured = 1;
                return;
            }
        }
    }
    if( rc == -1 ) {
        PrintALineThenDrop( "Unable to delete directory %s", dir );
        error_occured = 1;
    } else if( !sflag ) {
        PrintALine( "Deleting directory %s", dir );
    }
}

/* RecursiveRM - do an RM recursively on all files */
static void RecursiveRM( const char *dir )
{
    int         i;
    char        fname[_MAX_PATH];
    const char  *suffix;
    size_t      len;
    size_t      suffix_len;

    /* purge the files */
    suffix = rxflag ? "/*" : "/*.*";
    len = strlen( dir );
    suffix_len = strlen( suffix );
    if( len + suffix_len >= sizeof( fname ) ) {
        PrintALineThenDrop( "Path too long: %s", dir );
        error_occured = 1;
        return;
    }
    memcpy( fname, dir, len );
    memcpy( fname + len, suffix, suffix_len + 1 );
    DoRM( fname );

    /* purge the directory */
    if( iflag ) {
        PrintALine( "Delete directory %s (y/n)", dir );
        while( ( i = tolower( getch() ) ) != 'y' && i != 'n' )
            ;
        DropALine();
    } else {
        i = 'y';
    }
    if( i == 'y' )
        DoRMdir( dir );
}
