/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  pmake action functions
*
****************************************************************************/

#include <ctype.h>
#include <signal.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef __UNIX__
#include <dirent.h>
#else
#include <direct.h>
#include <dos.h>
#endif
#include <assert.h>
#include <setjmp.h>
#include <stdarg.h>
#include "wio.h"
#include "bool.h"
#include "watcom.h"
#include "pmake.h"
#include "iopath.h"
#include "memutils.h"

#include "clibext.h"


#ifdef __UNIX__
#define DEFAULT_MAKE_CMD        "wmake"
#else
#define DEFAULT_MAKE_CMD        "wmake.exe"
#endif
#define DEFAULT_MAKE_FILE       "makefile"
#define DEFAULT_PRIORITY        100
#define ALL_TARGET              "all"

#define COOKIE          "pmake"

#define SKIP_SPACES(p)  while( isspace(*(p)) ) ++(p)
#define SKIP_NOSPACE(p) while( *(p) != '\0' && !isspace(*(p)) ) ++(p)

#ifdef __UNIX__
#define IS_SUBDIR(d)  (stat(d->d_name, &buf) == 0 && S_ISDIR(buf.st_mode))
#else
#define IS_SUBDIR(d)  (d->d_attr & _A_SUBDIR)
#endif
#define DOT_OR_DOTDOT(d) (d->d_name[0] == '.' && (d->d_name[1] == '\0' || d->d_name[1] == '.' && d->d_name[2] == '\0'))

typedef struct dirqueue {
    struct dirqueue     *next;
    unsigned            depth;
    char                name[1];
} dirqueue;

static int              NumDirectories;

static dirqueue         *QueueHead = NULL;
static dirqueue         *QueueTail = NULL;
static volatile int     DoneFlag;
static jmp_buf          exit_buff;

static pmake_data       Options;
static char             Buff[512];
static char             *CmdLine;
static char             saveDirBuff[_MAX_PATH];
static char             *SaveDir = saveDirBuff;

static char *StringCopy( char *dst, const char *src )
{
    while( (*dst = *src) != '\0' ) {
        ++dst;
        ++src;
    }
    return( dst );
}

static void error( const char *fmt, ... )
{
    va_list     arg;

#define PREFIX      "PMAKE: "

    StringCopy( Buff, PREFIX );
    va_start( arg, fmt );
    vsprintf( &Buff[sizeof( PREFIX )], fmt, arg );
    va_end( arg );
    longjmp( exit_buff, 1 );
}

static int _comparison( const void *pp1, const void *pp2 )
{
    pmake_list  *p1;
    pmake_list  *p2;

    p1 = *(pmake_list **)pp1;
    p2 = *(pmake_list **)pp2;
    if( p1->priority > p2->priority )
        return( +1 );
    if( p1->priority < p2->priority )
        return( -1 );

    if( Options.reverse ) {
        pmake_list  *t;

        t = p1;
        p1 = p2;
        p2 = t;
    }
    if( p1->depth > p2->depth )
        return( -1 );
    if( p1->depth < p2->depth )
        return( +1 );
    return( stricmp( p1->dir_name, p2->dir_name ) );
}

static void ResetMatches( void )
{
    target_list *curr;

    for( curr = Options.targ_list; curr != NULL; curr = curr->next ) {
        if( curr->flags == TARGET_USED ) {
            curr->flags = TARGET_NOT_USED;
        }
    }
}

static unsigned CompareTargets( const char *line )
{
    unsigned    priority;
    target_list *curr;
    char        *numend;

    priority = DEFAULT_PRIORITY;
    SKIP_SPACES( line );
    if( *line != '#' )
        return( 0 );
    line++;
    SKIP_SPACES( line );
    if( strnicmp( line, COOKIE, sizeof( COOKIE ) - 1 ) != 0 )
        return( 0 );
    line += ( sizeof( COOKIE ) - 1 );
    SKIP_SPACES( line );
    if( *line == '/' ) {
        ++line;
        priority = strtoul( line, &numend, 0 );
        if( priority == 0 )
            return( 0 );
        line = numend;
        SKIP_SPACES( line );
    }
    if( *line != ':' )
        return( 0 );
    line++;
    SKIP_SPACES( line );
    while( *line != '\0' ) {
        for( curr = Options.targ_list; curr != NULL; curr = curr->next ) {
            if( curr->flags == TARGET_NOT_USED ) {
                size_t len = curr->len;
                if( strnicmp( line, curr->string, len ) == 0 ) {
                    unsigned char c = line[len];
                    if( c == '\0' || isspace( c ) ) {
                        curr->flags = TARGET_USED;
                    }
                }
            }
        }
        SKIP_NOSPACE( line );
        SKIP_SPACES( line );
    }
    return( priority );
}

