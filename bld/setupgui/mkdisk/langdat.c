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
* Description:  Langdat tool mainline.
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#ifndef __UNIX__
#include <direct.h>
#endif
#include "wio.h"
#include "watcom.h"
#include "bldutils.h"
#include "memutils.h"
#include "iopath.h"
#include "pathgrp2.h"

#include "clibext.h"


#define DEFCTLNAME      "files.dat"
#define DEFCTLENV       "FILES_DAT"

typedef struct ctl_file {
    struct ctl_file     *next;
    char                name[_MAX_PATH];
} ctl_file;

typedef struct include {
    struct include      *prev;
    FILE                *fp;
    unsigned            skipping;
    unsigned            ifdefskipping;
    unsigned            lineno;
    char                name[_MAX_PATH];
    char                cwd[_MAX_PATH];
} include;

bool                Quiet;
FILE                *LogFile;
static ctl_file     *CtlList = NULL;
static include      *IncludeStk;
static char         Line[MAX_LINE];
static char         ProcLine[MAX_LINE];
static unsigned     VerbLevel;
static bool         UndefWarn;
static unsigned     ParmCount;
static ctl_file     *Product = NULL;
static ctl_file     *KeyList = NULL;
static char         Product_ver[3];

/* Defaults for all output values */
static const char   *DefType   = NULL;
static const char   *DefRedist = NULL;
static const char   *DefDir    = NULL;
static const char   *DefUsr    = NULL;
static const char   *DefRel    = NULL;
static const char   *DefCond   = NULL;
static const char   *DefWhere  = NULL;
static const char   *DefDesc   = NULL;
static const char   *DefOld    = NULL;
static const char   *DefDstvar = NULL;
static const char   *DefKeys   = NULL;

static const char   * const blank = "";


static void AddToList( const char *name, ctl_file **owner )
{
    ctl_file    *curr;

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
    if( argv[0][2] != '\0' ) {
        strcpy( buff, &argv[0][2] );
        return( argv );
    }
    ++argv;
    strcpy( buff, argv[0] );
    return( argv );
}

static void Usage( void )
{
    printf( "Usage: langdat [-c <ctl_file>]* [-k <key value>]* [-l <log_file>]\n"
            "               [-r <version number>] [-v] [-u] [-q] <product>\n" );
    exit( 0 );
}

