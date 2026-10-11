/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
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


#define NULC        '\0'
#define RC_not_ok   (-1)
#define RC_ok       0

typedef struct operator {
        int     priority;
        char    operc;
} operator;

static  operator opers[] = {
    {  4,    '+'  },
    {  8,    '-'  },
    { 64,    '*'  },
    {128,    '/'  },
    {256,    ')'  },
    {  0,    '('  },
    {  0,    NULC }                     // terminating entry
};

#define MAXOPER 128                     // operator maximum
#define MAXTERM 128                     // terms maximum

static  char    tokbuf[256];            // workarea

static  int     coper;                  // current operator stack ptr
static  int     cvalue;                 // current argument stack ptr
static  int     nparens;                // nesting level

static  char    oper_stack[MAXOPER];    // operator stack
static  int     value_stack[MAXTERM];   // argument stack
static  int     ignore_blanks;


/*
 *   get priority of operator
 */

static  int get_prio( char token )
{
    operator *op;

    for( op = opers; op->operc; ++op ) {
        if( token == op->operc ) {
            break;
        }
    }
    return( op->priority );
}

/*
 *  get priority of the top of stack operator
 */

static  int get_prio_m1( void )
{
    if( coper == 0 ) {
        return( 0 );
    }
    return( get_prio( oper_stack[coper - 1] ) );
}


/*
 * stack functions
 */

static  int pop_val( int *arg )
{
    if( --cvalue < 0 ) {
        return( RC_not_ok );
    }
    *arg = value_stack[cvalue];
    return( RC_ok );
}

static  void push_val( int arg )
{
    value_stack[cvalue++] = arg;
}

static  int pop_op( int *op )
{
    if( --coper < 0 ) {
        return( RC_not_ok );
    }
    *op = oper_stack[coper];
    return( RC_ok );
}

static  void push_op( char op )
{
    if( get_prio( op ) == 0 ) {
        nparens++;
    }
    oper_stack[coper++] = op;
}


/*
 *  evaluate expression
 */

static  int do_expr( void )
{
    int arg1;
    int arg2;
    int op;

    if( RC_not_ok == pop_op( &op ) ) {
        return( RC_not_ok );
    }

    if( RC_not_ok == pop_val( &arg1 ) ) {
        return( RC_not_ok );
    }

    pop_val( &arg2 );

    switch( op ) {
    case '+':
        push_val( arg2 + arg1 );
        break;

    case '-':
        push_val( arg2 - arg1 );
        break;

    case '*':
        push_val( arg2 * arg1 );
        break;

    case '/':
        if( 0 == arg1 ) {
            return( RC_not_ok );
        }
        push_val( arg2 / arg1 );
        break;

    case '(':
        cvalue += 2;
        break;

    default:
        return( RC_not_ok );
    }

    if( 1 > cvalue ) {
        return( RC_not_ok );
    }
    return( op );
}

/*
 *  Evaluate one level
 */

static int do_paren( void )
{
    int op;

    if( 1 > nparens-- ) {
        return( RC_not_ok );
    }

    do {
        op = do_expr();
        if( op == RC_not_ok ) {
            break;
        }
    } while( get_prio( (char)op ) );

    return( op );
}


/*
 *  Get an operator
 */

static  operator *get_op( const char *str )
{
    operator *op;

    for( op = opers; op->operc; ++op ) {
        if( *str == op->operc ) {
            return( op );
        }
    }
    return( NULL );
}


/*
 *  Get an expression
 */

static char *get_exp( const char *start, const char *end )
{
    const char *p = start;
    char *tptr = tokbuf;
    struct operator *op;

    while( p < end ) {
        if( *p == ' ' ) {
            if( ignore_blanks ) {
                p++;
                continue;
            }
            break;
        }
        op = get_op( p );
        if( NULL != op ) {
            if( ('-' == p[0]) || ('+' == p[0]) ) {
                if( ( p + 1 < end ) && ( ('-' == p[1]) || ('+' == p[1]) ) ) {
                    return( NULL );
                }
                if( start != p )
                    break;
                if( ( p + 1 == end ) || !my_isdigit( p[1] ) && '.' != p[1] ) {
                    push_val( 0 );
                    *tptr++ = *p++;
                    break;
                }
            } else {
                if( start == p )
                    *tptr++ = *p++;
                break;
            }
        }
        *tptr++ = *p++;
    }
    *tptr = NULC;

    return tokbuf;
}