static unsigned CheckTargets( const char *filename )
{
    FILE        *mf;
    unsigned    curr_prio;
    unsigned    prio;

    mf = fopen( filename, "r" );
    if( mf == NULL )
        return( 0 );
    ResetMatches();
    prio = 0;
    while( fgets( Buff, sizeof( Buff ), mf ) != NULL ) {
        curr_prio = CompareTargets( Buff );
        if( prio != 0 && curr_prio == 0 )
            break;
        prio = curr_prio;
    }
    fclose( mf );
    return( prio );
}

static void InitQueue( const char *cwd )
{
    dirqueue    *qp;
    size_t      len;

    len = strlen( cwd );
    qp = MAlloc( sizeof( *qp ) + len );
    qp->next = NULL;
    qp->depth = 0;
    StringCopy( qp->name, cwd );
    QueueHead = qp;
    QueueTail = qp;
}

static void EnQueue( const char *path )
{
    dirqueue    *qp;
    char        *p;

    if( QueueHead->depth < Options.levels ) {
        qp = MAlloc( sizeof( *qp ) + strlen( QueueHead->name ) + 1 + strlen( path ) );
        qp->next = NULL;
        qp->depth = QueueHead->depth + 1;
        p = StringCopy( qp->name, QueueHead->name );
#ifdef __UNIX__
        p = StringCopy( p, "/" );
#else
        p = StringCopy( p, "\\" );
#endif
        StringCopy( p, path );
        QueueTail->next = qp;
        QueueTail = qp;
    }
}

static void DeQueue( void )
{
    dirqueue    *qp;

    qp = QueueHead;
    if( qp != NULL ) {
        QueueHead = qp->next;
        MFree( qp );
    }
}

static unsigned CountDepth( const char *path, unsigned slashcount )
{
    while( *path != '\0' ) {
        if( IS_DIR_SEP( *path ) ) {
            slashcount++;
        }
        path++;
    }
    return( slashcount );
}

static char *PrependDotDotSlash( char *str, int count )
{
    while( count-- ) {
#ifdef __UNIX__
        str = StringCopy( str, "../" );
#else
        str = StringCopy( str, "..\\" );
#endif
    }
    return( str );
}

static const char *RelativePath( const char *oldpath, const char *newpath )
{
    int         ofs;
    char        *tp;
    unsigned    newdepth;
    unsigned    olddepth;

    if( oldpath == NULL )
        return( newpath );
    for( ofs = 0; newpath[ofs] == oldpath[ofs]; ++ofs ) {
        // newpath and oldpath are identical
        if( newpath[ofs] == '\0' ) {
            return( "" );
        }
    }
    // oldpath is a prefix of newpath
    if( oldpath[ofs] == '\0' && IS_DIR_SEP( newpath[ofs] ) ) {
        return( newpath + ofs + 1 );
    }
    // newpath is a prefix of oldpath
    if( newpath[0] == '\0' && IS_DIR_SEP( oldpath[ofs] ) ) {
        newdepth = CountDepth( newpath, 0 );
        olddepth = CountDepth( oldpath, 0 );
        tp = PrependDotDotSlash( Buff, olddepth - newdepth );
        *(--tp) = '\0'; // remove trailing slash
        return( Buff );
    }
    /* back up to start of directory */
    while( ofs != 0 ) {
        if( IS_PATH_SEP( newpath[ofs - 1] ) )
            break;
        --ofs;
    }
    newpath += ofs;
    oldpath += ofs;
    olddepth = CountDepth( oldpath, 1 );
    tp = PrependDotDotSlash( Buff, olddepth );
    tp = StringCopy( tp, newpath );
    return( Buff );
}

