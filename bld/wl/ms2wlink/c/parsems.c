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


/*
 *  PARSEMS : Routines for parsing Microsoft command files.
 *
*/

#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <process.h>
#include "ms2wlink.h"
#include "command.h"

cmdfilelist *    CmdFile;

static int              OverlayLevel = 0;

extern cmdentry *   Commands[];
extern char *       PromptText[];
extern bool         HaveDefFile;

extern void         Error( char * );
extern void         OutPutPrompt( int );
extern f_handle     QOpenR( char * );
extern unsigned     QRead( f_handle, void *, unsigned, char * );
extern void         QClose( f_handle, char * );
extern unsigned long QFileSize( f_handle );
extern bool         QReadStr( f_handle, char *, unsigned, char * );
extern void         AddCommand( char *, int, bool );
extern void         Warning( char *, int );
extern char *       FileName( char *, int, char, bool );
extern char *       Msg3Splice( char *, char *, char * );
extern void *       MemAlloc( unsigned );
extern void *       TryAlloc( unsigned );
extern void         MemFree( void * );
extern bool         ProcessOptions( void );
extern void         ProcessDefCommand( void );
extern void *       FindNotAsIs( int );

extern bool         MakeToken( sep_type, bool );
extern bool         GetNumber( unsigned long * );
extern char *       ToString( void );

extern bool InitParsing( void )
/*****************************/
// initialize the parsing stuff, and see if there is anything on the command
// line.
{
    CmdFile = MemAlloc( sizeof( cmdfilelist ) );
    CmdFile->next = NULL;
    CmdFile->buffer = MemAlloc( MAX_LINE );   // maximum size of a command line.
    CmdFile->how = COMMANDLINE;
    CmdFile->oldhow = COMMANDLINE;
    CmdFile->where = MIDST;
    CmdFile->name = NULL;
    CmdFile->file = NIL_HANDLE;
    getcmd( CmdFile->buffer );
    CmdFile->current = CmdFile->buffer;
    EatWhite();
    if( *CmdFile->current == '?' ) {
        return( FALSE );
    }
    return( TRUE );
}

extern void FreeParserMem( void )
/*******************************/
{
    cmdfilelist *   nextone;


    while( CmdFile != NULL ) {
        if( CmdFile->file != NIL_HANDLE ) {
            QClose( CmdFile->file, CmdFile->name );
        }
        MemFree( CmdFile->buffer );
        MemFree( CmdFile->name );
        nextone = CmdFile->next;
        MemFree( CmdFile );
        CmdFile = nextone;
    }
}

