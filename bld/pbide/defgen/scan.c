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
#include <sys\types.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <io.h>
#include "defgen.h"
#include "ytab.h"
#include "scan.h"

/*
        T_NAME          :=  [A-Z|a-z|_][A-Z|a-z|0-9|_]*
        T_NUM           :=  [0-9]*
        T_LPAREN        :=  (
        T_RPAREN        :=  )
        T_LSQ_BRACKET   :=  [
        T_RSQ_BRACKET   :=  ]
        T_SEMICOLON     :=  ;
        T_STAR          :=  *
        T_COMMA         :=  ,
        T_POUND         :=  #
        T_SLASH         :=  /
*/

#define         BUF_SIZE        512
static int      FileHdl;
static char     Buffer[512];
static char     *BufEnd;
static char     *CurPos;

typedef struct {
    char        *str;
    int         tok;
} KeyWord;

static KeyWord  ReservedWords[] = {
    "__cdecl",          T_CDECL,
    "__export",         T_EXPORT,
    "__far",            T_FAR,
    "__fortran",        T_FORTRAN,
    "__huge",           T_HUGE,
    "__near",           T_NEAR,
    "__pascal",         T_PASCAL,
    "__syscall",        T_SYSCALL,
    "char",             T_CHAR,
    "const",            T_CONST,
    "double",           T_DOUBLE,
    "extern",           T_EXTERN,
    "float",            T_FLOAT,
    "int",              T_INT,
    "line",             T_LINE,
    "long",             T_LONG,
    "pascal",           T_PASCAL,
    "short",            T_SHORT,
    "signed",           T_SIGNED,
    "struct",           T_STRUCT,
    "union",            T_UNION,
    "unsigned",         T_UNSIGNED,
    "void",             T_VOID
};

static int GetNextChar( void ) {
    char        ch;
    int         size;

    if( CurPos == BufEnd ) {
        size = read( FileHdl, Buffer, BUF_SIZE );
        if( size == 0 || size == -1 ) {
            return( EOF );
        }
        BufEnd = Buffer + size;
        CurPos = Buffer;
    }
    ch = *CurPos;
    CurPos++;
    return( ch );
}

static void PutBackChar( void ) {
    CurPos--;
}

int MyComp( const void *p1, const void *p2 ) {

    KeyWord     *ptr;

    ptr = (KeyWord *)p2;
    return( strcmp( p1, ptr->str ) );
}

static int CheckReservedWords( char *tokbuf ) {
    KeyWord     *match;

    match = bsearch( tokbuf, ReservedWords,
                     sizeof( ReservedWords ) / sizeof( KeyWord ),
                     sizeof( KeyWord ), MyComp );
    if( match == NULL ) {
        return( T_NAME );
    } else {
        return( match->tok );
    }
}

BOOL ScanInit( char *fname ) {
    FileHdl = open( fname, O_RDONLY | O_TEXT );
    BufEnd = Buffer;
    CurPos = BufEnd;
    if( FileHdl == -1 ) {
        ReportError( "Unable to open '%s'", fname );
        return( TRUE );
    } else {
        return( FALSE );
    }
}

void ScanFini( void ) {
    close( FileHdl );
}

typedef enum {
    ST_START,
    ST_NAME,
    ST_NUM,
    ST_FNAME,
    ST_STRING,
    ST_ERR
} ScanState;

