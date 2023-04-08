/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Builder tool mainline.
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#ifdef __UNIX__
    #include <unistd.h>
#else
    #include <direct.h>
#endif
#include "watcom.h"
#include "builder.h"
#include "memutils.h"
#include "pathgrp2.h"

#include "clibext.h"


#define DEFCTLNAME      "builder.ctl"
#define DEFCTLENV       "BUILDER_CTL"
#define OPTCTLENV       "BUILDER_OPT"

#define DEF_BACKUP      1
#define MAX_BACKUP      9

#define DOS_EOF_CHAR    0x1a

typedef struct include_entry {
    struct include_entry    *prev;
    FILE                    *fp;
    unsigned                skipping;
    unsigned                ifdefskipping;
    unsigned                lineno;
    char                    name[_MAX_PATH];
    char                    cwd[_MAX_PATH];
    copy_entry              reset_abit;
} include_entry;

typedef struct ctl_file {
    struct ctl_file     *next;
    char                name[_MAX_PATH];
} ctl_file;

bool                    Quiet;
FILE                    *LogFile = NULL;

static include_entry    *includeStk;
static ctl_file         *CtlList;
static char             Line[MAX_LINE];
static char             ProcLine[MAX_LINE];
static unsigned         VerbLevel;
static bool             UndefWarn;
static bool             IgnoreErrors;
static unsigned         ParmCount;
static unsigned         LogBackup;

static void PutNumber( const char *src, char *dst, unsigned num )
{
    char        dig;
    int         i;

    memset( dst, 0, 5 );
    strncpy( dst, src, 3 );
    dst[0] = '.';
    for( i = 3; i > 0;--i ) {
        dig = num % 10;
        num /= 10;
        if( dig != 0 || dst[i] == '\0' ) {
            dst[i] = dig + '0';
        }
    }
}

