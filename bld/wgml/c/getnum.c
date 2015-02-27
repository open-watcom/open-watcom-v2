/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WGML internal subroutine getnum  for expression evaluation
*
****************************************************************************/

#include <errno.h>

#include "wgml.h"

#define NULC    '\0'
#define not_ok  true
#define ok      false

#define OPERATORS_DEF \
    pick( PLUS,     1,  '+' ), \
    pick( MINUS,    2,  '-' ), \
    pick( MUL,      3,  '*' ), \
    pick( DIV,      4,  '/' ), \
    pick( RPAREN,   5,  ')' ), \
    pick( LPAREN,   0,  '(' )

typedef enum {
    #define pick(e,f,c) OPER_##e
    OPERATORS_DEF,
    #undef pick
    OPER_NULL
} operator;

#define OPER_FIRST  OPER_PLUS
#define OPER_LAST   OPER_NULL

typedef struct operator_info {
    int     priority;
    char    operc;
} operator_info;

static  operator_info opers[] = {
    #define pick(e,f,c) {f,c}
    OPERATORS_DEF,
    #undef pick
    {  0,    NULC }                     // terminating entry
};

#define MAXOPER 128                     // operator maximum
#define MAXTERM 128                     // terms maximum

static char     tokbuf[256];            // workarea

static int      coper;                  // current operator stack ptr
static int      cvalue;                 // current argument stack ptr
static int      nparens;                // nesting level

static operator oper_stack[MAXOPER];    // operator stack
static long     value_stack[MAXTERM];   // argument stack
static bool     ignore_blanks;


/*
 * stack functions
 */

static bool pop_val( long *arg )
{
    if( --cvalue < 0 ) {
        return( not_ok );
    }
    *arg = value_stack[cvalue];
    return( ok );
}

static  void push_val( long arg )
{
    value_stack[cvalue++] = arg;
}

static bool pop_op( operator *op )
{
    if( --coper < 0 ) {
        return( not_ok );
    }
    *op = oper_stack[coper];
    return( ok );
}

static void push_op( operator op )
{
    if( opers[op].priority == 0 ) {
        nparens++;
    }
    oper_stack[coper++] = op;
}


/*
 *  evaluate expression
 */

static operator do_expr( void )
{
    long arg1;
    long arg2;
    operator op;

    if( pop_op( &op ) ) {
        return( OPER_NULL );
    }
    if( pop_val( &arg1 ) ) {
        return( OPER_NULL );
    }
    pop_val( &arg2 );

    switch( op ) {
    case OPER_PLUS:
        push_val( arg2 + arg1 );
        break;
    case OPER_MINUS:
        push_val( arg2 - arg1 );
        break;
    case OPER_MUL:
        push_val( arg2 * arg1 );
        break;
    case OPER_DIV:
        if( arg1 == 0 ) {
            return( OPER_NULL );
        }
        push_val( arg2 / arg1 );
        break;
    case OPER_LPAREN:
        cvalue += 2;
        break;
    case OPER_RPAREN:
    default:
        return( OPER_NULL );
    }

    if( cvalue > 0 ) {
        return( op );
    } else {
        return( OPER_NULL );
    }
}

/*
 *  Evaluate one level
 */

static operator do_paren( void )
{
    operator    op;

    if( nparens-- == 0 ) {
        return( OPER_NULL );
    }

    do {
        op = do_expr();
        if( op == OPER_NULL ) {
            break;
        }
    } while( opers[op].priority != 0 );

    return( op );
}


/*
 *  Get an operator
 */

static operator get_op( char token )
{
    operator    op;

    for( op = OPER_FIRST; op < OPER_LAST; ++op ) {
        if( token == opers[op].operc ) {
            break;
        }
    }
    return( op );
}


/*
 *  Get an expression
 */

