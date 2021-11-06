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
* Description:  Macro execution and inline file handling.
*
****************************************************************************/


#include <ctype.h>
#include <time.h>
#if defined( __UNIX__ ) || defined( __WATCOMC__ )
    #include <utime.h>
    #include <fnmatch.h>
#else
    #include <sys/utime.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#ifdef __UNIX__
    #include <sys/wait.h>
    #include <dirent.h>
#else
    #include <direct.h>
#endif
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
    #include <process.h>
#endif
#ifdef __RDOS__
    #include "rdos.h"
#endif
#ifdef DLLS_IMPLEMENTED
    #include "idedrv.h"
#endif
#include "make.h"
#include "wio.h"
#include "mtarget.h"
#include "macros.h"
#include "mcache.h"
#include "mmemory.h"
#include "mexec.h"
#include "mmisc.h"
#include "mparse.h"
#include "mpreproc.h"
#include "mrcmsg.h"
#include "msg.h"
#include "msuffix.h"
#include "mupdate.h"
#include "mvecstr.h"
#include "pathgrp2.h"

#include "clibext.h"


#ifdef __UNIX__
    #define MASK_ALL_ITEMS      "*"
    #define ENTRY_INVALID(n,e)  (IsDotOrDotDot(e->d_name) || fnmatch(n, e->d_name, FNM_PATHNAME | FNM_NOESCAPE) == FNM_NOMATCH)
    #define ENTRY_SUBDIR(n,e)   chk_is_dir(n)
    #define ENTRY_RDONLY(n,e)   (access( n, W_OK ) == -1 && errno == EACCES)
    #define PATH_SEP_CHAR       '/'
    #define PATH_SEP_STR        "/"
    #define MKDIR(p)            mkdir( (p), S_IRWXU | S_IRWXG | S_IRWXO )
#else
    #define MASK_ALL_ITEMS      "*.*"
    #define ENTRY_INVALID(n,e)  (IsDotOrDotDot(e->d_name) || fnmatch(n, e->d_name, FNM_PATHNAME | FNM_NOESCAPE | FNM_IGNORECASE) == FNM_NOMATCH)
    #define ENTRY_SUBDIR(n,e)   (e->d_attr & _A_SUBDIR)
    #define ENTRY_RDONLY(n,e)   (e->d_attr & _A_RDONLY)
    #define PATH_SEP_CHAR       '\\'
    #define PATH_SEP_STR        "\\"
    #define MKDIR(p)            mkdir( (p) )
#endif

#define IS_PATH_SEP(p)          ((p)[0] == PATH_SEP_CHAR)
#define SKIP_PATH_SEP(p)        if( IS_PATH_SEP(p) ) (p)++

#define SkipUntilWSorEqual(p)   while( *p != NULLCHAR && !cisws( *p ) && *p != '=' ) ++p
#define SkipUntilRparent(p)     while( *p != NULLCHAR && *p != ')' ) ++p

#define CLOWER(c)               (((c) < 'a') ? (c) - 'A' + 'a' : (c))
#define CUPPER(c)               (((c) >= 'a') ? (c) - 'a' + 'A' : (c))

#if defined( __DOS__ )
    #define FIX_CHAR_OS(c,f)    (((c) == '/') ? '\\' : (cisalpha( (c) ) ? ((f) ? CUPPER(c) : CLOWER(c)) : (c)))
#elif defined( __OS2__ ) || defined( __NT__ ) || defined( __RDOS__ )
    #define FIX_CHAR_OS(c,f)    (((c) == '/') ? '\\' : (c))
#else   /* __UNIX__ */
    #define FIX_CHAR_OS(c,f)    (c)
#endif

typedef enum {
    FLAG_SHELL      = 0x01,
    FLAG_SILENT     = 0x02,
    FLAG_ENV_ARGS   = 0x04,
    FLAG_IGNORE     = 0x08,
    FLAG_SHELL_RC   = 0x10
} shell_flags;

typedef struct dd {
    struct dd   *next;
    char        attr;
    char        name[1];
} iolist;

typedef struct {
    boolbit     bForce   : 1;
    boolbit     bDirs    : 1;
    boolbit     bVerbose : 1;
} rm_flags;

STATIC  UINT8   lastErrorLevel;
STATIC  UINT16  tmpFileNumber;          /* temp file number         */
STATIC  char    tmpFileChar  ;          /* temp file number chari   */
STATIC  FILE    *currentFileHandle;     /* %write, %append, %create */
STATIC  char    *currentFileName;

static bool     RecursiveRM( const char *dir, const rm_flags *flags );

#define COM_MAX_LEN 16              /* must be able to hold any OS cmdname */

typedef enum {
    #define pick(id)    COM_ ## id,
    #include "_mexec.h"
    #undef pick
} commands;

STATIC const char * const   dosInternals[] = {   /* COMMAND.COM commands */
    #define pick(id)    # id,
    #include "_mexec.h"
    #undef pick
};

#define CNUM    (sizeof( dosInternals ) / sizeof( char * ))

static const char * const   percentCmds[] = {
    "ABORT",
    "APPEND",
    "CREATE",
    "ERASE",
    "MAKE",
    "NULL",
    "QUIT",
    "REN",
    "STOP",
    "WRITE",
};

#define PNUM    (sizeof( percentCmds ) / sizeof( char * ))

enum {
    PER_ABORT,
    PER_APPEND,
    PER_CREATE,
    PER_ERASE,
    PER_MAKE,
    PER_NULL,
    PER_QUIT,
    PER_RENAME,
    PER_STOP,
    PER_WRITE
};

enum write_type {
    WR_WRITE,
    WR_APPEND,
    WR_CREATE
};


STATIC bool     execLine( char *line ); /* called recursively in handleFor */

STATIC NKLIST   *noKeepList;            /* contains the list of files that
                                           needs to be cleaned when wmake
                                           exits */

STATIC char *CmdGetFileName( char *src, char **fname, bool osname )
/*****************************************************************/
{
    bool    dquote;
    char    *dst;
    char    t;

#ifndef __DOS__
    /* unused parameters */ (void)osname;
#endif

    dquote = false;
    *fname = src;
    for( dst = src; (t = *src) != NULLCHAR; src++ ) {
        if( t == '\\' ) {
            if( !dquote ) {
                t = src[1];
                if( cisws( t ) || t == '\"' || t == '\\' ) {
                    src++;
                } else {
                    t = '\\';
                }
            }
        } else if( t == '\"' ) {
            dquote = !dquote;
            continue;
        } else if( !dquote && cisws( t ) ) {
            break;
        }
        *dst++ = FIX_CHAR_OS( t, osname );
    }
    if( dst != src ) {
        *dst = NULLCHAR;
    }
    return( src );
}

STATIC bool KeywordEqualUcase( const char *kwd, const char *start, bool anyterm )
/********************************************************************************
 * check string for keyword (upper cased)
 */
{
    while( *kwd != NULLCHAR ) {
        if( *kwd++ != ctoupper( *start++ ) ) {
            return( false );
        }
    }
    return( cisws( *start ) || *start == NULLCHAR || anyterm && !cisalpha( *start ) );
}

STATIC char *createTmpFileName( void )
/*************************************
 * create file name for temporary file
 */
{
    VECSTR  buf;
    VECSTR  buf2;
    char    *result;
    char    *tmpPath;
    char    fileName[_MAX_PATH];

    tmpPath = GetMacroValue( TEMPENVVAR );
    if( tmpPath == NULL && !Glob.compat_nmake ) {
        tmpPath = getenv( TEMPENVVAR );
        if( tmpPath != NULL ) {
            tmpPath = StrDupSafe( tmpPath );
        }
    }

    for( ;; ) {
        tmpFileChar = tmpFileNumber % 26 + 'a' ;
        buf = StartVec();
        FmtStr( fileName, "wm%c%u.tmp", tmpFileChar, tmpFileNumber );
        if( tmpPath != NULL ) {
            if( strlen( tmpPath ) >= _MAX_PATH ) {
                FreeVec( buf );
                FreeSafe( tmpPath );
                PrtMsg( FTL | TMP_PATH_TOO_LONG );
                ExitFatal();
                // never return
            } else if( strlen( tmpPath ) + strlen( fileName ) >= _MAX_PATH ) {
                FreeVec( buf );
                FreeSafe( tmpPath );
                PrtMsg( FTL | TMP_PATH_TOO_LONG );
                ExitFatal();
                // never return
            }
        }
        if( tmpPath == NULL ) {
            WriteVec( buf, fileName );
            result = FinishVec( buf );
        } else {
            WriteVec( buf, tmpPath );
            if( !IS_PATH_SEP( &tmpPath[strlen( tmpPath ) - 1] ) ) {
                buf2 = StartVec();
                WriteVec( buf2, PATH_SEP_STR );
                CatVec( buf, buf2 );
            }
            buf2 = StartVec();
            WriteVec( buf2, fileName );
            CatVec( buf, buf2 );
            result = FinishVec( buf );
        }

        if( !ExistFile( result ) ) {
            /* touch the file */
            TouchFile( result );
            FreeSafe( tmpPath );
            return( result );
        }
        FreeSafe( result );
        tmpFileNumber = (UINT16)((tmpFileNumber + time( NULL )) % 100000);
    }
}


