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


#include <stdlib.h>
#include <string.h>
#include "preproc.h"

void PP_ConstExpr( PPVALUE *val );
void PP_Expr1( PPVALUE *val );
void PP_Expr2( PPVALUE *val );
void PP_Expr3( PPVALUE *val );
void PP_Expr4( PPVALUE *val );
void PP_Expr5( PPVALUE *val );
void PP_Expr6( PPVALUE *val );
void PP_Expr7( PPVALUE *val );
void PP_Expr8( PPVALUE *val );
void PP_Expr9( PPVALUE *val );
void PP_Expr10( PPVALUE *val );
void PP_Expr11( PPVALUE *val );
void PP_AdvanceToken();
void PP_Identifier( PPVALUE *val );
long int PP_HexNumber();
void PP_CharConst( PPVALUE *val );
void PP_Constant( PPVALUE *val );
void PP_Constant( PPVALUE *val );

int PPEvalExpr( char *ptr, char **endptr, PPVALUE *val )
{
    int         value;

    PPTokenPtr = ptr;
    PPCurToken = NULL;
    PPFlags &= ~PPFLAG_PREPROCESSING;
    PPFlags &= ~PPFLAG_UNDEFINED_VAR;
    PPFlags |=  PPFLAG_DONT_READ;
    PP_ConstExpr( val );
    *endptr = PPTokenPtr;
    if( PPFlags & PPFLAG_UNDEFINED_VAR ) {
        value = 0;
    } else {
        value = 1;
    }
    return( value );
}


void PP_ConstExpr( PPVALUE *val )
{
    PPVALUE     value1;
    PPVALUE     value2;

    PP_Expr1( val );
    if( PPTokenPtr[0] == '?' ) {
        ++PPTokenPtr;
        PP_ConstExpr( &value1 );
        if( PPTokenPtr[0] == ':' ) {
            ++PPTokenPtr;
        } else {
            // error
        }
        PP_ConstExpr( &value2 );
/*          value = value ? value1 : value2;  */
        if( val->val.ivalue != 0 ) {
            val->val.ivalue = value1.val.ivalue;
            val->type   = value1.type;
        } else {
            val->val.ivalue = value2.val.ivalue;
            val->type   = value2.type;
        }
    }
}


void PP_Expr1( PPVALUE *val )
{
    PPVALUE     val2;

    PP_Expr2( val );
    while( PPTokenPtr[0] == '|'  &&  PPTokenPtr[1] == '|' ) {
        PPTokenPtr += 2;
        PP_Expr2( &val2 );
        val->val.ivalue |= val2.val.ivalue;
        val->type = 0;
    }
}


void PP_Expr2( PPVALUE *val )
{
    PPVALUE     val2;

    PP_Expr3( val );
    while( PPTokenPtr[0] == '&'  &&  PPTokenPtr[1] == '&' ) {
        PPTokenPtr += 2;
        PP_Expr3( &val2 );
        val->val.ivalue &= val2.val.ivalue;
        val->type = 0;
    }
}


void PP_Expr3( PPVALUE *val )
{
    PPVALUE     val2;

    PP_Expr4( val );
    while( PPTokenPtr[0] == '|'  &&  PPTokenPtr[1] != '|' ) {
        ++PPTokenPtr;
        PP_Expr4( &val2 );
        val->val.ivalue |= val2.val.ivalue;
        val->type |= val2.type;
    }
}


void PP_Expr4( PPVALUE *val )
{
    PPVALUE     val2;

    PP_Expr5( val );
    while( PPTokenPtr[0] == '^' ) {
        ++PPTokenPtr;
        PP_Expr5( &val2 );
        val->val.ivalue ^= val2.val.ivalue;
        val->type |= val2.type;
    }
}


void PP_Expr5( PPVALUE *val )
{
    PPVALUE     val2;

    PP_Expr6( val );
    while( PPTokenPtr[0] == '&'  &&  PPTokenPtr[1] != '&' ) {
        ++PPTokenPtr;
        PP_Expr6( &val2 );
        val->val.ivalue &= val2.val.ivalue;
        val->type |= val2.type;
    }
}