static void ProcessOptions( char *argv[] )
{
    char        parm_buff[_MAX_PATH];
    bool        opt_end;

    opt_end = false;
    while( argv[0] != NULL ) {
        if( !opt_end && argv[0][0] == '-' ) {
            switch( tolower( argv[0][1] ) ) {
            case 'c':
                argv = getvalue( argv, parm_buff );
                AddToList( parm_buff, &CtlList );
                break;
            case 'k':
                argv = getvalue( argv, parm_buff );
                AddToList( parm_buff, &KeyList );
                break;
            case 'l':
                argv = getvalue( argv, parm_buff );
                if( LogFile != NULL ) {
                    Fatal( "-l option specified twice\n" );
                }
                OpenLog( parm_buff );
                break;
            case 'r':
                argv = getvalue( argv, parm_buff );
                Product_ver[0] = parm_buff[0];
                Product_ver[1] = parm_buff[1];
                Product_ver[2] = '\0';
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
        } else if( strchr( argv[0], '=' ) != NULL ) {
            putenv( argv[0] );
        } else {
            if( Product != NULL ) {
                Fatal( "Product already set (was %s, now %s)\n", Product->name, argv[0] );
            }
            AddToList( argv[0], &Product );
        }
        ++argv;
    }
}

static int sysChdir( const char *dir )
{
    size_t      len;
#ifndef __UNIX__
    int         drive;
#endif
    char        tmp_buf[_MAX_PATH];

    if( dir[0] == '\0' )
        return( 0 );
#ifndef __UNIX__
    drive = ( dir[1] == ':' ) ? toupper( (unsigned char)dir[0] ) - 'A' + 1 : 0;
#endif
    if( dir[1] != '\0' ) {
        len = strlen( dir );
#ifdef __UNIX__
        if( dir[len - 1] == '/' ) {
#else
        if( ( dir[len - 1] == '\\' ) && ( len > 3 || drive == 0 ) ) {
#endif
            len--;
            memcpy( tmp_buf, dir, len );
            tmp_buf[len] = '\0';
            dir = tmp_buf;
        }
    }
#ifndef __UNIX__
    if( drive ) {
        _chdrive( drive );
    }
#endif
    return( chdir( dir ) );
}

static void PushInclude( const char *name )
{
    include     *new;
    pgroup2     pg;
    char        *dir;
    char        dir_name[_MAX_PATH];

    new = MAlloc( sizeof( *new ) );
    new->prev = IncludeStk;
    new->skipping = 0;
    new->ifdefskipping = 0;
    new->lineno = 0;
    IncludeStk = new;
    strcpy( new->name, name );
#ifdef __UNIX__
    for( dir = new->name; (dir = strchr( dir, '\\' )) != NULL; ) {
#else
    for( dir = new->name; (dir = strchr( dir, '/' )) != NULL; ) {
#endif
        *dir = DIR_SEP;
    }
    new->fp = fopen( new->name, "rb" );
    if( new->fp == NULL ) {
        Fatal( "Could not open '%s': %s\n", new->name, strerror( errno ) );
    }
    _splitpath2( new->name, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    _makepath( dir_name, pg.drive, pg.dir, NULL, NULL );
    if( sysChdir( dir_name ) != 0 ) {
        Fatal( "Could not chdir to '%s': %s\n", dir_name, strerror( errno ) );
    }
    if( getcwd( IncludeStk->cwd, sizeof( IncludeStk->cwd ) ) == NULL ) {
        Fatal( "Could not get current working directory: %s\n", strerror( errno ) );
    }
}

static bool PopInclude( void )
{
    include     *curr;

    curr = IncludeStk;
    fclose( curr->fp );
    IncludeStk = curr->prev;
    MFree( curr );
    if( IncludeStk == NULL ) {
        return( false );
    }
    if( chdir( IncludeStk->cwd ) )
        Fatal( "Could not chdir to '%s': %s\n", IncludeStk->cwd, strerror( errno ) );
    return( true );
}

static bool GetALine( char *line, int max_len )
{
    for( ;; ) {
        if( fgets( line, max_len, IncludeStk->fp ) == NULL ) {
            if( ferror( IncludeStk->fp ) ) {
                Fatal( "Error reading '%s' line %d: %s\n", IncludeStk->name, IncludeStk->lineno, strerror( errno ) );
            }
        }
        if( !feof( IncludeStk->fp ) ) {
            IncludeStk->lineno++;
            break;
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
                rep = IncludeStk->cwd;
            } else {
                rep = getenv( out );
            }
            if( rep == NULL ) {
                if( UndefWarn ) {
                    Log( false, "<%s> is undefined\n", out );
                }
                rep = blank;
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

    first = true;
    SKIP_BLANKS( in );
    for( ;; ) {
        switch( *in ) {
        case '^':
            ++in;
            switch( *in ) {
            case '\r':
            case '\n':
            case '\0':
                break;
            default:
                *out++ = *in++;
                break;
            }
            break;
        case '[':                           // Surround special chars with a space
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
        case '\r':
        case '\n':
        case '\0':
            *out = '\0';
            return;
        default:
            *out++ = *in++;
        }
        first = false;
    }
}

static char *FirstWord( char *p )
{
    char        *start;

    SKIP_BLANKS( p );
    if( *p == '\0' )
        return( NULL );
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

static char *GetNextPathOrFile( char *p )
{
    char        c;
    char        quotechar;
    char        *start;

    p += strlen( p ) + 1;
    SKIP_BLANKS( p );
    if( *p == '\0' )
        return( NULL );
    quotechar = ( *p == '"' ) ? *p++ : ' ';
    start = p;
    while( (c = *p) != '\0' ) {
        if( c == quotechar ) {
            *p = '\0';
            return( start );
        }
#ifdef __UNIX__
        if( c == '\\' ) {
            *p = '/';
        }
#else
        if( c == '/' ) {
            *p = '\\';
        }
#endif
        p++;
    }
    *p++ = '\0';
    *p = '\0';
    return( start );
}

bool checkWord( char *p, ctl_file *word_list )
{
    ctl_file    *w;
    bool        not_op;

    not_op = ( *p == '!' );
    if( not_op )
        ++p;
    for( w = word_list; w != NULL; w = w->next ) {
        if( strcmp( p, w->name ) == 0 ) {
            return( !not_op );
        }
    }
    return( not_op );
}

static bool ContainsWord( const char *str, ctl_file *word_list, bool and_op )
{
    char        *s_copy;
    char        *p;
    size_t      len;
    bool        found;

    len = strlen( str ) + 1;
    s_copy = MAlloc( len + 1 ); // extra 1 byte is required for processing by First/NextWord
    memcpy( s_copy, str, len );
    for( p = FirstWord( s_copy ); p != NULL; p = NextWord( p ) ) {
        found = checkWord( p, word_list );
        if( found && !and_op || !found && and_op ) {
            MFree( s_copy );
            return( !and_op );
        }
    }
    MFree( s_copy );
    return( and_op );
}

static void set_product_version( const char *filename )
{
    char    *filever;

    filever = strstr( filename, "??" );
    if( filever != NULL ) {
        if( Product_ver[0] == '\0' ) {
            strcpy( filever, filever + 2 );
        } else {
            *filever++ = Product_ver[0];
            if( Product_ver[1] == '\0' ) {
                strcpy( filever, filever + 1 );
            } else {
                *filever = Product_ver[1];
            }
        }
    }
}

static char *item_def( const char *old, const char *new, const char *item )
{
    if( old != NULL ) {
        printf( "langdat warning: default item '%s' already defined\n", item );
        printf( "(in file %s line %d)\n", IncludeStk->name, IncludeStk->lineno );
        MFree( (void *)old );
    }
    return( MStrdup( new ) );
}

static char *item_redef( const char *old, const char *new )
{
    if( old != NULL ) {
        MFree( (void *)old );
    }
    return( MStrdup( new ) );
}

static char *item_append( const char *old, const char *new )
{
    char    *p;

    if( *new == '\0' ) {
        p = (char *)old;
    } else {
        if( old == NULL ) {
            p = MStrdup( new );
        } else {
            if( *old == '\0' ) {
                p = MStrdup( new );
            } else {
                size_t  len;

                len = strlen( old ) + strlen( new ) + 1 + 1;
                p = MAlloc( len );
                strcpy( p, old );
                strcat( p, " " );
                strcat( p, new );
            }
            MFree( (void *)old );
        }
    }
    return( p );
}

#define FREE_ITEM(x) if( x != NULL && x != blank ) { MFree( (void *)x ); } x = NULL

#define DEFVAL(x)   ((x==NULL)?blank:x)
#define DEFVALA(x)  ((x==NULL)?NULL:MStrdup(x))

static void ProcessLine( const char *line )
{
    char    *cmd, *p, *str;
    char    *line_copy;
    const char  *type;
    const char  *redist;
    const char  *dir;
    const char  *usr;
    const char  *rel;
    const char  *cond;
    const char  *where;
    const char  *desc;
    const char  *old;
    const char  *dstvar;
    const char  *keys;
    bool    special;

    special = false;
    type = DEFVAL( DefType );
    redist = DEFVAL( DefRedist );
    dir = DEFVAL( DefDir );
    usr = DEFVAL( DefUsr );
    rel = DEFVAL( DefRel );
    cond = DEFVALA( DefCond );
    where = DEFVALA( DefWhere );
    desc = DEFVAL( DefDesc );
    old = DEFVAL( DefOld );
    dstvar = DEFVAL( DefDstvar );
    keys = DEFVALA( DefKeys );

    p = line_copy = MStrdup( line );
    SKIP_BLANKS( p );
    cmd = strtok( p, "=" );
    do {
        str = strtok( NULL, "\"" );
        if( !stricmp( cmd, "echo" ) ) {
            Log( true, "%s\n", str );
            special = true;
            break;
        } else if( !stricmp( cmd, "type" ) ) {
            type = str;
        } else if( !stricmp( cmd, "redist" ) ) {
            redist = str;
        } else if( !stricmp( cmd, "dir" ) ) {
            dir = str;
        } else if( !stricmp( cmd, "usr" ) ) {
            usr = str;
        } else if( !stricmp( cmd, "rel" ) ) {
            rel = str;
        } else if( !stricmp( cmd, "cond" ) ) {
            cond = item_redef( cond, str );
        } else if( !stricmp( cmd, "conda" ) ) {
            cond = item_append( cond, str );
        } else if( !stricmp( cmd, "where" ) ) {
            where = item_redef( where, str );
        } else if( !stricmp( cmd, "wherea" ) ) {
            where = item_append( where, str );
        } else if( !stricmp( cmd, "desc" ) ) {
            desc = str;
        } else if( !stricmp( cmd, "descr" ) ) {     //  Multiple spellings
            desc = str;
        } else if( !stricmp( cmd, "old" ) ) {
            old = str;
        } else if( !stricmp( cmd, "dstvar" ) ) {
            dstvar = str;
        } else if( !stricmp( cmd, "keys" ) ) {
            keys = item_redef( keys, str );
        } else if( !stricmp( cmd, "keysa" ) ) {
            keys = item_append( keys, str );
        } else {
            printf( "langdat warning: unknown keyword %s\n", cmd );
            printf( "(in file %s line %d)\n", IncludeStk->name, IncludeStk->lineno );
        }
        cmd = strtok( NULL, " \t=" );
    } while( cmd != NULL );
    if( !special ) {
        if( cond == NULL ) {
            cond = blank;
        }
        if( where == NULL ) {
            where = blank;
        }
        if( keys == NULL ) {
            keys = blank;
        }
        set_product_version( usr );
        set_product_version( rel );
        /* Check if 'where' matches specified product */
        if( ( Product == NULL || *where == '\0' || ContainsWord( where, Product, false ) )
          && ( *keys == '\0' || ContainsWord( keys, KeyList, true ) ) ) {
            Log( true, "<%s><%s><%s><%s><%s><%s><%s><%s><%s><%s>\n", type, redist, dir, old, usr, rel, where, dstvar, cond, desc );
        }
    }
    FREE_ITEM( cond );
    FREE_ITEM( where );
    FREE_ITEM( keys );
    MFree( line_copy );
}

static void FreeDefault( void )
{
    /* Reset any existing defaults */
    FREE_ITEM( DefType );
    FREE_ITEM( DefRedist );
    FREE_ITEM( DefDir );
    FREE_ITEM( DefUsr );
    FREE_ITEM( DefRel );
    FREE_ITEM( DefCond );
    FREE_ITEM( DefWhere );
    FREE_ITEM( DefDesc );
    FREE_ITEM( DefOld );
    FREE_ITEM( DefDstvar );
    FREE_ITEM( DefKeys );
}

static void ProcessDefault( const char *line )
{
    char    *cmd, *p, *q, *str;
    char    *line_copy;

    FreeDefault();

    /* Process new defaults (if provided) */
    p = line_copy = MStrdup( line );
    SKIP_BLANKS( p );
    q = strtok( p, "]" );
    q += strlen( q ) - 1;
    while( (q >= p) && IS_BLANK( *q ) )
        --q;
    if( *q == '\"' )
        ++q;
    *q = '\0';
    cmd = strtok( p, "=" );
    if( cmd != NULL ) {
        do {
            str = strtok( NULL, "\"" );
            SKIP_BLANKS( str );
            if( !stricmp( cmd, "type" ) ) {
                DefType = item_def( DefType, str, cmd );
            } else if( !stricmp( cmd, "redist" ) ) {
                DefRedist = item_def( DefRedist, str, cmd );
            } else if( !stricmp( cmd, "dir" ) ) {
                DefDir = item_def( DefDir, str, cmd );
            } else if( !stricmp( cmd, "usr" ) ) {
                DefUsr = item_def( DefUsr, str, cmd );
            } else if( !stricmp( cmd, "rel" ) ) {
                DefRel = item_def( DefRel, str, cmd );
            } else if( !stricmp( cmd, "cond" ) ) {
                DefCond = item_def( DefCond, str, cmd );
            } else if( !stricmp( cmd, "where" ) ) {
                DefWhere = item_def( DefWhere, str, cmd );
            } else if( !stricmp( cmd, "desc" ) ) {
                DefDesc = item_def( DefDesc, str, cmd );
            } else if( !stricmp( cmd, "old" ) ) {
                DefOld = item_def( DefOld, str, cmd );
            } else if( !stricmp( cmd, "dstvar" ) ) {
                DefDstvar = item_def( DefDstvar, str, cmd );
            } else if( !stricmp( cmd, "keys" ) ) {
                DefKeys = item_def( DefKeys, str, cmd );
            } else {
                printf( "langdat warning: unknown default %s\n", cmd );
                printf( "(in file %s line %d)\n", IncludeStk->name, IncludeStk->lineno );
            }
            cmd = strtok( NULL, " \t=" );
        } while( cmd != NULL );
    }
    MFree( line_copy );
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
    char   *Match[20];                     // 20 is enough for builder
    int     MatchWords = 0;
    int     i;
    bool    EmptyOk = false;
    int     WordsExamined = 0;

    p = NextWord( p );
    if( p == NULL )
        Fatal( "Missing match word\n" );

    if( *p == '(' ) { // Multiple match words, store them
        p = NextWord( p );
        for( ; MatchWords < 20; ) {
            if( p == NULL )
                Fatal( "Missing match word\n" );
            if( stricmp( p, "\"\"" ) == 0 ) // 'No parameter' indicator
                EmptyOk = true;
            else
                Match[MatchWords++] = p;
            p = NextWord( p );
            if( strcmp( p, ")" ) == 0 ) {
                p = NextWord( p );
                break;
            }
        }
    } else {
        Match[MatchWords++] = p;
        p = NextWord( p );
    }

    // At this point, p must point to the first word after the (last) match word

    for( ;; ) {
        if( p == NULL || strcmp( p, "]" ) == 0 ) { // End of string
            if( WordsExamined == 0 && EmptyOk )
                return 1;
            else
                return 0;
        }
        WordsExamined++;
        for( i = 0; i < MatchWords; i++ )
            if( stricmp( Match[i], p ) == 0 )
                return 1;
        p = NextWord( p );
    }
}

static void ProcessCtlFile( const char *name )
{
    char        *p;
    char        *log_name;
    bool        logit;

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
            p = FirstWord( p + 1 );
            if( stricmp( p, "INCLUDE" ) == 0 ) {
                if( IncludeStk->skipping == 0 ) {
                    PushInclude( GetNextPathOrFile( p ) );
                }
            }
            else if( stricmp( p, "LOG" ) == 0 ) {
                if( IncludeStk->skipping == 0 ) {
                    log_name = GetNextPathOrFile( p );
                    p = NextWord( log_name );
                    if( LogFile == NULL ) {
                        OpenLog( log_name );
                    }
                }
            } else if( stricmp( p, "BLOCK" ) == 0 ) {
                IncludeStk->skipping = 0;   // New block: reset skip flags
                IncludeStk->ifdefskipping = 0;
                if( !MatchFound( p ) )
                    IncludeStk->skipping++;
                break;
            } else if( stricmp( p, "IFDEF" ) == 0 ) {
                if( IncludeStk->ifdefskipping != 0 )
                    IncludeStk->ifdefskipping--;
                if( !MatchFound( p ) )
                    IncludeStk->ifdefskipping++;
                break;
            } else if( stricmp( p, "ENDIF" ) == 0 ) {
                if( IncludeStk->ifdefskipping != 0 )
                    IncludeStk->ifdefskipping--;
                break;
            } else if( stricmp( p, "DEFAULT" ) == 0 ) {
                ProcessDefault( p + strlen( p ) + 1 );
            } else {
                Fatal( "Unknown directive '%s'\n", p );
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
            if( IncludeStk->skipping == 0 && IncludeStk->ifdefskipping == 0 ) {
                if( logit ) {
                    Log( false, "+++<%s>+++\n", p );
                }
                ProcessLine( p );
                LogFlush();
            } else if( logit && ( VerbLevel > 1 ) ) {
                Log( false, "---<%s>---\n", p );
            }
        }
    }
}

static char *SearchUpDirs( const char *name )
{
    pgroup2     pg;
    char        *end;
    FILE        *fp;

    _fullpath( Line, name, sizeof( Line ) );
    for( ;; ) {
        fp = fopen( Line, "r" );
        if( fp != NULL ) {
            fclose( fp );
            return( Line );
        }
        _splitpath2( Line, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
        end = &pg.dir[strlen( pg.dir ) - 1];
        if( end == pg.dir ) {
            _searchenv( name, "PATH", Line );
            if( Line[0] == '\0' )
                return( NULL );
            return( Line );
        }
        if( IS_DIR_SEP( *end ) )
            --end;
        for( ;; ) {
            if( end == pg.dir ) {
                *end++ = DIR_SEP;
                break;
            }
            if( IS_DIR_SEP( *end ) )
                break;
            --end;
        }
        *end = '\0';
        _makepath( Line, pg.drive, pg.dir, pg.fname, pg.ext );
    }
}


int main( int argc, char *argv[] )
{
    ctl_file    *next;
    char        *p;
    char        *fn;

    /* unused parameters */ (void)argc;

    MOpen();
    Product_ver[0] = '\0';
    ProcessOptions( argv + 1 );
    if( Product == NULL ) {
        printf( "langdat warning: no product specified, processing all entries\n" );
    }
    if( CtlList == NULL ) {
        p = getenv( DEFCTLENV );
        if( p == NULL )
            p = DEFCTLNAME;
        fn = SearchUpDirs( p );
        if( fn == NULL ) {
            MClose();
            Fatal( "Can not find '%s'\n", p );
        }
        AddToList( fn, &CtlList );
    }
    while( CtlList != NULL ) {
        ProcessCtlFile( CtlList->name );
        next = CtlList->next;
        MFree( CtlList );
        CtlList = next;
    }
    FreeDefault();
    CloseLog();
    MClose();
    return( 0 );
}