STATIC bool processInlineFile( FILE *fp, const char *body, const char *fileName )
/*******************************************************************************/
{
    int         index;
    bool        ok;
    char        *DeMacroBody;
    int         currentSent;
    bool        firstTime;
    VECSTR      outText;
    char        c;

    firstTime = true;
    currentSent = 0;

    assert( body != NULL );

    ok = true;
    // we will push the whole body back into the stream to be fully
    // deMacroed
    for( index = 0; (c = body[index++]) != NULLCHAR; ) {
        if( c == '\n' ) {
            InsString( body + currentSent, false );
            DeMacroBody = ignoreWSDeMacro( false, ForceDeMacro() );
            currentSent = index;
            if( fp != NULL ) {
                size_t bytes = strlen( DeMacroBody );

                if( bytes != fwrite( DeMacroBody, 1, bytes, fp ) ) {
                    ok = false;
                }
                if( 1 != fwrite( "\n", 1, 1, fp ) ) {
                    ok = false;
                }
            } else {
                if( !Glob.noheader ) {
                    PrtMsg( INF | NEOL | JUST_A_TAB );
                }
                outText = StartVec();
                WriteVec( outText, "echo." );
                if( DeMacroBody != NULL ) {
                    if( *DeMacroBody != NULLCHAR ) {
                        WriteVec( outText, DeMacroBody );
                    }
                    FreeSafe( DeMacroBody );
                }
                if( firstTime ) {
                    WriteVec( outText, " > " );
                    firstTime = false;
                } else {
                    WriteVec( outText, " >> " );
                }
                WriteVec( outText, fileName );
                DeMacroBody = FinishVec( outText );
                PrtMsg( INF | PRNTSTR, DeMacroBody );
            }
            FreeSafe( DeMacroBody );
        }
    }
    return( ok );
}

STATIC bool writeLineByLine( FILE *fp, const char *body )
/*******************************************************/
{
    return( processInlineFile( fp, body, NULL ) );
}


STATIC char *RemoveBackSlash( const char *inString )
/***************************************************
 * remove backslash from \"
 */
{
    char        buffer[_MAX_PATH];
    const char  *p;
    int         pos;
    char        c;

    assert( inString != NULL );

    pos = 0;
    for( p = inString; (c = *p++) != NULLCHAR; ) {
        if( pos >= sizeof( buffer ) - 1 )
            break;
        if( c == '\\' ) {
            if( *p == '\"' ) {
                c = *p++;
            }
        }
        buffer[pos++] = c;
    }
    buffer[pos] = NULLCHAR;

    return( StrDupSafe( buffer ) );
}


STATIC bool verbosePrintTempFile( const FLIST *head )
/***************************************************/
{
    FLIST const *current;
    bool        ok;

    ok = true;      // success if list empty
    for( current = head; current != NULL; current = current->next ) {
        assert( current->fileName != NULL );
        ok = processInlineFile( NULL, current->body, current->fileName );
    }
    return( ok );
}

STATIC bool createFile( const FLIST *head )
/******************************************
 * create file given information in the FLIST
 */
{
    NKLIST  *temp;
    FILE    *fp;
    char    *fileName = NULL;
    char    *tmpFileName = NULL;
    bool    ok;

    assert( head != NULL );

    ok = ( head->fileName != NULL );
    if( ok ) {
        /* Push the filename back into the stream
         * and then get it back out using DeMacro to fully DeMacro
         */
        UnGetCHR( STRM_MAGIC );
        InsString( head->fileName, false );
        fileName = DeMacro( TOK_MAGIC );
        GetCHR();           /* eat STRM_MAGIC */

        tmpFileName = RemoveBackSlash( fileName );
        fp = fopen( tmpFileName, "w" );
        if( fp != NULL ) {
            if( !writeLineByLine( fp, head->body ) ) {
                PrtMsg( ERR | ERROR_WRITING_FILE, tmpFileName );
                ok = false;
            }
            if( fclose( fp ) == 0 ) {
                if( !head->keep ) {
                    temp = NewNKList();
                    temp->fileName = StrDupSafe( tmpFileName );
                    temp->next     = noKeepList;
                    noKeepList     = temp;
                }
            } else {
                PrtMsg( ERR | ERROR_CLOSING_FILE, tmpFileName );
                ok = false;
            }
        } else {
            PrtMsg( ERR | ERROR_OPENING_FILE, tmpFileName );
            ok = false;
        }
    }
    FreeSafe( fileName );
    FreeSafe( tmpFileName );
    return( ok );
}


STATIC bool writeInlineFiles( FLIST *head, char **commandIn )
/************************************************************
 * This part writes the inline files
 * modifies the command text to show the temporary file names
 * assumption is that all << are removed for explicitly defined
 * file names so the only << left are for temporary files
 */
{
    char    *cmdText;
    FLIST   *current;
    bool    ok;
    VECSTR  newCommand;
    size_t  start;  // start of cmdText to be copied into newCommand;
    size_t  index;  // current index of cmdText
    NKLIST  *temp;

    assert( *commandIn != NULL );

    cmdText    = *commandIn;
    newCommand = StartVec();
    index      = 0;
    start      = index;

    ok = true;
    for( current = head;
        current != NULL && ok && cmdText[index] != NULLCHAR;
        current = current->next )
    {
        // if the filename is the inline symbol then we need change
        // the filename into a temp filename
        if( strcmp( current->fileName, INLINE_SYMBOL ) == 0 ) {
            for( ;; ) {
                if( cmdText[index] == '<' ) {
                    if( cmdText[index + 1] == '<' ) {
                        index += 2;
                        break;
                    }
                } else if( cmdText[index] == NULLCHAR ) {
                    /* not possible to come here*/
                    ok = false;
                    break;
                }
                ++index;
            }
            if( !ok ) {
                break;
            }
            WriteNVec( newCommand, cmdText + start, index - start - 2 );
            start = index;
            FreeSafe( current->fileName );
            current->fileName = createTmpFileName();

            WriteVec( newCommand, current->fileName );
        }
        if( !Glob.noexec ) {
            ok = createFile( current );
        } else {
            if( !current->keep ) {
                temp = NewNKList();
                temp->fileName = StrDupSafe( current->fileName );
                temp->next     = noKeepList;
                noKeepList     = temp;
            }
        }
    }
    WriteNVec( newCommand, cmdText+start, strlen( cmdText ) - start );
    FreeSafe( cmdText );
    *commandIn = FinishVec( newCommand );
    return( ok );
}


STATIC int findInternal( const char *cmd )
/*****************************************
 * check cmd for command.com command, return index if it is
 * return CNUM if is of form x:
 * otherwise return -1
 * expects cmd to be just the command - ie: no args
 */
{
    char * const    *key;
    size_t          len;
    char            buff[COM_MAX_LEN + 1];

    assert( cmd != NULL );

    /* test if of form x: */
#ifndef __UNIX__
    if( cisalpha( cmd[0] ) && cmd[1] == ':' && cmd[2] == NULLCHAR ) {
        return( CNUM );
    }
#endif
    while( (key = bsearch( &cmd, dosInternals, CNUM, sizeof( char * ), KWCompare )) == NULL ) {
        len = strlen( cmd );
        // should work if buff == cmd (i.e., cd..)
        if( len < 2 || len > COM_MAX_LEN || cmd[len - 1] != '.' ) {
            return( -1 );
        }
        // remove '.' from the command end
        strcpy( buff, cmd );
        buff[len - 1] = NULLCHAR;
        cmd = buff;
    }
    return( (int)( key - (char **)dosInternals ) );
}


STATIC bool percentMake( char *arg )
/***********************************
 * do a recursive make of the target in arg
 */
{
    char        *finish;
    TARGET      *calltarg;
    bool        ok;
    char        *buf;
    char        *start;
    bool        newtarg;
    bool        more_targets;

    /* %make <target> <target> ... */
    buf = MallocSafe( _MAX_PATH );

    ok = false;
    start = arg;
    for( ;; ) {
        start = SkipWS( start );
        if( *start == NULLCHAR ) {
            break;
        }
        more_targets = false;
        for( finish = start; *finish != NULLCHAR; ++finish ) {
            if( cisws( *finish ) ) {
                more_targets = true;
                *finish = NULLCHAR;
                break;
            }
        }

        /* try to find this file on path or in targets */
        ok = TrySufPath( buf, start, &calltarg, false );

        newtarg = false;
        if( ( ok && calltarg == NULL ) || !ok ) {
            /* Either file doesn't exist, or it exists and we don't already
             * have a target for it.  Either way, we create a new target.
             */
            calltarg = NewTarget( buf );
            newtarg = true;
        }
        ok = Update( calltarg );
        if( newtarg && !Glob.noexec ) {
            /* we created a target - don't need it any more */
            KillTarget( calltarg->node.name );
        }
        if( more_targets ) {
            *finish = ' ';
        }
        start = finish;
    }
    FreeSafe( buf );

    return( ok );
}


