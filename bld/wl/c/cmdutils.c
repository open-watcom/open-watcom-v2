/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
#include <stdio.h>
#include <ctype.h>
#if !defined( __UNIX__ )
    #include <direct.h>
#endif
#if defined( __WATCOMC__ )
    #include <process.h>
#endif
#include "wio.h"
#include "walloca.h"
#include "linkstd.h"
#include "loadfile.h"
#include "cmdutils.h"
#include "alloc.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "fileio.h"
#include "ideentry.h"
#include "cmdline.h"
#include "pathgrp2.h"

#include "clibext.h"


#define _LinkerPrompt "WLINK>"

#define IS_WHITESPACE(ptr) (*(ptr) == ' ' || *(ptr) =='\t' || *(ptr) == '\r')

cmdfilelist     *CmdFile = NULL;

static bool WildCard( bool (*rtn)( void ), tokcontrol ctrl )
/**********************************************************/
{
#if defined( __UNIX__ )
    /* unused parameters */ (void)ctrl;

    //opendir - readdir wildcarding not supported here.
    return( rtn() );
#else
    const char          *p;
    char                *start;
    DIR                 *dirp;
    struct dirent       *dire;
    pgroup2             pg;
    char                pathin[_MAX_PATH];
    bool                wildcrd;
    bool                retval;

    wildcrd = false;
    if( ctrl & TOK_IS_FILENAME ) {
        for( p = Token.this; *p != '\0'; ++p ) {      // check if wildcard
            /* end of parm: NULLCHAR or blank */
            if( *p == '\'' )
                break;     // don't wildcard a quoted string.
            if( *p == ' ' )
                break;
            if( *p == '?' || *p == '*' ) {
                wildcrd = true;
                break;
            }
        }
    }
    if( !wildcrd ) {
        retval = rtn();
    } else {
        retval = true;
        /* expand file names */
        start = tostring();
        dirp = opendir( start );
        if( dirp != NULL ) {
            _splitpath2( start, pg.buffer, &pg.drive, &pg.dir, NULL, NULL );
            while( (dire = readdir( dirp )) != NULL ) {
                if( dire->d_attr & (_A_HIDDEN | _A_SYSTEM | _A_VOLID | _A_SUBDIR) )
                    continue;
                _makepath( pathin, pg.drive, pg.dir, dire->d_name, NULL );
                Token.this = pathin;            // dangerous?
                Token.len = strlen( pathin );
                if( !(*rtn)() ) {
                    Token.this = NULL;
                    Token.thumb = false;    // make _sure_ we don't use token.this
                    retval = false;
                    break;
                }
            }
            closedir( dirp );
        } else {
            retval = rtn();
        }
        _LnkFree( start );
    }
    return( retval );
#endif
}

static bool CheckFence( void )
/****************************/
/* check for a "fence", and skip it if it is there */
{
    if( Token.thumb ) {
        if( Token.quoted )
            return( false );   /* no fence inside quotes */
        if( *Token.this == '}' ) {
            Token.this++;
            return( true );
        }
    } else {
        return( GetToken( SEP_RCURLY, TOK_NORMAL ) );
    }
    return( false );
}

bool ProcArgListEx( bool (*rtn)( void ), tokcontrol ctrl, cmdfilelist *resetpoint )
/*********************************************************************************/
{
    bool bfilereset = false;    /* did we open a file and get reset ? */

    if( GetTokenEx( SEP_LCURLY, ctrl, resetpoint, &bfilereset ) ) {
        for( ;; ) {
            if( !WildCard( rtn, ctrl ) ) {
                return( false );
            }
            if( CheckFence() ) {
                break;
            } else if( !GetTokenEx( SEP_NO, ctrl, resetpoint, &bfilereset ) ) {
                LnkMsg( LOC+LINE+ERR+MSG_BAD_CURLY_LIST, NULL );
                break;
            }
        }
    } else {
        if( resetpoint != NULL && bfilereset )
            return( true );
        if( !GetTokenEx( SEP_NO, ctrl, resetpoint, &bfilereset ) )
            return( false );
        do {
            if( resetpoint != NULL && bfilereset )
                return( true );
            if( !WildCard( rtn, ctrl ) ) {
                return( false );
            }
        } while( GetTokenEx( SEP_COMMA, ctrl, resetpoint, &bfilereset ) );
    }
    return( true );
}

