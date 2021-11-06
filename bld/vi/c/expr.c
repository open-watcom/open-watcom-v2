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


#include "vi.h"
#include <setjmp.h>
#include "expr.h"

static long _NEAR   cExpr1( void );
static long _NEAR   cExpr2( void );
static long _NEAR   cExpr3( void );
static long _NEAR   cExpr4( void );
static long _NEAR   cExpr5( void );
static long _NEAR   cExpr6( void );
static long _NEAR   cExpr7( void );
static long _NEAR   cExpr8( void );
static long _NEAR   cExpr9( void );
static long _NEAR   cExpr10( void );
static long _NEAR   cExpr11( void );
static long _NEAR   cExpr12( void );
static token        nextToken( void );

static char         wasString;
static char         lastString[TBUFF_SIZE];
static int          nextCh;
static token        currToken;
static char         tokenBuff[TBUFF_SIZE];
static long         constantVal;
static const char   *exprData;
static jmp_buf      abortAddr;
static int          tokenBuffCnt;

static const char   ColourTokens[] = {
    #define pick(a) #a "\0"
    COLOURTOKENS()
    #undef pick
};

#ifdef __WIN__
#define DDETOKENS() \
    pick( DDE_FACK,             0x8000 ) \
    pick( DDE_FBUSY,            0x4000 ) \
    pick( DDE_FDEFERUPD,        0x4000 ) \
    pick( DDE_FACKREQ,          0x8000 ) \
    pick( DDE_FRELEASE,         0x2000 ) \
    pick( DDE_FREQUESTED,       0x1000 ) \
    pick( DDE_FAPPSTATUS,       0x00ff ) \
    pick( DDE_FNOTPROCESSED,    0x0000 ) \
    pick( XTYP_CONNECT,         0x1062 ) \
    pick( XTYP_CONNECT_CONFIRM, 0x8072 ) \
    pick( XTYP_DISCONNECT,      0x80c2 ) \
    pick( XTYP_REQUEST,         0x20b0 ) \
    pick( XTYP_POKE,            0x4090 )

static const char DDETokens[] = {
    #define pick(t,n)   #t "\0"
    DDETOKENS()
    #undef pick
};

static const unsigned long ddeNums[] = {
    #define pick(t,n)   n,
    DDETOKENS()
    #undef pick
};
#endif

/*
 * nextChar - fetch next character in buffer
 */
static void nextChar( void )
{
    if( exprData == NULL ) {
        nextCh = '\0';
    } else {
        nextCh = *exprData++;
        if( nextCh == '\0' ) {
            exprData = NULL;
        }
    }
}

/*
 * StartExprParse - get read to parse an expression
 */
void StartExprParse( const char *data, jmp_buf abort_addr )
{
    exprData = data;
    memcpy( abortAddr, abort_addr, sizeof( jmp_buf ) );
    nextChar();
    nextToken();

} /* StartExprParse */

static void abortExpr( vi_rc err )
{
    longjmp( abortAddr, (int)err );
}

/*
 * _nextToken - get the next raw token
 */