STATIC void closeCurrentFile( void )
/**********************************/
{
    if( currentFileHandle != NULL ) {
        fclose( currentFileHandle );
        currentFileHandle = NULL;
    }
    if( currentFileName != NULL ) {
        FreeSafe( currentFileName );
        currentFileName = NULL;
    }
    CacheRelease();     /* so that the cache is updated */
}


STATIC bool percentWrite( char *arg, enum write_type type )
/*********************************************************/
{
    char        *p;
    char const  *text;
    char        *fn;
    char const  *cmd_name;
    char const  *open_flags;
    size_t      len;

    assert( arg != NULL );

    if( Glob.noexec ) {
        return( true );
    }
    /* handle File name */
    p = CmdGetFileName( arg, &fn, true );
    if( *p != NULLCHAR ) {
        if( !cisws( *p ) ) {
            switch( type ) {
            case WR_APPEND:
                cmd_name = percentCmds[PER_APPEND];
                break;
            case WR_CREATE:
                cmd_name = percentCmds[PER_CREATE];
                break;
            case WR_WRITE:
                cmd_name = percentCmds[PER_WRITE];
                break;
            default:
                cmd_name = "?";
                break;
            }
            PrtMsg( ERR | SYNTAX_ERROR_IN, cmd_name );
            closeCurrentFile();
            return( false );
        }
        *p++ = NULLCHAR;    /* terminate file name */
        text = p;           /* set text pointer */
        p += strlen( p );   /* find null terminator */
    } else {
        text = p;           /* set text pointer */
    }

    /* now text points to the beginning of string to write, and p points to
     * the end of the string.  fn points to the name of the file to write to
     */
    if( type == WR_CREATE || currentFileName == NULL || !FNameEq( currentFileName, fn ) ) {
        closeCurrentFile();
        currentFileName = StrDupSafe( fn );
        if( type == WR_APPEND ) {
            open_flags = "a";
        } else {
            open_flags = "w";
        }

        currentFileHandle = fopen( fn, open_flags );
        if( currentFileHandle == NULL ) {
            PrtMsg( ERR | OPENING_FOR_WRITE, fn );
            closeCurrentFile();
            return( false );
        }
    }

    if( type != WR_CREATE ) {
        *p = '\n';          /* replace null terminator with newline */
        len = ( p - text ) + 1;
        if( fwrite( text, 1, len, currentFileHandle ) != len ) {
            PrtMsg( ERR | DOING_THE_WRITE );
            closeCurrentFile();
            return( false );
        }
    }

    CacheRelease();     /* so that the cache is updated */

    return( true );
}


STATIC bool percentErase( char *arg )
/***********************************/
{
    char    *fn;
    char    *p;
    bool    ok;

    ok = false;
    /* handle File name */
    p = CmdGetFileName( arg, &fn, true );
    if( *p != NULLCHAR && !cisws( *p ) ) {
        PrtMsg( ERR | SYNTAX_ERROR_IN, percentCmds[PER_ERASE] );
        PrtMsg( INF | PRNTSTR, "File" );
        PrtMsg( INF | PRNTSTR, fn );
    } else {
        *p = NULLCHAR;      /* terminate file name */
        if( remove( fn ) == 0 ) {
            ok = true;
        }
    }
    return( ok );
}

STATIC bool percentRename( char *arg )
/************************************/
{
    char        *p;
    char        *fn1, *fn2;

    assert( arg != NULL );

    if( Glob.noexec ) {
        return( true );
    }

    /* Get first LFN */
    p = CmdGetFileName( arg, &fn1, true );
    if( *p == NULLCHAR || !cisws( *p ) ) {
        PrtMsg( ERR | SYNTAX_ERROR_IN, percentCmds[PER_RENAME] );
        PrtMsg( INF | PRNTSTR, "First file" );
        PrtMsg( INF | PRNTSTR, fn1 );
        return( false );
    }
    *p++ = NULLCHAR;        /* terminate first file name */
    /* skip ws after first and before second file name */
    p = SkipWS( p );
    /* Get second LFN as well */
    p = CmdGetFileName( p, &fn2, true );
    if( *p != NULLCHAR && !cisws( *p ) ) {
        PrtMsg( ERR | SYNTAX_ERROR_IN, percentCmds[PER_RENAME] );
        return( false );
    }
    *p = NULLCHAR;          /* terminate second file name */
    return( rename( fn1, fn2 ) == 0 );
}

STATIC bool percentCmd( const char *cmdname, char *arg )
/*******************************************************
 * handle our special percent commands
 */
{
    char const * const  *key;
    char const          *ptr;
    int                 num;
    bool                ok;

    assert( cmdname != NULL && arg != NULL );

    ptr = cmdname + 1;
    key = bsearch( &ptr, percentCmds, PNUM, sizeof( char * ), KWCompare );
    ok = ( key != NULL );
    if( !ok ) {
        PrtMsg( ERR | UNKNOWN_PERCENT_CMD );
        closeCurrentFile();
    } else {
        num = (int)( key - (char const **)percentCmds );
        if( !Glob.noexec || num == PER_MAKE ) {
            switch( num ) {
            case PER_ABORT:
                closeCurrentFile();
                ExitError();
                // never return
            case PER_APPEND:
                ok = percentWrite( arg, WR_APPEND );
                break;
            case PER_CREATE:
                ok = percentWrite( arg, WR_CREATE );
                break;
            case PER_ERASE:
                ok = percentErase( arg );
                break;
            case PER_MAKE:
                ok = percentMake( arg );
                break;
            case PER_NULL:
                break;
            case PER_QUIT:
                closeCurrentFile();
                ExitOK();
                // never return
            case PER_RENAME:
                ok = percentRename( arg );
                break;
            case PER_STOP:
                closeCurrentFile();
                if( !GetYes( DO_YOU_WISH_TO_CONT ) ) {
                    ExitOK();
                    // never return
                }
                break;
            case PER_WRITE:
                ok = percentWrite( arg, WR_WRITE );
                break;
            default:
                assert( false );
                break;
            }
        }
    }
    return( ok );
}

#ifdef __UNIX__
STATIC int intSystem( const char *cmd )
/**************************************
 * interruptable "system" (so that ctrl-c works)
 */
{
    pid_t   pid = fork();
    int     status;

    if( pid == -1 ) {
        return( -1 );
    }
    if( pid == 0 ) {
        execl( "/bin/sh", "sh", "-c", cmd, NULL );
        exit( 127 );
        // never return
    }
    for( ;; ) {
        if( waitpid( pid, &status, 0 ) == -1 ) {
            if( errno == EINTR ) {
                continue;
            }
            status = -1;
        } else if( WIFSIGNALED( status ) ) {
            if( WTERMSIG( status ) > 0 && WTERMSIG( status ) <= 15 ) {
                PrtMsg( INF | (SIG_ERR_0 + WTERMSIG( status ) ) );
            } else {
                PrtMsg( INF | SIG_ERR_0, WTERMSIG( status ) );
            }
        }
        CheckForBreak();
        return( status );
    }
}
#endif

STATIC RET_T mySystem( const char *cmdname, const char *cmd )
/************************************************************
 * execute a command using system()
 */
{
    int retcode;

    assert( cmd != NULL );

    if( Glob.noexec ) {
        return( RET_SUCCESS );
    }

    closeCurrentFile();
#ifdef __UNIX__
    retcode = intSystem( cmd );
    lastErrorLevel = (UINT8)retcode;
    if( retcode != -1 && WIFEXITED( retcode ) ) {
        lastErrorLevel = WEXITSTATUS( retcode );
        if( lastErrorLevel == 0 ) {
            return( RET_SUCCESS );
        }
        if( lastErrorLevel == 127 ) {
            PrtMsg( ERR | UNABLE_TO_EXEC, cmdname );
        }
    }
#else
    retcode = system( cmd );
    lastErrorLevel = (UINT8)retcode;
    if( retcode < 0 ) {
        PrtMsg( ERR | UNABLE_TO_EXEC, cmdname );
    }
    if( retcode == 0 ) {
        return( RET_SUCCESS );
    }
#endif
    return( RET_ERROR );
}