extern void ParseMicrosoft( void )
/********************************/
// read in microsoft linker commands
{
    char    hmm;
    bool    sameprompt;
    bool    gottoken;
    int     prompt;
    char *  sect;

    sameprompt = TRUE;
    gottoken = FALSE;
    prompt = 0;                     // object files.
    while( CmdFile->where != ENDOFCMD ) {
        switch( CmdFile->where ) {
        case MIDST:
            EatWhite();
            hmm = *CmdFile->current;
            switch( hmm ) {
            case '\0':
                if( CmdFile->how == BUFFERED ) {
                    CmdFile->where = ENDOFFILE;
                    sameprompt = TRUE;
                    break;
                } else if( CmdFile->how == NONBUFFERED ){
                    CmdFile->where = ENDOFLINE;
                    sameprompt = TRUE;
                    break;
                }             // note the possible fall through.
            case '\n':
                if( CmdFile->how == BUFFERED ) {
                    CmdFile->current++;
                } else {
                    CmdFile->where = ENDOFLINE;
                }
                if( !sameprompt ) {
                    NextPrompt( &prompt );
                }
                sameprompt = FALSE;
                gottoken = FALSE;
                break;
            case '@':
                CmdFile->current++;
                if( !MakeToken( SEP_NO, TRUE ) ) {
                    Error( "file name not recognized" );
                }
                StartNewFile( ToString() );
                break;
            case '+':
                CmdFile->current++;
                if( !gottoken ) {
                    Warning( "unexpected + sign found ... ignoring", prompt );
                } else {
                    sameprompt = TRUE;
                }
                gottoken = FALSE;
                break;
            case ',':
                CmdFile->current++;
                if( prompt >= 5 ) {
                    Warning( "unexpected comma found ... ignoring", prompt );
                } else {
                    NextPrompt( &prompt );
                }
                gottoken = FALSE;
                break;
            case ';':
                CmdFile->where = ENDOFCMD;
                break;
            case '/':
                CmdFile->current++;
                if( !ProcessOptions() ) {
                    DirectiveError();
                }
                gottoken = TRUE;
                break;
            case '(':
                CmdFile->current++;
                if( prompt != 0 ) {
                    Warning( "can only overlay object files", prompt );
                    break;
                }
                if( OverlayLevel > 0 ) {
                    Warning( "overlay nesting ignored since it's not allowed by Microsoft",
                                                                         OVERLAY_SLOT );
                } else {     // need a section keyword.
                    sect = MemAlloc( 8 );
                    memcpy( sect, "section", 8 );
                    AddCommand( sect , OVERLAY_SLOT, TRUE );
                }
                OverlayLevel++;       // so spurious errors not produced.
                break;
            case ')':
                CmdFile->current++;
                if( prompt == 0 ) {       // warning already printed otherwise.
                    if( OverlayLevel <= 0 ){
                        Warning("unexpected ) found ... ignoring",OVERLAY_SLOT);
                     } else {
                        OverlayLevel--;
                    }
                }
                break;
            default:          // must be a file name
                ProcessFileName( prompt );
                gottoken = TRUE;
                sameprompt = FALSE;
                break;
            }
            break;
        case ENDOFLINE:
            GetNewLine( prompt );
            break;
        case ENDOFFILE:
            RestoreCmdLine();
            break;
        }
    }
    ProcessDefFile();
}

static void NextPrompt( int *prompt )
/***********************************/
{
    (*prompt)++;
    if( *prompt >= 5 ) {
        CmdFile->where = ENDOFCMD;
    } else if( OverlayLevel > 0 ) {
        Warning( "ending overlay parenthesis not found", OVERLAY_SLOT );
        OverlayLevel = 0;
    }
}

static void ProcessDefFile( void )
/********************************/
{
    cmdentry *  cmd;

    if( Commands[ DEF_SLOT ] == NULL ) {
        return;
    }
    HaveDefFile = 1;
    cmd = Commands[ DEF_SLOT ];
    StartNewFile( FileName( cmd->command, strlen( cmd->command ), E_DEF,FALSE));
    ParseDefFile();
}

extern void ParseDefFile( void )
/******************************/
// parse a .def file
{
    char        hmm;

    while( CmdFile->where != ENDOFFILE ) {
        switch( CmdFile->where ) {
        case MIDST:
            EatWhite();
            hmm = *CmdFile->current;
            switch( hmm ) {
            case '\0':
                if( CmdFile->how == BUFFERED ) {
                    CmdFile->where = ENDOFFILE;
                    break;
                }                // NOTE the fall through.
            case '\n':
                if( CmdFile->how == BUFFERED ) {
                    CmdFile->current++;
                } else {
                    CmdFile->where = ENDOFLINE;
                }
                break;
            case ';':
                CmdFile->where = ENDOFLINE;    // a comment.
                break;
            default:          // must be a command
                ProcessDefCommand();
                break;
            }
            break;
        case ENDOFLINE:
            GetNewLine( DEF_SLOT );
            break;
        }
    }
    RestoreCmdLine();
}

static void ProcessFileName( int prompt )
/***************************************/
// process a file name and put it into the proper slot.
{
    char *  name;
    char *  msg;

    if( OverlayLevel > 0 ) {
        prompt = OVERLAY_SLOT;
    }
    if( !MakeToken( SEP_NO, TRUE ) ) {
        Error( "file name not recognized" );
    } else {
        if( (prompt == MAP_SLOT || prompt == RUN_SLOT || prompt == DEF_SLOT)
                                          && FindNotAsIs( prompt ) != NULL ) {
            msg = Msg3Splice( "only one ",PromptText[ prompt ], "allowed" );
            Warning( msg, prompt );
            MemFree( msg );
        } else {
            name = FileName( CmdFile->token, CmdFile->len, prompt, FALSE );
            AddCommand( name, prompt, FALSE );
        }
    }
}

