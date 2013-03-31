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
#define IS_PATH_SEP(x) ( (x) == '/')
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#else
#define IS_PATH_SEP(x) ( ( (x) == '\\') || (x) == '/')
#include <direct.h>
#include <dos.h>
#include <io.h>
#endif
#include <assert.h>
#include <setjmp.h>
#include <stdarg.h>
#include "watcom.h"
#include "pmake.h"

#ifdef __UNIX__
#define DEFAULT_MAKE_CMD        "wmake"
#else
#define DEFAULT_MAKE_CMD        "wmake.exe"
#endif
#define DEFAULT_MAKE_FILE       "makefile"
#define DEFAULT_PRIORITY        100
#define ALL_TARGET              "all"

#define COOKIE          "pmake"

#define TARGET_NOT_USED 0
#define TARGET_USED     1

typedef struct dirqueue {
    struct dirqueue     *next;
    unsigned            depth;
    char                name[_MAX_PATH];
}                       dirqueue;

int                     NumDirectories;

dirqueue                *QueueHead = NULL;
dirqueue                *QueueTail = NULL;
volatile int            DoneFlag;
jmp_buf                 exit_buff;

pmake_data              Options;
char                    Buff[512];
char                    *CmdLine;
char                    saveDirBuff[_MAX_PATH];
char                    *SaveDir = saveDirBuff;

static char *StringCopy( char *dst, char *src )
{
    while( ( *dst = *src ) != '\0' ) {
        ++dst;
        ++src;
    }
    return( dst );
}

static void error( char *fmt, ... )
{
    va_list     arg;

#define PREFIX      "PMAKE: "

    StringCopy( Buff, PREFIX );
    va_start( arg, fmt );
    vsprintf( &Buff[sizeof( PREFIX )], fmt, arg );
    va_end( arg );
    longjmp( exit_buff, 1 );
}

static void *safe_malloc( size_t n )
{
    void        *p = malloc( n );

    if( p == NULL ) {
        error( "out of memory when allocating %d bytes", n );
    }
    return( p );
}

static int _comparison( const void *pp1, const void *pp2 )
{
    pmake_list  *p1;
    pmake_list  *p2;

    p1 = *( pmake_list**) pp1;
    p2 = *( pmake_list**) pp2;
    if( p1->priority > p2->priority )
        return( +1 );
    if( p1->priority < p2->priority )
        return( -1 );

    if( Options.reverse ) {
        pmake_list      *t = p1;

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
        curr->used = TARGET_NOT_USED;
    }
}

static unsigned CompareTargets( char *line )
{
    unsigned    priority;
    target_list *curr;

    priority = DEFAULT_PRIORITY;
    while( isspace( *line ) )
        line++;
    if( *line != '#' )
        return( 0 );
    line++;
    while( isspace( *line ) )
        line++;
    if( strnicmp( line, COOKIE, sizeof( COOKIE ) - 1 ) != 0 )
        return( 0 );
    line += ( sizeof( COOKIE ) - 1 );
    while( isspace( *line ) )
        line++;
    if( *line == '/' ) {
        ++line;
        priority = strtoul( line, &line, 0 );
        if( priority == 0 )
            return( 0 );
        while( isspace( *line ) )
            line++;
    }
    if( *line != ':' )
        return( 0 );
    line++;
    while( isspace( *line ) )
        line++;
    while( *line != '\0' ) {
        for( curr = Options.targ_list; curr != NULL; curr = curr->next ) {
            if( curr->used == TARGET_NOT_USED ) {
                size_t len = strlen( curr->string );
                if( strnicmp( line, curr->string, len ) == 0 ) {
                    if( line[len] == '\0' || isspace( line[len] ) ) {
                        line += len;
                        curr->used = TARGET_USED;
                        break;
                    }
                }
            }
        }
        while( !isspace( *line ) )
            line++;
        while( isspace( *line ) )
            line++;
    }
    return( priority );
}

static unsigned CheckTargets( char *filename )
{
    FILE        *mf;
    unsigned    curr_prio;
    unsigned    prio;

    mf = fopen( filename, "r" );
    if( mf == NULL )
        return( 0 );
    ResetMatches();
    prio = 0;
    for( ;; ) {
        if( fgets( Buff, sizeof( Buff ), mf ) == NULL )
            break;
        curr_prio = CompareTargets( Buff );
        if( prio != 0 && curr_prio == 0 )
            break;
        prio = curr_prio;
    }
    fclose( mf );
    return( prio );
}

static void InitQueue( char *cwd )
{
    dirqueue    *qp;
    char        *p;

    qp = safe_malloc( sizeof( *qp ) );
    qp->next = NULL;
    qp->depth = 0;
    p = StringCopy( qp->name, cwd );
    QueueHead = qp;
    QueueTail = qp;
}

static void EnQueue( char *path )
{
    dirqueue    *qp;
    char        *p;

    if( QueueHead->depth < Options.levels ) {
        qp = safe_malloc( sizeof( *qp ) );
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
        free( qp );
    }
}