static char *get_exp( const char *str, const char *stop )
{
    const char  *ptr;
    char        *tptr;
    operator    op;
    char        c;

    tptr = tokbuf;
    for( ptr = str; ptr < stop; ++ptr ) {
        c = *ptr;
        if( c == ' ' ) {
            if( ignore_blanks ) {
                continue;
            } else {
                break;
            }
        }
        op = get_op( c );
        if( op != OPER_NULL ) {
            if( (op == OPER_MINUS) || (op == OPER_PLUS) ) {
                char c1 = ptr[1];
                if( (c1 == '-') || (c1 == '+') )
                    return( NULL );
                if( str == ptr ) {
                    if( isdigit( c1 ) || (c1 == '.') ) {
                        *tptr++ = c;
                        continue;
                    }
                    push_val( 0 );
                }
            }
            if( str == ptr )
                *tptr++ = c;
            break;
        }
        *tptr++ = c;
    }
    *tptr = NULC;

    return( tokbuf );
}

static bool evaluate( const char **line, const char *stop, long *val )
{
    long        arg;
    const char  *ptr;
    char        *str;
    char        *endptr;
    operator    op;
    bool        expr_oper;              // looking for term or operator

    coper     = 0;
    cvalue    = 0;
    nparens   = 0;

    expr_oper = false;
    for( ptr = *line; ptr < stop; ++ptr ) {
        if( *ptr == ' ' ) {
            if( ignore_blanks ) {
                continue;
            } else {
                break;
            }
        }
        if( !expr_oper ) {
            // look for term
            str = get_exp( ptr, stop );
            if( str == NULL || *str == NULC ) {         // nothing is error
                return( not_ok );
            }

            if( str[1] == NULC ) {
                op = get_op( *str );
                if( op != OPER_NULL ) {
                    push_op( op );
                    continue;
                }
#if 0
                if( (*str == '-' ) || (*str == '+' ) ) {
                    push_op(*str);
                    continue;
                }
#endif
            }

            arg = strtol( str, &endptr, 10 );
            if( (((arg == LONG_MIN) || (arg == LONG_MAX)) && errno == ERANGE) || (str == endptr) ) {
                 return( not_ok );
            }

            push_val( arg );

            ptr += endptr - str - 1;    // to the next unprocessed char

            expr_oper = true;           // look for operator next
        } else {
            // look for operator
            op = get_op( *ptr );
            if( op == OPER_NULL ) {
                return( not_ok );
            }
            if( op == OPER_RPAREN ) {
                if( do_paren() == OPER_NULL ) {
                    return( not_ok );
                }
            } else {
                int op_priority = opers[op].priority;
                while( coper != 0 && opers[oper_stack[coper - 1]].priority >= op_priority ) {
                    do_expr();
                }
                push_op( op );
                expr_oper = false;      // look for term next
            }
        }
    }

    while( cvalue > 1 ) {
        if( do_expr() == OPER_NULL ) {
            return( not_ok );
        }
    }
    if( coper == 0 ) {
        *line = ptr;                    // next scan position
        return( pop_val( val ) );       // no operations left return result
    } else {
        return( not_ok );
    }
}

/***************************************************************************/
/*  ideas from cbt282.122                                                  */
/*  getnum  evaluate a numeric result                                      */
/*                                                                         */
/***************************************************************************/

condcode getnum( getnum_block *gn )
{
    char        c;
    char        *start;                 // arg start  (X2)
    char        *stop;                  // arg stop   (R1)

    start = gn->argstart;
    stop = gn->argstop;
    while( start < stop && *start == ' ' ) {
        start++;                        // skip leading blanks
    }
    gn->errstart = start;
    gn->first = start;
    if( start == stop ) {
        gn->cc = omit;
        return( omit );                 // nothing there
    }
    c = *start;
    if( c == '+' || c == '-' ) {
        gn->num_sign = c;               // unary sign
    } else {
        gn->num_sign = ' ';             // no unary sign
    }
    ignore_blanks = gn->ignore_blanks;
    if( evaluate( (const char **)&start, stop, &gn->result ) ) {
        gn->cc = notnum;
    } else {
        gn->argstart = start;   // start for next scan
        gn->length = sprintf( gn->resultstr, "%ld", gn->result );
        if( gn->result >= 0 ) {
            gn->cc = pos;
        } else {
            gn->cc = neg;
        }
    }
    return( gn->cc );
}
