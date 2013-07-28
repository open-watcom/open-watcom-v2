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
* Description:  Utility routines for the command line parser.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "wio.h"
#include "walloca.h"
#include "linkstd.h"
#include "loadfile.h"
#include "command.h"
#include "alloc.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "fileio.h"
#include "ideentry.h"
#include "cmdline.h"
#if !defined( __UNIX__ )
#include <direct.h>
#endif

#define _LinkerPrompt "WLINK>"

cmdfilelist      *CmdFile = NULL;

/* Default File Extension array, see ldefext.h */

static  char    *DefExt[] = {
#define pick1(enum,text) text,
#include "ldefext.h"
#undef pick1
};

static bool     CheckFence( void );
static bool     MakeToken( tokcontrol, sep_type );
static void     GetNewLine( void );
static void     BackupParser( void );
static void     StartNewFile( void );

static bool WildCard( bool (*rtn)( void ), tokcontrol ctrl )
/**********************************************************/
{
#if defined( __UNIX__ ) || defined( __ZDOS__ )
    //opendir - readdir wildcarding not supported here.
    ctrl = ctrl;
    return( rtn() );
#else
    char                *p;
    char                *start;
    DIR                 *dir;
    struct dirent       *dirent;
    char                drive[_MAX_DRIVE];
    char                directory[_MAX_DIR];
    char                name[_MAX_FNAME];
    char                extin[_MAX_EXT];
    char                pathin[_MAX_PATH];
    bool                wildcrd;
    bool                retval;

    wildcrd = FALSE;
    if( ctrl & TOK_IS_FILENAME ) {
        p = Token.this;
        for(;;) {               // check if wildcard
            /* end of parm: NULLCHAR or blank */
            if( *p == '\'' ) break;     // don't wildcard a quoted string.
            if( *p == '\0' ) break;
            if( *p == ' ' ) break;
            if( *p == '?' || *p == '*' ) {
                wildcrd = TRUE;
                break;
            }
            p++;
        }
    }
    if( !wildcrd ) {
        retval = rtn();
    } else {
        retval = TRUE;
        /* expand file names */
        start = tostring();
        dir = opendir( start );
        if( dir != NULL ) {
            _splitpath( start, drive, directory, NULL, NULL );
            for(;;) {
                dirent = readdir( dir );
                if( dirent == NULL )
                    break;
                if( dirent->d_attr & (_A_HIDDEN+_A_SYSTEM+_A_VOLID+_A_SUBDIR) )
                    continue;
                _splitpath( dirent->d_name, NULL, NULL, name, extin );
                _makepath( pathin, drive, directory, name, extin );
                Token.this = pathin;            // dangerous?
                Token.len = strlen( pathin );
                if( !(*rtn)() ) {
                    Token.this = NULL;
                    Token.thumb = OK;   // make _sure_ we don't use token.this
                    retval = FALSE;
                    break;
                }
            }
            closedir( dir );
        } else {
            retval = rtn();
        }
        _LnkFree( start );
    }
    return( retval );
#endif
}

bool ProcArgList( bool (*rtn)( void ), tokcontrol ctrl )
{
    return(ProcArgListEx(rtn, ctrl ,NULL));
}

bool ProcArgListEx( bool (*rtn)( void ), tokcontrol ctrl ,cmdfilelist *resetpoint)
/*************************************************************/
{
    bool bfilereset = FALSE;    /* did we open a file and get reset ? */

    if( GetTokenEx( SEP_LCURLY, ctrl, resetpoint, &bfilereset) ) {
        for(;;) {
            if( !WildCard( rtn, ctrl ) ) {
                return( FALSE );
            }
            if( CheckFence() ) {
                break;
            } else if( !GetTokenEx( SEP_NO, ctrl ,resetpoint, &bfilereset) ) {
                LnkMsg( LOC+LINE+ERR+MSG_BAD_CURLY_LIST, NULL );
                break;
            }
        }
    } else {
        if(resetpoint && bfilereset)
            return( TRUE );
        if( GetTokenEx( SEP_NO, ctrl, resetpoint, &bfilereset) == FALSE )
            return( FALSE );
        do {
            if(resetpoint && bfilereset)
                return( TRUE );
            if( !WildCard( rtn, ctrl ) ) {
                return( FALSE );
            }
        } while( GetTokenEx( SEP_COMMA, ctrl , resetpoint, &bfilereset) );
    }
    return( TRUE );
}

