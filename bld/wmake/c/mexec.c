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


#if !defined(__QNX__)
 #include <direct.h>
 #include <dos.h>
#endif
#include <fcntl.h>
#include <unistd.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "macros.h"
#include "make.h"
#include "massert.h"
#include "mcache.h"
#include "memory.h"
#include "mexec.h"
#include "misc.h"
#include "mparse.h"
#include "mpreproc.h"
#include "mrcmsg.h"
#include "msg.h"
#include "msuffix.h"
#include "mtarget.h"
#include "mtypes.h"
#include "mupdate.h"
#include "mvecstr.h"

#include "msysdep.h"
#ifdef DLLS_IMPLEMENTED
#include <idedrv.h>
#endif
STATIC  UINT8           lastErrorLevel;
STATIC  UINT16          tmpFileNumber;          /* temp file number         */
STATIC  char            tmpFileChar  ;          /* temp file number chari   */
STATIC  int             currentFileHandle;      /* %write, %append, %create */
STATIC  char            *currentFileName;

enum {
    FLAG_SHELL          = 0x01,
    FLAG_SILENT         = 0x02,
    FLAG_ENV_ARGS       = 0x04,
    FLAG_IGNORE         = 0x08
};

#define COM_MAX_LEN     16              /* must be able to hold any OS cmdname */

STATIC const char *const dosInternals[] = {   /* COMMAND.COM commands */

#if defined( __WINDOWS__ )
    "BREAK",            /* this list must be in alpha order */
    "CALL",
    "CD",
    "CHDIR",
    "CLS",
    "COMMAND",
    "COPY",
    "CTTY",
    "DATE",
    "DEL",
    "DIR",
    "ECHO",
    "ERASE",
    "FOR",
#define COM_FOR     13  /* index of the for keyword */
    "IF",
#define COM_IF      14  /* index of the if keyword  */
    "MD",
    "MKDIR",
    "PATH",
    "PAUSE",
    "PROMPT",
    "RD",
    "REM",
    "REN",
    "RENAME",
    "RMDIR",
    "SET",
#define COM_SET     25  /* index of the set keyword */
#define LEN_SET     3   /* strlen( "SET" ) */
    "TIME",
    "TYPE",
    "VER",
    "VERIFY",
    "VOL"
#elif   defined( __DOS__ )

    "BREAK",            /* this list must be in alpha order */
    "CALL",
    "CD",
    "CHDIR",
    "CLS",
    "COMMAND",
    "COPY",
    "CTTY",
    "DATE",
    "DEL",
    "DIR",
    "ECHO",
    "ERASE",
    "FOR",
#define COM_FOR     13  /* index of the for keyword */
    "IF",
#define COM_IF      14  /* index of the if keyword  */
    "MD",
    "MKDIR",
    "PATH",
    "PAUSE",
    "PROMPT",
    "RD",
    "REM",
    "REN",
    "RENAME",
    "RMDIR",
    "SET",
#define COM_SET     25  /* index of the set keyword */
#define LEN_SET     3   /* strlen( "SET" ) */
    "TIME",
    "TYPE",
    "VER",
    "VERIFY",
    "VOL"

#elif   defined( __OS2__ ) || defined( __NT__ )

    "BREAK",
    "CALL",
    "CD",
#define COM_CD      2
    "CHCP",
    "CHDIR",
#define COM_CHDIR   4
    "CLS",
    "COPY",
    "DATE",
    "DEL",
    "DETACH",
    "DIR",
    "DPATH",
    "ECHO",
    "ENDLOCAL",
    "ERASE",
    "EXIT",
    "EXTPROC",
    "FOR",
#define COM_FOR     17
    "GOTO",
    "IF",
#define COM_IF      19
    "MD",
    "MKDIR",
    "MOVE",
    "PATH",
    "PAUSE",
    "PROMPT",
    "RD",
    "REM",
    "REN",
    "RENAME",
    "RMDIR",
    "SET",
#define COM_SET     31
#define LEN_SET     3
    "SETLOCAL",
    "SHIFT",
    "START",
    "TIME",
    "TYPE",
    "VER",
    "VERIFY",
    "VOL"

#elif   defined( __QNX__ )

    "BREAK",
    "CALL",
    "CD",
#define COM_CD      2
    "CHCP",
    "CHDIR",
#define COM_CHDIR   4
    "CLS",
    "COPY",
    "DATE",
    "DEL",
    "DETACH",
    "DIR",
    "DPATH",
    "ECHO",
    "ENDLOCAL",
    "ERASE",
    "EXIT",
    "EXTPROC",
    "FOR",
#define COM_FOR     17
    "GOTO",
    "IF",
#define COM_IF      19
    "MD",
    "MKDIR",
    "PATH",
    "PAUSE",
    "PROMPT",
    "RD",
    "REM",
    "REN",
    "RENAME",
    "RMDIR",
    "SET",
#define COM_SET     30
#define LEN_SET     3
    "SETLOCAL",
    "SHIFT",
    "START",
    "TIME",
    "TYPE",
    "VER",
    "VERIFY",
    "VOL"

#endif
};

#define CNUM    ( sizeof( dosInternals ) / sizeof( char * ) )


static const char * const percentCmds[] = {
    "ABORT",
    "APPEND",
    "CREATE",
    "MAKE",
    "NULL",
    "QUIT",
    "STOP",
    "WRITE",
};

#define PNUM    ( sizeof( percentCmds ) / sizeof( char * ) )

enum {
    PER_ABORT,
    PER_APPEND,
    PER_CREATE,
    PER_MAKE,
    PER_NULL,
    PER_QUIT,
    PER_STOP,
    PER_WRITE
};

enum write_type {
    WR_WRITE,
    WR_APPEND,
    WR_CREATE
};


STATIC RET_T execLine( char *line );    /* called recursively in handleFor */