STATIC RET_T handleSet( char *cmd )
/**********************************
 * "SET" {ws}* <name> {ws}* "="[<value>]
 */
{
    char        *p;         /* we walk cmd with this        */
    char        *name;      /* beginning of variable name   */
    char        *endname;   /* end of name                  */
    int         retcode;    /* from setenv                  */

    assert( cmd != NULL );

#ifdef DEVELOPMENT
    PrtMsg( DBG | INF | INTERPRETING, dosInternals[COM_SET] );
#endif

    if( Glob.noexec ) {
        return( RET_SUCCESS );
    }

    p = SkipWS( cmd + 3 );      /* skip ws after "SET" */
    if( *p == NULLCHAR ) {      /* just "SET" with no options... pass on */
        return( mySystem( cmd, cmd ) );
    }

    /* anything goes in a dos set name... even punctuation! */
    name = p;
    SkipUntilWSorEqual( p );
    endname = p;

    p = SkipWS( p );            /* skip ws after name */
    if( *p != '=' || endname == name ) {
        PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_SET] );
        return( RET_ERROR );
    }
    *endname = NULLCHAR;        /* null terminate name */
    ++p;                        /* advance to character after '=' */

    retcode = SetEnvSafe( name, p );
    if( retcode != 0 ) {
        return( RET_ERROR );
    }
    return( RET_SUCCESS );
}


STATIC RET_T handleEcho( const char *cmd )
/*****************************************
 * "ECHO" <string>
 */
{
    const char  *p;         /* we walk cmd with this */

    assert( cmd != NULL );

#ifdef DEVELOPMENT
    PrtMsg( DBG | INF | INTERPRETING, dosInternals[COM_ECHO] );
#endif

    if( Glob.noexec ) {
        return( RET_SUCCESS );
    }

    if( cmd[4] != NULLCHAR ) {  /* check for echo with no arguments */
        p = cmd + 5;            /* assume "ECHO "; whitespace gets printed! */

        PrtMsg( INF | PRNTSTR, p );
    }
    return( RET_SUCCESS );
}

STATIC RET_T handleIf( char *cmd )
/*********************************
 *                      { ERRORLEVEL {ws}+ <number>    }
 * IF {ws}+ [NOT {ws}+] { <str1> {ws}* == {ws}* <str2> } {ws}+ <command>
 *                      { EXIST {ws}+ <file>           }
 */
{
    bool        not;        /* flag for not keyword                     */
    bool        condition;  /* whether the condition was T or F         */
    char        *p;         /* used to scan the string                  */
    char        *tmp1;      /* one of NOT | ERRORLEVEL | <str1> | EXIST */
    char        *tmp2;      /* one of <number> | "==" | <file> | <str2> */
    char        *end1;      /* location of end of tmp1 string           */
    const char  *file;      /* for checking file existence              */

    assert( cmd != NULL );

#ifdef DEVELOPMENT
    PrtMsg( DBG | INF | INTERPRETING, dosInternals[COM_IF] );
#endif

    if( Glob.noexec ) {
        return( RET_SUCCESS );
    }
    closeCurrentFile();

    p = SkipWS( cmd + 2 );      /* skip ws after "IF" */
    if( *p == NULLCHAR ) {
        PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_IF] );
        return( RET_ERROR );
    }

    not = KeywordEqualUcase( "NOT", p, false );
    if( not ) {
        p = SkipWS( p + 3 );    /* skip ws after "NOT" */
        if( *p == NULLCHAR ) {
            PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_IF] );
            return( RET_ERROR );
        }
    }

    if( KeywordEqualUcase( "ERRORLEVEL", p, false ) ) {
        p = SkipWS( p + 10 );    /* skip ws after "ERRORLEVEL" */
        if( *p == NULLCHAR ) {
            PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_IF] );
            return( RET_ERROR );
        }
        tmp2 = p;
        p = FindNextWS( p );
        if( *p == NULLCHAR ) {
            PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_IF] );
            return( RET_ERROR );
        }
        *p++ = NULLCHAR;
        condition = ( lastErrorLevel >= atoi( tmp2 ) );
    } else if( KeywordEqualUcase( "EXIST", p, false ) ) {
        p = SkipWS( p + 5 );        /* skip ws after "EXIST" */
        if( *p == NULLCHAR ) {
            PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_IF] );
            return( RET_ERROR );
        }
        /* handle File name */
        p = CmdGetFileName( p, &tmp2, false );
        if( *p == NULLCHAR ) {
            PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_IF] );
            return( RET_ERROR );
        }
        *p++ = NULLCHAR;            /* terminate file name */

        file = DoWildCard( tmp2 );
        condition = ( ( file != NULL ) && CacheExists( file ) );
        /* abandon rest of entries if any */
        DoWildCardClose();
    } else {
        tmp1 = p;                   /* find first string after IF [NOT] */
        p = FindNextWSorEqual( p );
        if( *p == NULLCHAR ) {
            PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_IF] );
            return( RET_ERROR );
        }
        end1 = p;
        p = SkipWS( p );            /* skip ws after first string and before "==" */
        if( p[0] != '=' || p[1] != '=' ) {
            PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_IF] );
            return( RET_ERROR );
        }
        p = SkipWS( p + 2 );        /* skip ws after "==" and before second string */
        if( *p == NULLCHAR ) {
            PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_IF] );
            return( RET_ERROR );
        }
        tmp2 = p;
        p = FindNextWS( p );
        if( *p == NULLCHAR ) {
            PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_IF] );
            return( RET_ERROR );
        }
        /* compare first and second strings */
        condition = ( end1 - tmp1 == p - tmp2 && memcmp( tmp1, tmp2, end1 - tmp1 ) == 0 );
    }

    p = SkipWS( p );   /* skip ws before <command> */
    if( *p == NULLCHAR ) {
        PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_IF] );
        return( RET_ERROR );
    }
    if( not ^ condition ) {
        return( execLine( p ) ? RET_SUCCESS : RET_ERROR );
    }
    return( RET_SUCCESS );
}


STATIC bool handleForSyntaxError( void )
/***************************************/
{
    PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_FOR] );
    return( false );
}


STATIC bool getForArgs( char *line, const char **pvar, char **pset, const char **pcmd )
/**************************************************************************************
 * "FOR" {ws}* "%"["%"]<var> {ws}+ "IN" {ws}* "("<set>")" {ws}* "DO" {ws}+ <command>
 */
{
    char    *p;

    assert( line != NULL && pvar != NULL && pset != NULL && pcmd != NULL );

    /* remember we can hack up line all we like... */

    p = SkipWS( line + 3 ); /* skip ws after "FOR" */

    /* got "%"["%"]<var>, now test if legal */
    if( p[0] != '%' ) {
        return( handleForSyntaxError() );
    }
    if( ( p[1] == '%' && !cisalpha( p[2] ) ) ||
        ( p[1] != '%' && !cisalpha( p[1] ) ) ) {
        return( handleForSyntaxError() );
    }
    *pvar = (const char *)p;

    /* move to end of <var> */
    while( cisalpha( *p ) || *p == '%' ) {
        ++p;
    }
    if( *p == NULLCHAR ) {  /* premature eol? */
        return( handleForSyntaxError() );
    }
    *p++ = NULLCHAR;        /* terminate variable name */

    p = SkipWS( p );        /* skip ws before "IN" */
    if( !KeywordEqualUcase( "IN", p, true ) ) {
        return( handleForSyntaxError() );
    }

    p = SkipWS( p + 2 );    /* skip ws before "("<set>")" */
    if( p[0] != '(' ) {
        return( handleForSyntaxError() );
    }
    ++p;

    *pset = p;              /* beginning of set string */
    SkipUntilRparent( p );
    if( *p == NULLCHAR ) {
        return( handleForSyntaxError() );
    }
    *p++ = NULLCHAR;        /* terminate set string */

    p = SkipWS( p );        /* skip ws before "DO" */
    if( !KeywordEqualUcase( "DO", p, false ) ) {
        return( handleForSyntaxError() );
    }
    p = SkipWS( p + 2 );    /* skip ws before <command> */
    if( *p == NULLCHAR ) {
        return( handleForSyntaxError() );
    }

    *pcmd = (const char *)p;

    return( true );
}


STATIC const char *nextVar( const char *str, const char *var, size_t varlen )
/****************************************************************************
 * return a pointer to next %variable in str, or NULL
 */
{
    const char  *p;

    assert( str != NULL && var != NULL && *var == '%' );

    for( p = strchr( str, '%' ); p != NULL; p = strchr( p + 1, '%' ) ) {
        if( strncmp( p, var, varlen ) == 0 ) {
            break;
        }
    }
    return( p );
}