bool ProcOne( parse_entry *entry, sep_type req, bool suicide )
/*******************************************************************/
/* recognize token out of parse table, with required separator            */
/* return FALSE if no separator, Suicide if not recognized (if suicide is */
/* TRUE) otherwise use return code from action routine in matching entry  */
{
    char                *key;
    char                *ptr;
    unsigned            plen;
    bool                ret;
    char                keybuff[20];

    ret = GetToken( req, TOK_INCLUDE_DOT );
    if( ret == FALSE ) {
        return( ret );
    }
    while( entry->keyword != NULL ) {
        key = entry->keyword;
        ptr = Token.this;
        plen = Token.len;
        for(;;) {
            if( plen == 0 && !isupper( *key ) ) {
                if( HintFormat( entry->format ) ) {
                    ret = (*entry->rtn)();
                    CmdFlags |= entry->flags;
                } else {
                    strcpy( keybuff, entry->keyword );
                    strlwr( keybuff );
                    LnkMsg( LOC+LINE+WRN+MSG_FORMAT_BAD_OPTION, "s", keybuff );
                    ret = TRUE;
                }
                return( ret );
            }
            if( *key == '\0' || tolower( *ptr ) != tolower( *key ) ) break;
            ptr++;
            key++;
            plen--;
        }
        /* here if this is no match */
        entry++;
    }
    /* here if no match in table */
    if( suicide ) {
        Syntax();
    } else {
        Token.thumb = REJECT;       /*  try again later */
        ret = FALSE;
    }
    return( ret );
}

bool MatchOne( parse_entry *entry , sep_type req , char *match, unsigned len )
/****************************************************************************/
/* recognize token out of parse table */
{
    char                *key;
    char                *ptr;
    unsigned            plen;
    bool                ret = FALSE;

    while( entry->keyword != NULL ) {
        key = entry->keyword;
        ptr = match;
        plen = len;
        for(;;) {
            if( plen == 0 && !isupper( *key ) ) {
                ret = TRUE;
                return( ret );
            }
            if( *key == '\0' || tolower( *ptr ) != tolower( *key ) )
                break;
            ptr++;
            key++;
            plen--;
        }
        /* here if this is no match */
        entry++;
    }

    /* here if no match in table */
    return( ret );
}

ord_state getatoi( unsigned_16 *pnt )
/*******************************************/
{
    unsigned_32 value;
    ord_state   retval;

    retval = getatol( &value );
    if( retval == ST_IS_ORDINAL ) {
        if( value > 0xffff ) {
            return( ST_INVALID_ORDINAL );
        }
        *pnt = (unsigned)value;
    }
    return( retval );
}

ord_state getatol( unsigned_32 *pnt )
/*******************************************/
{
    char            *p;
    unsigned        len;
    unsigned long   value;
    unsigned        radix;
    bool            isvalid;
    bool            isdig;
    bool            gotdigit;
    char            ch;

    len = Token.len;
    if( len == 0 )
        return( ST_NOT_ORDINAL );
    p = Token.this;
    gotdigit = FALSE;
    value = 0ul;
    radix = 10;
    if( len >= 2 && *p == '0' ) {
        --len;
        ++p;
        if( tolower( *p ) == 'x' ) {
            radix = 16;
            ++p;
            --len;
        }
    }
    for( ; len != 0; --len ) {
        ch = tolower( *p++ );
        if( ch == 'k' ) {         // constant of the form 64k
            if( len > 1 || !gotdigit ) {
                return( ST_NOT_ORDINAL );
            } else {
                value <<= 10;        // value = value * 1024;
            }
        } else if( ch == 'm' ) {        // constant of the form 64M
            if( len > 1 || !gotdigit ) {
                return( ST_NOT_ORDINAL );
            } else {
                value <<= 20;
            }
        } else {
            isdig = ( isdigit( ch ) != 0 );
            if( radix == 10 ) {
                isvalid = isdig;
            } else {
                isvalid = ( isxdigit( ch ) != 0 );
            }
            if( !isvalid ) {
                return( ST_NOT_ORDINAL );
            }
            value *= radix;
            if( isdig ) {
                value += ch - '0';
            } else {
                value += ch - 'a' + 10;
            }
            gotdigit = TRUE;
        }
    }
    *pnt = value;
    return( ST_IS_ORDINAL );
}