#define MAX_EVAL_DEPTH  64

static bool TrueTarget( void )
{
    target_list *curr;
    bool        eval_stk[MAX_EVAL_DEPTH];
    int         sp;

    eval_stk[0] = false;
    sp = -1;
    for( curr = Options.targ_list; curr != NULL; curr = curr->next ) {
        switch( curr->flags ) {
        case TARGET_OPERATOR_AND:
            if( sp < 1 )
                error( "too few elements on expr stack" );
            --sp;
            eval_stk[sp] &= eval_stk[sp + 1];
            break;
        case TARGET_OPERATOR_OR:
            if( sp < 1 )
                error( "too few elements on expr stack" );
            --sp;
            eval_stk[sp] |= eval_stk[sp + 1];
            break;
        case TARGET_OPERATOR_NOT:
            if( sp < 0 )
                error( "too few elements on expr stack" );
            eval_stk[sp] = !eval_stk[sp];
            break;
        case TARGET_ALL:
            if( ++sp >= MAX_EVAL_DEPTH ) {
                error( "expr stack depth exceeds max of %u", MAX_EVAL_DEPTH );
            }
            eval_stk[sp] = true;
            break;
        default:
            if( ++sp >= MAX_EVAL_DEPTH ) {
                error( "expr stack depth exceeds max of %u", MAX_EVAL_DEPTH );
            }
            eval_stk[sp] = ( curr->flags == TARGET_USED );
            break;
        }
    }
    while( sp > 0 ) {
        eval_stk[sp - 1] &= eval_stk[sp];
        --sp;
    }
    return( eval_stk[0] );
}

static void TestDirectory( const char *makefile )
{
    unsigned    prio;
    pmake_list  *new;
    size_t      len;

    if( Options.verbose ) {
        sprintf( Buff, ">>> PMAKE >>> %s/%s", QueueHead->name, makefile );
        PMakeOutput( "" );
        PMakeOutput( Buff );
    }
    prio = CheckTargets( makefile );
    if( prio != 0 && TrueTarget() ) {
        len = strlen( QueueHead->name );
        new = MAlloc( sizeof( *new ) + len );
        new->next = Options.dir_list;
        Options.dir_list = new;
        new->depth = QueueHead->depth;
        new->priority = prio;
        StringCopy( new->dir_name, QueueHead->name );
        ++NumDirectories;
    }
}

static void SetDoneFlag( int sig_no )
{
    /* unused parameters */ (void)sig_no;

    DoneFlag = 1;
}

static void NextSubdir( void )
{
    dirqueue    *last_ok;

    /* skip subdirectory entry if it cannot be set as current directory */
    last_ok = NULL;
    while( QueueHead->next != NULL ) {
        if( last_ok == NULL ) {
            if( chdir( RelativePath( QueueHead->name, QueueHead->next->name ) ) == 0 )
                break;
            last_ok = QueueHead;
            QueueHead = QueueHead->next;
        } else if( chdir( RelativePath( last_ok->name, QueueHead->next->name ) ) == 0 ) {
            MFree( last_ok );
            break;
        } else {
            DeQueue();
        }
        sprintf( Buff, "PMAKE warning: can not change directory to %s", QueueHead->next->name );
        PMakeOutput( Buff );
    }
    /* remove last/current entry from queue a set to next entry for processing */
    DeQueue();
}

static void ProcessDirectoryQueue( void )
{
    DIR                 *dirh;
    struct dirent       *dp;
    const char          *makefile;
#ifdef __UNIX__
    struct stat         buf;
#endif

    makefile = Options.makefile;
    while( QueueHead != NULL ) {
        /* process directory */
        dirh = opendir( "." );
        if( dirh != NULL ) {
            while( (dp = readdir( dirh )) != NULL ) {
                if( IS_SUBDIR( dp ) ) {
                    if( DOT_OR_DOTDOT( dp ) )
                        continue;
                    /* queue subdirectory */
                    EnQueue( dp->d_name );
                } else if( stricmp( dp->d_name, makefile ) == 0 ) {
                    /* add directory with makefile to the list for next processing */
                    TestDirectory( makefile );
                }
                if( DoneFlag ) {
                    break;
                }
            }
            closedir( dirh );
        }
        if( DoneFlag ) {
            return;
        }
        /* set current directory to first possible queued subdirectory */
        NextSubdir();
    }
}

