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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#include <env.h>
#include <direct.h>
#include "builder.h"

#define DEFCTLNAME      "BUILDER.CTL"

#define DEF_BACKUP      1
#define MAX_BACKUP      9

ctl_file        *CtlList;
include         *IncludeStk;
FILE            *LogFile;
char            Line[MAX_LINE];
char            ProcLine[MAX_LINE];
unsigned        VerbLevel;
bool            UndefWarn;
bool            Quiet;
unsigned        ParmCount;
unsigned        LogBackup;

static void PutNumber( char *src, char *dst, unsigned num )
{
    char        dig;
    int         i;

    memset( dst, 0, 5 );
    strncpy( dst, src, 3 );
    dst[0] = '.';
    for( i = 3; i > 0; --i ) {
        dig = num % 10;
        num /= 10;
        if( dig != 0 || dst[i] == '\0' ) {
            dst[i] = dig + '0';
        }
    }
}

static void BackupLog( const char *log_name, unsigned copies )
{
    char        buff[_MAX_PATH2];
    char        *drive;
    char        *dir;
    char        *fn;
    char        *ext;
    char        old_name[_MAX_PATH];
    char        new_name[_MAX_PATH];
    char        temp_ext[5];

    if( copies > MAX_BACKUP ) copies = MAX_BACKUP;
    if( copies == 0 ) {
        remove( log_name );
        return;
    }
    _splitpath2( log_name, buff, &drive, &dir, &fn, &ext );
    while( copies != 0 ) {
        PutNumber( ext, temp_ext, copies );
        _makepath( new_name, drive, dir, fn, temp_ext );
        remove( new_name );
        if( copies == 1 ) {
            strcpy( old_name, log_name );
        } else {
            PutNumber( ext, temp_ext, copies - 1 );
            _makepath( old_name, drive, dir, fn, temp_ext );
        }
        rename( old_name, new_name );
        --copies;
    }
}

static void AddCtlFile( const char *name )
{
    ctl_file    **owner;
    ctl_file    *curr;

    owner = &CtlList;
    for( ;; ) {
        curr = *owner;
        if( curr == NULL ) break;
        owner = &curr->next;
    }
    curr = Alloc( sizeof( *curr ) );
    curr->next = NULL;
    strcpy( curr->name, name );
    *owner = curr;
}

static char **getvalue( char **argv, char *buff )
{
    if( argv[0][2] != '\0' ) {
        strcpy( buff, &argv[0][2] );
        return( argv );
    }
    ++argv;
    strcpy( buff, argv[0] );
    return( argv );
}

static void Usage()
{
    printf( "Usage: BUILDER [-c <ctl_file>]* [-l <log_file>] [-b <backup>] [-v] [-u] [-q] [--] <parm>*\n" );
    printf( "    See BUILDER.DOC for more information\n" );
    exit( 0 );
}

static void ProcessOptions( char *argv[] )
{
    char        parm_buff[_MAX_PATH];
    bool        opt_end;

    LogBackup = DEF_BACKUP;
    opt_end = FALSE;
    while( argv[0] != NULL ) {
        if( !opt_end && argv[0][0] == '-' ) {
            switch( tolower(argv[0][1]) ) {
            case 'c':
               argv = getvalue( argv, parm_buff );
               AddCtlFile( parm_buff );
               break;
            case 'l':
               argv = getvalue( argv, parm_buff );
               if( LogFile != NULL ) {
                   Fatal( "-l option specified twice\n" );
               }
               BackupLog( parm_buff, LogBackup );
               OpenLog( parm_buff );
               break;
            case 'b':
               argv = getvalue( argv, parm_buff );
               LogBackup = strtoul( parm_buff, NULL, 0 );
               if( LogBackup > MAX_BACKUP ) {
                   Fatal( "-b value is exceeds maximum of %d\n", MAX_BACKUP );
               }
               break;
            case 'v':
                ++VerbLevel;
                break;
            case 'u':
                UndefWarn = TRUE;
                break;
                case 'q':
                Quiet = TRUE;
                        break;
            case '-':
                opt_end = TRUE;
                break;
            default:
                fprintf( stderr, "Unknown option '%c'\n\n", argv[0][1] );
                /* fall through */
            case '?':
                Usage();
                break;
            }
        } else if( strchr( argv[0], '=' ) != NULL ) {
            putenv( argv[0] );
        } else {
            sprintf( parm_buff, "%d", ++ParmCount );
            if( setenv( parm_buff, argv[0], 1 ) != 0 ) {
                Fatal( "Can not set parameter %u\n", ParmCount );
            }
        }
        ++argv;
    }
}