bool HaveEquals( tokcontrol ctrl )
/***************************************/
{
    if( GetToken( SEP_EQUALS, ctrl ) == FALSE ) {
        Token.this = Token.next;
        /* collect the token that caused the problem */
        GetToken( SEP_NO, ctrl );
        return( FALSE );
    }
    return( TRUE );
}

bool GetLong( unsigned_32 *addr )
/**************************************/
{
    unsigned_32     value;
    ord_state       ok;

    if( !HaveEquals(0) ) return( FALSE );
    ok = getatol( &value );
    if( ok != ST_IS_ORDINAL ) {
        return( FALSE );
    } else {
        *addr = value;
    }
    return( TRUE );
}

char *tostring( void )
/****************************/
// make the current token into a C string.
{
    return( ChkToString( Token.this, Token.len ) );
}

char *totext( void )
/********************/
/* get a possiblly quoted string */
{
    Token.thumb = REJECT;
    if( !GetToken( SEP_NO, 0 ) ) {
        GetToken( SEP_NO, TOK_INCLUDE_DOT );
    }
    return( tostring() );
}

#define IS_WHITESPACE(ptr) (*(ptr) == ' ' || *(ptr) =='\t' || *(ptr) == '\r')

static void ExpandEnvVariable( void )
/***********************************/
/* parse the specified environment variable & deal with it */
{
    char    *envname;
    char    *env;
    char    *buff;
    size_t  envlen;

    Token.next++;
    if( !MakeToken( TOK_INCLUDE_DOT, SEP_PERCENT ) ) {
        LnkMsg( LOC+LINE+FTL+MSG_ENV_NAME_INCORRECT, NULL );
    }
    envname = tostring();
    env = GetEnvString( envname );
    if( env == NULL ) {
        LnkMsg( LOC+LINE+WRN+MSG_ENV_NOT_FOUND, "s", envname );
        _LnkFree( envname );
    } else {
        envlen = strlen( env );
        if( !IS_WHITESPACE( Token.next ) ) {
            MakeToken( TOK_INCLUDE_DOT, SEP_SPACE );
            _ChkAlloc( buff, envlen + Token.len + 1);
            memcpy( buff, env, envlen );
            memcpy( buff + envlen, Token.this, Token.len );
            buff[Token.len + envlen] = '\0';
        } else {
            buff = ChkToString( env, envlen );
        }
        NewCommandSource( envname, buff, ENVIRONMENT );
    }
}

static bool CheckFence( void )
/****************************/
/* check for a "fence", and skip it if it is there */
{
    if( Token.thumb == REJECT ) {
        if( Token.quoted ) return( FALSE );   /* no fence inside quotes */
        if( *Token.this == '}' ) {
            Token.this++;
            return( TRUE );
        }
    } else {
        return( GetToken( SEP_RCURLY, 0 ) );
    }
    return( FALSE );
}

bool GetToken( sep_type req, tokcontrol ctrl)
{
    return(GetTokenEx(req, ctrl, NULL, NULL));
}