static int GetNumber( int default_num )
{
    int         number;

    if( !isdigit( *CmdLine ) )
        return( default_num );
    number = 0;
    for( ; isdigit( *CmdLine ); ++CmdLine ) {
        number *= 10;
        number += *CmdLine - '0';
    }
    return( number );
}

static char *GetString( void )
{
    char    *start;

    SKIP_SPACES( CmdLine );
    if( *CmdLine == '\0' )
        return( NULL );
    start = CmdLine;
    SKIP_NOSPACE( CmdLine );
    if( *CmdLine != '\0' ) {
        *CmdLine++ = '\0';
    }
    return( start );
}

static void SortDirectories( void )
{
    pmake_list  **dir_array;
    pmake_list  *curr;
    char        *prev_name;
    const char  *new_name;
    char        buff[_MAX_PATH];
    int         i;

    dir_array = MAlloc( sizeof( *dir_array ) * NumDirectories );
    i = 0;
    for( curr = Options.dir_list; curr != NULL; curr = curr->next ) {
        dir_array[i++] = curr;
    }
    qsort( dir_array, NumDirectories, sizeof( *dir_array ), &_comparison );
    /* rebuild list in sorted order */
    Options.dir_list = NULL;
    for( i = NumDirectories - 1; i >= 0; --i ) {
        curr = dir_array[i];
        curr->next = Options.dir_list;
        Options.dir_list = curr;
    }
    MFree( dir_array );
    if( Options.optimize ) {
        prev_name = NULL;
        for( curr = Options.dir_list; curr != NULL; curr = curr->next ) {
            new_name = RelativePath( prev_name, curr->dir_name );
            StringCopy( buff, curr->dir_name );
            prev_name = buff;
            StringCopy( curr->dir_name, new_name );
        }
    }
}