void PP_Expr6( PPVALUE *val )
{
    PPVALUE     val2;

    PP_Expr7( val );
    for(;;) {
        if( PPTokenPtr[0] == '='  &&  PPTokenPtr[1] == '=' ) {
            PPTokenPtr += 2;
            PP_Expr7( &val2 );
            val->val.ivalue = val->val.ivalue == val2.val.ivalue;
            val->type = 0;
        } else if( PPTokenPtr[0] == '!'  &&  PPTokenPtr[1] == '=' ) {
            PPTokenPtr += 2;
            PP_Expr7( &val2 );
            val->val.ivalue = val->val.ivalue != val2.val.ivalue;
            val->type = 0;
        } else {
            break;
        }
    }
}


void PP_Expr7( PPVALUE *val )
{
    PPVALUE     val2;

    PP_Expr8( val );
    for(;;) {
        if( PPTokenPtr[0] == '<' ) {
            ++PPTokenPtr;
            PP_Expr8( &val2 );
            val->type |= val2.type;
            if( PPTokenPtr[0] == '=' ) {
                ++PPTokenPtr;
                val->val.ivalue = val->val.ivalue <= val2.val.ivalue;
            } else {
                val->val.ivalue = val->val.ivalue < val2.val.ivalue;
            }
        } else if( PPTokenPtr[0] == '>' ) {
            ++PPTokenPtr;
            PP_Expr8( &val2 );
            val->type |= val2.type;
            if( PPTokenPtr[0] == '=' ) {
                ++PPTokenPtr;
                if( val->type == 0 ) {
                    val->val.ivalue = val->val.ivalue >= val2.val.ivalue;
                } else {
                    val->val.uvalue = val->val.uvalue >= val2.val.uvalue;
                }
            } else {
                if( val->type == 0 ) {
                    val->val.ivalue = val->val.ivalue > val2.val.ivalue;
                } else {
                    val->val.uvalue = val->val.uvalue > val2.val.uvalue;
                }
            }
        } else {
            break;
        }
    }
}


void PP_Expr8( PPVALUE *val )
{
    PPVALUE     val2;

    PP_Expr9( val );
    for(;;) {
        if( PPTokenPtr[0] == '>'  &&  PPTokenPtr[1] == '>' ) {
            PPTokenPtr += 2;
            PP_Expr9( &val2 );
            val->type |= val2.type;
            if( val->type == 0 ) {
                val->val.ivalue >>= val2.val.uvalue;
            } else {
                val->val.uvalue >>= val2.val.uvalue;
            }
        } else if( PPTokenPtr[0] == '<'  &&  PPTokenPtr[1] == '<' ) {
            PPTokenPtr += 2;
            PP_Expr9( &val2 );
            val->val.ivalue <<= val2.val.uvalue;
            val->type |= val2.type;
        } else {
            break;
        }
    }
}


void PP_Expr9( PPVALUE *val )
{
    PPVALUE     val2;

    PP_Expr10( val );
    for(;;) {
        if( PPTokenPtr[0] == '+' ) {
            ++PPTokenPtr;
            PP_Expr10( &val2 );
            val->val.ivalue += val2.val.ivalue;
            val->type |= val2.type;
        } else if( PPTokenPtr[0] == '-' ) {
            ++PPTokenPtr;
            PP_Expr10( &val2 );
            val->val.ivalue -= val2.val.ivalue;
            val->type |= val2.type;
        } else {
            break;
        }
    }
}