static token _nextToken( void )
{
    char        ch;

    tokenBuffCnt = 0;
    for( ;; ) {
        ch = nextCh;
        if( ch == '\0' ) {
            nextChar();
            tokenBuff[tokenBuffCnt] = '\0';
            if( tokenBuffCnt == 0 ) {
                return( T_EOF );
            }
            break;
        }
        if( isspace( ch ) ) {
            nextChar();
            if( tokenBuffCnt > 0 ) {
                break;
            }
            continue;
        }
        if( ch == '"' ) {
            if( tokenBuffCnt == 0 ) {
                for( ;; ) {
                    nextChar();
                    if( nextCh == '"' || nextCh == '\0' ) {
                        nextChar();
                        tokenBuff[tokenBuffCnt] = '\0';
                        return( T_STRING );
                    }
                    tokenBuff[tokenBuffCnt++] = nextCh;
                }
            }
            break;
        }
        if( ch == '%' ) {
            if( tokenBuffCnt == 0 ) {
                nextChar();
                return( T_PERCENT );
            }
            break;
        }
        if( ch == '+' ) {
            if( tokenBuffCnt == 0 ) {
                nextChar();
                return( T_PLUS );
            }
            break;
        }
        if( ch == '^' ) {
            if( tokenBuffCnt == 0 ) {
                nextChar();
                return( T_XOR );
            }
            break;
        }
        if( ch == '~' ) {
            if( tokenBuffCnt == 0 ) {
                nextChar();
                return( T_TILDE );
            }
            break;
        }
        if( ch == '-' ) {
            if( tokenBuffCnt == 0 ) {
                nextChar();
                return( T_MINUS );
            }
            break;
        }
        if( ch == '*' ) {
            if( tokenBuffCnt == 0 ) {
                nextChar();
                if( nextCh == '*' ) {
                    nextChar();
                    return( T_EXPONENT );
                }
                return( T_TIMES );
            }
            break;
        }
        if( ch == '/' ) {
            if( tokenBuffCnt == 0 ) {
                nextChar();
                return( T_DIV );
            }
            break;
        }
        if( ch == '(' ) {
            if( tokenBuffCnt == 0 ) {
                nextChar();
                return( T_LEFT_PAREN );
            }
            break;
        }
        if( ch == ')' ) {
            if( tokenBuffCnt == 0 ) {
                nextChar();
                return( T_RIGHT_PAREN );
            }
            break;
        }
        if( ch == ':' ) {
            if( tokenBuffCnt == 0 ) {
                nextChar();
                return( T_COLON );
            }
            break;
        }
        if( ch == '?' ) {
            if( tokenBuffCnt == 0 ) {
                nextChar();
                return( T_QUESTION );
            }
            break;
        }
        if( ch == '>' || ch == '<' || ch == '=' || ch == '!' ||
            ch == '|' || ch == '&' ) {
            if( tokenBuffCnt > 0 ) {
                break;
            }
            nextChar();
            if( ch == '|' ) {
                if( nextCh == '|' ) {
                    nextChar();
                    return( T_OR_OR );
                }
                return( T_OR );
            }
            if( ch == '&' ) {
                if( nextCh == '&' ) {
                    nextChar();
                    return( T_AND_AND );
                }
                return( T_AND );
            }
            if( ch == '>' ) {
                if( nextCh == '=' ) {
                    nextChar();
                    return( T_GE );
                }
                if( nextCh == '>' ) {
                    nextChar();
                    return( T_RSHIFT );
                }
                return( T_GT );
            }
            if( ch == '<' ) {
                if( nextCh == '=' ) {
                    nextChar();
                    return( T_LE );
                }
                if( nextCh == '<' ) {
                    nextChar();
                    return( T_LSHIFT );
                }
                return( T_LT );
            }
            if( ch == '!' ) {
                if( nextCh == '=' ) {
                    nextChar();
                    return( T_NE );
                }
                return( T_EXCLAMATION );
            }
            if( ch == '=' ) {
                if( nextCh == '=' ) {
                    nextChar();
                    return( T_EQ );
                }
            }
        }
        tokenBuff[tokenBuffCnt++] = ch;
        nextChar();
    }
    tokenBuff[tokenBuffCnt] = '\0';
    return( T_UNKNOWN );

} /* _nextToken */

/*
 * nextToken - fetch the next real token from the buffer
 */