static void GetNewLine( prompt )
/******************************/
{
    if( CmdFile->how == NONBUFFERED ) {
        if( QReadStr( CmdFile->file, CmdFile->buffer, MAX_LINE,CmdFile->name )){
            CmdFile->where = ENDOFFILE;
        } else {
            CmdFile->where = MIDST;
        }
        CmdFile->current = CmdFile->buffer;
    } else if( CmdFile->how == BUFFERED ) {               // interactive.
        CmdFile->where = MIDST;
        while( *CmdFile->current != '\n' ) {       //go until next line found;
            if( *CmdFile->current == '\0' ) {
                CmdFile->where = ENDOFFILE;
                break;
            }
            CmdFile->current++;
        }
    } else {
        CmdFile->how = INTERACTIVE;
        CmdFile->oldhow = INTERACTIVE;
        OutPutPrompt( prompt );
        if( QReadStr( STDIN_HANDLE, CmdFile->buffer, MAX_LINE, "console" ) ) {
            CmdFile->where = ENDOFCMD;
        } else {
            CmdFile->where = MIDST;
        }
        CmdFile->current = CmdFile->buffer;
    }
}

extern void StartNewFile( char *fname )
/*************************************/
{
    cmdfilelist *   newfile;
    unsigned long   size;

    newfile = MemAlloc( sizeof( cmdfilelist ) );
    newfile->name = fname;
    newfile->file = NIL_HANDLE;
    newfile->next = CmdFile;
    CmdFile = newfile;
    CmdFile->buffer = NULL;
    newfile->file = QOpenR( newfile->name );
    size = QFileSize( newfile->file );
    if( size < 65510 ) {       // if can alloc a chunk big enough
        CmdFile->buffer = TryAlloc( size + 1 );
        if( CmdFile->buffer != NULL ) {
            size = QRead( newfile->file, CmdFile->buffer, size, newfile->name );
            *(CmdFile->buffer + size) = '\0';
            CmdFile->where = MIDST;
            CmdFile->how = BUFFERED;
            CmdFile->current = CmdFile->buffer;
        }
    }
    if( CmdFile->buffer == NULL ) {  // if couldn't buffer for some reason.
        CmdFile->where = ENDOFLINE;
        CmdFile->how = NONBUFFERED;
        CmdFile->buffer = MemAlloc( MAX_LINE + 1 );// have to have at least this
        CmdFile->current = CmdFile->buffer;        // much RAM or death ensues.
    }
}


extern void EatWhite( void )
/**************************/
{
    while(*CmdFile->current == ' ' || *CmdFile->current == '\t'
                                          || *CmdFile->current == '\r') {
        CmdFile->current++;
    }
}


