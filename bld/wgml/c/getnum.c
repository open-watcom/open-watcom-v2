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

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include <errno.h>

#include "wgml.h"
#include "gvars.h"

#define NULC    '\0'
#define not_ok  (-1)
#define ok      0

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
static  long    value_stack[MAXTERM];   // argument stack
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
    if( !coper ) {
        return( 0 );
    }
    return( get_prio( oper_stack[coper - 1] ) );
}


/*
 * stack functions
 */

static  int pop_val( long *arg )
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

static  int pop_op( int *op )
{
    if( --coper < 0 ) {
        return( not_ok );
    }
    *op = oper_stack[coper];
    return( ok );
}

static  void push_op( char op )
{
    if( !get_prio( op ) ) {
        nparens++;
    }
    oper_stack[coper++] = op;
}


/*
 *  evaluate expression
 */

static  int do_expr( void )
{
    long arg1;
    long arg2;
    int op;

    if( not_ok == pop_op( &op ) ) {
        return( not_ok );
    }

    if( not_ok == pop_val( &arg1 ) ) {
        return( not_ok );
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
            return( not_ok );
        }
        push_val( arg2 / arg1 );
        break;

    case '(':
        cvalue += 2;
        break;

    default:
        return( not_ok );
    }

    if( 1 > cvalue ) {
        return( not_ok );
    } else {
        return( op );
    }
}

/*
 *  Evaluate one level
 */

static int do_paren( void )
{
    int op;

    if( 1 > nparens-- ) {
        return( not_ok );
    }

    do {
        op = do_expr();
        if( op < ok ) {
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

static char *get_exp( const char *str )
{
    const char *ptr  = str;
    char *tptr = tokbuf;
    struct operator *op;

    while( *ptr ) {
        if( *ptr == ' ' ) {
            if( ignore_blanks ) {
                ptr++;
                continue;
            } else {
                break;
            }
        }
        op = get_op( ptr );
        if (NULL != op ) {
            if( ('-' == *ptr)  || ('+' == *ptr) ) {
                if( ('-' == *(ptr+1))  || ('+' == *(ptr+1)) ) {
                    return( NULL );
                }
                if( str != ptr )
                    break;
                if( str == ptr && !isdigit( ptr[1] ) && '.' != *(ptr+1) ) {
                    push_val(0);
                    *tptr++ = *ptr++;
                    break;
                }
            } else if (str == ptr) {
                *tptr++ = *ptr++;
                break;
            } else
                break;
        }

        *tptr++ = *ptr++;
    }
    *tptr = NULC;

    return tokbuf;
}

static  int evaluate( char **line, long *val )
{
    long        arg;
    char    *   ptr;
    char    *   str;
    char    *   endptr;
    int         ercode;
    operator *  op;
    int         expr_oper;              // looking for term or operator

    expr_oper = 0;
    coper     = 0;
    cvalue    = 0;
    nparens   = 0;
    ptr       = *line;

    while( *ptr ) {
        if( *ptr == ' ' ) {
            if( ignore_blanks ) {
                ptr++;
                continue;
            } else {
                break;
            }
        }
        switch( expr_oper ) {
        case 0:                         // look for term
            str = get_exp( ptr );

            if( str == NULL ) {         // nothing is error
                return( not_ok );
            }

            op = get_op( str );
            if( *(str +1) == NULC ) {
                if( NULL != op ) {
                    push_op( op->operc );
                    ptr++;
                    break;
                }

                if( (*str == '-' ) || (*str == '+' ) ) {
                    push_op(*str);
                    ++ptr;
                    break;
                }
            }

            arg = strtol( str, &endptr, 10 );
            if( (((arg == LONG_MIN) || (arg == LONG_MAX)) && errno == ERANGE)
                 || (str == endptr) ) {
                 return( not_ok );
            }

            push_val( arg );

            ptr += endptr - str;        // to the next unprocessed char

            expr_oper = 1;              // look for operator next
            break;

        case 1:                         // look for operator
            op = get_op( ptr );
            if( NULL == op ) {
                return( not_ok );
            }
            if( ')' == *ptr ) {
                ercode = do_paren();
                if( ok > ercode ) {
                    return( ercode );
                }
            } else {
                while( coper && op->priority <= get_prio_m1() ) {
                    do_expr();
                }
                push_op( op->operc );
                expr_oper = 0;      // look for term next
            }
            ptr++;
            break;
        }
    }

    while( 1 < cvalue ) {
        ercode = do_expr();
        if( ok > ercode )
             return ercode;
    }
    if( !coper ) {
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
    char    *   a;                      // arg start  (X2)
    char    *   z;                      // arg stop   (R1)
    char        c;
    int         rc;

    a = gn->argstart;
    z = gn->argstop;
    while( a < z && *a == ' ' ) {
        a++;                            // skip leading blanks
    }
    gn->errstart = a;
    gn->first    = a;
    if( a > z ) {
        gn->cc = omit;
        return( omit );                 // nothing there
    }
    c = *a;
    if( c == '+' || c == '-' ) {
        gn->num_sign = c;               // unary sign
    } else {
        gn->num_sign = ' ';             // no unary sign
    }
    ignore_blanks = gn->ignore_blanks;
    c = *(z + 1);
    *(z + 1) = '\0';                    // make null terminated string
    rc = evaluate( &a, &gn->result );
    *(z + 1) = c;
    if( rc != 0 ) {
        gn->cc = notnum;
    } else {
        gn->argstart = a + 1;           // start for next scan
        gn->length = sprintf_s( gn->resultstr, sizeof( gn->resultstr ), "%ld",
                                gn->result );
        if( gn->result >= 0 ) {
            gn->cc = pos;
        } else {
            gn->cc = neg;
        }
    }
    return( gn->cc );
}