bool GetTokenEx( sep_type req, tokcontrol ctrl, cmdfilelist *resetpoint, bool *pbreset)
/***************************************************/
/* return TRUE if no problem */
/* return FALSE if problem   */
{
    char    hmm;
    bool    ret;
    bool    need_sep;

    if( Token.thumb == REJECT ) {
        Token.thumb = OK;
        if( Token.quoted ) return( TRUE );
        Token.next = Token.this;        /* re-process last token */
    }
    need_sep = TRUE;
    for(;;) {                           /* finite state machine */

        /*
        //  carl.young
        //  We had a situation where an input file (in this case a Novell
        //  import or export file) does not have the consistent format
        //  expected from this FSM code. If the skipToNext flag is set,
        //  then we just skip to the next token and return rather than
        //  reporting an error.
        //  For reference the import files looked like:
        //      (PREFIX)
        //          symbol1,
        //          symbol2,
        //          symbolnm1,
        //          symboln
        //
        //  Note the missing comma separator after the prefix token. The
        //  prefix token(s) may also appear anywhere in the file.
        */

        if( (Token.skipToNext) && (req == SEP_COMMA) ) {
            Token.skipToNext = 0;
            need_sep = FALSE;
        }

        switch( Token.where ) {
        case MIDST:
            EatWhite();
            hmm = *Token.next;
            switch( hmm ) {
            case CTRLZ:
                Token.where = ENDOFFILE;
                break;
            case '\0':
                if( Token.how == BUFFERED
                 || Token.how == ENVIRONMENT
                 || Token.how == SYSTEM ) {
                    Token.where = ENDOFFILE;
                    break;
                }                // NOTE the fall through.
            case '\n':
                if( Token.how == BUFFERED
                 || Token.how == ENVIRONMENT
                 || Token.how == SYSTEM ) {
                    Token.next++;               // just skip this.
                } else if( Token.how == COMMANDLINE ) {
                    Token.where = ENDOFCMD;
                } else {
                    Token.where = ENDOFLINE;
                }
                Token.line++;
                break;
            case '@':
                if( req != SEP_SPACE ) {
                    Token.next++;
                    GetToken( SEP_NO, TOK_INCLUDE_DOT|TOK_IS_FILENAME );
                    StartNewFile();
                    break;
                }
                Token.next--;   /* make a token out of this */
                ret = MakeToken( ctrl, req );
                Token.quoted = FALSE;
                return( ret );
            case '#':
                Token.where = ENDOFLINE;            /* begin comment */
                Token.line++;
                break;
            case '^':
                if( req != SEP_SPACE ) {    /* if not storing system blocks */
                    Token.next++;
                    BackupParser();
                    break;
                }
                Token.next--;   /* make a token out of this */
                ret = MakeToken( ctrl, req );
                Token.quoted = FALSE;
                return( ret );
            case '%':
                if( req != SEP_SPACE ) {
                    ExpandEnvVariable();
                    break;
                }
            default:
                if( need_sep ) {
                    Token.quoted = FALSE;
                    switch( req ) {
                    case SEP_NO:
                        if( hmm == ',' || hmm == '=' ) return( FALSE );
                        break;
                    case SEP_COMMA:
                        if(hmm != ',' ) return( FALSE);
                        Token.next++;
                        break;
                    case SEP_EQUALS:
                        if( hmm != '=' ) return( FALSE );
                        Token.next++;
                        break;
                    case SEP_PERIOD:
                    case SEP_DOT_EXT:
                        if( hmm != '.' ) return( FALSE );
                        Token.next++;
                        break;
                    case SEP_PAREN:
                        if( hmm != '(' ) return( FALSE );
                        Token.next++;
                        break;
                    case SEP_LCURLY:
                        if( hmm != '{' ) return( FALSE );
                        Token.next++;
                        break;
                    case SEP_QUOTE:
                        if( hmm != '\'' ) return( FALSE );
                        Token.next++;
                        Token.quoted = TRUE;
                        break;
                    case SEP_RCURLY:
                        if( hmm != '}' ) return( FALSE );
                        Token.next++;
                        return( TRUE );
                    case SEP_END:
                        return( FALSE );
                    }
                    need_sep = FALSE;
                    EatWhite();
                } else {                /*  must have good separator here */
                    if( hmm == '\'' && req != SEP_PAREN && req != SEP_SPACE ) {
                        req = SEP_QUOTE;   /* token has been quoted */
                        Token.next++;      /* don't include the quote */
                        Token.quoted = TRUE;
                    }
                    ret = MakeToken( ctrl, req );
                    return( ret );
                }
                break;
            }
            break;
        case ENDOFLINE:
            GetNewLine();
            break;
        case ENDOFFILE:
            if( Token.locked ) return( FALSE );
            RestoreCmdLine();
            if( Token.thumb == REJECT ) {
                Token.thumb = OK;
                Token.next = Token.this;        /* re-process last token */
            }
            Token.quoted = FALSE;
            if( resetpoint && (CmdFile == resetpoint) ) {
                if( *Token.next == ',' )
                    break;
                if( pbreset )
                    *pbreset = TRUE;            /* Show we have hit a file end-point for a directive */
                return( FALSE );
            }
            break;
        case ENDOFCMD:
            if( CmdFile->next != NULL ) {
                RestoreParser();
                break;
            }
            Token.quoted = FALSE;
            ret = ( req == SEP_END ) ? TRUE : FALSE;
            return( ret );
        }
    }
}

