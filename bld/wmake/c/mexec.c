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
* Description:  Macro execution and inline file handling.
*
****************************************************************************/


#include <sys/types.h>
#ifdef __UNIX__
    #include <dirent.h>
    #include <fnmatch.h>
#else
    #include <direct.h>
    #include <dos.h>
  #if defined( __WATCOMC__ )
    #include <fnmatch.h>
  #endif
#endif
#include <sys/stat.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
    #include <process.h>
#endif
#ifdef __UNIX__
    #include <sys/wait.h>
#endif
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#ifdef DLLS_IMPLEMENTED
    #include "idedrv.h"
#endif
#include "make.h"
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

#include "clibext.h"


#ifdef __UNIX__
  #define MASK_ALL_ITEMS  "*"
#else
  #define MASK_ALL_ITEMS  "*.*"
#endif

typedef enum {
    FLAG_SHELL      = 0x01,
    FLAG_SILENT     = 0x02,
    FLAG_ENV_ARGS   = 0x04,
    FLAG_IGNORE     = 0x08,
    FLAG_SHELL_RC   = 0x10
} shell_flags;

typedef struct dd {
  struct dd     *next;
  char          attr;
  char          name[1];
} iolist;

typedef struct {
    BIT bForce   : 1;
    BIT bDirs    : 1;
    BIT bVerbose : 1;
} rm_flags;

STATIC  UINT8   lastErrorLevel;
STATIC  UINT16  tmpFileNumber;          /* temp file number         */
STATIC  char    tmpFileChar  ;          /* temp file number chari   */
STATIC  int     currentFileHandle;      /* %write, %append, %create */
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


STATIC RET_T execLine( char *line );    /* called recursively in handleFor */

STATIC NKLIST   *noKeepList;            /* contains the list of files that
                                           needs to be cleaned when wmake
                                           exits */

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

    tmpPath    = GetMacroValue( TEMPENVVAR );
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
            } else if( strlen( tmpPath ) + strlen( fileName ) >= _MAX_PATH ) {
                FreeVec( buf );
                FreeSafe( tmpPath );
                PrtMsg( FTL | TMP_PATH_TOO_LONG );
                ExitFatal();
            }
        }
        if( tmpPath == NULL ) {
            WriteVec( buf, fileName );
            result = FinishVec( buf );
        } else {
            WriteVec( buf, tmpPath );
            if( tmpPath[strlen( tmpPath ) - 1] != BACKSLASH ) {
                buf2 = StartVec();
#if defined( __UNIX__ )
                WriteVec( buf2, "/" );
#else
                WriteVec( buf2, "\\" );
#endif
                CatVec( buf, buf2 );
            }
            buf2 = StartVec();
            WriteVec( buf2, fileName );
            CatVec( buf, buf2 );
            result = FinishVec( buf );
        }

        if( !existFile( result ) ) {
            /* touch the file */
            TouchFile( result );
            FreeSafe( tmpPath );
            return( result );
        } else {
            FreeSafe( result );
        }
        tmpFileNumber = (UINT16)((tmpFileNumber + time( NULL )) % 100000);
    }
}


STATIC RET_T processInlineFile( int handle, const char *body,
    const char *fileName, bool writeToFile )