static void DoIt( void )
{
    target_list **owner;
    target_list *curr;
    char        *arg;

    memset( &Options, 0, sizeof( Options ) );
    Options.levels = INT_MAX;
    SKIP_SPACES( CmdLine );
    if( *CmdLine == '\0' || *CmdLine == '?' ) {
        Options.want_help = 1;
        return;
    }
    /* gather options */
    for( ;; ) {
        SKIP_SPACES( CmdLine );
        if( *CmdLine != '-' && *CmdLine != '/' )
            break;
        ++CmdLine;
        if( *CmdLine == '-' || *CmdLine == '/' ) {
            ++CmdLine;
            Options.notargets = 1;
            break;
        }
        switch( *CmdLine++ ) {
        case 'b':
            Options.batch = 1;
            break;
        case 'd':
            Options.display = 1;
            break;
        case 'f':
            arg = GetString();
            if( arg == NULL ) {
                Options.want_help = 1;
                return;
            }
            MFree( Options.makefile );
            Options.makefile = MAlloc( strlen( arg ) + 1 );
            StringCopy( Options.makefile, arg );
            break;
        case 'i':
            Options.ignore_errors = 1;
            break;
        case 'l':
            Options.levels = GetNumber( 1 );
            break;
        case 'm':
            arg = GetString();
            if( arg == NULL ) {
                Options.want_help = 1;
                return;
            }
            MFree( Options.command );
            Options.command = MAlloc( strlen( arg ) + 1 );
            StringCopy( Options.command, arg );
            break;
        case 'o':
            Options.optimize = 1;
            break;
        case 'r':
            Options.reverse = 1;
            break;
        case 'v':
            Options.verbose = 1;
            break;
        case '?':
        default:
            Options.want_help = 1;
            return;
        }
    }
    /* gather targ_list */
    Options.targ_list = NULL;
    if( !Options.notargets ) {
        owner = &Options.targ_list;
        for( ;; ) {
            target_flags    flags;

            SKIP_SPACES( CmdLine );
            if( *CmdLine == '\0' )
                break;
            if( *CmdLine == '-' || *CmdLine == '/' ) {
                if( CmdLine[1] == '-' || CmdLine[1] == '/' ) {
                    CmdLine += 2;
                }
                break;
            }
            arg = GetString();
            if( stricmp( arg, ".and" ) == 0 ) {
                flags = TARGET_OPERATOR_AND;
            } else if( stricmp( arg, ".or" ) == 0 ) {
                flags = TARGET_OPERATOR_OR;
            } else if( stricmp( arg, ".not" ) == 0 ) {
                flags = TARGET_OPERATOR_NOT;
            } else if( stricmp( arg, ALL_TARGET ) == 0 ) {
                flags = TARGET_ALL;
            } else {
                flags = TARGET_NOT_USED;
            }
            switch( flags ) {
            case TARGET_OPERATOR_AND:
            case TARGET_OPERATOR_OR:
            case TARGET_OPERATOR_NOT:
            case TARGET_ALL:
                curr = MAlloc( sizeof( *Options.targ_list ) - 1 );
                curr->len = 0;
                break;
            case TARGET_NOT_USED:
            default:
                {
                    size_t len = strlen( arg );
                    curr = MAlloc( sizeof( *Options.targ_list ) + len );
                    StringCopy( curr->string, arg );
                    curr->len = len;
                }
                break;
            }
            curr->flags = flags;
            curr->next = NULL;
            *owner = curr;
            owner = &curr->next;
        }
    }
    if( Options.targ_list == NULL ) {
        Options.targ_list = MAlloc( sizeof( *Options.targ_list ) );
        Options.targ_list->next = NULL;
        Options.targ_list->flags = TARGET_ALL;
        Options.targ_list->len = 0;
    }
    SKIP_SPACES( CmdLine );
    Options.cmd_args = MAlloc( strlen( CmdLine ) + 1 );
    StringCopy( Options.cmd_args, CmdLine );
    /* end of command line processing */

    /* setup default value if not defined on command line */
    if( Options.makefile == NULL || *Options.makefile == '\0' ) {
        MFree( Options.makefile );
        Options.makefile = MAlloc( sizeof( DEFAULT_MAKE_FILE ) );
        StringCopy( Options.makefile, DEFAULT_MAKE_FILE );
    }
    if( Options.command == NULL || *Options.command == '\0' ) {
        MFree( Options.command );
        Options.command = MAlloc( sizeof( DEFAULT_MAKE_CMD ) );
        StringCopy( Options.command, DEFAULT_MAKE_CMD );
    }

    /* start directory tree processing */
    NumDirectories = 0;
    InitQueue( SaveDir );
    ProcessDirectoryQueue();
    if( NumDirectories > 0 ) {
        SortDirectories();
    }
}

pmake_data *PMakeBuild( char *cmd )
{
    void                (*old_sig)( int );
    volatile int        ret;

    getcwd( SaveDir, _MAX_PATH );
    DoneFlag = 0;
    old_sig = signal( SIGINT, SetDoneFlag );
    CmdLine = cmd;
    ret = setjmp( exit_buff );
    if( ret == 0 )
        DoIt();
    signal( SIGINT, old_sig );
    chdir( SaveDir );
    if( DoneFlag )
        Options.signaled = 1;
    while( QueueHead != NULL ) {
        DeQueue();
    }
    if( ret != 0 ) {
        PMakeCleanup( &Options );
        return( NULL );
    }
    return( &Options );
}

void PMakeCommand( pmake_data *data, char *cmd )
{
    sprintf( cmd, "%s -f %s %s", data->command, data->makefile, data->cmd_args );
}

void PMakeCleanup( pmake_data *data )
{
    void        *tmp;

    while( data->dir_list != NULL ) {
        tmp = data->dir_list->next;
        MFree( data->dir_list );
        data->dir_list = tmp;
    }
    while( data->targ_list != NULL ) {
        tmp = data->targ_list->next;
        MFree( data->targ_list );
        data->targ_list = tmp;
    }
    if( data->command != NULL ) {
        MFree( data->command );
        data->command = NULL;
    }
    if( data->makefile != NULL ) {
        MFree( data->makefile );
        data->makefile = NULL;
    }
    if( data->cmd_args != NULL ) {
        MFree( data->cmd_args );
        data->cmd_args = NULL;
    }
}