static unsigned CountDepth( char *path, unsigned slashcount )
{
    while( *path != '\0' ) {
        if( IS_PATH_SEP( *path ) ) {
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

static char *RelativePath( char *oldpath, char *newpath )
{
    int         ofs = 0;
    char        *tp;
    unsigned    newdepth;
    unsigned    olddepth;

    if( oldpath == NULL )
        return( newpath );
    while( newpath[ofs] == oldpath[ofs] ) {
        // newpath and oldpath are identical
        if( newpath[ofs] == '\0' )
            return( "" );
        ofs++;
    }
    // oldpath is a prefix of newpath
    if( oldpath[ofs] == '\0' && IS_PATH_SEP( newpath[ofs] ) ) {
        return( &newpath[ofs + 1] );
    }
    // newpath is a prefix of oldpath
    if( newpath[0] == '\0' && IS_PATH_SEP( oldpath[ofs] ) ) {
        newdepth = CountDepth( newpath, 0 );
        olddepth = CountDepth( oldpath, 0 );
        tp = PrependDotDotSlash( Buff, olddepth - newdepth );
        *( --tp ) = '\0'; // remove trailing slash
        return( Buff );
    }
    /* back up to start of directory */
    for( ;; ) {
        if( ofs == 0 )
            break;
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

static int TrueTarget( void )
{
    target_list *curr;
    char        eval_stk[MAX_EVAL_DEPTH];
    int         sp;

    eval_stk[0] = TARGET_NOT_USED;
    sp = -1;
    for( curr = Options.targ_list; curr != NULL; curr = curr->next ) {
        if( stricmp( curr->string, ".and" ) == 0 ) {
            if( sp < 1 )
                error( "too few elements on expr stack" );
            --sp;
            eval_stk[sp] &= eval_stk[sp + 1];
        } else if( stricmp( curr->string, ".or" ) == 0 ) {
            if( sp < 1 )
                error( "too few elements on expr stack" );
            --sp;
            eval_stk[sp] |= eval_stk[sp + 1];
        } else if( stricmp( curr->string, ".not" ) == 0 ) {
            if( sp < 0 )
                error( "too few elements on expr stack" );
            eval_stk[sp] = !eval_stk[sp];
        } else {
            if( stricmp( curr->string, ALL_TARGET ) == 0 ) {
                curr->used = TARGET_USED;
            }
            if( ++sp >= MAX_EVAL_DEPTH ) {
                error( "expr stack depth exceeds max of %u", MAX_EVAL_DEPTH );
            }
            eval_stk[sp] = curr->used;
        }
    }
    while( sp > 0 ) {
        eval_stk[sp - 1] &= eval_stk[sp];
        --sp;
    }
    return( eval_stk[0] );
}

static void TestDirectory( dirqueue *head, char *makefile )
{
    unsigned    prio;
    pmake_list  *new;
    size_t      len;

    if( Options.verbose ) {
        sprintf( Buff, ">>> PMAKE >>> %s/%s", head->name, makefile );
        PMakeOutput( "" );
        PMakeOutput( Buff );
    }
    prio = CheckTargets( makefile );
    if( prio != 0 && TrueTarget() ) {
        len = strlen( head->name );
        new = safe_malloc( sizeof( *new ) + len );
        new->next = Options.dir_list;
        Options.dir_list = new;
        new->depth = head->depth;
        new->priority = prio;
        StringCopy( new->dir_name, head->name );
        ++NumDirectories;
    }
}

static void SetDoneFlag( int sig_no )
{
    sig_no = sig_no;
    DoneFlag = 1;
}

static void ProcessDirectoryQueue( void )
{
    DIR                 *dirh;
    struct dirent       *dp;
    dirqueue            *head;
    dirqueue            *last_ok;
    char                *makefile;
#ifdef __UNIX__
    struct stat          buf;
#endif

    makefile = Options.makefile;
    if( makefile == NULL ) {
        makefile = DEFAULT_MAKE_FILE;
    }
    head = QueueHead;
    while( head != NULL ) {
        dirh = opendir( "." );
        if( dirh != NULL ) {
            for( ;; ) {
                if( DoneFlag )
                    return;
                dp = readdir( dirh );
                if( dp == NULL )
                    break;
#ifdef __UNIX__
                if( !stat( dp->d_name, &buf ) && S_ISDIR( buf.st_mode ) ) {
#else
                if( dp->d_attr & _A_SUBDIR ) {
#endif
                    if( dp->d_name[0] == '.' ) {
                        if( dp->d_name[1] == '.' || dp->d_name[1] == '\0' )
                            continue;
                    }
                    EnQueue( dp->d_name );
                } else if( stricmp( dp->d_name, makefile ) == 0 ) {
                    TestDirectory( head, makefile );
                }
            }
            closedir( dirh );
        }
        last_ok = NULL;
        while( head->next != NULL ) {
            if( last_ok == NULL ) {
                if( chdir( RelativePath( head->name, head->next->name ) ) == 0 )
                    break;
                last_ok = head;
                QueueHead = head->next;
            } else if( chdir( RelativePath( last_ok->name, head->next->name ) ) == 0 ) {
                free( last_ok );
                break;
            } else {
                DeQueue();
            }
            sprintf( Buff, "PMAKE warning: can not change directory to %s", head->next->name );
            PMakeOutput( Buff );
            head = QueueHead;
        }
        DeQueue();
        head = QueueHead;
    }
}

static int GetNumber( int default_num )
{
    int         number;

    if( !isdigit( CmdLine[0] ) )
        return( default_num );
    number = 0;
    for( ;; ) {
        if( !isdigit( CmdLine[0] ) )
            return( number );
        number *= 10;
        number += CmdLine[0] - '0';
        ++CmdLine;
    }
}

static char *GetString( void )
{
    char        *p;
    size_t      len;
    char        *new;

    while( isspace( CmdLine[0] ) )
        ++CmdLine;
    if( CmdLine[0] == '\0' )
        return( NULL );
    p = CmdLine;
    for( ;; ) {
        if( isspace( CmdLine[0] ) )
            break;
        if( CmdLine[0] == '\0' )
            break;
        ++CmdLine;
    }
    len = CmdLine - p;
    new = safe_malloc( len + 1 );
    memcpy( new, p, len );
    new[len] = '\0';
    return( new );
}

static void SortDirectories( void )
{
    pmake_list  **dir_array;
    pmake_list  *curr;
    char        *prev_name;
    char        *new_name;
    char        buff[_MAX_PATH];
    int         i;

    dir_array = safe_malloc( sizeof( *dir_array )* NumDirectories );
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
    free( dir_array );
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

    memset( &Options, 0, sizeof( Options ) );
    Options.command = safe_malloc( sizeof( DEFAULT_MAKE_CMD ) );
    StringCopy( Options.command, DEFAULT_MAKE_CMD );
    Options.levels = INT_MAX;
    while( *CmdLine == ' ' )
        ++CmdLine;
    if( *CmdLine == '\0' || *CmdLine == '?' ) {
        Options.want_help = 1;
        return;
    }
    /* gather options */
    for( ;; ) {
        while( isspace( *CmdLine ) )
            ++CmdLine;
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
            free( Options.makefile );
            Options.makefile = GetString();
            if( Options.makefile == NULL ) {
                Options.want_help = 1;
                return;
            }
            break;
        case 'i':
            Options.ignore_err = 1;
            break;
        case 'l':
            Options.levels = GetNumber( 1 );
            break;
        case 'm':
            free( Options.command );
            Options.command = GetString();
            if( Options.command == NULL ) {
                Options.want_help = 1;
                return;
            }
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
            while( isspace( *CmdLine ) )
                ++CmdLine;
            if( *CmdLine == '\0' )
                break;
            if( *CmdLine == '-' || *CmdLine == '/' ) {
                if( CmdLine[1] == '-' || CmdLine[1] == '/' ) {
                    CmdLine += 2;
                }
                break;
            }
            curr = safe_malloc( sizeof( *Options.targ_list ) );
            curr->next = NULL;
            curr->string = GetString();
            *owner = curr;
            owner = &curr->next;
        }
    }
    if( Options.targ_list == NULL ) {
        Options.targ_list = safe_malloc( sizeof( *Options.targ_list ) );
        Options.targ_list->next = NULL;
        Options.targ_list->string = safe_malloc( sizeof( ALL_TARGET ) );
        StringCopy( Options.targ_list->string, ALL_TARGET );
    }
    while( isspace( *CmdLine ) )
        CmdLine++;
    NumDirectories = 0;
    InitQueue( SaveDir );
    ProcessDirectoryQueue();
    if( NumDirectories > 0 ) {
        SortDirectories();
    }
}

pmake_data *PMakeBuild( const char *cmd )
{
    void                ( *old_sig ) ( int );
    volatile int        ret;

    getcwd( SaveDir, _MAX_PATH );
    DoneFlag = 0;
    old_sig = signal( SIGINT, SetDoneFlag );
    CmdLine = ( char *) cmd;
    ret = setjmp( exit_buff );
    if( ret == 0 )
        DoIt();
    signal( SIGINT, old_sig );
    chdir( SaveDir );
    while( QueueHead != NULL ) {
        DeQueue();
    }
    if( ret != 0 ) {
        PMakeCleanup( &Options );
        return( NULL );
    }
    if( DoneFlag )
        Options.signaled = 1;
    Options.cmd_args = CmdLine;
    return( &Options );
}

void PMakeCommand( pmake_data *data, char *cmd )
{
    if( data->makefile == NULL ) {
        sprintf( cmd, "%s %s", data->command, data->cmd_args );
    } else {
        sprintf( cmd, "%s -f %s %s", data->command, data->makefile, data->cmd_args );
    }
}

void PMakeCleanup( pmake_data *data )
{
    void        *tmp;

    free( data->command );
    free( data->makefile );
    while( data->dir_list != NULL ) {
        tmp = data->dir_list->next;
        free( data->dir_list );
        data->dir_list = tmp;
    }
    while( data->targ_list != NULL ) {
        tmp = data->targ_list->next;
        free( data->targ_list->string );
        free( data->targ_list );
        data->targ_list = tmp;
    }
}