STATIC void doForSubst( const char *var, size_t varlen,
     const char *subst, const char *src, char *dest )
/******************************************************
 * substitute all occurances of var in src with subst, write to dest
 * dest must be large enough
 */
{
    const char  *p;

    assert( var != NULL && subst != NULL && src != NULL && dest != NULL );

    while( *src != NULLCHAR ) {
        p = nextVar( src, var, varlen );
        if( p != NULL ) {
            while( p > src ) {      /* copy upto first rplcment */
                *dest++ = *src++;
            }
            src += varlen;
            p = subst;
            while( *p != NULLCHAR ) {
                *dest++ = *p++;
            }
        } else {
            while( *src != NULLCHAR ) {
                *dest++ = *src++;
            }
        }
    }
    *dest = NULLCHAR;
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC RET_T handleFor( char *line )
/***********************************
 * "FOR" {ws}* "%"["%"]<var> {ws}+ "IN" {ws}* "("<set>")" {ws}* "DO" {ws}+ <command>
 */
{
    static bool     busy = false;   /* recursion protection */
    const char      *var;           /* loop variable name incl. %           */
    char            *set;           /* set of values for looping            */
    const char      *cmd;           /* command to execute                   */
    const char      *p;             /* working pointer                      */
    const char      *subst;         /* pointer to the element to substitute */
    size_t          varlen;         /* strlen( var )                        */
    unsigned        numsubst;       /* number of substitutions per cmd      */
    size_t          cmdlen;         /* strlen( cmd ) - numsubst * varlen    */
    size_t          newlen;         /* size of memory we need               */
    size_t          lastlen;        /* last size of memory we asked for     */
    char            *exec;          /* line to execute                      */
    size_t          incr;           /* increment to next item during loop   */

    assert( line != NULL );

    if( busy ) {
        PrtMsg( ERR | NO_NESTED_FOR, dosInternals[COM_FOR] );
        return( RET_ERROR );
    }
    busy = true;

#ifdef DEVELOPMENT
    PrtMsg( DBG | INF | INTERPRETING, dosInternals[COM_FOR] );
#endif

    cmd = var = set = NULL;     /* Just to shut up gcc */
    if( !getForArgs( line, &var, &set, &cmd ) ) {
        busy = false;
        return( RET_ERROR );
    }

    varlen = strlen( var );

    numsubst = 0;
    for( p = nextVar( cmd, var, varlen ); p != NULL; p = nextVar( p + varlen, var, varlen ) ) {
        ++numsubst;
    }

    cmdlen = strlen( cmd ) - numsubst * varlen + 1;
    lastlen = 0;
    exec = NULL;
    for( incr = 1; *(set = SkipWS( set )) != NULLCHAR; set += incr ) {
        subst = set;   /* remember start of subst string */
        set = FindNextWS( set );
        if( *set == NULLCHAR )
            incr = 0;
        *set = NULLCHAR;
        for( subst = DoWildCard( subst ); subst != NULL; subst = DoWildCard( NULL ) ) {
            newlen = numsubst * strlen( subst ) + cmdlen;
            if( lastlen < newlen ) {
                lastlen = newlen;
                FreeSafe( exec );
                exec = MallocSafe( newlen );
            }

            /* make variable substitutions */
            doForSubst( var, varlen, subst, cmd, exec );

            if( !execLine( exec ) ) {
                FreeSafe( exec );
                busy = false;
                /* abandon remaining file entries */
                DoWildCardClose();
                return( RET_ERROR );
            }
        }
    }

    FreeSafe( exec );
    busy = false;
    return( RET_SUCCESS );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


#if defined( __OS2__ ) || defined( __NT__ ) || defined( __UNIX__ ) || defined( __RDOS__ )
STATIC RET_T handleCD( char *cmd )
/********************************/
{
    char        *p;     // pointer to walk with
    char        *path;

#ifdef DEVELOPMENT
    PrtMsg( DBG | INF | INTERPRETING, dosInternals[COM_CD] );
#endif

    closeCurrentFile();
    p = cmd;
    while( cisalpha( *p ) ) {       /* advance past command name */
        ++p;
    }

    p = SkipWS( p );
    if( *p == NULLCHAR ) {          /* no args - just print the cd */
        return( mySystem( cmd, cmd ) );
    }

#ifndef __UNIX__
    if( cisalpha( p[0] ) && p[1] == ':' ) {             /* just a drive: arg, print the cd */
        if( *SkipWS( p + 2 ) == NULLCHAR ) {
            return( mySystem( cmd, cmd ) );
        }
    }
#endif

    /* handle File name */
    p = CmdGetFileName( p, &path, true );
    *p = NULLCHAR;      /* terminate path */
    if( chdir( path ) != 0 ) {         /* an error changing path */
        PrtMsg( ERR | CHANGING_DIR, path );
        return( RET_ERROR );
    }
    return( RET_SUCCESS );
}


#if defined( __OS2__ ) || defined( __NT__ ) || defined( __RDOS__ )
STATIC RET_T handleChangeDrive( const char *cmd )
/***********************************************/
{
    int         drive_index;

#ifdef DEVELOPMENT
    PrtMsg( DBG | INF | INTERPRETING, dosInternals[CNUM] );
#endif

    drive_index = (ctoupper( *cmd ) - 'A' + 1);
    if( drive_index == 0 || drive_index > 26 ) {
        return( RET_ERROR );
    }
    if( _chdrive( drive_index ) ) {
        return( RET_ERROR );
    }
    return( RET_SUCCESS );
}
#endif
#endif


STATIC bool handleRMSyntaxError( void )
/*************************************/
{
    PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_RM] );
    return( false );
}

STATIC bool getRMArgs( char *line, rm_flags *flags, char **name )
/****************************************************************
 * returns true and *name = NULL when there are no more arguments
 */
{
    static char *p = NULL;

    if( line != NULL ) {        /* first run? */
        flags->bForce   = false;
        flags->bDirs    = false;
        flags->bVerbose = false;

        /* find all options after "RM " */
        for( p = SkipWS( line + 3 ); p[0] == '-'; p = SkipWS( p ) ) {
            p++;
            while( cisalpha( p[0] ) ) {
                switch( ctolower( p[0] ) ) {
                case 'f':
                    flags->bForce = true;
                    break;
                case 'r':
                    flags->bDirs = true;
                    break;
                case 'v':
                    flags->bVerbose = true;
                    break;
                default:
                    return( handleRMSyntaxError() );
                }
                p++;
            }
        }
    }
    *name = p;
    if( p != NULL && *p != NULLCHAR ) {
        p = FindNextWS( p );
        if( *p != NULLCHAR ) {
            *p++ = NULLCHAR;
            p = SkipWS( p );
        }
    }
    return( true );
}

STATIC bool remove_item( const char *name, const rm_flags *flags, bool dir )
/**************************************************************************/
{
    int     rc;
    char    *inf_msg;

    if( dir ) {
        inf_msg = "directory";
        rc = rmdir( name );
    } else {
        inf_msg = "file";
        rc = remove( name );
    }
    if( rc != 0 && flags->bForce && errno == EACCES ) {
        rc = chmod( name, PMODE_RW );
        if( rc == 0 ) {
            if( dir ) {
                rc = rmdir( name );
            } else {
                rc = remove( name );
            }
        }
    }
    if( rc != 0 && flags->bForce && errno == ENOENT ) {
        rc = 0;
    }
    if( rc != 0 ) {
        PrtMsg( ERR | SYSERR_DELETING_ITEM, name );
    } else if( flags->bVerbose && errno != ENOENT ) {
        PrtMsg( INF | DELETING_ITEM, inf_msg, name );
    }

    CacheRelease();     /* so that the cache is updated */

    return( rc == 0 );
}

static bool IsDotOrDotDot( const char *fname )
{
    /* return 1 if fname is "." or "..", 0 otherwise */
    return( fname[0] == '.' && ( fname[1] == NULLCHAR || ( fname[1] == '.' && fname[2] == NULLCHAR ) ) );
}

static bool chk_is_dir( const char *name )
{
    struct stat     s;

    return( stat( name, &s ) == 0 && S_ISDIR( s.st_mode ) );
}

static bool doRM( const char *fullpath, const rm_flags *flags )
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
    bool                rc = true;

    /* separate file name to path and file name parts */
    len = strlen( fullpath );
    for( i = len; i > 0; --i ) {
        if( cisdirc( fullpath[i - 1] ) ) {
            break;
        }
    }
    j = i;
    /* if no path then use current directory */
    if( i == 0 ) {
        fpath[i++] = '.';
        fpath[i++] = PATH_SEP_CHAR;
    } else {
        memcpy( fpath, fullpath, i );
    }
    fpathend = fpath + i;
    *fpathend = NULLCHAR;
#ifdef __UNIX__
    memcpy( fname, fullpath + j, len - j + 1 );