bool ProcArgList( bool (*rtn)( void ), tokcontrol ctrl )
{
    return( ProcArgListEx( rtn, ctrl ,NULL ) );
}

static bool procOne( parse_entry *entry, sep_type req, bool suicide, bool subset )
/********************************************************************************/
/* recognize token out of parse table, with required separator            */
/* return false if no separator, Suicide if not recognized (if suicide is */
/* true) otherwise use return code from action routine in matching entry  */
{
    const char          *key;
    const char          *ptr;
    size_t              len;
    bool                ret;
    char                keybuff[20];

    if( !GetToken( req, TOK_INCLUDE_DOT ) )
        return( false );
    ret = true;
    for( ; entry->keyword != NULL; ++entry ) {
        if( subset && (entry->flags & CF_SUBSET) == 0 ) {
            continue;
        }
        key = entry->keyword;
        ptr = Token.this;
        len = Token.len;
        for( ;; ) {
            if( len == 0 && !isupper( *key ) ) {
                if( HintFormat( entry->format ) ) {
                    ret = (*entry->rtn)();
                    CmdFlags |= entry->flags & ~CF_SUBSET;
                } else {
                    strcpy( keybuff, entry->keyword );
                    strlwr( keybuff );
                    LnkMsg( LOC+LINE+WRN+MSG_FORMAT_BAD_OPTION, "s", keybuff );
                }
                return( ret );
            }
            if( *key == '\0' || tolower( *ptr ) != tolower( *key ) )
                break;
            ptr++;
            key++;
            len--;
        }
        /* here if this is no match */
    }
    /* here if no match in table */
    if( suicide ) {
        Syntax();
    } else {
        Token.thumb = true;         /*  try again later */
        ret = false;
    }
    return( ret );
}

bool ProcOne( parse_entry *entry, sep_type req )
/**********************************************/
{
    return( procOne( entry, req, false, false ) );
}

bool ProcOneSubset( parse_entry *entry, sep_type req )
/****************************************************/
{
    return( procOne( entry, req, false, true ) );
}

bool ProcOneSuicide( parse_entry *entry, sep_type req )
/*****************************************************/
{
    return( procOne( entry, req, true, false ) );
}

bool MatchOne( parse_entry *entry, sep_type req, const char *match, size_t match_len )
/************************************************************************************/
/* recognize token out of parse table */
{
    const char          *key;
    const char          *ptr;
    size_t              len;

    /* unused parameters */ (void)req;

    for( ; entry->keyword != NULL; ++entry ) {
        key = entry->keyword;
        ptr = match;
        len = match_len;
        for( ;; ) {
            if( len == 0 && !isupper( *key ) ) {
                return( true );
            }
            if( *key == '\0' || tolower( *ptr ) != tolower( *key ) )
                break;
            ptr++;
            key++;
            len--;
        }
        /* here if this is no match */
    }

    /* here if no match in table */
    return( false );
}

ord_state getatoi( unsigned_16 *pnt )
/***********************************/
{
    unsigned_32 value;
    ord_state   retval;

    retval = getatol( &value );
    if( retval == ST_IS_ORDINAL ) {
        if( value > 0xffff ) {
            return( ST_INVALID_ORDINAL );
        }
        *pnt = (unsigned_16)value;
    }
    return( retval );
}