static token nextToken( void )
{
    int         j;
    char        *endptr;
    char        tmpstr[MAX_STR];

    currToken = _nextToken();
    if( currToken == T_UNKNOWN ) {
        currToken = T_CONSTANT;
        if( isdigit( tokenBuff[0] ) ) {
            constantVal = strtol( tokenBuff, &endptr, 10 );
            if( (endptr - tokenBuff) != tokenBuffCnt ) {
                constantVal = strtol( tokenBuff, &endptr, 16 );
                if( (endptr - tokenBuff) != tokenBuffCnt ) {
                    abortExpr( ERR_INVALID_VALUE );
                }
            }
        } else {
            if( tokenBuff[0] == '.' ) {
                strcpy( tokenBuff, GetASetVal( &tokenBuff[1], tmpstr ) );
                constantVal = strtol( tokenBuff, NULL, 0 );
                for( j = tokenBuffCnt; j-- > 0; ) {
                    if( !isdigit( tokenBuff[j] ) ) {
                        currToken = T_STRING;
                        break;
                    }
                }
            } else if( strcmp( tokenBuff, "config" ) == 0 ) {
                constantVal = EditFlags.Color * 100 + EditFlags.BlackAndWhite * 10 +
                    EditFlags.Monocolor;
            } else if( strcmp( tokenBuff, "rdonly" ) == 0 ) {
                constantVal = CFileReadOnly();
            } else if( strcmp( tokenBuff, "lastrc" ) == 0 ) {
                constantVal = (long)LastRC;
            } else if( strcmp( tokenBuff, "pagelen" ) == 0 ) {
                constantVal = EditVars.WindMaxHeight;
            } else if( strcmp( tokenBuff, "endcolumn" ) == 0 ) {
                constantVal = EditVars.WindMaxWidth;
            } else if( strcmp( tokenBuff, "numundos" ) == 0 ) {
                if( UndoStack == NULL ) {
                    constantVal = 0;
                } else {
                    constantVal = UndoStack->current + 1;
                }
            } else if( strcmp( tokenBuff, "numredos" ) == 0 ) {
                if( UndoUndoStack == NULL ) {
                    constantVal = 0;
                } else {
                    constantVal = UndoUndoStack->current + 1;
                }
            } else if( strcmp( tokenBuff, "hassel" ) == 0 ) {
                constantVal = SelRgn.selected;
            } else if( strcmp( tokenBuff, "hasfile" ) == 0 ) {
                constantVal = (CurrentFile != NULL);
            } else if( strncmp( tokenBuff, "emptybuf", 8 ) == 0 ) {
                j = tokenBuff[8];
                constantVal = IsEmptySavebuf( j );
            } else if( (j = Tokenize( ColourTokens, tokenBuff, true )) != TOK_INVALID ) {
                constantVal = j;
#ifdef __WIN__
            } else if( (j = Tokenize( DDETokens, tokenBuff, true )) != TOK_INVALID ) {
                constantVal = ddeNums[j];
#endif
            } else {
                if( GetErrorTokenValue( &j, tokenBuff ) == ERR_NO_ERR ) {
                    constantVal = j;
                } else {
                    currToken = T_STRING;
                }
            }
        }
    }
    return( currToken );

} /* nextToken */

static void mustRecog( token t )
{
    if( currToken != t ) {
        if( t == T_RIGHT_PAREN ) {
            abortExpr( ERR_RE_UNMATCHED_ROUND_BRACKETS );
        }
        if( t == T_COLON ) {
            abortExpr( ERR_EXPECTING_COLON );
        }
    }
    nextToken();
}

long GetConstExpr( void )
{
    long value, value1, value2;

    value = cExpr1();
    if( currToken == T_QUESTION ) {
        nextToken();
        value1 = GetConstExpr();
        mustRecog( T_COLON );
        value2 = GetConstExpr();
        if( value != 0 ) {
            value = value1;
        } else {
            value = value2;
        }
    }
    return( value );
}

static long _NEAR cExpr1( void )
{
    long value;

    value = cExpr2();
    while( currToken == T_OR_OR ) {
        nextToken();
        value |= cExpr2();
    }
    return( value );
}

static long _NEAR cExpr2( void )
{
    long value;

    value = cExpr3();
    while( currToken == T_AND_AND ) {
        nextToken();
        value &= cExpr3();
    }
    return( value );
}

static long _NEAR cExpr3( void )
{
    long value;

    value = cExpr4();
    while( currToken == T_OR ) {
        nextToken();
        value |= cExpr4();
    }
    return( value );
}

static long _NEAR cExpr4( void )
{
    long value;

    value = cExpr5();
    while( currToken == T_XOR ) {
        nextToken();
        value = value ^ cExpr5();
    }
    return( value );
}

static long _NEAR cExpr5( void )
{
    long value;

    value = cExpr6();
    while( currToken == T_AND ) {
        nextToken();
        value &= cExpr6();
    }
    return( value );
}