static  int evaluate( tok_type *arg, int *val )
{
    char    *   p;
    char    *   str;
    char    *   endptr;
    int         ercode;
    operator *  op;
    int         expr_oper;              // looking for term or operator
    long        num;

    expr_oper = 0;
    coper     = 0;
    cvalue    = 0;
    nparens   = 0;
    p         = arg->s;

    while( p < arg->e ) {
        if( *p == ' ' ) {
            if( ignore_blanks ) {
                p++;
                continue;
            }
            break;
        }
        switch( expr_oper ) {
        case 0:                         // look for term
            str = get_exp( p, arg->e );
            if( str == NULL ) {         // nothing is error
                return( RC_not_ok );
            }

            op = get_op( str );
            if( *(str + 1) == NULC ) {
                if( NULL != op ) {
                    push_op( op->operc );
                    p++;
                    break;
                }

                if( (*str == '-' ) || (*str == '+' ) ) {
                    push_op( *str );
                    p++;
                    break;
                }
            }
            num = strtol( str, &endptr, 10 );
            if( (errno == ERANGE)
              || (num <= INT_MIN)
              || (num >= INT_MAX)
              || (str == endptr) ) {
                return( RC_not_ok );
            }
            push_val( num );
            p += endptr - str;          // to the next unprocessed char
            expr_oper = 1;              // look for operator next
            break;

        case 1:                         // look for operator
            op = get_op( p );
            if( NULL == op ) {
                if( coper ) {
                    return( RC_not_ok );
                }
                arg->s = p;                    // next scan position

                /********************************************************/
                /* This little bit of confusion is brought to you by    */
                /* the tendency of the calling code to replace the byte */
                /* following the expression with '\0' before invoking   */
                /* this function and then restoring it afterwards.      */
                /* Thus, this byte /should/ be '\0' here but can be     */
                /* expected to become something else after this         */
                /* function returns                                     */
                /********************************************************/

                if( arg->s < arg->e ) {           // should be '\0' here
                    return( RC_not_ok );
                }
                return( pop_val( val ) );   // no operations left return result
            }
            if( ')' == *p ) {
                ercode = do_paren();
                if( ercode == RC_not_ok ) {
                    return( ercode );
                }
            } else {
                while( coper && op->priority <= get_prio_m1() ) {
                    do_expr();
                }
                push_op( op->operc );
                expr_oper = 0;      // look for term next
            }
            p++;
            break;
        }
    }

    while( 1 < cvalue ) {
        ercode = do_expr();
        if( ercode == RC_not_ok ) {
             return( ercode );
        }
    }
    if( coper )
        return( RC_not_ok );

    arg->s = p;                   // next scan position
    return( pop_val( val ) );       // no operations left return result
}

/***************************************************************************/
/*  ideas from cbt282.122                                                  */
/*  getnum  evaluate a numeric result                                      */
/***************************************************************************/

condcode getnum( getnum_block *gn )
{
    tok_type    arg;
    char        c;
    int         rc;

    arg = gn->arg;

    while( arg.s < arg.e && *arg.s == ' ' ) {
        arg.s++;                        // skip leading blanks
    }
    gn->errstart = arg.s;
    gn->first    = arg.s;
    if( arg.s >= arg.e ) {
        gn->cc = CC_omit;
        return( CC_omit );                 // nothing there
    }
    c = *arg.s;
    if( c == '+' || c == '-' ) {
        gn->num_sign = c;               // unary sign
    } else {
        gn->num_sign = ' ';             // no unary sign
    }
    ignore_blanks = gn->ignore_blanks;
    rc = evaluate( &arg, &gn->result );
    if( rc != 0 ) {
        gn->cc = CC_notnum;
    } else {
        gn->arg.s = arg.s;              // start for next scan
        gn->length = sprintf( gn->resultstr, "%d", gn->result );
        if( gn->result >= 0 ) {
            gn->cc = CC_pos;
        } else {
            gn->cc = CC_neg;
        }
    }
    return( gn->cc );
}