ord_state getatol( unsigned_32 *pnt )
/***********************************/
{
    const char          *p;
    size_t              len;
    unsigned long       value;
    unsigned            radix;
    bool                isvalid;
    bool                isdig;
    bool                gotdigit;
    int                 ch;

    len = Token.len;
    if( len == 0 )
        return( ST_NOT_ORDINAL );
    p = Token.this;
    gotdigit = false;
    value = 0;
    radix = 10;
    if( len >= 2 && *p == '0' ) {
        --len;
        ++p;
        if( tolower( *(unsigned char *)p ) == 'x' ) {
            radix = 16;
            ++p;
            --len;
        }
    }
    for( ; len != 0; --len ) {
        ch = tolower( *(unsigned char *)p++ );
        if( ch == 'k' ) {               // constant of the form 64k
            if( len > 1 || !gotdigit ) {
                return( ST_NOT_ORDINAL );
            } else {
                value <<= 10;           // value = value * 1024;
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
            gotdigit = true;
        }
    }
    *pnt = (unsigned_32)value;
    return( ST_IS_ORDINAL );
}

bool HaveEquals( tokcontrol ctrl )
/********************************/
{
    if( !GetToken( SEP_EQUALS, ctrl ) ) {
        Token.this = Token.next;
        /* collect the token that caused the problem */
        GetToken( SEP_NO, ctrl );
        return( false );
    }
    return( true );
}

bool GetLong( unsigned_32 *addr )
/*******************************/
{
    unsigned_32     value;
    ord_state       ok;

    if( !HaveEquals( TOK_NORMAL ) )
        return( false );
    ok = getatol( &value );
    if( ok != ST_IS_ORDINAL ) {
        return( false );
    } else {
        *addr = value;
    }
    return( true );
}

char *tostring( void )
/********************/
// make the current token into a C string.
{
    return( ChkToString( Token.this, Token.len ) );
}

char *totext( void )
/******************/
/* get a possiblly quoted string */
{
    Token.thumb = true;
    if( !GetToken( SEP_NO, TOK_NORMAL ) ) {
        GetToken( SEP_NO, TOK_INCLUDE_DOT );
    }
    return( tostring() );
}

static unsigned ParseNumber( const char *str, int radix, int *shift )
/*******************************************************************/
/* read a (possibly hexadecimal) number */
{
    bool        isdig;
    bool        isvalid;
    int         ch;
    int         size;
    unsigned    value;

    size = 0;
    value = 0;
    for( ;; ) {
        ch = tolower( *(unsigned char *)str );
        isdig = ( isdigit( ch ) != 0 );
        if( radix == 8 ) {
            isvalid = ( isdig && ch != '8' && ch != '9' );
        } else {
            isvalid = ( isxdigit( ch ) != 0 );
        }
        if( !isvalid )
            break;
        value *= radix;
        if( isdig ) {
            value += ch - '0';
        } else {
            value += ch - 'a' + 10;
        }
        size++;
        str++;
    }
    *shift += size;
    return( value );
}

static void MapEscapeChar( void )
/*******************************/
/* turn the current character located at Token.next into a possibly unprintable
 * character using C escape codes */
{
    const char      *str;
    int             shift;
    int             c;

    shift = 2;
    str = Token.next + 1;
    switch( *(unsigned char *)str ) {
    case 'a':
        c = '\a';
        break;
    case 'b':
        c = '\b';
        break;
    case 'f':
        c = '\f';
        break;
    case 'n':
        c = '\n';
        break;
    case 'r':
        c = '\r';
        break;
    case 't':
        c = '\t';
        break;
    case 'v':
        c = '\v';
        break;
    case 'x':   /* '\x' */
        c = (unsigned char)ParseNumber( ++str, 16, &shift );
        break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        c = (unsigned char)ParseNumber( str, 8, &shift );
        break;
    default:
        c = *str;
        break;
    }
    *(char *)Token.next = c;
    str = Token.next + shift;
    memmove( (char *)Token.next + 1, str, strlen( str ) + 1 );
}

static unsigned MapDoubleByteChar( int c )
/****************************************/
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
        if( c > 0xFC )
            break;
        /* fall through */
    case CF_LANGUAGE_KOREAN:
        if( c > 0xFD )
            break;
        if( c < 0x81 )
            break;
        Token.next++;
        return( 1 );
    }
    return( 0 );
}