static void BackupLog( const char *log_name, unsigned copies )
{
    pgroup2     pg;
    char        old_name[_MAX_PATH];
    char        new_name[_MAX_PATH];
    char        temp_ext[5];

    if( copies > MAX_BACKUP )
        copies = MAX_BACKUP;
    if( copies == 0 ) {
        remove( log_name );
        return;
    }
    _splitpath2( log_name, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    while( copies != 0 ) {
        PutNumber( pg.ext, temp_ext, copies );
        _makepath( new_name, pg.drive, pg.dir, pg.fname, temp_ext );
        remove( new_name );
        if( copies == 1 ) {
            strcpy( old_name, log_name );
        } else {
            PutNumber( pg.ext, temp_ext, copies - 1 );
            _makepath( old_name, pg.drive, pg.dir, pg.fname, temp_ext );
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
        if( curr == NULL )
            break;
        owner = &curr->next;
    }
    curr = MAlloc( sizeof( *curr ) );
    curr->next = NULL;
    strcpy( curr->name, name );
    *owner = curr;
}

static char **getvalue( char **argv, char *buff )
{
    *buff = '\0';
    if( argv[0][2] != '\0' ) {
        strcpy( buff, &argv[0][2] );
        return( argv );
    }
    ++argv;
    if( argv[0] != NULL )
        strcpy( buff, argv[0] );
    return( argv );
}

static void Usage( void )
{
    printf( "Usage: builder [-c <ctl>] [-l <log>] [-b <bak>] [-i] [-v] [-u] [-q] [--] <parm>\n" );
    printf( "    See builder.doc for more information\n" );
    exit( 0 );
}

static bool ProcessOptions( char *argv[], bool opt_end )
{
    char        parm_buff[_MAX_PATH];

    while( argv[0] != NULL ) {
        if( !opt_end && argv[0][0] == '-' ) {
            switch( tolower( argv[0][1] ) ) {
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
            case 'i':
                IgnoreErrors = true;
                break;
            case 'v':
                ++VerbLevel;
                break;
            case 'u':
                UndefWarn = true;
                break;
            case 'q':
                Quiet = true;
                break;
            case '-':
                opt_end = true;
                break;
            default:
                fprintf( stderr, "Unknown option '%c'\n\n", argv[0][1] );
                /* fall through */
            case '?':
                Usage();
                break;
            }
        } else if( !opt_end && strchr( argv[0], '=' ) != NULL ) {
            putenv( argv[0] );
        } else {
            sprintf( parm_buff, "%d", ++ParmCount );
            if( setenv( parm_buff, argv[0], 1 ) != 0 ) {
                Fatal( "Can not set parameter %u\n", ParmCount );
            }
        }
        ++argv;
    }
    return( opt_end );
}

static int parse_string( const char *env, char **args )
{
    char        parm_buff[_MAX_PATH];
    int         pos;
    bool        quoted;
    char        prev, c;
    int         argc;

    quoted = false;
    pos = 0;
    argc = 0;
    prev = '\0';
    while( (c = *env++) != '\0' ) {
        if( prev == '\\' ) {
            if( args != NULL )
                parm_buff[pos - 1] = c;
            prev = '\0';
            continue;
        } else if( isspace( c ) ) {
            if( !quoted ) {
                if( pos > 0 ) {
                    if( args != NULL ) {
                        parm_buff[pos] = '\0';
                        args[argc] = MStrdup( parm_buff );
                    }
                    ++argc;
                    pos = 0;
                    prev = '\0';
                }
                continue;
            }
        } else if( c == '"' ) {
            quoted = !quoted;
            if( !quoted ) {
                if( args != NULL ) {
                    parm_buff[pos] = '\0';
                    args[argc] = MStrdup( parm_buff );
                }
                ++argc;
                pos = 0;
                prev = '\0';
            }
            continue;
        }
        if( args != NULL )
            parm_buff[pos] = c;
        ++pos;
        prev = c;
    }
    if( pos > 0 ) {
        if( args != NULL ) {
            parm_buff[pos] = '\0';
            args[argc] = MStrdup( parm_buff );
        }
        ++argc;
    }
    return( argc );
}

static bool ProcessEnv( bool opt_end )
{
    char        **args;
    char        *env;
    int         argc;

    env = getenv( OPTCTLENV );
    if( env != NULL ) {
        argc = parse_string( env, NULL );
        if( argc > 0 ) {
            args = MAlloc( ( argc + 1 ) * sizeof( char * ) );
            argc = parse_string( env, args );
            args[argc] = NULL;
            opt_end = ProcessOptions( args, opt_end );
            while( argc > 0 ) {
                MFree( args[--argc] );
            }
            MFree( args );
        }
    }
    return( opt_end );
}

static void PushInclude( const char *name )
{
    include_entry   *new;
    pgroup2         pg;
    char            dir_name[_MAX_PATH];

    new = MAlloc( sizeof( *new ) );
    new->prev = includeStk;
    new->skipping = 0;
    new->ifdefskipping = 0;
    new->lineno = 0;
    new->reset_abit = NULL;
    includeStk = new;
    new->fp = fopen( name, "rb" );      // We will cook (handle \r) internally
    if( new->fp == NULL ) {
        Fatal( "Could not open '%s': %s\n", name, strerror( errno ) );
    }
    strcpy( new->name, name );
    _splitpath2( name, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    _makepath( dir_name, pg.drive, pg.dir, NULL, NULL );
    if( SysChdir( dir_name ) != 0 ) {
        Fatal( "Could not CD to '%s': %s\n", dir_name, strerror( errno ) );
    }
    SetIncludeCWD();
}

static bool PopInclude( void )
{
    include_entry   *curr;

    curr = includeStk;
    fclose( curr->fp );
    includeStk = curr->prev;
    ResetArchives( curr->reset_abit );
    MFree( curr );
    if( includeStk == NULL )
        return( false );
    SysChdir( GetIncludeCWD() );
    return( true );
}

static bool GetALine( char *line, int max_len )
{
    for( ;; ) {
        if( fgets( line, max_len, includeStk->fp ) != NULL ) {
            includeStk->lineno++;
            break;
        }
        if( ferror( includeStk->fp ) ) {
            Fatal( "Error reading '%s' line %d: %s\n", includeStk->name, includeStk->lineno + 1, strerror( errno ) );
        }
        if( !PopInclude() ) {
            return( false );
        }
    }
    return( true );
}

static char *SubstOne( const char **inp, char *out )
{
    const char  *in;
    char        *p;
    char        *starpos;
    const char  *rep;
    unsigned    parm;

    in = *inp;
    p = out;
    for( ;; ) {
        switch( *in ) {
        case '>':
            *p = '\0';
            // If the parameter is a number (n) followed by an asterisk,
            // copy from parameter n to the end to out. E.g. <2*>
            parm = 1;
            for( starpos = out; isdigit( *starpos ); starpos++ )
                ;
            if( stricmp( starpos, "*" ) == 0 ) {
                rep = NULL;
                p = out;
                sscanf( out, "%u", &parm );
                for( ; parm <= ParmCount; ++parm ) {
                    sprintf( out, "%d", parm );
                    rep = getenv( out );
                    if( rep != NULL ) {
                        if( out != p )
                            *out++ = ' ';
                        strcpy( out, rep );
                        out += strlen( out );
                    }
                }
                *inp = in + 1;
                *out = '\0';
                return( out );
            } else if( stricmp( out, "CWD" ) == 0 ) {
                rep = GetIncludeCWD();
            } else {
                rep = getenv( out );
            }
            if( rep == NULL ) {
                if( UndefWarn ) {
                    Log( false, "<%s> is undefined\n", out );
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
        case '\0':
            Fatal( "Missing '>' in '%s' line %d\n", includeStk->name, includeStk->lineno );
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

    first = true;
    SKIP_BLANKS( in );
    for( ;; ) {
        switch( *in ) {
        case '^':                       // Escape next byte special meaning
            ++in;
            switch( *in ) {
            case '\n':
            case '\0':
            case '\r':                  // Allow DOS line in UNIX port
            case DOS_EOF_CHAR:          // Allow DOS EOF in UNIX port
                break;
            default:
                *out++ = *in++;
                break;
            }
            break;
        case '[':                       // Surround special chars with a space
        case ']':
        case '(':
        case ')':
            if( !first )
                *out++ = ' ';
            *out++ = *in++;
            *out++ = ' ';
            break;
        case '<':
            ++in;
            out = SubstOne( &in, out );
            break;
        case '\n':
        case '\0':
        case '\r':                      // Allow DOS line in UNIX port
        case DOS_EOF_CHAR:              // Allow DOS EOF in UNIX port
            *out = '\0';
            return;
        default:
            *out++ = *in++;
        }
        first = false;
    }
}

static char *GetWord( char *p, char **start )
{
    SKIP_BLANKS( p );
    for( *start = p; *p != '\0'; ++p ) {
        if( IS_BLANK( *p ) ) {
            *p++ = '\0';
            break;
        }
    }
    return( p );
}

/****************************************************************************
*
* MatchFound. Examines a string of space separated words. If the first word or
* words (between parentheses) matches any of the words following it, returns 1.
* If not, returns 0. String is terminated by 0 or ']'.
* If there isn't at least one word in the string, terminates program.
*
***************************************************************************/
static int MatchFound( char *p )
{
    char    *Match[20];                     // 20 is enough for builder
    int     MatchWords = 0;
    int     i;
    int     EmptyOk = false;
    int     WordsExamined = 0;
    char    *word;

    p = GetWord( p, &word );
    if( *word == '\0' )
        Fatal( "Missing match word\n" );

    if( *word == '(' ) { // Multiple match words, store them
        p = GetWord( p, &word );
        for( ; MatchWords < 20; ) {
            if( *word == '\0' )
                Fatal( "Missing match word\n" );
            if( stricmp( word, "\"\"" ) == 0 ) { // 'No parameter' indicator
                EmptyOk = true;
            } else {
                Match[MatchWords++] = word;
            }
            p = GetWord( p, &word );
            if( strcmp( word, ")" ) == 0 ) {
                p = GetWord( p, &word );
                break;
            }
        }
    } else {
        Match[MatchWords++] = word;
        p = GetWord( p, &word );
    }

    // At this point, p must point to the first word after the (last) match word

    for( ;; ) {
        if( *word == '\0' || strcmp( word, "]" ) == 0 ) { // End of string
            if( WordsExamined == 0 && EmptyOk ) {
                return( 1 );
            } else {
                return( 0 );
            }
        }
        WordsExamined++;
        for( i = 0; i < MatchWords; i++ ) {
            if( stricmp( Match[i], word ) == 0 ) {
                return( 1 );
            }
        }
        p = GetWord( p, &word );
    }
}


static int ProcessCtlFile( const char *name )
{
    char        *p;
    char        *word;
    int         res;
    bool        logit;
    bool        res_nolog;
    int         rc;

    rc = 0;
    PushInclude( name );
    while( GetALine( Line, sizeof( Line ) ) ) {
        SubstLine( Line, ProcLine );
        p = ProcLine;
        switch( *p ) {
        case '#':
        case '\0':
            /* just a comment */
            break;
        case '[':
            /* a directive */
            p = GetWord( p + 1, &word );
            if( stricmp( word, "INCLUDE" ) == 0 ) {
                if( !includeStk->skipping && !includeStk->ifdefskipping ) {
                    char    inc_file[_MAX_PATH];

                    p = GetPathOrFile( p, inc_file );
                    PushInclude( inc_file );
                }
            } else if( stricmp( word, "LOG" ) == 0 ) {
                if( includeStk->skipping == 0 ) {
                    char    log_name[_MAX_PATH];

                    p = GetPathOrFile( p, log_name );
                    p = GetWord( p, &word );
                    if( *word == '\0' || strcmp( word, "]" ) == 0 ) {
                        BackupLog( log_name, LogBackup );
                    } else {
                        BackupLog( log_name, strtoul( word, NULL, 0 ) );
                    }
                    if( LogFile == NULL ) {
                        OpenLog( log_name );
                    }
                }
            } else if( stricmp( word, "BLOCK" ) == 0 ) {
                includeStk->skipping = 0;   // New block: reset skip flags
                includeStk->ifdefskipping = 0;
                if( !MatchFound( p ) )
                    includeStk->skipping++;
                break;
            } else if( stricmp( word, "IFDEF" ) == 0 ) {
                if( includeStk->ifdefskipping != 0 )
                    includeStk->ifdefskipping--;
                if( !MatchFound( p ) )
                    includeStk->ifdefskipping++;
                break;
            } else if( stricmp( word, "ENDIF" ) == 0 ) {
                if( includeStk->ifdefskipping != 0 )
                    includeStk->ifdefskipping--;
                break;
            } else {
                Fatal( "Unknown directive '%s' in '%s' line %d\n", word, includeStk->name, includeStk->lineno );
            }
            break;
        default:
            /* a command */
            logit = ( VerbLevel > 0 );
            if( *p == '@' ) {
                p++;
                SKIP_BLANKS( p );
                logit = false;
            }
            if( includeStk->skipping == 0 && includeStk->ifdefskipping == 0 ) {
                if( logit ) {
                    Log( false, "+++<%s>+++\n", p );
                }
                res = RunIt( p, IgnoreErrors, &res_nolog );
                if( res != 0 ) {
                    if( !res_nolog ) {
                        if( !logit ) {
                            Log( false, "<%s> => ", p );
                        }
                        Log( false, "non-zero return: %d\n", res );
                    }
                    if( !IgnoreErrors ) {
                        Fatal( "Build failed\n" );
                    }
                    rc = res;
                }
                LogFlush();
            } else if( logit && ( VerbLevel > 1 ) ) {
                Log( false, "---<%s>---\n", p );
                LogFlush();
            }
        }
    }
    return( rc );
}

static bool SearchUpDirs( const char *name, char *result, size_t max_len )
{
    pgroup2     pg;
    char        *end;
    FILE        *fp;

    _fullpath( result, name, max_len );
    for( ;; ) {
        fp = fopen( result, "r" );
        if( fp != NULL ) {
            fclose( fp );
            return( true );
        }
        _splitpath2( result, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
        if( pg.dir[0] == '\0' )
            return( false );
        end = pg.dir + strlen( pg.dir ) - 1;
        if( end == pg.dir )
            return( false );
        switch( *end ) {
        case '\\':
        case '/':
            --end;
        }
        for( ;; ) {
            if( end == pg.dir ) {
                *end++ = '/';
                break;
            }
            if( *end == '\\' || *end == '/' )
                break;
            --end;
        }
        *end = '\0';
        _makepath( result, pg.drive, pg.dir, pg.fname, pg.ext );
    }
}


int main( int argc, char *argv[] )
{
    ctl_file    *next;
    const char  *p;
    int         rc = 0;
    bool        opt_end;

    MOpen();
    SysInit( argc, argv );
    LogBackup = DEF_BACKUP;
    opt_end = false;
    opt_end = ProcessEnv( opt_end );
    opt_end = ProcessOptions( argv + 1, opt_end );
    if( CtlList == NULL ) {
        p = getenv( DEFCTLENV );
        if( p == NULL )
            p = DEFCTLNAME;
        if( !SearchUpDirs( p, Line, sizeof( Line ) ) ) {
            Fatal( "Can not find '%s'\n", p );
        }
        AddCtlFile( Line );
    }
    while( CtlList != NULL ) {
        if( ProcessCtlFile( CtlList->name ) ) {
            rc = 1;
        }
        next = CtlList->next;
        MFree( CtlList );
        CtlList = next;
    }
    CloseLog();
    MClose();
    return( rc );
}

const char *GetIncludeCWD( void )
{
    return( includeStk->cwd );
}

void SetIncludeCWD( void )
{
    if( getcwd( includeStk->cwd, sizeof( includeStk->cwd ) ) == NULL ) {
        includeStk->cwd[0] = '\0';
    }
}

void SetArchive( copy_entry entry )
{
    includeStk->reset_abit = entry;
}

copy_entry GetArchive( void )
{
    return( includeStk->reset_abit );
}
