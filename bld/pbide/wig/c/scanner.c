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


#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "global.h"
#include "types.h"
#include "list.h"
#include "sruinter.h"
#include "ytab.h"
#include "error.h"
#include "mem.h"
#include "filelist.h"
#include "keywords.h"

#define ID_BUFLEN               (8*1024)
#define BUF_SIZE_INCREMENT      1024

extern YYSTYPE yylval;

static BOOL     atEOF;                  /* have we reached end of file  */
static BOOL     yyLineFini;             /* have we finished a line      */
static unsigned yyTextSize;             /* size of current token buffer */
static unsigned yyTextLen;              /* length of current token      */
static char     *yyText;                /* token buffer                 */
static unsigned yyLineLen;              /* Current line size            */
static unsigned yyLineSize;             /* size of line buffer          */
static char     *yyLine;                /* Current line buffer          */
static FILE     *yyFhdl;                /* Current file being read      */
static char     *yyFileName;            /* name of Current File         */

// This rotating destruction scheme will eventually cause a bug and should
// be reimplemented
static char     idBuffer[ID_BUFLEN];    /* delayed token destroy buffer */
static int      idPtr;                  /* pointer to current token     */

static void addToTok( char ch ) {
/********************************/
    if( yyTextLen == yyTextSize ) {
        yyTextSize += BUF_SIZE_INCREMENT;
        yyText = MemRealloc( yyText, yyTextSize );
    }
    yyText[ yyTextLen ] = ch;
    yyTextLen ++;

}
static void putBackChar( int c ) {
/***********************************/

    ungetc( c, yyFhdl );
    yyLineLen--;
}


static int nextChar( void ) {
/********************************/

    int         c;

    assert( yyFhdl );

    errno = 0;
    c = fgetc( yyFhdl );
    if( ( c == EOF ) && errno ) {
        Error( FILE_READ_ERR, yyFileName );
    }
    if( yyLineLen == yyLineSize ) {
        yyLineSize += BUF_SIZE_INCREMENT;
        yyLine = MemRealloc( yyLine, yyLineSize );
    }
    yyLine[ yyLineLen ] = c;
    yyLineLen++;
    return( c );
}

static char *add2IdBuffer( char *src ) {
/**************************************/

    int         len;

    assert( src );

    /* add token to identifier buffer for delayed destruction */

    len = strlen( src ) + 1;
    if( idPtr + len >= ID_BUFLEN ) {
        idPtr = 0;
    }
    strcpy( idBuffer + idPtr, src );
    idPtr += len;
    return( idBuffer + idPtr - len );
}

BOOL InitLex( char *fname ) {
/****************************/

    yyLineFini = FALSE;
    atEOF = FALSE;

    yyTextSize = BUF_SIZE_INCREMENT;
    yyTextLen = 0;
    yyText = MemMalloc( BUF_SIZE_INCREMENT );
    *yyText = '\0';

    yyLineLen = 0;
    yyLineSize = 0;
    yyLine = MemMalloc( BUF_SIZE_INCREMENT );
    *yyLine = '\0';

    memset( idBuffer, 0, sizeof( idBuffer ) );
    idPtr = 0;

    yyFhdl= WigOpenFile( fname, "rt" );
    if( yyFhdl == NULL ) {
        Error( FILE_OPEN_ERR, fname );
    }
    yyFileName = MemStrDup( fname );
    return( FALSE );
}

void FiniLex( void ) {
/*********************/

    fclose( yyFhdl );
    MemFree( yyFileName );
    MemFree( yyLine );
    MemFree( yyText );

    yyTextSize = 0;
    yyText = NULL;
    yyLineLen = 0;
    yyLine = NULL;
}

int compKeywords( char *p1, keyword *p2 ) {
/******************************************/

    return( stricmp( p1, p2->key ) );
}

id_type checkKeyWord( char *text ) {
/***********************************/

    keyword             *item;

    item = bsearch( text, Statements,
                    GetStmntCnt() - 1, sizeof( keyword ), compKeywords );
    if( item == NULL ) {
        return( ID_IDENTIFIER );
    } else {
        return( item->id );
    }
}

enum {
    S_COMMENT_END,
    S_ERROR,
    S_IDENT,
    S_INT,
    S_SLASH,
    S_SLASH_SLASH,
    S_SLASH_STAR,
    S_START,
    S_STRING,
    S_STRING_END
};