#else
    if( strcmp( fullpath + j, MASK_ALL_ITEMS ) == 0 ) {
        fname[0] = '*';
        fname[1] = NULLCHAR;
    } else {
        memcpy( fname, fullpath + j, len - j + 1 );
    }
#endif
    dirp = opendir( fpath );
    if( dirp == NULL ) {
//        Log( false, "File (%s) not found.\n", f );
        return( true );
    }

    while( ( dire = readdir( dirp ) ) != NULL ) {
        if( ENTRY_INVALID( fname, dire ) )
            continue;
        /* set up file name, then try to delete it */
        len = strlen( dire->d_name );
        memcpy( fpathend, dire->d_name, len );
        fpathend[len] = NULLCHAR;
        if( ENTRY_SUBDIR( fpath, dire ) ) {
            /* process a directory */
            if( flags->bDirs ) {
                /* build directory list */
                len += i + 1;
                tmp = MallocSafe( offsetof( iolist, name ) + len );
                tmp->next = NULL;
                if( dtail == NULL ) {
                    dhead = tmp;
                } else {
                    dtail->next = tmp;
                }
                dtail = tmp;
                memcpy( tmp->name, fpath, len );
            } else {
//                Log( false, "%s is a directory, use -r\n", fpath );
//                retval = EACCES;
                rc = false;
            }
        } else if( !flags->bDirs && ENTRY_RDONLY( fpath, dire ) ) {
//            Log( false, "%s is read-only, use -f\n", fpath );
//            retval = EACCES;
            rc = false;
        } else {
            if( !remove_item( fpath, flags, false ) ) {
                rc = false;
            }
        }
    }
    closedir( dirp );
    /* process any directories found */
    for( tmp = dhead; tmp != NULL; tmp = dhead ) {
        dhead = tmp->next;
        if( !RecursiveRM( tmp->name, flags ) ) {
            rc = false;
        }
        free( tmp );
    }
    return( rc );
}


static bool RecursiveRM( const char *dir, const rm_flags *flags )
/***************************************************************/
/* RecursiveRM - do an RM recursively on all files */
{
    bool        rc;
    bool        rc2;
    char        fname[_MAX_PATH];

    /* purge the files */
    strcpy( fname, dir );
    strcat( fname, PATH_SEP_STR MASK_ALL_ITEMS );
    rc = doRM( fname, flags );
    /* purge the directory */
    rc2 = remove_item( dir, flags, true );
    if( rc )
        rc = rc2;
    return( rc );
}

STATIC bool processRM( const char *name, const rm_flags *flags )
/**************************************************************/
{
    if( flags->bDirs ) {
        if( strcmp( name, MASK_ALL_ITEMS ) == 0 ) {
            return( RecursiveRM( ".", flags ) );
        } else if( strpbrk( name, WILD_METAS ) != NULL ) {
            /* don't process wild cards on directories */
        } else if( chk_is_dir( name ) ) {
            return( RecursiveRM( name, flags ) );
        } else {
            return( remove_item( name, flags, false ) );
        }
        return( true );
    } else {
        if( strpbrk( name, WILD_METAS ) != NULL ) {
            return( doRM( name, flags ) );
        } else {
            return( remove_item( name, flags, false ) );
        }
    }
}

STATIC RET_T handleRM( char *cmd )
/*********************************
 * RM {ws}+ [-f -r -v {ws}+] <file>|<dir> ...
 *
 * -f   Force deletion of read-only files.
 * -r   Recursive deletion of directories.
 * -v   Verbose operation.
 */
{
    rm_flags    flags;
    bool        ok;
    char        *name;
    char        *p;

#ifdef DEVELOPMENT
    PrtMsg( DBG | INF | INTERPRETING, dosInternals[COM_RM] );
#endif

    if( Glob.noexec )
        return( RET_SUCCESS );

    for( ok = getRMArgs( cmd, &flags, &p ); ok && p != NULL && *p != NULLCHAR; ok = getRMArgs( NULL, NULL, &p ) ) {
        /* handle File name */
        p = CmdGetFileName( p, &name, true );
        *p = NULLCHAR;      /* terminate file name */
        if( !processRM( name, &flags ) ) {
            return( RET_ERROR );
        }
    }
    return( ( ok ) ? RET_SUCCESS : RET_ERROR );
}

STATIC RET_T handleMkdirSyntaxError( void )
/*****************************************/
{
    PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_MKDIR] );
    return( RET_ERROR );
}

#define DOMKDIR(d)  (MKDIR(d) == 0 || errno == EEXIST || errno == EACCES)

STATIC bool processMkdir( char *path, bool mkparents )
/****************************************************/
{
    char        *p;
    char        save_char;

    if( mkparents ) {
        p = path;
#ifndef __UNIX__
        /* special case for drive letters */
        if( cisalpha( p[0] ) && p[1] == ':' ) {
            p += 2;
        }
#endif
        /* find the next path component */
        while( *p != NULLCHAR ) {
            /* skip initial path separator if present */
            SKIP_PATH_SEP( p );

            while( *p != NULLCHAR && !IS_PATH_SEP( p ) )
                ++p;
            save_char = *p;
            *p = NULLCHAR;

            /* create directory */
            if( !DOMKDIR( path ) ) {
                /* Can not create directory for some reason */
                return( false );
            }
            /* put back the path separator */
            *p = save_char;
        }
        return( true );
    } else {
        return( DOMKDIR( path ) );
    }
}

STATIC RET_T handleMkdir( char *cmd )
/************************************
 * MKDIR {ws}+ [-p {ws}+] <dir>
 *
 * -p   Recursive creation of missing directories in path.
 */
{
    bool        mkparents;
    char        *path;
    char        *p;

#ifdef DEVELOPMENT
    PrtMsg( DBG | INF | INTERPRETING, dosInternals[COM_MKDIR] );
#endif

    if( Glob.noexec )
        return( RET_SUCCESS );

    mkparents = false;
    /* find "-p" options after "MKDIR " */
    p = SkipWS( cmd + 6 );
    if( p[0] == '-' ) {
        p++;
        if( !cisalpha( p[0] ) || ctolower( p[0] ) != 'p' ) {
            return( handleMkdirSyntaxError() );
        }
        mkparents = true;
        p = SkipWS( p + 1 );
    }
    /* handle File name */
    p = CmdGetFileName( p, &path, true );
    if( *p != NULLCHAR ) {
        return( handleMkdirSyntaxError() );
    }
    if( !processMkdir( path, mkparents ) ) {
        return( RET_ERROR );
    }
    return( RET_SUCCESS );
}

static FILE *open_file( const char *name, const char *mode )
/**********************************************************/
{
    FILE    *fp;

    fp = fopen( name, mode );
    if( fp == NULL )
        PrtMsg( ERR | ERROR_OPENING_FILE, name );
    return( fp );
}

static bool close_file( FILE *fp, const char *name )
/**************************************************/
{
    if( fp != NULL ) {
        if( fclose( fp ) ) {
            PrtMsg( ERR | ERROR_CLOSING_FILE, name );
            return( false );
        }
    }
    return( true );
}

static size_t read_block( void *buf, size_t len, FILE *fp, const char *name )
/***************************************************************************/
{
    size_t readlen;

    readlen = fread( buf, 1, len, fp );
    if( ferror( fp ) )
        PrtMsg( ERR | READ_ERROR, name );
    return( readlen );
}

static size_t write_block( const void *buf, size_t len, FILE *fp, const char *name )
/**********************************************************************************/
{
    size_t writelen;

    writelen = fwrite( buf, 1, len, fp );
    if( ferror( fp ) )
        PrtMsg( ERR | ERROR_WRITING_FILE, name );
    return( writelen );
}

STATIC bool processCopy( const char *src, const char *dst )
/*********************************************************/
{
    FILE            *fps;
    FILE            *fpd;
    bool            ok;
    char            buf[FILE_BUFFER_SIZE];
    size_t          len;
    pgroup2         pg;
    struct stat     st;
    struct utimbuf  dsttimes;

    if( chk_is_dir( dst ) ) {
        _splitpath2( src, pg.buffer, NULL, NULL, &pg.fname, &pg.ext );
        _makepath( buf, NULL, dst, pg.fname, pg.ext );
        dst = strcpy( pg.buffer, buf );
    }
    ok = false;
    fps = open_file( src, "rb" );
    if( fps != NULL ) {
        fpd = open_file( dst, "wb" );
        if( fpd != NULL ) {
            while( (len = read_block( buf, FILE_BUFFER_SIZE, fps, src )) == FILE_BUFFER_SIZE ) {
                if( len != write_block( buf, len, fpd, dst ) ) {
                    break;
                }
            }
            if( len != FILE_BUFFER_SIZE ) {
                ok = ( len == write_block( buf, len, fpd, dst ) );
            }
            ok &= close_file( fpd, dst );

            stat( src, &st );
            dsttimes.actime = st.st_atime;
            dsttimes.modtime = st.st_mtime;
            utime( dst, &dsttimes );
            /*
             * copy permissions: mostly necessary for the "x" bit
             * some files is copied with the read-only permission
             */
            chmod( dst, st.st_mode );
        }
        ok &= close_file( fps, src );
    }
    return( ok );
}