STATIC NKLIST* noKeepList;              /* contains the list of files that
                                           needs to be cleaned when wmake
                                           exits */

STATIC char *createTmpFileName ( void )
/*
 * create file name for temporary file
 */
{
    VECSTR  buf;
    VECSTR  buf2;
    char*   result;
    char*   tmpPath;
    char    fileName[_MAX_PATH];

    tmpPath    = GetMacroValue(TEMPENVVAR);
    if (tmpPath == NULL  && !Glob.microsoft) {
        tmpPath = getenv(TEMPENVVAR);
        if (tmpPath != NULL) {
            tmpPath = StrDupSafe(tmpPath);
        }
    }

    if (tmpPath == NULL) {
        tmpPath = StrDupSafe("");
    }

    for (;;) {
        tmpFileChar = tmpFileNumber % 26 + 'a' ;
        buf = StartVec();
        FmtStr(fileName, "wm%c%u.tmp",tmpFileChar,tmpFileNumber);
        if (tmpPath != NULL) {
            if (strlen(tmpPath) >= _MAX_PATH) {
                PrtMsg(ERR|FTL|TMP_PATH_TOO_LONG);
                FreeVec(buf);
                FreeSafe(tmpPath);
                return (NULL);
            } else if (strlen(tmpPath) + strlen(fileName) >= _MAX_PATH) {
                PrtMsg(ERR|FTL|TMP_PATH_TOO_LONG);
                FreeSafe(tmpPath);
                FreeVec(buf);
                return (NULL);
            }
        }
        if (tmpPath == NULL ) {
            WriteVec(buf,fileName);
            result = FinishVec(buf);
        } else {
            WriteVec(buf,tmpPath);
            if (tmpPath[strlen(tmpPath)-1] != BACKSLASH) {
                buf2 = StartVec();
                WriteVec(buf2,"\\");
                CatVec(buf,buf2);
            }
            buf2 = StartVec();
            WriteVec(buf2,fileName);
            CatVec(buf,buf2);
            result = FinishVec(buf);
        }


        if (!existFile(result)) {
            /* touch the file */
            TouchFile(result);
            FreeSafe(tmpPath);
            return (result);

        } else {
            FreeSafe(result);
        }
        tmpFileNumber = (tmpFileNumber + time (NULL) ) % 100000;
    }

}


STATIC RET_T processInlineFile (int handle, char* body, char* fileName,
                                BOOLEAN writeToFile) {
    int    index;
    RET_T  ret  ;
    char*  DeMacroBody;
    int    currentSent;
    BOOLEAN  firstTime;
    VECSTR   outText;

    firstTime = TRUE;
    index       = 0;
    currentSent = 0;
    ret         = RET_SUCCESS;

    assert(body != NULL);

    // we will push the whole body back into the stream to be fully
    // deMacroed
    while (body [index] != NULLCHAR ) {
        if (body[index] == EOL) {
            InsString(body+currentSent, FALSE );
            DeMacroBody = ignoreWSDeMacro( FALSE, ForceDeMacro() );
            currentSent = index + 1;
            if (writeToFile) {
                if (strlen(DeMacroBody) !=
                        write(handle,DeMacroBody,strlen(DeMacroBody))) {
                    ret = RET_ERROR;
                }
                if (body[index+1] != NULLCHAR) {
                    if (write(handle,"\n",1) != 1) {
                        ret = RET_ERROR;
                    }
                }
            } else {
                if( !Glob.noheader ) {
                    PrtMsg( INF|NEOL| JUST_A_TAB );
                }
                outText = StartVec();
                WriteVec(outText,"echo.");
                if (DeMacroBody != NULL) {
                    if (strlen(DeMacroBody) > 0) {
                        WriteVec(outText,DeMacroBody);
                    }
                    FreeSafe (DeMacroBody);
                }
                if (firstTime == TRUE) {
                    WriteVec(outText," > ");
                    firstTime = FALSE;
                } else {
                    WriteVec(outText," >> ");
                }
                WriteVec(outText, fileName);
                DeMacroBody = FinishVec(outText);
                PrtMsg( INF|PRNTSTR,DeMacroBody);
            }
            FreeSafe(DeMacroBody);
        }
        index++;
    }
    return (ret);
}

STATIC RET_T writeLineByLine(int handle, char* body) {
    return (processInlineFile(handle, body, NULL, TRUE));
}


STATIC char* RemoveBackSlash ( const char* inString ) {
/************************************
 * remove backslash from \"
 */

    char    buffer[_MAX_PATH];
    char    *current;
    int     pos;

    assert ( inString != NULL);
    current = (char*) inString;
    pos = 0;

    while (*current != NULLCHAR &&
           pos < _MAX_PATH - 1) {
        if (*current == BACKSLASH) {
            if (*(current+1) == DOUBLEQUOTE) {
                buffer[pos++] = DOUBLEQUOTE;
                current = current + 2;
                continue;
            }
        }
        buffer[pos++] = *(current ++);

    }
    buffer[pos] = NULLCHAR;

    return (StrDupSafe(buffer));

}


STATIC RET_T VerbosePrintTempFile(FLIST *head) {

    FLIST* current;
    RET_T  ret;

    current = head;
    while (current != NULL) {
        assert(current->fileName != NULL);
        ret = processInlineFile(0,current->body,current->fileName,FALSE);
        current = current->next;

    }
    return (ret);
}