int yylex() {
/************/

    unsigned    state;
    char        ch;
    unsigned    len;
    int         ret;

    yyLineFini = FALSE;
    yyTextLen = 0;
    if( atEOF ) return( 0 );    //yacc wants 0 for EOF

    len = 0;
    ch = nextChar();
    state = S_START;
    for( ;; ) {
        switch( state ) {
        case S_START:
            if( isalpha( ch ) ) {
                state = S_IDENT;
            } else if( isdigit( ch ) ) {
                state = S_INT;
            } else if( ch == '\n' || ch == ';' ) {
                return( 0 );    // we parse on a line by line basis so
                                // these characters are treated like EOF
            } else if( isspace( ch ) ) {
                ch = nextChar(); // strip off white space
            } else {
                switch( ch ) {
                case '.': return( ST_PERIOD );
                case ':': return( ST_COLON );
                case ',': return( ST_COMMA );
                case '[': return( ST_LSQ_BRACKET );
                case ']': return( ST_RSQ_BRACKET );
                case '(': return( ST_LBRACKET );
                case ')': return( ST_RBRACKET );
                case '=': return( ST_EQ );
                case '$': return( ST_DOLLAR );
                case '!': return( ST_EXCLAM );
                case '/':
                    state = S_SLASH;
                    break;
                case '\"':
                    state = S_STRING;
                    break;
                case EOF:
                    atEOF = TRUE;
                    return( FI_EOF );
                default:
                    state = S_ERROR;
                    break;
                }
            }
            break;
        case S_INT:
            while( isdigit( ch ) ) {
                addToTok( ch );
                ch = nextChar();
            }
            putBackChar( ch );
            addToTok( '\0' );
            yylval.o_value = atoi( yyText );
            return( CT_INTEGER );
            break;
        case S_IDENT:
            while( isalnum( ch ) || ch == '_' || ch == '-' || ch == '$'
                   || ch == '#' || ch == '%' ) {
                addToTok( ch );
                ch = nextChar();
            }
            putBackChar( ch );
            addToTok( '\0' );
            ret = checkKeyWord( yyText );
            if( ret == ID_IDENTIFIER ) {
                yylval.o_string = add2IdBuffer( yyText );
            } else {
                yylval.o_type = ret;
            }
            return( ret );
        case S_STRING:
            ch = nextChar();
            while( ch != '\"' ) {
                if( ch == '\n' || ch == EOF ) {
                    state = S_ERROR;
                    break;
                } else if( ch == '~' ) {        //~ is the PB escape character
                    addToTok( ch );
                    ch = nextChar();
                }
                addToTok( ch );
                ch = nextChar();
            }
            if( state != S_ERROR ) {
                addToTok( '\0' );
                yylval.o_string = add2IdBuffer( yyText );
                return( CT_STRING );
            } else {
                putBackChar( ch );
            }
            break;
        case S_SLASH:
            ch = nextChar();
            if( ch == '/' ) {
                state = S_SLASH_SLASH;
            } else if( ch == '*' ) {
                state = S_SLASH_STAR;
            } else {
                putBackChar( ch );
                state = S_ERROR;
            }
            break;
        case S_SLASH_SLASH:
            ch = nextChar();
            while( ch != '\n' && ch != EOF ) ch = nextChar();
            putBackChar( ch );
            return( ST_COMMENT );
            break;
        case S_SLASH_STAR:
            ch = nextChar();
            while( ch != '*' && ch != EOF ) ch = nextChar();
            if( ch == EOF ) {
                state = S_ERROR;
                putBackChar( ch );
            } else {
                state = S_COMMENT_END;
            }
            break;
        case S_COMMENT_END:
            ch = nextChar();
            while( ch == '*' ) ch = nextChar();
            if( ch != '/' ) state = S_SLASH_STAR;
            return( ST_COMMENT );
        case S_ERROR:
#ifdef DEBUG
            Error( SCANNER_ERROR );
#endif
            return( 0 );
            break;
        }
    }
}

char *GetParsedLine( void ) {
/***************************/

    return( yyLine );
}


BOOL LineFinished( void ) {
/*************************/

    return( yyLineFini );
}


void    FinishLine( void ) {
/**************************/

    /* if necessary terminate line */
    if( yyLineLen ) {
        yyLine[ yyLineLen ] = 0;
        yyLineLen = 0;
    }
}


void    GetToEOS(void) {
/**********************/

    int         c;

    /* process to end of statement, used for error handling */

    do {
        c = nextChar();
    } while( ( c != ';' ) && ( c != '\n' ) && ( c != EOF ) );

    if( c != EOF ) {
        putBackChar( c );
        yyLineLen++;
    } else {
        yyLineLen--;
    }

    yyLine[ yyLineLen ] = 0;
}