static void PushInclude( const char *name )
{
    include     *new;
    char        buff[_MAX_PATH2];
    char        *drive;
    char        *dir;
    char        *fn;
    char        *ext;
    char        dir_name[_MAX_PATH];

    new = Alloc( sizeof( *new ) );
    new->prev = IncludeStk;
    new->skipping = 0;
    new->reset_abit = NULL;
    IncludeStk = new;
    new->fp = fopen( name, "r" );
    if( new->fp == NULL ) {
        Fatal( "Could not open '%s': %s\n", name, strerror( errno ) );
    }
    strcpy( new->name, name );
    _splitpath2( name, buff, &drive, &dir, &fn, &ext );
    _makepath( dir_name, drive, dir, NULL, NULL );
    if( SysChdir( dir_name ) != 0 ) {
        Fatal( "Could not CD to '%s': %s\n", dir_name, strerror( errno ) );
    }
   getcwd( IncludeStk->cwd, sizeof( IncludeStk->cwd ) );
}

static bool PopInclude()
{
    include     *curr;

    curr = IncludeStk;
    fclose( curr->fp );
    IncludeStk = curr->prev;
    ResetArchives( curr->reset_abit );
    free( curr );
    if( IncludeStk == NULL ) return( FALSE );
    SysChdir( IncludeStk->cwd );
    return( TRUE );
}

static bool GetALine( char *line )
{
    for( ;; ) {
        fgets( line, MAX_LINE, IncludeStk->fp );
        if( ferror( IncludeStk->fp ) ) {
            Fatal( "Error reading '%s': %s\n", IncludeStk->name, strerror( errno ) );
        }
        if( !feof( IncludeStk->fp ) ) break;
        if( !PopInclude() ) return( FALSE );
    }
    return( TRUE );
}

static char *SubstOne( const char **inp, char *out )
{
    const char  *in;
    char        *p;
    char        *rep;
    unsigned    parm;

    in = *inp;
    p = out;
    for( ;; ) {
        switch( *in ) {
        case '>':
            *p = '\0';
            if( stricmp( out, "*" ) == 0 ) {
                p = out;
                for( parm = 1; parm <= ParmCount; ++parm ) {
                    sprintf( out, "%d", parm );
                    rep = getenv( out );
                    if( rep != NULL ) {
                        if( out != p ) *out++ = ' ';
                        strcpy( out, rep );
                        out += strlen( out );
                    }
                }
                *inp = in + 1;
                *out = '\0';
                return( out );
            } else if( stricmp( out, "CWD" ) == 0 ) {
                rep = IncludeStk->cwd;
            } else {
                rep = getenv( out );
            }
            if( rep == NULL ) {
                if( UndefWarn ) {
                    Log( FALSE, "<%s> is undefined\n", out );
                }
                rep = "";
            }
            strcpy( out, rep );
            *inp = in + 1;
            return( out + strlen( out ) );
        case '<':
            ++in;
            p = SubstOne( &in, p );
            break;
        default:
            *p++ = *in++;
            break;
        }
    }
}

static void SubstLine( const char *in, char *out )
{
    bool        first;

    first = TRUE;
    in = SkipBlanks( in );
    for( ;; ) {
        switch( *in ) {
        case '^':
             ++in;
             switch( *in ) {
             case '\n':
             case '\0':
                 break;
             default:
                 *out++ = *in++;
                 break;
             }
             break;
        case '[':
        case ']':
            if( !first) *out++= ' ';
            *out++ = *in++;
            *out++= ' ';
            break;
        case '<':
             ++in;
             out = SubstOne( &in, out );
             break;
        case '\n':
        case '\0':
            *out = '\0';
            return;
        default:
            *out++ = *in++;
        }
        first = FALSE;
    }
}

static char *FirstWord( char *p )
{
    char        *start;

    p = SkipBlanks( p );
    if( *p == '\0' ) return( NULL );
    start = p;
    for( ;; ) {
        switch( *p ) {
        case '\0':
            p[1] = '\0';
            /* fall through */
        case ' ':
        case '\t':
            *p = '\0';
            return( start );
        }
        ++p;
    }
}