/***********************************************************/
{
    int         index;
    RET_T       ret;
    char        *DeMacroBody;
    int         currentSent;
    bool        firstTime;
    VECSTR      outText;
    char        c;

    firstTime = true;
    currentSent = 0;
    ret         = RET_SUCCESS;

    assert( body != NULL );

    // we will push the whole body back into the stream to be fully
    // deMacroed
    for( index = 0; (c = body[index++]) != NULLCHAR; ) {
        if( c == EOL ) {
            InsString( body + currentSent, false );
            DeMacroBody = ignoreWSDeMacro( false, ForceDeMacro() );
            currentSent = index;
            if( writeToFile ) {
                size_t bytes = strlen( DeMacroBody );

                if( (int)bytes != write( handle, DeMacroBody, (unsigned)bytes ) ) {
                    ret = RET_ERROR;
                }
                if( 1 != write( handle, "\n", 1 ) ) {
                    ret = RET_ERROR;
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
    return( ret );
}

STATIC RET_T writeLineByLine( int handle, const char *body )
/**********************************************************/
{
    return( processInlineFile( handle, body, NULL, true ) );
}


STATIC char *RemoveBackSlash( const char *inString )
/***************************************************
 * remove backslash from \"
 */
{
    char    buffer[_MAX_PATH];
    char    *current;
    int     pos;
    char    c;

    assert( inString != NULL );

    for( pos = 0, current = (char *)inString; (c = *current++) != NULLCHAR && pos < _MAX_PATH - 1; ) {
        if( c == BACKSLASH ) {
            if( *current == DOUBLEQUOTE ) {
                c = *current++;
            }
        }
        buffer[pos++] = c;
    }
    buffer[pos] = NULLCHAR;

    return( StrDupSafe( buffer ) );
}


STATIC RET_T VerbosePrintTempFile( const FLIST *head )
/****************************************************/
{
    FLIST const *current;
    RET_T       ret = RET_SUCCESS; // success if list empty

    for( current = head; current != NULL; current = current->next ) {
        assert( current->fileName != NULL );
        ret = processInlineFile( 0, current->body, current->fileName, false );
    }
    return( ret );
}

STATIC RET_T createFile( const FLIST *head )
/*******************************************
 * create file given information in the FLIST
 */
{
    NKLIST  *temp;
    int     handle;
    char    *fileName = NULL;
    char    *tmpFileName = NULL;
    RET_T   ret;

    assert( head != NULL );
    ret = RET_SUCCESS;

    if( head->fileName != NULL ) {
        /* Push the filename back into the stream
         * and then get it back out using DeMacro to fully DeMacro
         */
        UnGetCH( STRM_MAGIC );
        InsString( head->fileName, false );
        fileName = DeMacro( TOK_MAGIC );
        GetCHR();           /* eat STRM_MAGIC */
    } else {
        ret = RET_ERROR;
    }

    if( ret != RET_ERROR ) {
        tmpFileName = RemoveBackSlash( fileName );
        handle = open( tmpFileName, O_TEXT | O_WRONLY | O_CREAT | O_TRUNC, PMODE_RW );
        if( handle != -1 ) {
            if( writeLineByLine( handle, head->body ) == RET_ERROR ) {
                PrtMsg( ERR | ERROR_WRITING_FILE, tmpFileName );
                ret = RET_ERROR;
            }
            if( close( handle ) != -1 ) {
                if( !head->keep ) {
                    temp = NewNKList();
                    temp->fileName = StrDupSafe( tmpFileName );
                    temp->next     = noKeepList;
                    noKeepList     = temp;
                }
            } else {
                PrtMsg( ERR | ERROR_CLOSING_FILE, tmpFileName );
                ret = RET_ERROR;
            }
        } else {
            PrtMsg( ERR | ERROR_OPENING_FILE, tmpFileName );
            ret = RET_ERROR;
        }
    }
    FreeSafe( fileName );
    FreeSafe( tmpFileName );
    return( ret );
}


STATIC RET_T writeInlineFiles( FLIST *head, char **commandIn )
/*************************************************************
 * This part writes the inline files
 * modifies the command text to show the temporary file names
 * assumption is that all << are removed for explicitly defined
 * file names so the only << left are for temporary files
 */
{
    char    *cmdText;
    FLIST   *current;
    RET_T   ret;
    VECSTR  newCommand;
    size_t  start;  // start of cmdText to be copied into newCommand;
    size_t  index;  // current index of cmdText
    NKLIST  *temp;

    assert( *commandIn != NULL );

    cmdText    = *commandIn;
    ret        = RET_SUCCESS;
    newCommand = StartVec();
    WriteVec( newCommand, "" );
    index      = 0;
    start      = index;

    for( current = head;
        current != NULL && ret == RET_SUCCESS && cmdText[index] != NULLCHAR;
        current = current->next )
    {
        // if the filename is the inline symbol then we need change
        // the filename into a temp filename
        if( strcmp( current->fileName, INLINE_SYMBOL ) == 0 ) {
            for( ;; ) {
                if( cmdText[index] == LESSTHAN ) {
                    if( cmdText[index + 1] == LESSTHAN ) {
                        index += 2;
                        break;
                    }
                } else if( cmdText[index] == NULLCHAR ) {
                    /* not possible to come here*/
                    ret = RET_ERROR;
                    break;
                }
                ++index;
            }
            if( ret == RET_ERROR ) {
                break;
            }
            CatNStrToVec( newCommand, cmdText+start, index-start-2 );
            start = index;
            FreeSafe( current->fileName );
            current->fileName = createTmpFileName();

            CatStrToVec( newCommand, current->fileName );
        }
        if( !Glob.noexec ) {
            ret = createFile( current );
        } else {
            if( !current->keep ) {
                temp = NewNKList();
                temp->fileName = StrDupSafe( current->fileName );
                temp->next     = noKeepList;
                noKeepList     = temp;
            }
        }
    }
    CatNStrToVec( newCommand, cmdText+start, strlen( cmdText ) - start );
    FreeSafe( cmdText );
    *commandIn = FinishVec( newCommand );
    return( ret );
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
    if( cisalpha( *cmd ) && cmd[1] == ':' && cmd[2] == NULLCHAR ) {
        return( CNUM );
    }
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


STATIC RET_T percentMake( char *arg )
/************************************
 * do a recursive make of the target in arg
 */
{
    char        *finish;
    TARGET      *calltarg;
    RET_T       ret;
    char        *buf;
    char        *start;
    bool        newtarg;
    bool        more_targets;

    /* %make <target> <target> ... */
    buf = MallocSafe( _MAX_PATH );

    ret = RET_ERROR;
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
        ret = TrySufPath( buf, start, &calltarg, false );

        newtarg = false;
        if( ( ret == RET_SUCCESS && calltarg == NULL ) || ret == RET_ERROR ) {
            /* Either file doesn't exist, or it exists and we don't already
             * have a target for it.  Either way, we create a new target.
             */
            calltarg = NewTarget( buf );
            newtarg = true;
        }
        ret = Update( calltarg );
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

    return( ret );
}


STATIC void closeCurrentFile( void )
/**********************************/
{
    if( currentFileHandle != -1 ) {
        close( currentFileHandle );
        currentFileHandle = -1;
    }
    if( currentFileName != NULL ) {
        FreeSafe( currentFileName );
        currentFileName = NULL;
    }
    CacheRelease();     /* so that the cache is updated */
}


STATIC RET_T percentWrite( char *arg, enum write_type type )
/**********************************************************/
{
    char        *p;
    char const  *text;
    char        *fn;
    char const  *cmd_name;
    int         open_flags;
    size_t      len;

    assert( arg != NULL );

    if( Glob.noexec ) {
        return( RET_SUCCESS );
    }

    p = SkipWS( arg );
    fn = p;

    if( *p != DOUBLEQUOTE ) {
        while( cisfilec( *p ) ) {
            ++p;
        }
    } else {
        ++p;    // Skip the first quote
        ++fn;
        while( *p != DOUBLEQUOTE && *p != NULLCHAR ) {
            ++p;
        }
        if( *p != NULLCHAR ) {
            *p++ = NULLCHAR;
        }
    }

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
            return( RET_ERROR );
        }
        *p++ = NULLCHAR;    /* terminate file name */
        text = p;           /* set text pointer */
        p += strlen( p );   /* find null terminator */
    } else {
        *p = NULLCHAR;      /* terminate file name */
        text = p;           /* set text pointer */
    }

    /* now text points to the beginning of string to write, and p points to
     * the end of the string.  fn points to the name of the file to write to
     */
    FixName( fn );
    if( type == WR_CREATE || currentFileName == NULL || !FNameEq( currentFileName, fn ) ) {
        closeCurrentFile();
        currentFileName = StrDupSafe( fn );
        open_flags = O_WRONLY | O_CREAT | O_TEXT;
        if( type == WR_APPEND ) {
            open_flags |= O_APPEND;
        } else {
            open_flags |= O_TRUNC;
        }

        currentFileHandle = open( fn, open_flags, PMODE_RW );
        if( currentFileHandle == -1 ) {
            PrtMsg( ERR | OPENING_FOR_WRITE, fn );
            closeCurrentFile();
            return( RET_ERROR );
        }
    }

    if( type != WR_CREATE ) {
        *p = '\n';          /* replace null terminator with newline */
        len = ( p - text ) + 1;
        if( write( currentFileHandle, text, (unsigned)len ) != (int)len ) {
            PrtMsg( ERR | DOING_THE_WRITE );
            closeCurrentFile();
            return( RET_ERROR );
        }
    }

    CacheRelease();     /* so that the cache is updated */

    return( RET_SUCCESS );
}


STATIC RET_T percentErase( char *arg )
/************************************/
{
    if( 0 == unlink( FixName( arg ) ) ) {
        return( RET_SUCCESS );
    }
    return( RET_ERROR );
}

STATIC RET_T percentRename( char *arg )
/************************************/
{
    char        *p;
    char        *fn1, *fn2;

    assert( arg != NULL );

    if( Glob.noexec ) {
        return( RET_SUCCESS );
    }

    /* Get first file name, must end in space but may be surrounded by double quotes */
    p = SkipWS( arg );
    fn1 = p;
    if( *p != DOUBLEQUOTE ) {
        while( cisfilec( *p ) ) {
            ++p;
        }
    } else {
        ++p;    // Skip the first quote
        ++fn1;
        while( *p != DOUBLEQUOTE && *p != NULLCHAR ) {
            ++p;
        }
        if( *p != NULLCHAR ) {
            *p++ = NULLCHAR;
        }
    }

    if( *p == NULLCHAR || !cisws( *p ) ) {
        PrtMsg( ERR | SYNTAX_ERROR_IN, percentCmds[PER_RENAME] );
        PrtMsg( INF | PRNTSTR, "First file" );
        PrtMsg( INF | PRNTSTR, p );
        return( RET_ERROR );
    }
    *p++ = NULLCHAR;        /* terminate first file name */

    /* Get second file name as well */
    p = SkipWS( p );
    fn2 = p;
    if( *p != DOUBLEQUOTE ) {
        while( cisfilec( *p ) ) {
            ++p;
        }
    } else {
        ++p;    // Skip the first quote
        ++fn2;
        while( *p != DOUBLEQUOTE && *p != NULLCHAR ) {
            ++p;
        }
        if( *p != NULLCHAR ) {
            *p++ = NULLCHAR;
        }
    }

    if( *p != NULLCHAR && !cisws( *p ) ) {
        PrtMsg( ERR | SYNTAX_ERROR_IN, percentCmds[PER_RENAME] );
        return( RET_ERROR );
    }
    *p = NULLCHAR;          /* terminate second file name */
    if( rename( fn1, fn2 ) == 0 )
        return( RET_SUCCESS );
    return( RET_ERROR );
}

STATIC RET_T percentCmd( const char *cmdname, char *arg )
/********************************************************
 * handle our special percent commands
 */
{
    char const * const  *key;
    char const          *ptr;
    int                 num;

    assert( cmdname != NULL && arg != NULL );

    ptr = cmdname + 1;
    key = bsearch( &ptr, percentCmds, PNUM, sizeof( char * ), KWCompare );

    if( key == NULL ) {
        PrtMsg( ERR | UNKNOWN_PERCENT_CMD );
        closeCurrentFile();
        return( RET_ERROR );
    } else {
        num = (int)( key - (char const **)percentCmds );
    }

    if( Glob.noexec && num != PER_MAKE ) {
        return( RET_SUCCESS );
    }

    switch( num ) {
    case PER_ABORT:
        closeCurrentFile();
        ExitError();

    case PER_APPEND:
        return( percentWrite( arg, WR_APPEND ) );

    case PER_CREATE:
        return( percentWrite( arg, WR_CREATE ) );

    case PER_ERASE:
        return( percentErase( arg ) );

    case PER_MAKE:
        return( percentMake( arg ) );

    case PER_NULL:
        break;

    case PER_QUIT:
        closeCurrentFile();
        ExitOK();

    case PER_RENAME:
        return( percentRename( arg ) );

    case PER_STOP:
        closeCurrentFile();
        if( !GetYes( DO_YOU_WISH_TO_CONT ) ) {
            ExitOK();
        }
        break;

    case PER_WRITE:
        return( percentWrite( arg, WR_WRITE ) );

    default:
        assert( false );
        break;
    };

    return( RET_SUCCESS );
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
#else
    retcode = system( cmd );
#endif
    lastErrorLevel = (UINT8)retcode;
#ifdef __UNIX__
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
    ENV_TRACKER *env;       /* space allocated for envvar   */
    int         retcode;    /* from putenv                  */

    assert( cmd != NULL );

#ifdef DEVELOPMENT
    PrtMsg( DBG | INF | INTERPRETING, dosInternals[COM_SET] );
#endif

    if( Glob.noexec ) {
        return( RET_SUCCESS );
    }

    p = SkipWS( cmd + 3 );      /* find first non-ws after "SET" */

    if( *p == NULLCHAR ) {      /* just "SET" with no options... pass on */
        return( mySystem( cmd, cmd ) );
    }

    /* anything goes in a dos set name... even punctuation! */
    name = p;
    while( *p != NULLCHAR && !cisws( *p ) && *p != '=' ) {
        ++p;
    }
    endname = p;

    p = SkipWS( p );            /* trim ws after name */

    if( *p != '=' || endname == name ) {
        PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_SET] );
        return( RET_ERROR );
    }

    *endname = NULLCHAR;        /* null terminate name */

    ++p;                        /* advance to character after '=' */

                        /* +1 for '=' (already +1 for NULLCHAR in ENV_TRACKER) */
    env = MallocSafe( sizeof( *env ) + 1 + (endname - name) + strlen( p ) );
    FmtStr( env->value, "%s=%s", name, p );
    retcode = PutEnvSafe( env );
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

    if( cmd[4] ) {      /* check for echo with no arguments */
        p = cmd + 5;    /* assume "ECHO "; whitespace gets printed! */

        PrtMsg( INF | PRNTSTR, p );
    }
    return( RET_SUCCESS );
}

STATIC RET_T handleIf( char *cmd )
/*********************************
 *          { ERRORLEVEL <number> }
 * IF [NOT] { <str1> == <str2>    } <command>
 *          { EXIST <file>        }
 */
{
    bool        not;        /* flag for not keyword                     */
    bool        condition;  /* whether the condition was T or F         */
    char        *p;         /* used to scan the string                  */
    char const  *tmp1;      /* one of NOT | ERRORLEVEL | <str1> | EXIST */
    char const  *tmp2;      /* one of <number> | "==" | <file> | <str2> */
    char        *end1;      /* location of end of tmp1 string           */
    char        save;       /* save character                           */
    const char  *file;      /* for checking file existence              */

    assert( cmd != NULL );

#ifdef DEVELOPMENT
    PrtMsg( DBG | INF | INTERPRETING, dosInternals[COM_IF] );
#endif

    if( Glob.noexec ) {
        return( RET_SUCCESS );
    }
    closeCurrentFile();

    p = SkipWS( cmd + 2 );      /* find first non-ws after "IF" */
    if( *p == NULLCHAR ) {
        PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_IF] );
        return( RET_ERROR );
    }

    tmp1 = p;                   /* find first word after IF */
    while( !cisws( *p ) && *p != NULLCHAR && *p != '=' ) {
        ++p;
    }
    if( *p == NULLCHAR ) {
        PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_IF] );
        return( RET_ERROR );
    }

    save = *p;
    *p = NULLCHAR;
    not = ( stricmp( tmp1, "NOT" ) == 0 );
    *p = save;

    if( not ) {             /* discard the "NOT" get next word */
        tmp1 = p = SkipWS( p );
        while( !cisws( *p ) && *p != NULLCHAR && *p != '=' ) {
            ++p;
        }
        if( *p == NULLCHAR ) {
            PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_IF] );
            return( RET_ERROR );
        }
        save = *p;
    }
    end1 = p;

    tmp2 = p = SkipWS( p );
    if( *p == NULLCHAR ) {
        PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_IF] );
        return( RET_ERROR );
    }

    //while( !cisws( *p ) && *p != NULLCHAR ) ++p;
    p = FindNextWS( p );

    if( *p == NULLCHAR ) {
        PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_IF] );
        return( RET_ERROR );
    }

    *end1 = NULLCHAR;
    if( stricmp( tmp1, "ERRORLEVEL" ) == 0 ) {
        *p = NULLCHAR;
        condition = ( lastErrorLevel >= atoi( tmp2 ) );
    } else if( stricmp( tmp1, "EXIST" ) == 0 ) {

        *p = NULLCHAR;

        // handle long filenames
        RemoveDoubleQuotes( (char *)tmp2, strlen( tmp2 ) + 1, tmp2 );

        file = DoWildCard( tmp2 );
        condition = ( ( file != NULL ) && CacheExists( file ) );
        /* abandon rest of entries if any */
        DoWildCardClose();
    } else {
        *end1 = save;
        p = end1;           /* back up to end of 1st token */
        for( ;; ) {
            while( ( *p != NULLCHAR ) && ( *p != '=' ) ) {
                ++p;
            }
            if( *p == NULLCHAR ) {
                PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_IF] );
                return( RET_ERROR );
            }
            if( p[1] == '=' ) {
                break;
            }
            ++p;
        }
                            /* we have found "==", get <str2> */
        tmp2 = p = SkipWS( p + 2 );
        while( !cisws( *p ) && *p != NULLCHAR ) {
            ++p;
        }
        if( *p == NULLCHAR ) {
            PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_IF] );
            return( RET_ERROR );
        }
        *p = NULLCHAR;
        *end1 = NULLCHAR;       /* null-terminate tmp1 again */
        condition = ( strcmp( tmp1, tmp2 ) == 0 );
    }

    p = SkipWS( p + 1 );
    if( *p == NULLCHAR ) {
        PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_IF] );
        return( RET_ERROR );
    }

    if( (not && !condition) || (!not && condition) ) {
        return( execLine( p ) );
    }
    return( RET_SUCCESS );
}