static void OutPutPrompt( char *str )
/***********************************/
{
    if( QIsDevice( CmdFile->file ) ) {
        WriteStdOut( str );
    }
}

static void GetNewLine( void )
/****************************/
{
    if( Token.how == BUFFERED
     || Token.how == ENVIRONMENT
     || Token.how == SYSTEM ) {
        Token.where = MIDST;
        while( *Token.next != '\n' ) {       //go until next line found;
            if( *Token.next == '\0' || *Token.next == CTRLZ ) {
                Token.where = ENDOFFILE;
                break;
            }
            Token.next++;
        }
        Token.next++;
    } else if( Token.how == NONBUFFERED ) {
        if( QReadStr( CmdFile->file, Token.buff, MAX_REC, CmdFile->name ) ) {
            Token.where = ENDOFFILE;
        } else {
            Token.where = MIDST;
        }
        Token.next = Token.buff;
    } else {               // interactive.
        OutPutPrompt( _LinkerPrompt );
        Token.how = INTERACTIVE;
        if( QReadStr( STDIN_HANDLE, Token.buff, MAX_REC, "console" ) ) {
            Token.where = ENDOFCMD;
        } else {
            Token.where = MIDST;
        }
        Token.next = Token.buff;
    }
}

static void BackupParser( void )
/******************************/
/* move the parser temporarily back to a previous input source */
{
    if( CmdFile->prev == NULL ) {
        LnkMsg( LOC+LINE+WRN + MSG_NO_PREVIOUS_INPUT, NULL );
        return;
    }
    memcpy( &CmdFile->token, &Token, sizeof( tok ) );   // save current state
    CmdFile = CmdFile->prev;
    memcpy( &Token, &CmdFile->token, sizeof( tok ) ); // restore old state.
}

void RestoreParser( void )
/*******************************/
/* return the parser to the previous command state */
{
    if( CmdFile->next == NULL ) return;
    memcpy( &CmdFile->token, &Token, sizeof( tok ) );  /* save current state */
    CmdFile = CmdFile->next;
    memcpy( &Token, &CmdFile->token, sizeof( tok ) ); // restore old state.
}

void NewCommandSource( char *name, char *buff, method how )
/****************************************************************/
/* start reading from a new command source, and save the old one */
{
    cmdfilelist     *newfile;

    _ChkAlloc( newfile, sizeof( cmdfilelist ) );
    newfile->file = STDIN_HANDLE;
    if( CmdFile != NULL ) {     /* save current state */
        memcpy( &CmdFile->token, &Token, sizeof( tok ) );
        newfile->next = CmdFile->next;
        if( newfile->next != NULL ) {
            newfile->next->prev = newfile;
        }
    } else {
        newfile->next = NULL;
    }
    newfile->prev = CmdFile;
    if( newfile->prev != NULL ) {
        newfile->prev->next = newfile;
    }
    CmdFile = newfile;
    CmdFile->name = name;
    CmdFile->token.buff = buff;     /* make sure token is freed */
    CmdFile->token.how = how;       /* but only if it needs to be */
    Token.buff = buff;
    Token.next = Token.buff;
    Token.where = MIDST;
    Token.line = 1;
    Token.how = how;
    Token.thumb = OK;
    Token.locked = FALSE;
    Token.quoted = FALSE;
}

void SetCommandFile( f_handle file, char *fname )
/******************************************************/
/* read input from given file */
{
    unsigned long   size;
    char            *buff;

    if( QIsDevice( file ) ) {
        size = 0x10000;
    } else {
        size = QFileSize( file );
    }
    buff = NULL;
    if( size < 65510 ) {       // if can alloc a chunk big enough
        _LnkAlloc( buff, size + 1 );
        if( buff != NULL ) {
            size = QRead( file, buff, size, fname );
            *(buff + size) = '\0';
            NewCommandSource( fname, buff, BUFFERED );
        }
    }
    if( buff == NULL ) {  // if couldn't buffer for some reason.
        _ChkAlloc( buff, MAX_REC + 1 ); // have to have at least this much RAM
        NewCommandSource( fname, buff, NONBUFFERED );
        Token.where = ENDOFLINE;
        Token.line++;
    }
    CmdFile->file = file;
}