void PP_Expr10( PPVALUE *val )
{
    PPVALUE     val2;

    PP_Expr11( val );
    for( ;; ) {
        if( PPTokenPtr[0] == '*' ) {
            ++PPTokenPtr;
            PP_Expr11( &val2 );
            val->val.ivalue *= val2.val.ivalue;
            val->type |= val2.type;
        } else if( PPTokenPtr[0] == '/' ) {
            ++PPTokenPtr;
            PP_Expr11( &val2 );
            val->type |= val2.type;
            if( val->type == 0 ) {
                val->val.ivalue /= val2.val.ivalue;
            } else {
                val->val.uvalue /= val2.val.uvalue;
            }
        } else if( PPTokenPtr[0] == '%' ) {
            ++PPTokenPtr;
            PP_Expr11( &val2 );
            val->type |= val2.type;
            if( val->type == 0 ) {
                val->val.ivalue %= val2.val.ivalue;
            } else {
                val->val.uvalue %= val2.val.uvalue;
            }
        } else {
            break;
        }
    }
}


void PP_Expr11( PPVALUE *val )
{
rescan:
    val->val.ivalue = 0;
    PP_AdvanceToken();
    switch( PPTokenPtr[0] ) {
    case '+':
        ++PPTokenPtr;
        PP_Expr11( val );
        break;
    case '-':
        ++PPTokenPtr;
        PP_Expr11( val );
        val->val.ivalue = - val->val.ivalue;
        break;
    case '!':
        ++PPTokenPtr;
        PP_Expr11( val );
        val->val.ivalue = ! val->val.ivalue;
        break;
    case '~':
        ++PPTokenPtr;
        PP_Expr11( val );
        val->val.ivalue = ~ val->val.ivalue;
        break;
    case '(':
        ++PPTokenPtr;
        PP_AdvanceToken();
        PP_ConstExpr( val );
        PP_AdvanceToken();
        if( PPTokenPtr[0] == ')' ) {
            ++PPTokenPtr;
        } else {
            // error
        }
        break;
    case '\'':
        PP_CharConst( val );
        break;
    default:
        switch( PP_Class( PPTokenPtr[0] ) ) {
        case CC_DIGIT:
            PP_Constant( val );
            break;
        case CC_ALPHA:
            PP_Identifier( val );
            if( val->type != 0 )  goto rescan;  // was a macro; must rescan
            break;
        }
        break;
    }
    PP_AdvanceToken();
}

void PP_AdvanceToken()
{
    char        white_space;

    for(;;) {
        PPTokenPtr = PP_SkipWhiteSpace( PPTokenPtr, &white_space );
        if( PPTokenPtr[0] != '\0' ) break;
        if( PPCurToken != NULL ) {
            PP_Free( PPCurToken );
            PPCurToken = PPNextToken();
        }
        if( PPCurToken == NULL ) {
            if( PPCharPtr != NULL ) {
                PPTokenPtr = PP_SkipWhiteSpace( PPCharPtr, &white_space );
                PPCharPtr = NULL;
            }
            break;
        }
    }
}

long int PP_HexNumber()
{
    long int    value;
    char        c;

    value = 0;
    for(;;) {
        c = PPTokenPtr[0];
        if( c >= '0' && c <= '9' ) {
            c -= '0';
        } else if( c >= 'a' && c <= 'f' ) {
            c = c - 'a' + 10;
        } else if( c >= 'A' && c <= 'F' ) {
            c = c - 'A' + 10;
        } else {
            break;
        }
        value = value * 16 + c;
        ++PPTokenPtr;
    }
    return( value );
}

void PP_CharConst( PPVALUE *val )
{
    long int    value;
    char        c;

    value = 0;
    ++PPTokenPtr;
    if( PPTokenPtr[0] == '\\' ) {
        ++PPTokenPtr;
        // look for hex constant or octal escape sequence
        switch( PPTokenPtr[0] ) {
        case 'a':       value = '\a';   break;
        case 'b':       value = '\b';   break;
        case 'f':       value = '\f';   break;
        case 'n':       value = '\n';   break;
        case 'r':       value = '\r';   break;
        case 't':       value = '\t';   break;
        case 'v':       value = '\v';   break;
        case 'x':
            ++PPTokenPtr;
            value = PP_HexNumber();
            --PPTokenPtr;
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
            value = PPTokenPtr[0] - '0';
            c = PPTokenPtr[1];
            if( c >= '0'  &&  c <= '7' ) {
                value = value * 8 + c - '0';
                ++PPTokenPtr;
                c = PPTokenPtr[1];
                if( c >= '0'  &&  c <= '7' ) {
                    value = value * 8 + c - '0';
                    ++PPTokenPtr;
                }
            }
            break;
        default:
            value = PPTokenPtr[0];
            break;
        }
        ++PPTokenPtr;
    } else {
        value = PPTokenPtr[0];
        ++PPTokenPtr;
    }
    if( PPTokenPtr[0] == '\'' ) {
        ++PPTokenPtr;
    }
    val->type = 0;
    val->val.ivalue = value;
}