static int doCompare( long val1, long (_NEAR *fn)( void ) )
{
    char        tmp1[TBUFF_SIZE];
    char        tmp2[TBUFF_SIZE];
    bool        cmp_str;
    long        val2;

    cmp_str = false;
    if( wasString ) {
        strcpy( tmp1, lastString );
        cmp_str = true;
    }
    nextToken();
    val2 = fn();
    if( wasString ) {
        strcpy( tmp2, lastString );
        if( !cmp_str ) {
            sprintf( tmp1, "%ld", val1 );
        }
        cmp_str = true;
    } else if( cmp_str ) {
        sprintf( tmp2, "%ld", val2 );
    }
    if( cmp_str ) {
        return( strcmp( tmp1, tmp2 ) );
    }
    if( val1 == val2 ) {
        return( 0 );
    }
    if( val1 > val2 ) {
        return( 1 );
    }
    // val1 < val2
    return( -1 );
}

static long _NEAR cExpr6( void )
{
    long value;

    value = cExpr7();
    for( ;; ) {
        if( currToken == T_EQ ) {
            value = ( doCompare( value, cExpr7 ) == 0 );
        } else if( currToken == T_NE ) {
            value = ( doCompare( value, cExpr7 ) != 0 );
        } else {
            break;
        }
    }
    return( value );
}

static long _NEAR cExpr7( void )
{
    long value;

    value = cExpr8();
    for( ;; ) {
        if( currToken == T_LT ) {
            value = ( doCompare( value, cExpr8 ) < 0 );
        } else if( currToken == T_LE ) {
            value = ( doCompare( value, cExpr8 ) <= 0 );
        } else if( currToken == T_GT ) {
            value = ( doCompare( value, cExpr8 ) > 0 );
        } else if( currToken == T_GE ) {
            value = ( doCompare( value, cExpr8 ) >= 0 );
        } else {
            break;
        }
    }
    return( value );
}

static long _NEAR cExpr8( void )
{
    long value;

    value = cExpr9();
    for( ;; ) {
        if( currToken == T_RSHIFT ) {
            nextToken();
            value >>= cExpr9();
        } else if( currToken == T_LSHIFT ) {
            nextToken();
            value <<= cExpr9();
        } else {
            break;
        }
    }
    return( value );
}

static long _NEAR cExpr9( void )
{
    long value;

    value = cExpr10();
    for( ;; ) {
        if( currToken == T_PLUS ) {
            nextToken();
            value += cExpr10();
        } else if( currToken == T_MINUS ) {
            nextToken();
            value -= cExpr10();
        } else {
            break;
        }
    }
    return( value );
}

static long _NEAR cExpr10( void )
{
    long value;
    long temp;

    value = cExpr11();
    for( ;; ) {
        if( currToken == T_TIMES ) {
            nextToken();
            value *= cExpr11();
        } else if( currToken == T_DIV ) {
            nextToken();
            temp = cExpr11();
            if( temp == 0 ) {
                value = 0;
            } else {
                value /= temp;
            }
        } else if( currToken == T_PERCENT ) {
            nextToken();
            temp = cExpr11();
            if( temp == 0 ) {
                value = 0;
            } else {
                value %= temp;
            }
        } else {
            break;
        }
    }
    return( value );
}

static long _NEAR cExpr11( void )
{
    long        value, v2, v3;
    long        i;

    value = cExpr12();
    while( currToken == T_EXPONENT ) {
        nextToken();
        v2 = cExpr2();
        v3 = 1;
        for( i = 0; i < v2; i++ ) {
            v3 *= value;
        }
        value = v3;
    }
    return( value );
}


static long _NEAR cExpr12( void )
{
    long value;

    value = 0;
    wasString = 0;
    switch( currToken ) {
    case T_PLUS:
        nextToken();
        value = cExpr12();
        break;
    case T_MINUS:
        nextToken();
        value = -cExpr12();
        break;
    case T_EXCLAMATION:
        nextToken();
        value = !cExpr12();
        break;
    case T_TILDE:
        nextToken();
        value = ~cExpr12();
        break;
    case T_LEFT_PAREN:
        nextToken();
        value = GetConstExpr();
        mustRecog( T_RIGHT_PAREN );
        break;
    case T_CONSTANT:
        value = constantVal;
        nextToken();
        break;
    case T_STRING:
        value = 0;
        wasString = 1;
        strcpy( lastString, tokenBuff );
        nextToken();
        break;
    }
    return( value );
}