static void StartNewFile( void )
/******************************/
{
    char        *fname;
    char        *envstring;
    char        *buff;
    f_handle    file;

    fname = FileName( Token.this, Token.len, E_COMMAND, FALSE );
    file = QObjOpen( fname );
    if( file == NIL_FHANDLE ) {
        _LnkFree( fname );
        fname = tostring();
        envstring = GetEnvString( fname );
        if( envstring != NULL ) {
            buff = ChkStrDup( envstring );
            NewCommandSource( fname, buff, ENVIRONMENT );
        } else {
            LnkMsg( LOC+LINE+ERR+MSG_CANT_OPEN_NO_REASON, "s", fname );
            _LnkFree( fname );
            Suicide();
        }
        return;
    } else {
        SetCommandFile( file, fname );
    }
    DEBUG(( DBG_OLD, "processing command file %s", CmdFile->name ));
}

void EatWhite( void )
/**************************/
{
    while( IS_WHITESPACE( Token.next ) ) {
        Token.next++;
    }
}

static int ParseNumber( char *str, int radix )
/*********************************************/
/* read a (possibly hexadecimal) number */
{
    bool        isdig;
    bool        isvalid;
    char        ch;
    int         size;
    unsigned    value;

    size = 0;
    value = 0;
    for(;;) {
        ch = tolower( *str );
        isdig = ( isdigit( ch ) != 0 );
        if( radix == 8 ) {
            isvalid = isdig && !(ch == '8' && ch == '9');
        } else {
            isvalid = ( isxdigit( ch ) != 0 );
        }
        if( !isvalid ) break;
        value *= radix;
        if( isdig ) {
            value += ch - '0';
        } else {
            value += ch - 'a' + 10;
        }
        size++;
        str++;
    }
    *Token.next = value;
    return( size );
}

static void MapEscapeChar( void )
/*******************************/
/* turn the current character located at Token.next into a possibly unprintable
 * character using C escape codes */
{
    char        *str;
    int         shift;

    shift = 2;
    str = Token.next + 1;
    switch( *str ) {
    case 'a':
        *Token.next = '\a';
        break;
    case 'b':
        *Token.next = '\b';
        break;
    case 'f':
        *Token.next = '\f';
        break;
    case 'n':
        *Token.next = '\n';
        break;
    case 'r':
        *Token.next = '\r';
        break;
    case 't':
        *Token.next = '\t';
        break;
    case 'v':
        *Token.next = '\v';
        break;
    case 'x':
        shift += ParseNumber( ++str, 16 );
        break;
    case '0': case '1': case '2': case '3': case '4': case'5': case '6':
    case '7': case '8': case '9':
        shift += ParseNumber( str, 8 ) - 1;
        break;
    default:
        *Token.next = *str;
        break;
    }
    str = Token.next + shift;
    memmove( Token.next + 1, str, strlen( str ) + 1 );
}

static unsigned MapDoubleByteChar( unsigned char c )
/**************************************************/
/* if the double byte character support is on, check if the current character
 * is a double byte character skip it */
{
    switch( CmdFlags & CF_LANGUAGE_MASK ) {
    case CF_LANGUAGE_JAPANESE:
        if( (c >= 0x81 && c <= 0x9F) || (c >= 0xE0 && c <=0xFC) ) {
            Token.next++;
            return( 1 );
        }
        break;
    case CF_LANGUAGE_CHINESE:
        if( c > 0xFC ) break;
    case CF_LANGUAGE_KOREAN:            // note the fall through
        if( c > 0xFD ) break;
        if( c < 0x81 ) break;
        Token.next++;
        return( 1 );
    }
    return( 0 );
}