extern bool MakeToken( sep_type separator, bool include_fn )
/**********************************************************/
// include_fn == TRUE if '.' and ':' are allowed to be part of the token
// (include filename).
{
    int     quit;
    char    hmm;
    char    matchchar;
    int     len;
    bool    forcematch;
    bool    hitmatch;

    EatWhite();
    hmm = *CmdFile->current;
    CmdFile->token = CmdFile->current;
    if( hmm == '\0' || hmm == '\n' ) {
        CmdFile->len = 0;
        return( FALSE );
    }
    CmdFile->len = 1;                 // for error reporting.
    switch( separator ){
    case SEP_QUOTE:
        if( hmm != '\'' && hmm != '"' ) {
            return( FALSE );
        }
        matchchar = hmm;
        break;
    case SEP_AT:
        if( hmm != '@' ) {
            return( FALSE );
        }
        break;
    case SEP_COLON:
        if( hmm != ':' ) {
            return( FALSE );
        }
        break;
    case SEP_EQUALS:
        if( hmm != '=' ) {
            return( FALSE );
        }
        break;
    case SEP_PERIOD:
        if( hmm != '.' ) {
            return( FALSE );
        }
        break;
    default:
        CmdFile->current--;        // no separator wanted.
    }
    CmdFile->current++;     // skip separator.
    CmdFile->token = CmdFile->current;
    hmm = *CmdFile->current;
    if( hmm == '\0' || hmm == '\n' ) {
        CmdFile->len = 0;
        return( FALSE );
    }
    len = 0;
    quit = FALSE;
    forcematch = (separator == SEP_QUOTE );
    hitmatch = FALSE;
    while( !quit ) {
        len++;
        CmdFile->current++;
        hmm = *CmdFile->current;
        switch( hmm ) {
        case '\'':
        case '"':
            if( separator == SEP_QUOTE && hmm == matchchar ) {
                CmdFile->current++;    // don't include end quote in next token.
                hitmatch = TRUE;
                quit = TRUE;
            }
            break;
        case '+':              // break a token on any of these.
        case ',':
        case ';':
        case '=':
        case '@':
        case '(':
        case ')':
            if( separator == SEP_SPACE ) break;   // NOTE: possible fall through
        case '/':
        case ' ':
        case '\t':
            if( !forcematch ) {
                quit = TRUE;
            }
            break;
        case '.':
        case ':':
            if( !forcematch && !include_fn ) {
                quit = TRUE;
            }
            break;
        case '\0':
        case '\r':
        case '\n':
            quit = TRUE;
            break;
        }
    }
    CmdFile->len = len;
    if( forcematch && !hitmatch ) {
        return( FALSE );
    }
    return( TRUE );
}

static void RestoreCmdLine( void )
/********************************/
// Restore a saved command line.
{
    void *  temp;

    QClose( CmdFile->file, CmdFile->name );
    MemFree( CmdFile->name );
    MemFree( CmdFile->buffer );
    temp = CmdFile->next;
    MemFree( CmdFile );
    CmdFile = temp;
}

extern void DirectiveError( void )
/********************************/
{
    char *  msg;
    int     offset;

    offset = 22 + CmdFile->len;
    msg = alloca( offset + 2 );
    memcpy( msg, "directive error near '", 22 );
    memcpy( msg+22, CmdFile->token, CmdFile->len );
    *( msg + offset) = '\'';
    *( msg + offset + 1) = '\0';
    Error( msg );
}

extern bool GetNumber( unsigned long * pnt )
/******************************************/
{
    char *          p;
    int             len;
    unsigned long   value;
    unsigned        radix;
    bool            isvalid;
    bool            isdig;
    char            ch;

    p = CmdFile->token;
    len = CmdFile->len;
    value = 0ul;
    radix = 10;
    if( *p == '0' ) {
        --len;
        radix = 8;
        if( tolower(*++p) == 'x') {
            radix = 16;
            ++p;
            --len;
        }
    }
    for( ; len != 0; --len ) {
        ch = tolower( *p++ );
        if( ch == 'k' ) {         // constant of the form 64k
            if( len > 1 ) {
                return( FALSE );
            } else {
                value <<= 10;        // value = value * 1024;
            }
        } else {
            isdig = isdigit( ch );
            if( radix == 10 ) {
                isvalid = isdig;
            } else if( radix == 8 ) {
                if( ch == '8' || ch == '9' || !isdig ) {
                    isvalid = FALSE;
                } else {
                    isvalid = TRUE;
                }
            } else {
                isvalid = isxdigit( ch );
            }
            if( !isvalid ) {
                return( FALSE );
            }
            value *= radix;
            if( isdig ) {
                value += ch - '0';
            } else {
                value += ch - 'a' + 10;
            }
        }
    }
    *pnt = value;
    return( TRUE );
}

extern char * ToString( void )
/****************************/
{
    char *          src;
    int             len;
    char *          str;

    src = CmdFile->token;
    len = CmdFile->len;
    str = MemAlloc( len + 1 );
    memcpy( str, src, len );
    str[ len ] = '\0';
    return( str );
}