STATIC RET_T handleForSyntaxError( void )
/***************************************/
{
    PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_FOR] );
    return( RET_ERROR );
}


STATIC RET_T getForArgs( char *line, const char **pvar, char **pset,
    const char **pcmd )
/******************************************************************/
{
    char    *p;

    assert( line != NULL && pvar != NULL && pset != NULL && pcmd != NULL );

    /* remember we can hack up line all we like... */

    p = SkipWS( line + 3 ); /* find first non-ws after "FOR" */

                            /* got <var>, now test if legal */
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

    *p = NULLCHAR;          /* truncate variable name */

    p = SkipWS( p + 1 );    /* move to "in" */

    if( ctoupper( p[0] ) != 'I' || ctoupper( p[1] ) != 'N' || !cisws( p[2] ) ) {
        return( handleForSyntaxError() );
    }

    p = SkipWS( p + 3 );    /* move to ( before <set> */

    if( p[0] != '(' ) {
        return( handleForSyntaxError() );
    }
    ++p;
    *pset = p;/* beginning of set */

    while( *p != NULLCHAR && *p != ')' ) {
        ++p;
    }
    if( *p == NULLCHAR ) {
        return( handleForSyntaxError() );
    }

    *p = NULLCHAR;          /* truncate set string */

    p = SkipWS( p + 1 );    /* move to "do" */

    if( ctoupper( p[0] ) != 'D' || ctoupper( p[1] ) != 'O' || !cisws( p[2] ) ) {
        return( handleForSyntaxError() );
    }

    p = SkipWS( p + 3 );    /* move to beginning of cmd */

    *pcmd = (const char *)p;

    return( RET_SUCCESS );
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
 * "FOR" {ws}* "%"["%"]<var> {ws}+ "IN" {ws}+ "("<set>")" {ws}+ "DO" {ws}+ <cmd>
 */
{
    static bool     busy = false;   /* recursion protection */
    const char      *var;           /* loop variable name incl. %           */
    char            *set;           /* set of values for looping            */
    const char      *cmd;           /* command to execute                   */
    const char      *p;             /* working pointer                      */
    char            hold;           /* final character of set during loop   */
    const char      *subst;         /* pointer to the element to substitute */
    size_t          varlen;         /* strlen( var )                        */
    unsigned        numsubst;       /* number of substitutions per cmd      */
    size_t          cmdlen;         /* strlen( cmd ) - numsubst * varlen    */
    size_t          newlen;         /* size of memory we need               */
    size_t          lastlen;        /* last size of memory we asked for     */
    char            *exec;          /* line to execute                      */

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
    if( getForArgs( line, &var, &set, &cmd ) != RET_SUCCESS ) {
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

                    /* always skip ws in set */
    set = SkipWS( set );
    hold = *set;
    while( hold != NULLCHAR ) {
        subst = set;        /* remember start of subst string */

        set = FindNextWS( set );

        hold = *set;
        *set = NULLCHAR;

        for( subst = DoWildCard( subst ); subst != NULL; subst = DoWildCard( NULL ) ) {
            newlen = numsubst * strlen( subst ) + cmdlen;
            if( newlen > lastlen ) {
                FreeSafe( exec );
                exec = MallocSafe( newlen );
                lastlen = newlen;
            }

            /* make variable substitutions */
            doForSubst( var, varlen, subst, cmd, exec );

            if( execLine( exec ) != RET_SUCCESS ) {
                FreeSafe( exec );
                busy = false;
                /* abandon remaining file entries */
                DoWildCardClose();
                return( RET_ERROR );
            }
        }

        set = SkipWS( set + 1 );
        if( *set == NULLCHAR ) {    /* if ws at end of set */
            hold = NULLCHAR;
        }
    }

    FreeSafe( exec );
    busy = false;
    return( RET_SUCCESS );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


#if defined( __OS2__ ) || defined( __NT__ ) || defined( __UNIX__ )
STATIC RET_T handleCD( char *cmd )
/********************************/
{
    char const  *p;     // pointer to walk with
    char const  *s;

#ifdef DEVELOPMENT
    PrtMsg( DBG | INF | INTERPRETING, dosInternals[COM_CD] );
#endif

    closeCurrentFile();
    p = cmd;
    while( cisalpha( *p ) ) {
        ++p;     /* advance past command name */
    }

    p = SkipWS( (char *)p );
    if( *p == NULLCHAR ) {          /* no args - just print the cd */
        return( mySystem( cmd, cmd ) );
    }

    if( p[1] == ':' ) {             /* just a drive: arg, print the cd */
        s = SkipWS( (char *)p + 2 );
        if( *s == NULLCHAR ) {
            return( mySystem( cmd, cmd ) );
        }
    }

    // handle long filenames
    RemoveDoubleQuotes( (char *)p, strlen( p ) + 1, p );

    if( chdir( p ) != 0 ) {         /* an error changing path */
        PrtMsg( ERR | CHANGING_DIR, p );
        return( RET_ERROR );
    }
    return( RET_SUCCESS );
}


#if defined( __OS2__ ) || defined( __NT__ )
STATIC RET_T handleChangeDrive( const char *cmd )
/***********************************************/
{
    unsigned    drive_index;
    unsigned    total;
    unsigned    curr_drive;

#ifdef DEVELOPMENT
    PrtMsg( DBG | INF | INTERPRETING, dosInternals[CNUM] );
#endif

    drive_index = (unsigned)(ctoupper( *cmd ) - 'A' + 1);
    if( drive_index == 0 || drive_index > 26 ) {
        return( RET_ERROR );
    }
    _dos_setdrive( drive_index, &total );
    _dos_getdrive( &curr_drive );
    if( curr_drive != drive_index ) {
        return( RET_ERROR );
    }
    return( RET_SUCCESS );
}
#endif
#endif


STATIC RET_T handleRMSyntaxError( void )
/**************************************/
{
    PrtMsg( ERR | SYNTAX_ERROR_IN, dosInternals[COM_RM] );
    return( RET_ERROR );
}

STATIC RET_T getRMArgs( char *line, rm_flags *flags, const char **pfile )
/************************************************************************
 * returns RET_WARN when there are no more arguments
 */
{
    static char *p  = NULL;

                            /* first run? */
    if( line ) {
        flags->bForce   = false;
        flags->bDirs    = false;
        flags->bVerbose = false;

        p = SkipWS( line + 2 ); /* find first non-ws after "RM" */

                                /* is it a switch? */
        while( p[0] == '-' ) {
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
            p = SkipWS( p );
        }
    }

    if( p != NULL && *p != NULLCHAR ) {
        *pfile = p;
        p = FindNextWS(p);
        if( *p == NULLCHAR ) {
            p = NULL;
        } else {
            *p++ = NULLCHAR;
        }
    } else {
        return( RET_WARN );
    }

    return( RET_SUCCESS );
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
        rc = unlink( name );
    }
    if( rc != 0 && flags->bForce && errno == EACCES ) {
        rc = chmod( name, PMODE_RW );
        if( rc == 0 ) {
            if( dir ) {
                rc = rmdir( name );
            } else {
                rc = unlink( name );
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

static int IsDotOrDotDot( const char *fname )
{
    /* return 1 if fname is "." or "..", 0 otherwise */
    return( fname[0] == '.' && ( fname[1] == NULLCHAR || ( fname[1] == '.' && fname[2] == NULLCHAR ) ) );
}

static bool doRM( const char *f, const rm_flags *flags )
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
    DIR                 *d;
    struct dirent       *nd;
    bool                rc = true;

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
    *fpathend = NULLCHAR;
#ifdef __UNIX__
    memcpy( fname, f + j, len - j + 1 );
#else
    if( strcmp( f + j, MASK_ALL_ITEMS ) == 0 ) {
        fname[0] = '*';
        fname[1] = NULLCHAR;
    } else {
        memcpy( fname, f + j, len - j + 1 );
    }
#endif
    d = opendir( fpath );
    if( d == NULL ) {
//        Log( false, "File (%s) not found.\n", f );
        return( true );
    }

    while( ( nd = readdir( d ) ) != NULL ) {
#ifdef __UNIX__
        struct stat buf;

        if( fnmatch( fname, nd->d_name, FNM_PATHNAME | FNM_NOESCAPE ) == FNM_NOMATCH )
#else
        if( fnmatch( fname, nd->d_name, FNM_PATHNAME | FNM_NOESCAPE | FNM_IGNORECASE ) == FNM_NOMATCH )
#endif
            continue;
        /* set up file name, then try to delete it */
        len = strlen( nd->d_name );
        memcpy( fpathend, nd->d_name, len );
        fpathend[len] = NULLCHAR;
        len += i + 1;
#ifdef __UNIX__
        stat( fpath, &buf );
        if( S_ISDIR( buf.st_mode ) ) {
#else
        if( nd->d_attr & _A_SUBDIR ) {
#endif
            /* process a directory */
            if( IsDotOrDotDot( nd->d_name ) )
                continue;

            if( flags->bDirs ) {
                /* build directory list */
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
#ifdef __UNIX__
        } else if( access( fpath, W_OK ) == -1 && errno == EACCES && !flags->bDirs ) {
#else
        } else if( (nd->d_attr & _A_RDONLY) && !flags->bDirs ) {
#endif
//            Log( false, "%s is read-only, use -f\n", fpath );
//            retval = EACCES;
            rc = false;
        } else {
            if( !remove_item( fpath, flags, false ) ) {
                rc = false;
            }
        }
    }
    closedir( d );
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
    strcat( fname, "/" MASK_ALL_ITEMS );
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
        } else {
            struct stat buf;
            if( stat( name, &buf ) == 0 ) {
                if( S_ISDIR( buf.st_mode ) ) {
                    return( RecursiveRM( name, flags ) );
                } else {
                    return( remove_item( name, flags, false ) );
                }
            }
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
 * RM [-f -r -v] <file>|<dir> ...
 *
 * -f   Force deletion of read-only files.
 * -r   Recursive deletion of directories.
 * -v   Verbose operation.
 */
{
    rm_flags    flags;
    RET_T       rt;
    const char  *pfname;

#ifdef DEVELOPMENT
    PrtMsg( DBG | INF | INTERPRETING, dosInternals[COM_RM] );
#endif

    if( Glob.noexec )
        return RET_SUCCESS;
        
    for( rt = getRMArgs( cmd, &flags, &pfname ); 
        rt == RET_SUCCESS;
        rt = getRMArgs( NULL, NULL, &pfname ) ) 
    {
        RemoveDoubleQuotes( (char *)pfname, strlen( pfname ) + 1, pfname );
        if( !processRM( pfname, &flags ) ) {
            return( RET_ERROR );
        }
    }

    if( rt == RET_WARN ) {
        rt = RET_SUCCESS;
    }

    return( rt );
}

STATIC bool hasMetas( const char *cmd )
/**************************************
 * determine whether a command line has meta characters in it or not
 */
{
#if defined( __DOS__ ) || defined( __NT__ )
    const char  *p;
    bool        quoted;

    quoted = false;
    for( p = cmd; *p != NULLCHAR; ++p ) {
        if( *p == '"' ) {
            quoted = !quoted;
        } else if( !quoted && strchr( SHELL_METAS, *p ) != NULL ) {
            return( true );
        }
    }
    return( false );

#elif defined( __OS2__ ) || defined( __UNIX__ )
    const char  *p;

    for( p = cmd; *p != NULLCHAR; ++p ) {
        if( *p == SHELL_ESC && p[1] != NULLCHAR ) {
            ++p;
        } else if( strchr( SHELL_METAS, *p ) != NULL ) {
            return( true );
        }
    }
    return( false );

#endif
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
    char        buf[20];    /* "WMAKExxxxx=" + NULLCHAR = 11 + room for FmtStr */
    size_t      len;
    ENV_TRACKER *env;

    tmp = 1;
    for( ;; ) {
        FmtStr( buf, "WMAKE%d", tmp );
        if( getenv( buf ) == NULL ) {
            break;
        }
        ++tmp;
    }
    len = strlen( arg );
    if( len < 13 ) {     /* need room for " @WMAKExxxxx" */
        return( 0 );
    }
                        /* "WMAKExxxxx=" + arg + NULLCHAR */
    env = MallocSafe( sizeof( ENV_TRACKER ) + len + 12 );
    FmtStr( env->value, "WMAKE%d=%s", tmp, arg );
    if( PutEnvSafe( env ) != 0 ) {
        return( 0 );
    }
    FmtStr( arg, " @WMAKE%d", tmp );
    return( tmp );
}

STATIC void killTmpEnv( UINT16 tmp )
/**********************************/
{
    ENV_TRACKER *env;

    if( tmp == 0 ) {
        return;
    }
    env = MallocSafe( sizeof( ENV_TRACKER ) + 20 );
    FmtStr( env->value, "WMAKE%d=", tmp );
    PutEnvSafe( env );
}
#else
STATIC UINT16 makeTmpEnv( const char *cmd )
/*****************************************/
{
    (void)cmd; // Unused
    return( 0 );
}

STATIC void killTmpEnv( UINT16 tmp )
/**********************************/
{
    (void)tmp; // Unused
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
    int         quote;              // true if inside quotes

    assert( cmd != NULL );

    percent_cmd = cmd[0] == '%';
    arg = cmd + (percent_cmd ? 1 : 0);      /* split cmd name from args */

    quote = 0;                              /* no quotes yet */
    while( !((cisws( *arg ) || *arg == Glob.swchar || *arg == '+' ||
        *arg == '=' ) && !quote) && *arg != NULLCHAR ) {
        if( *arg == '\"' ) {
            quote = !quote;  /* found a quote */
        }
        ++arg;
    }
    if( arg - cmd >= _MAX_PATH ) {
        PrtMsg( ERR | COMMAND_TOO_LONG );
        return( RET_ERROR );
    }
    if( quote ) {
        /* closing quote is missing */
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
        char    ext[_MAX_EXT];

        _splitpath( cmdname, NULL, NULL, NULL, ext );
        if( ext[0] == '.' ) {
            FixName( ext );
            /* if extension specified let the shell handle it (26-apr-91) */
            if( !FNameEq( ext, ".exe" ) && !FNameEq( ext, ".com" ) ) {
                flags |= FLAG_SHELL; /* .bat and .cmd need the shell anyway */
            }
        }
    }
#endif
    comnum = findInternal( cmdname );
    if( (flags & FLAG_SILENT) == 0 ||
        (Glob.noexec && (comnum != COM_FOR || (flags & FLAG_SHELL)) &&
        !percent_cmd) ) {
        if( !Glob.noheader && !Glob.compat_posix ) {
            PrtMsg( INF | NEOL | JUST_A_TAB );
        }
        dumpCommand( cmd );
    }
    if( percent_cmd ) {
        return( percentCmd( cmdname, arg ) );
    }
    /*
     * The SET command must be handled locally to have any effect. Consider
     * `Path=C:\Program Files (x86)\foo' which will be detected as containing
     * metacharacters. Stupid Microsoft...
     */
    if( hasMetas( cmd ) && comnum != COM_SET && comnum != COM_FOR ) {
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
#if defined( __OS2__ ) || defined( __NT__ ) || defined( __UNIX__ )
        case COM_CD:    /* fall through */
        case COM_CHDIR: my_ret = handleCD( cmd );           break;
#endif
#if defined( __OS2__ ) || defined( __NT__ )
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
#ifdef __UNIX__  /* For UNIX we must for now use system since
                    without splitting argv[1] the spawnvp below
                    does not always work */
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


STATIC RET_T execLine( char *line )
/**********************************
 * is allowed to hack up line any way it feels
 */
{
    char        *p;
    shell_flags flags;
    RET_T       rc;

    assert( line != NULL );

    CheckForBreak();
    /* make a copy of global flags */
    flags = ((Glob.silent && !Glob.silentno) ? FLAG_SILENT : 0)
           | (Glob.ignore ? FLAG_IGNORE : 0)
           | (Glob.shell ? FLAG_SHELL : 0);

    p = line;               /* process @*!- and strip leading ws */
    for( ;; ) {
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
        ++p;
    }

    assert( !cisws( *p ) );

    // NMAKE quietly ignores empty commands
    if( Glob.compat_nmake && *p == NULLCHAR ) {
        return( RET_SUCCESS );
    }
    rc = shellSpawn( p, flags );
    if( OSCorrupted() ) {
        PrtMsg( FTL | OS_CORRUPTED );
        ExitFatal();
    }
    CheckForBreak();
    if( rc != RET_SUCCESS && (flags & FLAG_IGNORE) == 0 ) {
        return( RET_ERROR );
    }
    return( RET_SUCCESS );
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
    }
    CheckForBreak();

    // Errors during [cmd] execution don't count
    Glob.erroryet = old_err;

    // Report return code from shell
    return( (UINT8)rc );
}


RET_T ExecCList( CLIST *clist )
/*****************************/
{
    char        *line;
    RET_T       ret = RET_SUCCESS;
    FLIST const *currentFlist;

    assert( clist != NULL );

    for( ; clist != NULL; clist = clist->next ) {
        ret = writeInlineFiles( clist->inlineHead, &(clist->text) );
        currentFlist = clist->inlineHead;
        if( ret == RET_SUCCESS ) {
            UnGetCH( STRM_MAGIC );
            InsString( clist->text, false );
            line = DeMacro( TOK_MAGIC );
            GetCHR();        /* eat STRM_MAGIC */
            if( Glob.verbose ) {
                 ret = VerbosePrintTempFile( currentFlist );
            }
            ret = execLine( line );
            FreeSafe( line );
            if( ret != RET_SUCCESS ) {
                return( ret );
            }
        } else {
            closeCurrentFile();
            return( ret );
        }
    }
    closeCurrentFile();
    return( ret );
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
    currentFileHandle = -1;
    /* Take any number first */
    tmpFileNumber   = (UINT16)(time( NULL ) % 100000);
}


void ExecFini( void )
/*******************/
{
    // destroy all the files that will not be kept
    destroyNKList();
}