STATIC RET_T createFile (FLIST *head) {
/***********************************
 * create file given information in the FLIST
 */

    NKLIST *temp;
    int    handle;
    char   *fileName;
    char   *tmpFileName;
    RET_T  ret;

    assert (head != NULL);
    ret = RET_SUCCESS;

    if (head->fileName != NULL) {
        /* Push the filename back into the stream
         * and then get it back out using DeMacro to fully DeMacro
         */
        UnGetCH( STRM_MAGIC );
        InsString( head->fileName, FALSE );
        fileName = DeMacro( STRM_MAGIC );
        GetCHR();           /* eat STRM_MAGIC */
    } else {
        ret = RET_ERROR;
    }

    if (head != NULL && ret != RET_ERROR) {
        tmpFileName = RemoveBackSlash (fileName);
        handle = open (tmpFileName,O_TEXT| O_WRONLY|O_CREAT|O_TRUNC|S_IRUSR|
                       S_IWUSR|S_IRGRP|S_IWGRP);
        if (handle != -1) {
            if (writeLineByLine(handle,head->body) == RET_ERROR) {
                PrtMsg( ERR| ERROR_WRITING_FILE, tmpFileName );
                ret = RET_ERROR;
            }
            if (close(handle) != -1) {
                if (head->keep == FALSE) {
                    temp = NewNKList();
                    temp->fileName = StrDupSafe (tmpFileName);
                    temp->next     = noKeepList;
                    noKeepList     = temp;
                }
            } else {
                PrtMsg( ERR| ERROR_CLOSING_FILE, tmpFileName );
                ret = RET_ERROR;
            }

        } else {
            PrtMsg( ERR| ERROR_OPENING_FILE, tmpFileName );
            ret = RET_ERROR;
        }
    }
    FreeSafe ( fileName );
    FreeSafe (tmpFileName);
    return (ret);
}


// This part writes the inline files
// modifies the command text to show the temporary file names
// assumption is that all << are removed for explicitly defined
// file names so the only << left are for temporary files
STATIC RET_T writeInlineFiles (FLIST *head, char** commandIn) {

    char  *cmdText;
    FLIST *current;
    RET_T  ret;
    VECSTR newCommand;
    int   start; // start of cmdText to be copied into newCommand;
    int   index; // current index of cmdText
    NKLIST *temp;

    assert(*commandIn != NULL);

    cmdText    = *commandIn;
    ret        = RET_SUCCESS;
    newCommand = StartVec();
    WriteVec(newCommand,"");
    index      = 0;
    start      = index;
    current    = head;

    while (current        != NULL        &&
           ret            == RET_SUCCESS &&
           cmdText[index] != NULLCHAR) {

        // if the filename is the inline symbol then we need change
        // the filename into a temp filename
        if (strcmp(current->fileName,INLINE_SYMBOL) == 0) {
            while (1) {
                if (cmdText[index] == LESSTHAN) {
                    if (cmdText[index+1] == LESSTHAN) {
                        index += 2;
                        break;
                    }
                } else if (cmdText[index] == NULLCHAR) {
                    /* not possible to come here*/
                    ret = RET_ERROR;
                    break;
                }
                ++index;

            }
            if (ret == RET_ERROR) {
                break;
            }
            CatNStrToVec(newCommand,cmdText+start,index-start-2);
            start = index;
            FreeSafe(current->fileName);
            current->fileName = createTmpFileName();

            CatStrToVec(newCommand,current->fileName);
        }
        if (!Glob.noexec) {
            ret = createFile(current);
        } else {
            if (current->keep == FALSE) {
                temp = NewNKList();
                temp->fileName = StrDupSafe (current->fileName);
                temp->next     = noKeepList;
                noKeepList     = temp;
            }
        }
        current = current->next;
    }
    CatNStrToVec(newCommand,cmdText+start,strlen(cmdText) - start);
    FreeSafe(cmdText);
    *commandIn = FinishVec(newCommand);
    return (ret);
}


STATIC int findInternal( const char *cmd )
/*****************************************
 * check cmd for command.com command, return index if it is
 * return CNUM if is of form x:
 * otherwise return -1
 * expects cmd to be just the command - ie: no args
 */
{
    char **key;
    size_t len;
    auto char buff[ COM_MAX_LEN ];

    assert( cmd != NULL );
    /* test if of form x: */
    if( isalpha( *cmd ) && cmd[1] == ':' && cmd[2] == NULLCHAR ) {
        return( CNUM );
    }
    for(;;) {
        key = bsearch( &cmd, dosInternals, CNUM, sizeof( char * ),
               (int (*)(const void*, const void*)) KWCompare );
        if( key != NULL ) break;
        len = strlen( cmd );
        if( len > 1 && len < COM_MAX_LEN ) {
            if( cmd[len-1] == '.' ) {
                // should work if buff == cmd (i.e., cd..)
                strcpy( buff, cmd );
                buff[len-1] = '\0';
                cmd = buff;
                continue;
            }
        }
        return( -1 );
    }
    return( key - (char **)dosInternals );
}