static bool MakeToken( tokcontrol ctrl, sep_type separator )
/**********************************************************/
{
    bool        quit;
    int         hmm;
    size_t      len;
    bool        forcematch;
    bool        hitmatch;
    bool        keepspecial;

    Token.this = Token.next;
    len = 0;
    quit = false;
    forcematch = (separator == SEP_QUOTE) || (separator == SEP_PAREN) || (separator == SEP_PERCENT);
    keepspecial = (separator == SEP_SPACE) || (separator == SEP_DOT_EXT);
    if( separator == SEP_DOT_EXT ) {    /* KLUDGE! we want to allow a zero*/
        len--;                  /* length token for parsing wlib files, so */
        Token.next--;           /* artificially back up one here. */
    }
    if( *Token.next == '\\' && separator == SEP_QUOTE && (ctrl & TOK_IS_FILENAME) == 0 ) {
        MapEscapeChar();        /* get escape chars starting in 1st pos. */
    }
    hmm = *(unsigned char *)Token.next;
    len += MapDoubleByteChar( hmm );
    hitmatch = false;
    for( ;; ) {
        len++;
        hmm = *(unsigned char *)++Token.next;
        switch( hmm ) {
        case '\'':
            if( separator == SEP_QUOTE ) {
                ++Token.next;      // don't include end quote in next token.
                hitmatch = true;
                quit = true;
            }
            break;
        case ')':
            if( separator == SEP_PAREN ) {
                ++Token.next;    // don't include end paren in next token.
                hitmatch = true;
                quit = true;
            }
            break;
        case '%':
            if( separator == SEP_PERCENT ) {
                ++Token.next;    // don't include end percent in next token.
                hitmatch = true;
                quit = true;
            }
            break;
        case '.':
            if( (ctrl & TOK_INCLUDE_DOT) == 0 && !forcematch ) {
                quit = true;
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
            }
            /* fall through */
        case '\t':
        case ' ':
            if( !forcematch ) {
                quit = true;
            }
            break;
        case '\\':
            if( separator == SEP_QUOTE && (ctrl & TOK_IS_FILENAME) == 0 ) {
                MapEscapeChar();
            }
            break;
        case '\0':
        case '\r':
        case '\n':
        case CTRLZ:
            quit = true;
            break;
        default:
            len += MapDoubleByteChar( hmm );
        }
        if( quit ) {
            break;
        }
    }
    Token.len = len;
    if( forcematch && !hitmatch ) {
        return( false );     // end quote/paren not found before token end.
    }
    return( true );
}

static void ExpandEnvVariable( void )
/***********************************/
/* parse the specified environment variable & deal with it */
{
    char        *envname;
    const char  *env;
    char        *buff;
    size_t      envlen;

    Token.next++;
    if( !MakeToken( TOK_INCLUDE_DOT, SEP_PERCENT ) ) {
        LnkMsg( LOC+LINE+FTL+MSG_ENV_NAME_INCORRECT, NULL );
    }
    envname = tostring();
    env = GetEnvString( envname );
    if( env == NULL ) {
        LnkMsg( LOC+LINE+WRN+MSG_ENV_NOT_FOUND, "s", envname );
    } else {
        if( !IS_WHITESPACE( Token.next ) ) {
            MakeToken( TOK_INCLUDE_DOT, SEP_SPACE );
            envlen = strlen( env );
            _ChkAlloc( buff, envlen + Token.len + 1);
            memcpy( buff, env, envlen );
            memcpy( buff + envlen, Token.this, Token.len );
            buff[Token.len + envlen] = '\0';
            NewCommandSource( envname, buff, ENVIRONMENT );
            _LnkFree( buff );
        } else {
            NewCommandSource( envname, env, ENVIRONMENT );
        }
    }
    _LnkFree( envname );
}

static void OutPutPrompt( const char *str )
/*****************************************/
{
    if( QIsDevice( CmdFile->file ) ) {
        WriteStdOut( str );
    }
}

static void GetNewLine( void )
/****************************/
{
    switch( Token.how ) {
    case BUFFERED:
    case ENVIRONMENT:
    case SYSTEM:
        Token.where = MIDST;
        //go until next line found;
        for( ; *Token.next != '\n'; Token.next++ ) {
            if( *Token.next == '\0' || *Token.next == CTRLZ ) {
                Token.where = ENDOFFILE;
                break;
            }
        }
        Token.next++;
        break;
    case NONBUFFERED:
        if( QReadStr( CmdFile->file, Token.buff, MAX_REC, CmdFile->name ) ) {
            Token.where = ENDOFFILE;
        } else {
            Token.where = MIDST;
        }
        Token.next = Token.buff;
        break;
    default:
    case COMMANDLINE:
        Token.how = INTERACTIVE;
        /* fall through */
    case INTERACTIVE:
        /* interactive prompt with entry */
        OutPutPrompt( _LinkerPrompt );
        if( QReadStr( STDIN_FILENO, Token.buff, MAX_REC, "console" ) ) {
            Token.where = ENDOFCMD;
        } else {
            Token.where = MIDST;
        }
        Token.next = Token.buff;
        break;
    }
}