STATIC bool handleCopy( char *arg )
/***********************************
 * "COPY" {ws}+ <source file> {ws}+ <destination file>
 */
{
    char        *p;
    char        *fn1, *fn2;

    assert( arg != NULL );

    if( Glob.noexec ) {
        return( RET_SUCCESS );
    }

    /* Get first LFN */
    p = arg + 4;    /* skip "COPY" */
    p = SkipWS( p );
    p = CmdGetFileName( p, &fn1, true );
    if( *p == NULLCHAR || !cisws( *p ) ) {
        PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_COPY] );
        PrtMsg( INF | PRNTSTR, "First file" );
        PrtMsg( INF | PRNTSTR, fn1 );
        return( RET_ERROR );
    }
    *p++ = NULLCHAR;        /* terminate first file name */
    /* skip ws after first and before second file name */
    p = SkipWS( p );
    /* Get second LFN as well */
    p = CmdGetFileName( p, &fn2, true );
    if( *p != NULLCHAR && !cisws( *p ) ) {
        PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_COPY] );
        return( RET_ERROR );
    }
    *p = NULLCHAR;          /* terminate second file name */
    if( processCopy( fn1, fn2 ) )
        return( RET_SUCCESS );
    return( RET_ERROR );
}


STATIC RET_T handleRmdir( char *cmd )
/************************************
 * RMDIR {ws}+ <dir>
 */
{
    char        *path;
    char        *p;

#ifdef DEVELOPMENT
    PrtMsg( DBG | INF | INTERPRETING, dosInternals[COM_RMDIR] );
#endif

    if( Glob.noexec )
        return( RET_SUCCESS );

    /* find argument after "RMDIR " */
    p = SkipWS( cmd + 6 );
    /* handle File name */
    p = CmdGetFileName( p, &path, true );
    if( *p != NULLCHAR ) {
        PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_RMDIR] );
        return( RET_ERROR );
    }
    if( rmdir( path ) ) {
        return( RET_ERROR );
    }
    return( RET_SUCCESS );
}

STATIC bool hasMetas( const char *cmd )
/**************************************
 * determine whether a command line has meta characters in it or not
 */
{
#if defined( __DOS__ ) || defined( __NT__ )
    const char  *p;
    bool        dquote;

    dquote = false;
    for( p = cmd; *p != NULLCHAR; ++p ) {
        if( *p == '"' ) {
            dquote = !dquote;
        } else if( !dquote && strchr( SHELL_METAS, *p ) != NULL ) {
            return( true );
        }
    }
#elif defined( __OS2__ ) || defined( __UNIX__ )
    const char  *p;

    for( p = cmd; *p != NULLCHAR; ++p ) {
        if( *p == SHELL_ESC && p[1] != NULLCHAR ) {
            ++p;
        } else if( strchr( SHELL_METAS, *p ) != NULL ) {
            return( true );
        }
    }
#else

    /* unused parameters */ (void)cmd;

#endif
    return( false );
}

static void dumpCommand( char *cmd )
/**********************************/
{
    char    c;
    char    *p;
    char    *z;

    // trim trailing white space before printing
    z = cmd;
    for( p = cmd; *p != NULLCHAR; ++p ) {
        if( !cisws( *p ) ) {
            z = p;
        }
    }
    ++z;
    if( z == p ) {
        PrtMsg( INF | PRNTSTR, cmd );
    } else {
        c = *z;
        *z = NULLCHAR;
        PrtMsg( INF | PRNTSTR, cmd );
        *z = c;
    }
}

#if defined( __DOS__ )
STATIC UINT16 makeTmpEnv( char *arg )
/************************************
 * Copy arg into an environment var if possible.
 * If succeeds, then changes arg to just "@WMAKExxxxx", and returns non-zero.
 * Otherwise leaves arg alone and returns zero.
 */
{
    UINT16      tmp;
    char        name[20];   /* " @WMAKE%d" */
    size_t      len;
    size_t      len1;

    tmp = 1;
    for( ;; ) {
        FmtStr( name, " @WMAKE%d", tmp );
        if( getenv( name + 2 ) == NULL ) {
            break;
        }
        ++tmp;
    }
    len1 = strlen( name );
    len = strlen( arg );
    if( len < len1 ) {      /* need room for " @WMAKE%d" in arg */
        return( 0 );
    }
    if( SetEnvSafe( name + 2, arg ) != 0 ) {
        return( 0 );
    }
    strcpy( arg, name );    /* copy " @WMAKE%d" to arg */
    return( tmp );
}

STATIC void killTmpEnv( UINT16 tmp )
/**********************************/
{
    char        name[20];    /* "WMAKE%d" */

    if( tmp == 0 ) {
        return;
    }
    FmtStr( name, "WMAKE%d", tmp );
    SetEnvSafe( name, NULL );
}
#else
STATIC UINT16 makeTmpEnv( const char *cmd )
/*****************************************/
{
    /* unused parameters */ (void)cmd;

    return( 0 );
}

STATIC void killTmpEnv( UINT16 tmp )
/**********************************/
{
    /* unused parameters */ (void)tmp;
}
#endif