STATIC RET_T percentMake( char *arg )
/************************************
 * do a recursive make of the target in arg
 */
{
    TARGET *calltarg;
    RET_T ret;
    char *buf;
    char *start, *finish;
    BOOLEAN newtarg;
    BOOLEAN more_targets;

    /* %make <target> <target> ... */
    buf = MallocSafe( _MAX_PATH );

    ret = RET_ERROR;
    start = arg;
    for(;;) {
        start = SkipWS( start );
        if( *start == NULLCHAR ) break;
        more_targets = FALSE;
        finish = start;
        for(;;) {
            if( *finish == NULLCHAR ) break;
            if( isws( *finish ) ) {
                more_targets = TRUE;
                *finish = NULLCHAR;
                break;
            }
            ++finish;
        }

        /* try to find this file on path or in targets */
        ret = TrySufPath( buf, start, &calltarg, FALSE );

        newtarg = FALSE;
        if( ( ret == RET_SUCCESS && calltarg == NULL ) || ret == RET_ERROR ) {
            /* Either file doesn't exist, or it exists and we don't already
             * have a target for it.  Either way, we create a new target.
             */
            calltarg = NewTarget( buf );
            newtarg = TRUE;
        }
        ret = Update( calltarg );
        if( newtarg && Glob.noexec == FALSE ) {
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


STATIC void closeCurrentFile( void ) {
/************************************/

    if( currentFileHandle != -1 ) {
        close( currentFileHandle );
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
    char        *text;
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

    if (*p != DOUBLEQUOTE) {
        while( isfilec( *p ) ) ++p;
    } else {
        ++p;    // Skip the first quote
        ++fn;
        while (*p!= DOUBLEQUOTE && *p!= NULLCHAR) {
             ++p;
        }
        if (*p!= NULLCHAR) {
            *p = NULLCHAR;
            p++;
        }
    }

    if( *p != NULLCHAR ) {
        if( !isws( *p ) ) {
            switch( type ) {
            case WR_APPEND:
                cmd_name = percentCmds[ PER_APPEND ];
                break;
            case WR_CREATE:
                cmd_name = percentCmds[ PER_CREATE ];
                break;
            case WR_WRITE:
                cmd_name = percentCmds[ PER_WRITE ];
                break;
            default:
                cmd_name = "?";
                break;
            }
            PrtMsg( ERR| SYNTAX_ERROR_IN, cmd_name );
            closeCurrentFile();
            return( RET_ERROR );
        }
        *p = '\0';          /* terminate file name */
        ++p;
        text = p;           /* set text pointer */
        p += strlen( p );   /* find null terminator */
    } else {
        *p = '\0';          /* terminate file name */
        text = p;           /* set text pointer */
    }

    /* now text points to the beginning of string to write, and p points to
     * the end of the string.  fn points to the name of the file to write to
     */
    FixName( fn );
    if( type == WR_CREATE || currentFileName == NULL ||
                                    FNameCmp( currentFileName, fn ) != 0 ) {
        closeCurrentFile();
        currentFileName = StrDupSafe( fn );
        open_flags = O_WRONLY | O_CREAT | O_TEXT;
        if( type == WR_APPEND ) {
            open_flags |= O_APPEND;
        } else {
            open_flags |= O_TRUNC;
        }

        currentFileHandle = open( fn, open_flags, S_IWRITE | S_IREAD );
        if( currentFileHandle == -1 ) {
            PrtMsg( ERR| OPENING_FOR_WRITE, fn );
            closeCurrentFile();
            return( RET_ERROR );
        }
    }

    if( type != WR_CREATE ) {
        *p = '\n';          /* replace null terminator with newline */
        len = ( p - text ) + 1;
        if( write( currentFileHandle, text, len ) != len ) {
            PrtMsg( ERR| DOING_THE_WRITE );
            closeCurrentFile();
            return( RET_ERROR );
        }
    }

    CacheRelease();     /* so that the cache is updated */

    return( RET_SUCCESS );
}


STATIC RET_T percentCmd( char *cmdname, char *arg )
/**************************************************
 * handle our special percent commands
 */
{
    char    **key;
    char    *ptr;
    int     num;

    assert( cmdname != NULL && arg != NULL );

    ptr = cmdname + 1;
    key = bsearch( &ptr, percentCmds, PNUM, sizeof( char * ),
          (int (*) (const void*, const void*)) KWCompare );

    if( key == NULL ) {
        PrtMsg( ERR| UNKNOWN_PERCENT_CMD );
        closeCurrentFile();
        return( RET_ERROR );
    } else {
        num = key - (char **)percentCmds;
    }

    if( Glob.noexec && num != PER_MAKE ) {
        return( RET_SUCCESS );
    }

    switch( num ) {
    case PER_ABORT:
        closeCurrentFile();
        ExitSafe( EXIT_ERROR );
        break;

    case PER_APPEND:
        return( percentWrite( arg, WR_APPEND ) );
        break;

    case PER_CREATE:
        return( percentWrite( arg, WR_CREATE ) );
        break;

    case PER_MAKE:
        return( percentMake( arg ) );
        break;

    case PER_NULL:
        break;

    case PER_QUIT:
        closeCurrentFile();
        ExitSafe( EXIT_OK );
        break;

    case PER_STOP:
        closeCurrentFile();
        if( !GetYes( DO_YOU_WISH_TO_CONT ) ) {
            ExitSafe( EXIT_OK );
        }
        break;

    case PER_WRITE:
        return( percentWrite( arg, WR_WRITE ) );
        break;

    default:
        assert( FALSE );
        break;
    };

    return( RET_SUCCESS );
}


STATIC RET_T mySystem( const char *cmdname, char *cmd )
/******************************************************
 * execute a command using system()
 */
{
    int retcode;

    assert( cmd != NULL );

    if( Glob.noexec ) {
        return( RET_SUCCESS );
    }

    closeCurrentFile();
    retcode = system( cmd );
    lastErrorLevel = retcode;
    if( retcode < 0 ) {
        PrtMsg( ERR| UNABLE_TO_EXEC, cmdname );
    }
    if( retcode != 0 ) {
        return( RET_ERROR );
    }
    return( RET_SUCCESS );
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
    PrtMsg( DBG|INF| INTERPRETING, dosInternals[ COM_SET ] );
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
    while( *p != NULLCHAR && !isws( *p ) && *p != '=' ) {
        ++p;
    }
    endname = p;

    p = SkipWS( p );            /* trim ws after name */

    if( *p != '=' || endname == name ) {
        PrtMsg( ERR| SYNTAX_ERROR_IN, dosInternals[ COM_SET ] );
        return( RET_ERROR );
    }

    *endname = NULLCHAR;        /* null terminate name */

    ++p;                        /* advance to character after '=' */

                        /* +1 for '=' (already +1 for '\0' in ENV_TRACKER) */
    env = MallocSafe( sizeof( ENV_TRACKER )+ 1 + (endname - name) + strlen(p) );
    FmtStr( env->value, "%s=%s", name, p );
    retcode = PutEnvSafe( env );
    if( retcode != 0 ) {
        return( RET_ERROR );
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
    BOOLEAN     not;        /* flag for not keyword                     */
    BOOLEAN     condition;  /* whether the condition was T or F         */
    char        *p;         /* used to scan the string                  */
    char        *tmp1;      /* one of NOT | ERRORLEVEL | <str1> | EXIST */
    char        *tmp2;      /* one of <number> | "==" | <file> | <str2> */
    char        *end1;      /* location of end of tmp1 string           */
    char        save;       /* save character                           */
    const char  *file;      /* for checking file existence              */

    assert( cmd != NULL );

#ifdef DEVELOPMENT
    PrtMsg( DBG|INF| INTERPRETING, dosInternals[ COM_IF ] );
#endif

    if( Glob.noexec ) {
        return( RET_SUCCESS );
    }
    closeCurrentFile();

    p = SkipWS( cmd + 2 );      /* find first non-ws after "IF" */
    if( *p == NULLCHAR ) {
        PrtMsg( ERR| SYNTAX_ERROR_IN, dosInternals[ COM_IF ] );
        return( RET_ERROR );
    }

    tmp1 = p;                   /* find first word after IF */
    while( !isws( *p ) && *p != NULLCHAR && *p != '=' ) ++p;
    if( *p == NULLCHAR ) {
        PrtMsg( ERR| SYNTAX_ERROR_IN, dosInternals[ COM_IF ] );
        return( RET_ERROR );
    }

    save = *p;
    *p = NULLCHAR;
    not = stricmp( tmp1, "NOT" ) == 0;
    *p = save;

    if( not ) {             /* discard the "NOT" get next word */
        tmp1 = p = SkipWS( p );
        while( !isws( *p ) && *p != NULLCHAR && *p != '=' ) ++p;
        if( *p == NULLCHAR ) {
            PrtMsg( ERR| SYNTAX_ERROR_IN, dosInternals[ COM_IF ] );
            return( RET_ERROR );
        }
        save = *p;
    }
    end1 = p;

    tmp2 = p = SkipWS( p );
    if( *p == NULLCHAR ) {
        PrtMsg( ERR| SYNTAX_ERROR_IN, dosInternals[ COM_IF ] );
        return( RET_ERROR );
    }
    while( !isws( *p ) && *p != NULLCHAR ) ++p;
    if( *p == NULLCHAR ) {
        PrtMsg( ERR| SYNTAX_ERROR_IN, dosInternals[ COM_IF ] );
        return( RET_ERROR );
    }

    *end1 = NULLCHAR;
    if( stricmp( tmp1, "ERRORLEVEL" ) == 0 ) {
        *p = NULLCHAR;
        condition = lastErrorLevel >= atoi( tmp2 );
    } else if( stricmp( tmp1, "EXIST" ) == 0 ) {
        *p = NULLCHAR;
        file = DoWildCard( tmp2 );
        condition = file != NULL && CacheExists( file );
        if( condition ) {
            while( DoWildCard( NULL ) != NULL )
                ;           /* eat rest of entries */
        }
    } else {
        *end1 = save;
        p = end1;           /* back up to end of 1st token */
        for(;;) {
            while( *p != NULLCHAR && *p != '=' ) ++p;
            if( *p == NULLCHAR ) {
                PrtMsg( ERR| SYNTAX_ERROR_IN, dosInternals[ COM_IF ] );
                return( RET_ERROR );
            }
            if( p[1] == '=' ) break;
            ++p;
        }
                            /* we have found "==", get <str2> */
        tmp2 = p = SkipWS( p + 2 );
        while( !isws( *p ) && *p != NULLCHAR ) ++p;
        if( *p == NULLCHAR ) {
            PrtMsg( ERR| SYNTAX_ERROR_IN, dosInternals[ COM_IF ] );
            return( RET_ERROR );
        }
        *p = NULLCHAR;
        *end1 = NULLCHAR;       /* null-terminate tmp1 again */
        condition = strcmp( tmp1, tmp2 ) == 0;
    }

    p = SkipWS( p + 1 );
    if( *p == NULLCHAR ) {
        PrtMsg( ERR| SYNTAX_ERROR_IN, dosInternals[ COM_IF ] );
        return( RET_ERROR );
    }

    if( ( not && !condition ) || ( !not && condition ) ) {
        return( execLine( p ) );
    }
    return( RET_SUCCESS );
}


STATIC RET_T handleForSyntaxError( void )
/***************************************/
{
    PrtMsg( ERR| SYNTAX_ERROR_IN, dosInternals[ COM_FOR ] );
    return( RET_ERROR );
}


STATIC RET_T getForArgs( char *line, const char **pvar, char **pset,
    const char **pcmd )
/************************************************************************/
{
    char    *p;

    assert( line != NULL && pvar != NULL && pset != NULL && pcmd != NULL );

    /* remember we can hack up line all we like... */

    p = SkipWS( line + 3 ); /* find first non-ws after "FOR" */

                            /* got <var>, now test if legal */
    if( p[0] != '%' ) {
        return( handleForSyntaxError() );
    }
    if( ( p[1] == '%' && !isalpha( p[2] ) ) ||
        ( p[1] != '%' && !isalpha( p[1] ) ) ) {
        return( handleForSyntaxError() );
    }
    *pvar = (const char *)p;

                            /* move to end of <var> */
    while( isalpha( *p ) || *p == '%' ) ++p;

    if( *p == NULLCHAR ) {  /* premature eol? */
        return( handleForSyntaxError() );
    }

    *p = NULLCHAR;          /* truncate variable name */

    p = SkipWS( p + 1 );    /* move to "in" */

    if( toupper( p[0] ) != 'I' || toupper( p[1] ) != 'N' || !isws( p[2] ) ) {
        return( handleForSyntaxError() );
    }

    p = SkipWS( p + 3 );    /* move to ( before <set> */

    if( p[0] != '(' ) {
        return( handleForSyntaxError() );
    }
    ++p;
    *pset = p;/* beginning of set */

    while( *p != NULLCHAR && *p != ')' ) ++p;
    if( *p == NULLCHAR ) {
        return( handleForSyntaxError() );
    }

    *p = NULLCHAR;          /* truncate set string */

    p = SkipWS( p + 1 );    /* move to "do" */

    if( toupper( p[0] ) != 'D' || toupper( p[1] ) != 'O' || !isws( p[2] ) ) {
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
    const char *p;

    assert( str != NULL && var != NULL && *var == '%' );

    p = strchr( str, '%' );
    for(;;) {
        if( p == NULL ) {
            return( NULL );
        }
        if( strncmp( p, var, varlen ) == 0 ) {
            return( p );
        }
        p = strchr( p+1, '%' );
    }
}


STATIC void doForSubst( const char *var, size_t varlen,
    const char *subst, const char *src, char *dest )
/******************************************************
 * substitute all occurances of var in src with subst, write to dest
 * dest must be large enough
 */
{
    const char *p;

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


#pragma on (check_stack);
STATIC RET_T handleFor( char *line )
/***********************************
 * "FOR" {ws}* "%"["%"]<var> {ws}+ "IN" {ws}+ "("<set>")" {ws}+ "DO" {ws}+ <cmd>
 */
{
    static BOOLEAN  busy = FALSE;   /* recursion protection */

    const char  *var;       /* loop variable name incl. %           */
    char        *set;       /* set of values for looping            */
    const char  *cmd;       /* command to execute                   */
    const char  *p;         /* working pointer                      */
    char        hold;       /* final character of set during loop   */
    const char  *subst;     /* pointer to the element to substitute */
    size_t      varlen;     /* strlen( var )                        */
    unsigned    numsubst;   /* number of substitutions per cmd      */
    size_t      cmdlen;     /* strlen( cmd ) - numsubst * varlen    */
    size_t      newlen;     /* size of memory we need               */
    size_t      lastlen;    /* last size of memory we asked for     */
    char        *exec;      /* line to execute                      */

    assert( line != NULL );

    if( busy ) {
        PrtMsg( ERR| NO_NESTED_FOR, dosInternals[ COM_FOR ] );
        return( RET_ERROR );
    }
    busy = TRUE;

#ifdef DEVELOPMENT
    PrtMsg( DBG|INF| INTERPRETING, dosInternals[ COM_FOR ] );
#endif

    if( getForArgs( line, &var, &set, &cmd ) != RET_SUCCESS ) {
        busy = FALSE;
        return( RET_ERROR );
    }

    varlen = strlen( var );

    numsubst = 0;
    p = nextVar( cmd, var, varlen );
    while( p != NULL ) {
        ++numsubst;
        p = nextVar( p + varlen, var, varlen );
    }

    cmdlen = strlen( cmd ) - numsubst * varlen + 1;
    lastlen = 0;
    exec = NULL;

                    /* always skip ws in set */
    set = SkipWS( set );
    hold = *set;
    while( hold != NULLCHAR ) {
        subst = set;        /* remember start of subst string */
        while( *set != NULLCHAR && !isws( *set ) ) ++set;
        hold = *set;
        *set = NULLCHAR;

        subst = DoWildCard( subst );
        while( subst != NULL ) {

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
                busy = FALSE;
                while( DoWildCard( NULL ) != NULL )  /* eat remaining files */
                    ;
                return( RET_ERROR );
            }

            subst = DoWildCard( NULL );
        }

        set = SkipWS( set + 1 );
        if( *set == NULLCHAR ) {    /* if ws at end of set */
            hold = NULLCHAR;
        }
    }

    FreeSafe( exec );
    busy = FALSE;
    return( RET_SUCCESS );
}
#pragma off(check_stack);


#if     defined( __OS2__ ) || defined( __NT__ ) || defined( __QNX__ )
STATIC RET_T handleCD( char *cmd )
/********************************/
{
    char    *p;     /* pointer to walk with */
    char    *s;

    closeCurrentFile();
    p = cmd;
    while( isalpha( *p ) ) ++p;     /* advance past command name */
    p = SkipWS( p );
    if( *p == NULLCHAR ) {          /* no args - just print the cd */
        return( mySystem( cmd, cmd ) );
    }

    if( p[1] == ':' ) {             /* just a drive: arg, print the cd */
        s = SkipWS( p+2 );
        if( *s == NULLCHAR ) {
            return( mySystem( cmd, cmd ) );
        }
    }

    if( chdir( p ) != 0 ) {         /* an error changing path */
        PrtMsg( ERR| CHANGING_DIR, p );
        return( RET_ERROR );
    }
    return( RET_SUCCESS );
}


#if     defined( __OS2__ ) || defined( __NT__ )
STATIC RET_T handleChangeDrive( char *cmd )
/*****************************************/
{
    unsigned drive_index;
    unsigned total;
    unsigned curr_drive;

    drive_index = ( toupper( *cmd ) - 'A' ) + 1;
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


#if 0
STATIC RET_T handleCHDrive( char *cmd )
/*************************************/
{
    unsigned    drive;

    if( !isalpha( *cmd ) ) {
        PrtMsg( ERR| CHANGING_DRIVE, *cmd );
        return( RET_ERROR );
    }
    drive = toupper( *cmd ) - ( 'A' - 1 );
    if( DosSelectDisk( drive ) != 0 ) {
        PrtMsg( ERR| CHANGING_DRIVE, *cmd );
        return( RET_ERROR );
    }
    return( RET_SUCCESS );
}
#endif
#endif
#endif


STATIC BOOLEAN hasMetas( const char *cmd )
/*****************************************
 * determine whether a command line has meta characters in it or not
 */
{
#if     defined( __DOS__ ) || defined( __NT__ )
    return( strpbrk( cmd, SHELL_METAS ) != NULL );

#elif   defined( __OS2__ ) || defined( __QNX__ )
    const char *p;

    p = cmd;
    while( *p != NULLCHAR ) {
        if( *p == SHELL_ESC && p[1] != NULLCHAR ) {
            ++p;
        } else if( strchr( SHELL_METAS, *p ) != NULL ) {
            return( TRUE );
        }
        ++p;
    }
    return( FALSE );

#endif
}

static void dumpCommand( char *cmd )
{
    char c;
    char *p;
    char *

    // trim trailing white space before executing
    z = cmd;
    for( p = cmd; *p; ++p ) {
        if( ! isws( *p ) ) {
            z = p;
        }
    }
    ++z;
    if( z == p ) {
        PrtMsg( INF| PRNTSTR, cmd );
    } else {
        c = *z;
        *z = '\0';
        PrtMsg( INF| PRNTSTR, cmd );
        *z = c;
    }
}

#if defined( __DOS__ )
STATIC UINT16 makeTmpEnv( char *arg )
/*
    Copy arg into an environment var if possible.  If succeeds, then changes
    arg to just "@WMAKExxxxx", and returns non-zero.  Otherwise leaves
    arg alone and returns zero.
*/
{
    UINT16      tmp;
    char        buf[ 20 ]; /* "WMAKExxxxx=" + '\0' = 11 + room for FmtStr */
    size_t      len;
    ENV_TRACKER *env;

    tmp = 1;
    for(;;) {
        FmtStr( buf, "WMAKE%d", tmp );
        if( getenv( buf ) == NULL ) break;
        ++tmp;
    }
    len = strlen( arg );
    if( len < 13 ) {     /* need room for " @WMAKExxxxx" */
        return( 0 );
    }
                        /* "WMAKExxxxx=" + arg + '\0' */
    env = MallocSafe( sizeof( ENV_TRACKER ) + len + 12 );
    FmtStr( env->value, "WMAKE%d=%s", tmp, arg );
    if( PutEnvSafe( env ) != 0 ) {
        return( 0 );
    }
    FmtStr( arg, " @WMAKE%d", tmp );
    return( tmp );
}

STATIC void killTmpEnv( UINT16 tmp )
{
    ENV_TRACKER *env;

    if( tmp == 0 ) return;
    env = MallocSafe( sizeof( ENV_TRACKER ) + 20 );
    FmtStr( env->value, "WMAKE%d=", tmp );
    PutEnvSafe( env );
}
#else
#pragma off(unreferenced);
STATIC UINT16 makeTmpEnv( char *cmd ) { return( 0 ); }
STATIC void killTmpEnv( UINT16 tmp ) {}
#pragma on (unreferenced);
#endif

#pragma on (check_stack);
STATIC RET_T shellSpawn( char *cmd, int flags )
{
    BOOLEAN percent_cmd;/* is this a percent cmd? */
    int     comnum;     /* index into dosInternals */
    char    cmdname[ _MAX_PATH ];
#if defined( __DOS__ )
    char    ext[ _MAX_EXT ];
#endif
    char    *arg;
    char const *argv[ 3 ]; /* for spawnvp */
    int     retcode;    /* from spawnvp */
    UINT16  tmp_env;    /* for * commands */
    RET_T   my_ret;     /* return code for this function */

    assert( cmd != NULL );

    percent_cmd = cmd[0] == '%';
    arg = cmd + ( percent_cmd ? 1 : 0 );    /* split cmd name from args */
    while( !(isws( *arg ) || *arg == Glob.swchar || *arg == '+' ||
        *arg == '=' || *arg == NULLCHAR ) ) {
        ++arg;
    }
    if( arg - cmd >= _MAX_PATH ) {
        PrtMsg( ERR| COMMAND_TOO_LONG );
        return( RET_ERROR );
    }
    memcpy( cmdname, cmd, arg - cmd );  /* copy command */
    cmdname[ arg - cmd ] = NULLCHAR;    /* null terminate it */
#if defined( __DOS__ )
    _splitpath( cmdname, NULL, NULL, NULL, ext );
    if( ext[0] == '.' ) {
        FixName( ext );
        /* if the extension is specified let the shell handle it (26-apr-91) */
        if( FNameCmp( ext, ".exe" ) != 0 &&
            FNameCmp( ext, ".com" ) != 0 ) {
            /* .bat and .cmd need the shell anyway */
            flags |= FLAG_SHELL;
        }
    }
#endif
    comnum = findInternal( cmdname );
    if( !( flags & FLAG_SILENT ) ||
        ( Glob.noexec && ( comnum != COM_FOR || ( flags & FLAG_SHELL ) ) &&
        !percent_cmd ) ) {
        if( !Glob.noheader ) {
            PrtMsg( INF|NEOL| JUST_A_TAB );
        }
        dumpCommand( cmd );
    }
    if( percent_cmd ) {
        return( percentCmd( cmdname, arg ) );
    }
    if( hasMetas( cmd ) && comnum != COM_FOR ) {
        /* pass to shell because of '>','<' or '|' */
        flags |= FLAG_SHELL;
    }
    if(( flags & FLAG_ENV_ARGS ) != 0 && ( flags & FLAG_SHELL ) == 0 ) {
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
        case COM_SET:   my_ret = handleSet( cmd );          break;
        case COM_FOR:   my_ret = handleFor( cmd );          break;
        case COM_IF:    my_ret = handleIf( cmd );           break;
#if defined( __OS2__ ) || defined( __NT__ ) || defined( __QNX__ )
        case COM_CD:    /* fall through */
        case COM_CHDIR: my_ret = handleCD( cmd );           break;
#if defined( __OS2__ ) || defined( __NT__ )
        case CNUM:      my_ret = handleChangeDrive( cmd );  break;
#endif
#endif
        default:        my_ret = mySystem( cmdname, cmd );  break;
        }
    } else if( Glob.noexec ) {
        my_ret = RET_SUCCESS;
    } else {                                /* pass to spawnvp */
        DLL_CMD* dll_cmd;
        argv[ 0 ] = cmdname;
        if( *arg == NULLCHAR ) {
            argv[ 1 ] = NULL;   /* no args */
        } else {
            argv[ 1 ] = arg;    /* pass the args */
            argv[ 2 ] = NULL;
        }
        closeCurrentFile();
        dll_cmd = OSFindDLL( argv[0] );
        if( dll_cmd == NULL ) {
            retcode = spawnvp( P_WAIT, cmdname, argv );
            if( retcode < 0 ) {
                PrtMsg( ERR| UNABLE_TO_EXEC, cmdname );
            }
        } else {
            retcode = OSExecDLL( dll_cmd, argv[1] );
#ifdef DLLS_IMPLEMENTED
            if ( retcode != IDEDRV_SUCCESS ) {
                if ( retcode == IDEDRV_ERR_RUN_FATAL ) {
                    retcode = 2;
                } else if ( retcode == IDEDRV_ERR_RUN_EXEC ) {
                    retcode = 1;
                } else if ( retcode == IDEDRV_ERR_RUN ) {
                    PrtMsg ( ERR | DLL_BAD_RETURN_STATUS,
                                  dll_cmd->inf.dll_name);
                    retcode = 4;
                } else if ( retcode == IDEDRV_ERR_LOAD ||
                            retcode == IDEDRV_ERR_UNLOAD ) {
                    PrtMsg (ERR | UNABLE_TO_LOAD_DLL,
                                  dll_cmd->inf.dll_name);
                    retcode = 4;
                } else {
                    PrtMsg (ERR | DLL_BAD_INIT_STATUS,
                                  dll_cmd->inf.dll_name);
                    retcode = 4;
                }
#else
            if ( retcode != 0 ) {
                PrtMsg( ERR| UNABLE_TO_EXEC, cmdname );
                retcode = 4;
#endif
            } else {
                retcode = 0;
            }
        }
        lastErrorLevel = retcode;
        my_ret = retcode ? RET_ERROR : RET_SUCCESS;
    }
    if( flags & FLAG_ENV_ARGS ) {    /* cleanup for makeTmpEnv */
        killTmpEnv( tmp_env );
    }
    return( my_ret );
}
#pragma off(check_stack);


STATIC RET_T execLine( char *line )
/**********************************
 * is allowed to hack up line any way it feels
 */
{
    char    *p;
    int     flags;
    RET_T   rc;


    CheckForBreak();
    /* make a copy of global flags */
    flags =     ( Glob.silent ? FLAG_SILENT : 0 )
                | ( Glob.ignore ? FLAG_IGNORE : 0 )
                | ( Glob.shell ? FLAG_SHELL : 0 );

    p = line;               /* process @*!- and strip leading ws */
    for(;;) {
        p = SkipWS( p );

        if( *p == '@' ) {
            flags |= FLAG_SILENT;
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

    assert( !isws( *p ) );

    rc = shellSpawn( p, flags );
    if( OSCorrupted() ) {
        PrtMsg( FTL| OS_CORRUPTED );
    }
    CheckForBreak();
    if( rc != RET_SUCCESS && !( flags & FLAG_IGNORE ) ) {
        return( RET_ERROR );
    }
    return( RET_SUCCESS );
}


extern RET_T ExecCList( CLIST *clist )
/***********************************/
{
    char    *line;
    RET_T   ret;
    FLIST   *currentFlist;

    while( clist != NULL ) {
        ret = writeInlineFiles(clist->inlineHead,
                               &(clist->text));
        currentFlist = clist->inlineHead;
        if (ret == RET_SUCCESS) {
            UnGetCH( STRM_MAGIC );
            InsString( clist->text, FALSE );
            line = DeMacro( STRM_MAGIC );
            GetCHR();        /* eat STRM_MAGIC */
            if (Glob.verbose && ret == RET_SUCCESS) {
                 ret = VerbosePrintTempFile(currentFlist);
            }
            ret = execLine( line );
            if (ret != RET_SUCCESS) {
                return (ret);
            }
            FreeSafe( line );
        } else {
            closeCurrentFile();
            return( ret );
        }

        clist = clist->next;
    }
    closeCurrentFile();
    return( ret );
}


// deletes the file specified in the nokeeplist
STATIC void destroyNKList () {

    NKLIST* temp;
    VECSTR  outText;
    char    *tempstr;

    temp = noKeepList;
    while (temp != NULL) {
        if (Glob.noexec) {
            if (!Glob.noheader) {
                PrtMsg( INF|NEOL| JUST_A_TAB );
            }
            outText = StartVec();
            WriteVec(outText,"del ");
            WriteVec(outText,temp->fileName);
            tempstr = FinishVec(outText);
            PrtMsg( INF|PRNTSTR,tempstr);
            FreeSafe(tempstr);
        }
        remove(temp->fileName);
        temp = temp->next;
    }
    FreeNKList(noKeepList);
}

extern void ExecInit( void )
/**************************/
{
    lastErrorLevel = 0;
    currentFileName = NULL;
    currentFileHandle = -1;
    /* Take any number first */
    tmpFileNumber   = time (NULL) % 100000;
}


extern void ExecFini( void )
/**************************/
{
    // destroy all the files that will not be kept
    destroyNKList();
}