int Scan( ScanValue *val ) {
    ScanState   state;
    char        ch;
    unsigned    len;
    int         ret;
    char        token[256];     // BEWARE-assume no token is more than
                                //        256 characters

    state = ST_START;
    len = 0;
    for( ;; ) {
        switch( state ) {
        case ST_START:
            ch = GetNextChar();
            while( isspace( ch ) ) ch = GetNextChar();
            switch( ch ) {
            case '(':
                return( T_LPAREN );
            case ')':
                return( T_RPAREN );
            case '[':
                return( T_LSQ_BRACKET );
            case ']':
                return( T_RSQ_BRACKET );
            case ';':
                return( T_SEMICOLON );
            case '*':
                return( T_STAR );
            case ',':
                return( T_COMMA );
            case '#':
                return( T_POUND );
            case '/':
                return( T_SLASH );
            case '.':
                return( T_PERIOD );
            case EOF:
                return( 0 );
            case '"':
                state = ST_STRING;
                break;
            case '\\':
                state = ST_FNAME;
                token[len] = ch;
                len++;
                break;
            default:
                if( ch == '_' || isalpha( ch ) ) {
                    state = ST_NAME;
                    token[len] = ch;
                    len++;
                } else if( isdigit( ch ) ) {
                    state = ST_NUM;
                    token[len] = ch;
                    len++;
                } else {
                    state = ST_ERR;
                }
                break;
            }
            break;
        case ST_ERR:
            ReportError( "Scanner Error" );
            return( 0 );
            break;
        case ST_NAME:
            ch = GetNextChar();
            while( isalnum( ch ) || ch == '_' ) {
                token[len] = ch;
                len++;
                ch = GetNextChar();
            }
            if( ch == '.' || ch == ':' || ch == '\\' ) {
                state = ST_FNAME;
                token[len] = ch;
                len++;
            } else {
                token[len] = '\0';
                PutBackChar();
                ret = CheckReservedWords( token );
                val->str = malloc( strlen( token ) + 1 );
                strcpy( val->str, token );
                return( ret );
            }
            break;
        case ST_NUM:
            ch = GetNextChar();
            while( isdigit( ch ) ) {
                token[len] = ch;
                len++;
                ch = GetNextChar();
            }
            if( isalnum( ch ) || ch == '_' || ch == '.' || ch == '\\' ) {
                state = ST_FNAME;
                token[len] = ch;
                len++;
            } else {
                PutBackChar();
                token[len] = '\0';
                val->str = malloc( strlen( token ) + 1 );
                strcpy( val->str, token );
                return( T_NUM );
            }
            break;
        case ST_FNAME:
            ch = GetNextChar();
            while( isalnum( ch ) || ch == '_' || ch == '.' || ch == '\\'
                   || ch == ':' ) {
                token[len] = ch;
                len++;
                ch = GetNextChar();
            }
            token[len] = '\0';
            PutBackChar();
            val->str = malloc( strlen( token ) + 1 );
            strcpy( val->str, token );
            return( T_FNAME );
            break;
        case ST_STRING:
            ch = GetNextChar();
            while( ch != '"' ) {
                token[len] = ch;
                len++;
                ch = GetNextChar();
            }
            token[len] = '\0';
            val->str = malloc( strlen( token ) + 1 );
            strcpy( val->str, token );
            return( T_STRING );
            break;
        }
    }
}

#if(0)
int Scan( ScanValue *val ) {

    int         ch;
    char        tokbuf[256];    // BEWARE-assume no token is more than
                                //        256 characters
    unsigned    len;
    int         ret;

    ch = GetNextChar();
    while( isspace( ch ) ) ch = GetNextChar();
    if( ch == '_' || isalpha( ch ) ) {
        len = 0;
        while( ch == '_' || isalnum( ch ) ) {
            tokbuf[len] = (char)(ch & 0xFF);
            len++;
            ch = GetNextChar();
        }
        tokbuf[len] = '\0';
        PutBackChar();
        ret = CheckReservedWords( tokbuf );
        val->str = malloc( strlen( tokbuf ) + 1 );
        strcpy( val->str, tokbuf );
        return( ret );
    } else if( isdigit( ch ) ) {
        while( isdigit( ch ) ) {
            ch = GetNextChar();
        }
        PutBackChar();
        return( T_NUM );
    } else switch( ch ) {
        case '(':
            return( T_LPAREN );
        case ')':
            return( T_RPAREN );
        case '[':
            return( T_LSQ_BRACKET );
        case ']':
            return( T_RSQ_BRACKET );
        case ';':
            return( T_SEMICOLON );
        case '*':
            return( T_STAR );
        case ',':
            return( T_COMMA );
        case '#':
            return( T_POUND );
        case '/':
            return( T_SLASH );
        case EOF:
            return( 0 );
        default:
            //NYI - report an error
            return( 0 );
            break;
    }
}
#endif