static bool MakeToken( tokcontrol ctrl, sep_type separator )
/**********************************************************/
{
    bool        quit;
    char        hmm;
    unsigned    len;
    bool        forcematch;
    bool        hitmatch;
    bool        keepspecial;

    Token.this = Token.next;
    len = 0;
    quit = FALSE;
    forcematch = (separator == SEP_QUOTE) || (separator == SEP_PAREN)
                 || (separator == SEP_PERCENT);
    keepspecial = (separator == SEP_SPACE) || (separator == SEP_DOT_EXT);
    if( separator == SEP_DOT_EXT ) {    /* KLUDGE! we want to allow a zero*/
        len--;                  /* length token for parsing wlib files, so */
        Token.next--;           /* artificially back up one here. */
    }
    if( *Token.next == '\\' && separator == SEP_QUOTE
                         && !(ctrl & TOK_IS_FILENAME) ) {
        MapEscapeChar();        /* get escape chars starting in 1st pos. */
    }
    hmm = *Token.next;
    len += MapDoubleByteChar( (unsigned char)hmm );
    hitmatch = FALSE;
    for(;;) {
        len++;
        hmm = *++Token.next;
        switch( hmm ) {
        case '\'':
            if( separator == SEP_QUOTE ) {
                ++Token.next;      // don't include end quote in next token.
                hitmatch = TRUE;
                quit = TRUE;
            }
            break;
        case ')':
            if( separator == SEP_PAREN ) {
                ++Token.next;    // don't include end paren in next token.
                hitmatch = TRUE;
                quit = TRUE;
            }
            break;
        case '%':
            if( separator == SEP_PERCENT ) {
                ++Token.next;    // don't include end percent in next token.
                hitmatch = TRUE;
                quit = TRUE;
            }
            break;
        case '.':
            if( !(ctrl & TOK_INCLUDE_DOT) && !forcematch ) {
                quit = TRUE;
            }
            break;
        case '{':
        case '}':
        case '(':
        case ',':
        case '=':
        case '#':
        case '@':
            if( keepspecial ) {
                break;
            }                   // NOTE the potential fall through
        case '\t':
        case ' ':
            if( !forcematch ) {
                quit = TRUE;
            }
            break;
        case '\\':
            if( separator == SEP_QUOTE && !(ctrl & TOK_IS_FILENAME) ) {
                MapEscapeChar();
            }
            break;
        case '\0':
        case '\r':
        case '\n':
        case CTRLZ:
            quit = TRUE;
            break;
        default:
            len += MapDoubleByteChar( (unsigned char)hmm );
        }
        if( quit ) {
            break;
        }
    }
    Token.len = len;
    if( forcematch && !hitmatch ) {
        return( FALSE );     // end quote/paren not found before token end.
    }
    return( TRUE );
}


char *FileName( char *buff, size_t len, file_defext etype, bool force )
/*********************************************************************/
{
    char        *namptr;
    char        *namstart;
    char        *ptr;
    size_t      cnt;
    size_t      namelen;
    char        c;

    
    for( namptr = buff + len; namptr != buff; --namptr ) {
        c = namptr[-1];
        if( IS_PATH_SEP( c ) ) {
            break;
        }
    }
    namstart = namptr;
    cnt = len - ( namptr - buff );
    if( cnt == 0 ) {
        ptr = alloca( len + 1 );
        memcpy( ptr, buff, len );
        ptr[len] = '\0';
        LnkMsg( LOC+LINE+FTL+MSG_INV_FILENAME, "s", ptr );
    }
    namelen = cnt;
    namptr = buff + len - 1;
    while( --cnt != 0 && *namptr != '.' ) {
        namptr--;
    }
    if( force || *namptr != '.' ) {
        if( force && etype == E_MAP ) {         // op map goes in current dir.
            buff = namstart;
            len = namelen;
        }
        if( cnt != 0 ) {
            len = namptr - buff;
        }
        _ChkAlloc( ptr, len + strlen( DefExt[etype] ) + 1 );
        memcpy( ptr, buff, len );
        strcpy( ptr + len, DefExt[etype] );
    } else {
        ptr = ChkToString( buff, len );
    }
    return( ptr );
}

void RestoreCmdLine( void )
/********************************/
// Restore a saved command line.
{
    cmdfilelist     *temp;

    if( CmdFile->prev == NULL ) {     /* can't free last file */
        Token.where = ENDOFCMD;
        return;
    }
    switch( Token.how ) {
    case SYSTEM:
        break;
    default:
        _LnkFree( Token.buff );
        if( CmdFile->file > STDIN_HANDLE ) {
            QClose( CmdFile->file, CmdFile->name );
        }
        break;
    }
    if( CmdFile->symprefix)
        _LnkFree( CmdFile->symprefix );
    CmdFile->symprefix = NULL;
    _LnkFree( CmdFile->name );
    temp = CmdFile->prev;
    temp->next = CmdFile->next;
    if( temp->next != NULL ) {
        temp->next->prev = temp;
    }
    _LnkFree( CmdFile );
    CmdFile = temp;
    memcpy( &Token, &CmdFile->token, sizeof( tok ) ); // restore old state.
}