#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC RET_T shellSpawn( char *cmd, shell_flags flags )
/*****************************************************/
{
    bool        percent_cmd;        // is this a percent cmd?
    int         comnum;             // index into dosInternals
    char        cmdname[_MAX_PATH]; // copied from cmd
    char        *arg;               // used in parsing cmd into "words"
    char const  *argv[3];           // for spawnvp
    int         retcode;            // from spawnvp
    UINT16      tmp_env = 0;        // for * commands
    RET_T       my_ret;             // return code for this function
    bool        dquote;             // true if inside double quotes

    assert( cmd != NULL );

    percent_cmd = ( cmd[0] == '%' );
    /* split cmd name from args */
    dquote = false;     /* no double quotes yet */
    for( arg = cmd + (percent_cmd ? 1 : 0); *arg != NULLCHAR; arg++ ) {
        if( !dquote ) {
            if( cisws( *arg ) || *arg == Glob.swchar || *arg == '+' || *arg == '=' ) {
                break;
            }
        }
        if( *arg == '\"' ) {
            dquote = !dquote;       /* found a double quote */
        }
    }
    if( arg - cmd >= _MAX_PATH ) {
        PrtMsg( ERR | COMMAND_TOO_LONG );
        return( RET_ERROR );
    }
    if( dquote ) {
        /* closing double quote is missing */
        PrtMsg( ERR | SYNTAX_ERROR_IN, cmd );
        return( RET_ERROR );
    }

    memcpy( cmdname, cmd, arg - cmd );  /* copy command */
    cmdname[arg - cmd] = NULLCHAR;      /* null terminate it */
    if( *cmdname == NULLCHAR ) {
        // handle blank command by shell
        flags |= FLAG_SHELL;
    }

    /* skip whitespace between the command and the argument */
    while( cisws( *arg ) ) {
        arg++;
    }

#if defined( __DOS__ )
    {
        char    ext_buf[10];
        char    *ext;

        _splitpath2( cmdname, ext_buf, NULL, NULL, NULL, &ext );
        if( ext[0] == '.' ) {
            FixName( ext );
            /* if extension specified let the shell handle it */
            if( !FNameEq( ext + 1, "exe" ) && !FNameEq( ext + 1, "com" ) ) {
                flags |= FLAG_SHELL; /* .bat and .cmd need the shell anyway */
            }
        }
    }
#endif
    comnum = findInternal( cmdname );
    if( (flags & FLAG_SILENT) == 0
      || (Glob.noexec && (comnum != COM_FOR && comnum != COM_IF || (flags & FLAG_SHELL))
      && !percent_cmd) ) {
        if( !Glob.noheader && !Glob.compat_posix ) {
            PrtMsg( INF | NEOL | JUST_A_TAB );
        }
        dumpCommand( cmd );
    }
    if( percent_cmd ) {
        if( percentCmd( cmdname, arg ) )
            return( RET_SUCCESS );
        return( RET_ERROR );
    }
    /*
     * The SET command must be handled locally to have any effect. Consider
     * `Path=C:\Program Files (x86)\foo' which will be detected as containing
     * metacharacters. Stupid Microsoft...
     */
    if( hasMetas( cmd ) && comnum != COM_SET && comnum != COM_FOR && comnum != COM_IF ) {
        flags |= FLAG_SHELL; /* pass to shell because of '>','<' or '|' */
    }
    if( (flags & FLAG_ENV_ARGS) && (flags & FLAG_SHELL) == 0 ) {
        tmp_env = makeTmpEnv( arg );
    }
/*
    makeTmpEnv has cleanup - any returns after this point must do this
    cleanup which is why these if else constructs don't have return
    statements in them.
*/
    if( flags & FLAG_SHELL ) {
        my_ret = mySystem( cmdname, cmd );
    } else if( comnum >= 0 ) {              /* check if we interpret it */
        switch( comnum ) {
        case COM_ECHO:  my_ret = handleEcho( cmd );         break;
        case COM_SET:   my_ret = handleSet( cmd );          break;
        case COM_FOR:   my_ret = handleFor( cmd );          break;
        case COM_IF:    my_ret = handleIf( cmd );           break;
        case COM_RM:    my_ret = handleRM( cmd );           break;
        case COM_MKDIR: my_ret = handleMkdir( cmd );        break;
        case COM_RMDIR: my_ret = handleRmdir( cmd );        break;
        case COM_COPY:  my_ret = handleCopy( cmd );         break;
#if defined( __OS2__ ) || defined( __NT__ ) || defined( __UNIX__ ) || defined( __RDOS__ )
        case COM_CD:
        case COM_CHDIR: my_ret = handleCD( cmd );           break;
#endif
#if defined( __OS2__ ) || defined( __NT__ ) || defined( __RDOS__ )
        case CNUM:      my_ret = handleChangeDrive( cmd );  break;
#endif
        default:        my_ret = mySystem( cmdname, cmd );  break;
        }
    } else if( Glob.noexec ) {
        my_ret = RET_SUCCESS;
    } else {                                /* pass to spawnvp */
        DLL_CMD     *dll_cmd;

        argv[0] = cmdname;
        if( *arg == NULLCHAR ) {
            argv[1] = NULL;     /* no args */
        } else {
            argv[1] = arg;      /* pass the args */
            argv[2] = NULL;
        }
        closeCurrentFile();
        dll_cmd = OSFindDLL( argv[0] );
        if( dll_cmd == NULL ) {
#ifdef __UNIX__
            /*
             * For UNIX we must for now use system since
             * without splitting argv[1] the spawnvp below
             * does not always work
             */
            my_ret = mySystem( cmdname, cmd );
            retcode = (UINT8)lastErrorLevel;
#else
            retcode = (int)spawnvp( P_WAIT, cmdname, argv );
#endif
            if( retcode < 0 ) {
                PrtMsg( ERR | UNABLE_TO_EXEC, cmdname );
            }
        } else {
            retcode = OSExecDLL( dll_cmd, argv[1] );
#ifdef DLLS_IMPLEMENTED
            if( retcode != IDEDRV_SUCCESS ) {
                if( retcode == IDEDRV_ERR_RUN_FATAL ) {
                    retcode = 2;
                } else if( retcode == IDEDRV_ERR_RUN_EXEC ) {
                    retcode = 1;
                } else if( retcode == IDEDRV_ERR_RUN ) {
                    PrtMsg( ERR | DLL_BAD_RETURN_STATUS, dll_cmd->inf.dll_name );
                    retcode = 4;
                } else if( retcode == IDEDRV_ERR_LOAD ||
                           retcode == IDEDRV_ERR_UNLOAD ) {
                    PrtMsg( ERR | UNABLE_TO_LOAD_DLL, dll_cmd->inf.dll_name );
                    retcode = 4;
                } else {
                    PrtMsg( ERR | DLL_BAD_INIT_STATUS, dll_cmd->inf.dll_name );
                    retcode = 4;
                }
#else
            if( retcode != 0 ) {
                PrtMsg( ERR | UNABLE_TO_EXEC, cmdname );
                retcode = 4;
#endif
            } else {
                retcode = 0;
            }
        }
        lastErrorLevel = (UINT8)retcode;
        if( flags & FLAG_SHELL_RC ) {
            my_ret = retcode;
        } else {
            my_ret = retcode ? RET_ERROR : RET_SUCCESS;
        }
    }
    if( flags & FLAG_ENV_ARGS ) {    /* cleanup for makeTmpEnv */
        killTmpEnv( tmp_env );
    }
    return( my_ret );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


STATIC bool execLine( char *line )
/*********************************
 * is allowed to hack up line any way it feels
 */
{
    char        *p;
    shell_flags flags;
    RET_T       rc;

    assert( line != NULL );

    CheckForBreak();
    flags = 0;
    /* make a copy of global flags */
    if( Glob.silent && !Glob.silentno )
        flags |= FLAG_SILENT;
    if( Glob.ignore )
        flags |= FLAG_IGNORE;
    if( Glob.shell )
        flags |= FLAG_SHELL;

    for( p = line; ; ++p ) {         /* process @*!- and strip leading ws */
        p = SkipWS( p );
        if( *p == '@' ) {
            if( !Glob.silentno ) {
                flags |= FLAG_SILENT;
            }
        } else if( *p == '*' ) {
            flags |= FLAG_ENV_ARGS;
        } else if( *p == '!' ) {
            flags |= FLAG_SHELL;
        } else if( *p == '-' ) {
            flags |= FLAG_IGNORE;
        } else {
            break;
        }
    }

    assert( !cisws( *p ) );

    // NMAKE quietly ignores empty commands
    if( Glob.compat_nmake && *p == NULLCHAR ) {
        return( true );
    }
    rc = shellSpawn( p, flags );
    if( OSCorrupted() ) {
        PrtMsg( FTL | OS_CORRUPTED );
        ExitFatal();
        // never return
    }
    CheckForBreak();
    if( rc != RET_SUCCESS && (flags & FLAG_IGNORE) == 0 ) {
        return( false );
    }
    return( true );
}

INT32 ExecCommand( char *line )
/******************************
 * Execute an '!if [cmd]' style command
 */
{
    char    *p;
    RET_T   rc;
    int     old_err = Glob.erroryet;

    assert( line != NULL );

    CheckForBreak();
    p = SkipWS( line );
    assert( !cisws( *p ) );

    // NMAKE quietly ignores empty commands here; should we as well?
    if( Glob.compat_nmake && *p == NULLCHAR ) {
        return( RET_SUCCESS );
    }
    // Execute command - run it always, always silent, and get real retcode
    rc = shellSpawn( p, FLAG_SILENT | FLAG_SHELL_RC );
    if( OSCorrupted() ) {
        PrtMsg( FTL | OS_CORRUPTED );
        ExitFatal();
        // never return
    }
    CheckForBreak();

    // Errors during [cmd] execution don't count
    Glob.erroryet = old_err;

    // Report return code from shell
    return( (UINT8)rc );
}


bool ExecCList( CLIST *clist )
/****************************/
{
    char        *line;
    bool        ok;
    FLIST const *currentFlist;

    assert( clist != NULL );

    ok = true;
    for( ; clist != NULL; clist = clist->next ) {
        ok = writeInlineFiles( clist->inlineHead, &(clist->text) );
        currentFlist = clist->inlineHead;
        if( ok ) {
            UnGetCHR( STRM_MAGIC );
            InsString( clist->text, false );
            line = DeMacro( TOK_MAGIC );
            GetCHR();        /* eat STRM_MAGIC */
            if( Glob.verbose ) {
                ok = verbosePrintTempFile( currentFlist );
            }
            ok = execLine( line );
            FreeSafe( line );
            if( !ok ) {
                return( ok );
            }
        } else {
            closeCurrentFile();
            return( ok );
        }
    }
    closeCurrentFile();
    return( ok );
}


STATIC void destroyNKList( void )
/********************************
 * deletes the file specified in the nokeeplist
 */
{
    NKLIST const    *temp;
    VECSTR          outText;
    char            *tempstr;

    for( temp = noKeepList; temp != NULL; temp = temp->next ) {
        if( Glob.noexec ) {
            if( !Glob.noheader ) {
                PrtMsg( INF | NEOL | JUST_A_TAB );
            }
            outText = StartVec();
            WriteVec( outText, "del " );
            WriteVec( outText, temp->fileName );
            tempstr = FinishVec( outText );
            PrtMsg( INF | PRNTSTR, tempstr );
            FreeSafe( tempstr );
        }
        remove( temp->fileName );
    }
    FreeNKList( noKeepList );
}


void ExecInit( void )
/*******************/
{
    lastErrorLevel = 0;
    currentFileName = NULL;
    currentFileHandle = NULL;
    /* Take any number first */
    tmpFileNumber = (UINT16)( time( NULL ) % 100000 );
}


void ExecFini( void )
/*******************/
{
    // destroy all the files that will not be kept
    destroyNKList();
}