void PP_Constant( PPVALUE *val )
{
    long int    value;
    char        c;

    value = 0;
    val->type = 0;                              // assume signed value
    c = PPTokenPtr[0];
    if( c == '0' ) {                            // octal or hex number
        ++PPTokenPtr;
        c = PPTokenPtr[0];
        if( c == 'x'  ||  c == 'X' ) {          // hex number
            ++PPTokenPtr;
            value = PP_HexNumber();
        } else {                                // octal number
            while( c >= '0'  &&  c <= '7' ) {
                value = value * 8 + c - '0';
                ++PPTokenPtr;
                c = PPTokenPtr[0];
            }
        }
    } else {                                    // decimal number
        while( c >= '0'  &&  c <= '9' ) {
            value = value * 10 + c - '0';
            ++PPTokenPtr;
            c = PPTokenPtr[0];
        }
    }
    if( (unsigned long)value > 0x7fffffff )  val->type = 1;     // mark as unsigned
    c = PPTokenPtr[0];
    if( c == 'u' || c == 'U' ) {
        ++PPTokenPtr;
        c = PPTokenPtr[0];
        if( c == 'l' || c == 'L' ) {
            ++PPTokenPtr;
            val->type = 1;
        }
    } else if( c == 'l' || c == 'L' ) {
        ++PPTokenPtr;
        c = PPTokenPtr[0];
        if( c == 'u' || c == 'U' ) {
            ++PPTokenPtr;
            val->type = 1;
        }
    }
    val->val.ivalue = value;
}

void PP_Identifier( PPVALUE *val )
{
    long int    value;
    char        *ptr;
    MACRO_ENTRY *me;
    char        c;

    val->type = 0;
    ptr = PP_ScanName( PPTokenPtr );
    c = *ptr;
    *ptr = '\0';
    if( strcmp( PPTokenPtr, "defined" ) == 0 ) {
        *ptr = c;
        PPTokenPtr = PP_SkipWhiteSpace( ptr, &c );
        if( PPTokenPtr[0] == '(' ) {
            ++PPTokenPtr;
            ptr = PPCharPtr;
            value = PP_ScanMacroLookup( PPTokenPtr ) != NULL;
            PPTokenPtr = PPCharPtr;                     /* 23-sep-94 */
            PPCharPtr = ptr;
            PPTokenPtr = PP_SkipWhiteSpace( PPTokenPtr, &c );
            if( PPTokenPtr[0] == ')' ) {
                ++PPTokenPtr;
            } else {
                // error
            }
        } else {
            ptr = PPCharPtr;
            value = PP_ScanMacroLookup( PPTokenPtr ) != NULL;
            PPTokenPtr = PPCharPtr;                     /* 23-sep-94 */
            PPCharPtr = ptr;
        }
    } else {
        if( PPTokenList != NULL ) {
            me = NULL;
        } else {
            me = PP_MacroLookup( PPTokenPtr );
        }
        *ptr = c;
        if( me != NULL ) {
            PPCharPtr = ptr;
            PPSavedChar = c;
            DoMacroExpansion( me );
            PPCurToken = NextMToken();
            val->type = 1;      // indicate macro
        } else {
            PPTokenPtr = ptr;
            PPFlags |= PPFLAG_UNDEFINED_VAR;
            value = 0;
        }
    }
    val->val.ivalue = value;
}