bool IsSystemBlock( void )
/*************************/
// Are we in a system block?
{
    cmdfilelist     *temp;

    if( Token.how == SYSTEM ) return( TRUE );

    for( temp = CmdFile; temp != NULL; temp = temp->prev ) {
        if( temp->token.how == SYSTEM ) return( TRUE );
    }
    return( FALSE );
}

void BurnUtils( void )
/***************************/
// Burn data structures used in command utils.
{
    void        *temp;

    if( CmdFile->next != NULL ) {
        LnkMsg( LOC+LINE+ERR+MSG_NO_INPUT_LEFT, NULL );
    }
    while( CmdFile != NULL ) {
        if( CmdFile->file > STDIN_HANDLE ) {
            QClose( CmdFile->file, CmdFile->name );
        }
        if( CmdFile->symprefix)
            _LnkFree( CmdFile->symprefix );
        CmdFile->symprefix = NULL;
        _LnkFree( CmdFile->name );
        switch( CmdFile->token.how ) {
        case ENVIRONMENT:
        case SYSTEM:
            break;
        default:
            _LnkFree( CmdFile->token.buff );
            break;
        }
        temp = CmdFile;
        CmdFile = CmdFile->prev;
        _LnkFree( temp );
    }
    Token.how = BUFFERED;       // so error message stuff reports right name
}

outfilelist *NewOutFile( char *filename )
/************************************************/
{
    outfilelist     *fnode;

    for( fnode = OutFiles; fnode != NULL; fnode = fnode->next ) {
        if( FNAMECMPSTR( filename, fnode->fname ) == 0 ) {
            _LnkFree( filename );       // don't need this now.
            return( fnode );
        }
    }
// file name not already in list, so add a list entry.
    _ChkAlloc( fnode, sizeof( outfilelist ) );
    InitBuffFile( fnode, filename, TRUE );
    fnode->next = OutFiles;
    OutFiles = fnode;
    return( fnode );
}

int stricmp_wrapper( const void *s1, const void *s2 )
{
    return( stricmp( s1, s2 ) );
}

section *NewSection( void )
/*********************************/
{
    section             *sect;

    OvlNum++;
    _ChkAlloc( sect, sizeof( section ) );
    sect->next_sect = NULL;
    sect->classlist = NULL;
    sect->orderlist = NULL;
    sect->areas = NULL;
    sect->files = NULL;
    sect->modFilesHashed = CreateHTable( 256, StringiHashFunc, stricmp_wrapper,
                                        ChkLAlloc, LFree );
    sect->mods = NULL;
    sect->reloclist = NULL;
    sect->sect_addr.off = 0;
    sect->sect_addr.seg = UNDEFINED;
    sect->ovl_num = 0;
    sect->parent = NULL;
    sect->relocs = 0;
    sect->size = 0;
    sect->outfile = NULL;
    sect->u.dist_mods = NULL;
    sect->dbg_info = NULL;
    return( sect );
}

char *GetFileName( char **membname, bool setname )
/********************************************************/
{
    char        *ptr;
    unsigned    namelen;
    char        *objname;
    char        *fullmemb;
    unsigned    memblen;
    char        ch;

    namelen = Token.len;
    objname = alloca( namelen );
    memcpy( objname, Token.this, namelen );
    if( GetToken( SEP_PAREN, TOK_INCLUDE_DOT ) ) {   // got LIBNAME(LIB_MEMBER)
        ch = Token.this[Token.len];
        Token.this[Token.len] = 0;
        fullmemb = RemovePath( Token.this, &memblen );
        Token.this[Token.len] = ch;
        *membname = ChkToString( fullmemb, memblen );
        ptr = FileName( objname, namelen, E_LIBRARY, FALSE );
    } else {
        *membname = NULL;
        if( setname && Name == NULL ) {
            Name = ChkToString( objname, namelen );
        }
        ptr = FileName( objname, namelen, E_OBJECT, FALSE );
    }
    return( ptr );
}