static char *NextWord( char *p )
{
    return( FirstWord( p + strlen( p ) + 1 ) );
}


static void ProcessCtlFile( const char *name )
{
    char        *p;
    char        *log_name;
    char        *match;
    unsigned    res;
    bool        logit;

    PushInclude( name );
    while( GetALine( Line ) ) {
        SubstLine( Line, ProcLine );
        p = ProcLine;
        switch( *p ) {
        case '#':
        case '\0':
            /* just a comment */
            break;
        case '[':
            /* a directive */
            p = FirstWord( p + 1 );
            if( stricmp( p, "INCLUDE" ) == 0 ) {
                if( IncludeStk->skipping == 0 ) {
                    PushInclude( NextWord( p ) );
                }
            } else if( stricmp( p, "LOG" ) == 0 ) {
                if( IncludeStk->skipping == 0 ) {
                    log_name = NextWord( p );
                    p = NextWord( log_name );
                    if( p == NULL || strcmp( p, "]" ) == 0 ) {
                        BackupLog( log_name, LogBackup );
                    } else {
                        BackupLog( log_name, strtoul( p, NULL, 0 ) );
                    }
                    if( LogFile == NULL ) {
                        OpenLog( log_name );
                    }
                }
            } else if( stricmp( p, "BLOCK" ) == 0 ) {
                if( IncludeStk->skipping != 0 ) IncludeStk->skipping--;
                p = NextWord( p );
                if( p == NULL ) {
                    Fatal( "Missing match word\n" );
                }
                match = p;
                for( ;; ) {
                    p = NextWord( p );
                    if( p == NULL || strcmp( p, "]" ) == 0 ) {
                        IncludeStk->skipping++;
                        break;
                    }
                    if( stricmp( match, p ) == 0 ) {
                        break;
                    }
                }
            } else {
                Fatal( "Unknown directive '%s'\n", p );
            }
            break;
        default:
            /* a command */
            logit = (VerbLevel > 0);
            if( *p == '@' ) {
                logit = FALSE;
                p = SkipBlanks( p + 1 );
            }
            if( IncludeStk->skipping == 0 ) {
                if( logit ) {
                    Log( FALSE, "+++<%s>+++\n", p );
                }
                strcpy( Line, p );
                res = RunIt( p );
                if( res != 0 ) {
                    if( !logit ) {
                        Log( FALSE, "<%s> => ", Line );
                    }
                    Log( FALSE, "non-zero return: %u\n", res );
                }
                LogFlush();
            } else if( logit && (VerbLevel > 1) ) {
                Log( FALSE, "---<%s>---\n", p );
            }
        }
    }
}

static bool SearchUpDirs( const char *name, char *result )
{
    char        buff[_MAX_PATH2];
    char        *drive;
    char        *dir;
    char        *fn;
    char        *ext;
    char        *end;
    FILE        *fp;

    _fullpath( result, name, _MAX_PATH );
    for( ;; ) {
        fp = fopen( result, "r" );
        if( fp != NULL ) {
            fclose( fp );
            return( TRUE );
        }
        _splitpath2( result, buff, &drive, &dir, &fn, &ext );
        end = &dir[strlen(dir)-1];
        if( end == dir ) return( FALSE );
        switch( *end ) {
        case '\\':
        case '/':
            --end;
        }
        for( ;; ) {
            if( end == dir ) {
                *end++ = '/';
                break;
            }
            if( *end == '\\' || *end == '/' ) break;
            --end;
        }
        *end = '\0';
        _makepath( result, drive, dir, fn, ext );
    }
}


int main( int argc, char *argv[] )
{
    ctl_file    *next;
    char        *p;

    SysInit( argc, argv );
    ProcessOptions( argv + 1 );
    if( CtlList == NULL ) {
        p = getenv( DEFCTLNAME );
        if( p == NULL ) p = DEFCTLNAME;
        if( !SearchUpDirs( p, Line ) ) {
            _searchenv( p, "PATH", Line );
            if( Line[0] == '\0' ) {
                Fatal( "Can not find '%s'\n", p );
            }
        }
        AddCtlFile( Line );
    }
    while( CtlList != NULL ) {
        ProcessCtlFile( CtlList->name );
        next = CtlList->next;
        free( CtlList );
        CtlList = next;
    }
    if( LogFile != NULL ) fclose( LogFile );
    return( 0 );
}