static void StartNewFile( void )
/******************************/
{
    char        *fname;
    const char  *envstring;
    f_handle    file;

    fname = FileName( Token.this, Token.len, E_COMMAND, false );
    file = QObjOpen( fname );
    if( file == NIL_FHANDLE ) {
        _LnkFree( fname );
        fname = tostring();
        envstring = GetEnvString( fname );
        if( envstring != NULL ) {
            NewCommandSource( fname, envstring, ENVIRONMENT );
            _LnkFree( fname );
        } else {
            LnkMsg( LOC+LINE+ERR+MSG_CANT_OPEN_NO_REASON, "s", fname );
            _LnkFree( fname );
            Suicide();
        }
    } else {
        SetCommandFile( file, fname );
        DEBUG(( DBG_OLD, "processing command file %s", fname ));
        _LnkFree( fname );
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
/************************/
/* return the parser to the previous command state */
{
    if( CmdFile->next == NULL )
        return;
    memcpy( &CmdFile->token, &Token, sizeof( tok ) );  /* save current state */
    CmdFile = CmdFile->next;
    memcpy( &Token, &CmdFile->token, sizeof( tok ) ); // restore old state.
}

bool GetTokenEx( sep_type req, tokcontrol ctrl, cmdfilelist *resetpoint, bool *pbreset )
/**************************************************************************************/
/* return true if no problem */
/* return false if problem   */
{
    char    hmm;
    bool    ret;
    bool    need_sep;

    if( Token.thumb ) {
        Token.thumb = false;
        if( Token.quoted )
            return( true );
        Token.next = Token.this;        /* re-process last token */
    }
    need_sep = true;
    for( ;; ) {                         /* finite state machine */

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

        if( Token.skipToNext && (req == SEP_COMMA) ) {
            Token.skipToNext = false;
            need_sep = false;
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
                }
                /* fall through */
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
                Token.quoted = false;
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
                Token.quoted = false;
                return( ret );
            case '%':
                if( req != SEP_SPACE ) {
                    ExpandEnvVariable();
                    break;
                }
                /* fall through */
            default:
                if( need_sep ) {
                    Token.quoted = false;
                    switch( req ) {
                    case SEP_NO:
                        if( hmm == ',' || hmm == '=' )
                            return( false );
                        break;
                    case SEP_COMMA:
                        if(hmm != ',' )
                            return( false);
                        Token.next++;
                        break;
                    case SEP_EQUALS:
                        if( hmm != '=' )
                            return( false );
                        Token.next++;
                        break;
                    case SEP_PERIOD:
                    case SEP_DOT_EXT:
                        if( hmm != '.' )
                            return( false );
                        Token.next++;
                        break;
                    case SEP_PAREN:
                        if( hmm != '(' )
                            return( false );
                        Token.next++;
                        break;
                    case SEP_LCURLY:
                        if( hmm != '{' )
                            return( false );
                        Token.next++;
                        break;
                    case SEP_QUOTE:
                        if( hmm != '\'' )
                            return( false );
                        Token.next++;
                        Token.quoted = true;
                        break;
                    case SEP_RCURLY:
                        if( hmm != '}' )
                            return( false );
                        Token.next++;
                        return( true );
                    case SEP_END:
                        return( false );
                    }
                    need_sep = false;
                    EatWhite();
                } else {                /*  must have good separator here */
                    if( hmm == '\'' && req != SEP_PAREN && req != SEP_SPACE ) {
                        req = SEP_QUOTE;   /* token has been quoted */
                        Token.next++;      /* don't include the quote */
                        Token.quoted = true;
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
            if( Token.locked )
                return( false );
            RestoreCmdLine();
            if( Token.thumb ) {
                Token.thumb = false;
                Token.next = Token.this;        /* re-process last token */
            }
            Token.quoted = false;
            if( resetpoint != NULL && (CmdFile == resetpoint) ) {
                if( *Token.next == ',' )
                    break;
                if( pbreset != NULL )
                    *pbreset = true;            /* Show we have hit a file end-point for a directive */
                return( false );
            }
            break;
        case ENDOFCMD:
            if( CmdFile->next != NULL ) {
                RestoreParser();
                break;
            }
            Token.quoted = false;
            ret = ( req == SEP_END );
            return( ret );
        }
    }
}

bool GetToken( sep_type req, tokcontrol ctrl )
/********************************************/
{
    return( GetTokenEx( req, ctrl, NULL, NULL ) );
}

static char *getCmdLine( void )
/*****************************/
{
    int     len;
    char    *cmdline;

    len = _bgetcmd( NULL, 0 ) + 1;
    _ChkAlloc( cmdline, len );
    if( cmdline != NULL ) {
        _bgetcmd( cmdline, len );
    }
    return( cmdline );
}

void NewCommandSource( const char *name, const char *buff, method how )
/*********************************************************************/
/* start reading from a new command source, and save the old one */
{
    cmdfilelist     *newfile;

    _ChkAlloc( newfile, sizeof( cmdfilelist ) );
    newfile->file = STDIN_FILENO;
    newfile->symprefix = NULL;
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
    if( name != NULL ) {
        newfile->name = ChkStrDup( name );
    } else {
        newfile->name = NULL;
    }
    newfile->token.how = how;
    if( how == NONBUFFERED ) {
        /* have to have at least this size */
        _ChkAlloc( newfile->token.buff, MAX_REC + 1 );
    } else if( buff != NULL ) {
        newfile->token.buff = ChkStrDup( buff );
    } else if( how == COMMANDLINE ) {
        newfile->token.buff = getCmdLine();
    } else {
        newfile->token.buff = NULL;
    }
    Token.how = newfile->token.how;
    Token.buff = newfile->token.buff;
    Token.next = newfile->token.buff;
    Token.where = MIDST;
    Token.line = 1;
    Token.thumb = false;
    Token.locked = false;
    Token.quoted = false;
}

void SetCommandFile( f_handle file, const char *fname )
/*****************************************************/
/* read input from given file */
{
    unsigned long   long_size;
    char            *buff;

    if( QIsDevice( file ) ) {
        long_size = 0x10000;
    } else {
        long_size = QFileSize( file );
    }
    buff = NULL;
    if( long_size < 0x10000 - 16 - 1 ) {       // if can alloc a chunk big enough
        size_t  size = (size_t)long_size;

        _LnkAlloc( buff, size + 1 );
        if( buff != NULL ) {
            size = QRead( file, buff, size, fname );
            if( size == IOERROR )
                size = 0;
            buff[size] = '\0';
            NewCommandSource( fname, buff, BUFFERED );
            _LnkFree( buff );
        }
    }
    if( buff == NULL ) {  // if couldn't buffer for some reason.
        NewCommandSource( fname, NULL, NONBUFFERED );
        Token.where = ENDOFLINE;
        Token.line++;
    }
    CmdFile->file = file;
}

void EatWhite( void )
/*******************/
{
    while( IS_WHITESPACE( Token.next ) ) {
        Token.next++;
    }
}

static void deleteCmdFile( cmdfilelist *cmdfile )
/***********************************************/
{
    f_handle    file;

    file = cmdfile->file;
    if( file != NIL_FHANDLE && file != STDIN_FILENO ) {
        QClose( file, cmdfile->name );
    }
    if( cmdfile->symprefix != NULL ) {
        _LnkFree( cmdfile->symprefix );
    }
    if( cmdfile->name != NULL ) {
        _LnkFree( cmdfile->name );
    }
    if( cmdfile->token.buff != NULL ) {
        _LnkFree( cmdfile->token.buff );
    }
    _LnkFree( cmdfile );
}

void RestoreCmdLine( void )
/*************************/
// Restore a saved command line.
{
    cmdfilelist     *temp;

    if( CmdFile->prev == NULL ) {     /* can't free last file */
        Token.where = ENDOFCMD;
        return;
    }
    // remove current cmdfile from linked list
    temp = CmdFile;
    CmdFile = CmdFile->prev;
    CmdFile->next = temp->next;
    if( CmdFile->next != NULL ) {
        CmdFile->next->prev = CmdFile;
    }
    // delete removed cmdfile
    deleteCmdFile( temp );
    // restore old state
    memcpy( &Token, &CmdFile->token, sizeof( tok ) );
}

void BurnUtils( void )
/********************/
// Burn data structures used in command utils.
{
    cmdfilelist     *temp;

    if( CmdFile->next != NULL ) {
        LnkMsg( LOC+LINE+ERR+MSG_NO_INPUT_LEFT, NULL );
    }
    while( (temp = CmdFile) != NULL ) {
        CmdFile = CmdFile->prev;
        deleteCmdFile( temp );
    }
    // so error message stuff reports right name
    Token.how = BUFFERED;
}

bool IsSystemBlock( void )
/************************/
// Are we in a system block?
{
    cmdfilelist     *temp;

    if( Token.how == SYSTEM )
        return( true );

    for( temp = CmdFile; temp != NULL; temp = temp->prev ) {
        if( temp->token.how == SYSTEM ) {
            return( true );
        }
    }
    return( false );
}

outfilelist *NewOutFile( char *filename )
/***************************************/
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
    InitBuffFile( fnode, filename, true );
    fnode->next = OutFiles;
    OutFiles = fnode;
    return( fnode );
}

char *GetFileName( char **membname, bool setname )
/************************************************/
{
    char        *ptr;
    size_t      namelen;
    char        *objname;
    const char  *fullmemb;
    size_t      memblen;

    DUPBUF_STACK( objname, Token.this, Token.len );
    namelen = Token.len;
    if( GetToken( SEP_PAREN, TOK_INCLUDE_DOT ) ) {   // got LIBNAME(LIB_MEMBER)
        fullmemb = GetBaseName( Token.this, Token.len, &memblen );
        *membname = ChkToString( fullmemb, memblen );
        ptr = FileName( objname, namelen, E_LIBRARY, false );
    } else {
        *membname = NULL;
        if( setname && Name == NULL ) {
            Name = ChkToString( objname, namelen );
        }
        ptr = FileName( objname, namelen, E_OBJECT, false );
    }
    return( ptr );
}

version_state GetGenVersion( version_block *vb, version_state enq, bool novell_revision )
/***************************************************************************************/
{
    version_state   state;
    ord_state       retval;
    unsigned_32     value;
    unsigned_32     major_limit;
    unsigned_32     minor_limit;
    unsigned_32     revision_limit;

    state = GENVER_ERROR;
    if( !GetToken( SEP_EQUALS, TOK_NORMAL ) ) {
        return( state );
    }
    /* finish if no value is required */
    if( (enq & (GENVER_MAJOR | GENVER_MINOR | GENVER_REVISION)) == 0 ) {
        return( state );
    }
    /* process major value */
    retval = getatol( &value );
    if( retval != ST_IS_ORDINAL ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", vb->message );
        return( state );
    }
    /* setup component limits */
    major_limit = vb->major;
    minor_limit = vb->minor;
    revision_limit = vb->revision;
    /* reset output values */
    vb->major = 0;
    vb->minor = 0;
    vb->revision = 0;
    /*
     *  From now on, all results are valid despite warnings
     */
    state = GENVER_MAJOR;
    if( enq & GENVER_MAJOR ) {
        if( ( major_limit ) && ( value > major_limit ) ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", vb->message );
            return( state );
        }
        vb->major = value;
    }
    /* finish if no value is required */
    if( (enq & (GENVER_MINOR | GENVER_REVISION)) == 0 ) {
        return( state );
    }
    /* process minor value */
    if( !GetToken( SEP_PERIOD, TOK_NORMAL ) ) { /* if we don't get a minor number */
        return( state );                        /* that's OK */
    }
    retval = getatol( &value );
    if( retval != ST_IS_ORDINAL ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", vb->message );
        return( state );
    }
    state |= GENVER_MINOR;
    if( enq & GENVER_MINOR ) {
        if( ( minor_limit ) && ( value > minor_limit ) ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", vb->message );
            return( state );
        }
        vb->minor = value;
    }
    /* finish if no value is required */
    if( (enq & GENVER_REVISION) == 0 ) {
        return( state );
    }
    /* process revision value */
    if( !GetToken( SEP_PERIOD, TOK_NORMAL ) ) { /* if we don't get a revision */
        return( state );                        /* that's all right */
    }
    retval = getatol( &value );
    if( novell_revision ) {
        /*
         * Netware supports a revision field 0-26 (null or a-z(A-Z))
         */
        if( retval == ST_NOT_ORDINAL && Token.len == 1 ) {
            value  = tolower( *(unsigned char *)Token.this ) - 'a' + 1;
        } else if( retval == ST_NOT_ORDINAL ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", vb->message );
            return( state );
        }
    } else {
        if( retval != ST_IS_ORDINAL ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", vb->message );
            return( state );
        }
    }
    state |= GENVER_REVISION;
    if( ( revision_limit ) && ( value > revision_limit ) ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", vb->message );
        return( state );
    }
    vb->revision = value;
    return( state );
}
