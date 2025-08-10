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
* Description:  WGML controls multi letter functions  &'substr(), ...
*               and calls corresponding function see gsfuncs.h
*
*               scr_multi_funcs()
*
*      static helper functions
*               alloc_resbuf()        result work buffer
*               err_info()            output err info and set error result
*               find_end_of_parm()    find parm separator char
*               find_start_of_parm()  find beginning of parm
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*    SCR multi letter functions                                           */
/***************************************************************************/

typedef struct scrfunc {
    char            funcname[FUNC_NAME_LENGTH + 1]; // function name
    unsigned        funclen;            // actual length of funcname
    unsigned        parm_cnt;           // mandatory parms
    unsigned        opt_parm_cnt;       // optional parms
    condcode        (*fun)( parm parms[MAX_FUN_PARMS], unsigned parm_count,
                            char **result, unsigned ressize );
} scrfunc;

static const scrfunc    scr_functions[] = {
    #define pick( name, length, parms, optparms, routine )  { #name, length, parms, optparms, routine },
    #include "gsfuncs.h"
    #undef pick
};

#define SCR_FUNC_MAX (sizeof( scr_functions ) / sizeof( scr_functions[0] ) - 1)

static bool         multiletter_function;   // parm contains function
static bool         var_in_parm;            // parm contains variable


/****************************************************/
/*allocate a buffer for resolving symvars+functions */
/****************************************************/
static char *alloc_resbuf( inp_line **in_wk )
{
    inp_line  * wk;

    wk = mem_alloc( sizeof( inp_line ) + BUF_SIZE );
    if( *in_wk == NULL ) {
        wk->next = NULL;
    } else {
        wk->next = *in_wk;
    }
    *in_wk = wk;
    return( wk->value );
}


/*  find end of parm for multi letter functions
 *     end of parm is either , or )
 *     but only if outside of string and not in deeper paren level
 *     string delimiters are several chars
 *
 */

static char *find_end_of_parm( const char *pchar, const char *pend )
{
    char    quotechar[MAX_PAREN];
    bool    instring[MAX_PAREN];
    int     paren_level;
    char    c;
    char    cm1;
    char    cm2;
    bool    finished;
    bool    test_for_quote;             // only at first char after ( or ,

    paren_level = 0;
    quotechar[paren_level] ='\0';
    instring[paren_level] = false;
    instring[1] = false;
    finished = false;
    test_for_quote = true;
    c = '\0';
    cm1 = '\0';
    for( ; *pchar != '\0' ; ) {
        cm2 = cm1;
        cm1 = c;
        c = *pchar;
        if( cm1 == ampchar ) {
            if( c == '\'' ) {
                multiletter_function = true;    // parm contains a function
            } else {
                var_in_parm = true;             // parm contains variable
            }
        }
        if( multiletter_function ) {
            multiletter_function = false;       // multiletter_function is static, reset
            pchar++;                            // over "'"
            pchar = find_end_of_parm( pchar, pend );
            continue;
        } else {
            if( instring[paren_level] ) {
                if( c == quotechar[paren_level] ) {
                    instring[paren_level] = false;
                }
            } else {
                if( test_for_quote
                  && is_quote_char( c ) ) {
                    if( (cm1 == ampchar)        // &' sequence
                      || ((cm2 == ampchar)
                      && my_isalpha( cm1 )) ) { // &X' sequence
                        /* no instring change */
                    } else {
                        instring[paren_level] = true;
                        quotechar[paren_level] = c;
                    }
                    test_for_quote = false;
                } else {
                    switch( c ) {
                    case '(' :
                        paren_level += 1;
                        test_for_quote = true;
                        if( paren_level < MAX_PAREN ) {
                            instring[paren_level] = false;
                        } else {
                            paren_level--;
                            finished = true;// error msg ??? TBD
                        }
                        break;
                    case ')' :
                        paren_level--;
                        if( paren_level <= 0 ) {
                            finished = true;
                            if( pchar < pend ) {
                                pchar++;    // step over final ')'
                            }
                            if( *pchar == '.' ) {
                                pchar++;    // step over '.' terminating subscripted symbol
                            }
                        }
                        break;
                    case ',' :
                        if( paren_level == 0 ) {
                            finished = true;
                        }
                        test_for_quote = true;
                        break;
                    default:
                        test_for_quote = false;
                        break;
                    }
                }
            }
        }
        if( finished
          || (pchar >= pend) ) {
            break;
        }
        pchar++;
    }
    return( (char *)pchar );
}


/***************************************************************************/
/*  find start of parm                                                     */
/*     leading blanks are skipped                                          */
/***************************************************************************/

static char *find_start_of_parm( const char *pchar )
{
    ++pchar;                            // over ( or ,
    SkipSpaces( pchar );                // over unquoted blanks
    return( (char *)pchar );
}

static const scrfunc *find_funcinfo( const char *funcname )
{
    unsigned        funclen;
    const scrfunc   *funcinfo;
    int             k;

    funclen = strlen( funcname );
    funcinfo = scr_functions;
    for( k = 0; k < SCR_FUNC_MAX; k++ ) {
        if( funclen == funcinfo->funclen
          && strcmp( funcname, funcinfo->funcname ) == 0 ) {
            if( (input_cbs->fmflags & II_research)
              && GlobalFlags.firstpass ) {
                out_msg( " Function %s found\n", funcname );
                add_multi_func_research( funcname );
            }
            return( funcinfo );
        }
        funcinfo++;
    }
    return( NULL );
}

/***************************************************************************/
/*  scr_multi_funcs  isolate function name, lookup name in table           */
/*                   scan function arguments                               */
/*                   call corresponding function                           */
/***************************************************************************/

char *scr_multi_funcs( const char *funcname, char *args, char **result, unsigned ressize )
{
    char            *p;                 // points into input buffer
    char            *pchar;             // points into input buffer
    char            *pend;              // points into resbuf
    char            *pret;              // end of function in input buffer
    char            *ps;                // points into resbuf
    char            *resbuf;            // buffer into which parm.arg.s and parm.arg.e point
    condcode        cc;
    inp_line        *in_wk;             // stack of resbufs, one for each parameter
    const scrfunc   *funcinfo;
    unsigned        k;
    unsigned        m;
    unsigned        parmcount;
    int             p_level;
    int             rc;
    parm            parms[MAX_FUN_PARMS];

    /* args points to the outer open parenthesis ('(') */

    ProcFlags.unresolved = false;
    in_wk = NULL;                       // no result buffer yet
    rc = 0;

    // find true end of function
    pend = args + strlen( args );
    p_level = 0;
    for( p = args; *p != '\0'; p++ ) {  // to end of buffer
        if( *p == '(' ) {
            p_level++;
        } else if( *p == ')' ) {
            p_level--;
            if( p_level == 0 ) {
                pend = p;               // pend points to outermost ')'
                break;
            }
        }
    }
    pret = p;                           // save for return (points to final ')')

    // test for valid functionname

    funcinfo = find_funcinfo( funcname );

    if( funcinfo == NULL ) {
        /* this is not an error in wgml 4.0 */
        ProcFlags.unresolved = true;
        return( (char *)args );
    }

    /* Missing ')' is only a problem if the function was found */

    if( p_level > 0 ) {                 // at least one missing ')'
        xx_line_err_exit_c( ERR_FUNC_PARM_END, p - 1 );
        /* never return */
    }

    // collect the mandatory parm(s)

    pchar = args;

    for( k = 0; k < funcinfo->parm_cnt; k++ ) {

        multiletter_function = false;
        var_in_parm = false;

        parms[k].arg.s = find_start_of_parm( pchar );
        pchar = find_end_of_parm( parms[k].arg.s, pend );

        if( multiletter_function
          || var_in_parm ) {
            parms[k].redo = true;
        } else {
            parms[k].redo = false;
        }
        parms[k].arg.e = pchar;
        parms[k + 1].arg.s = pchar + 1;

        if( pchar >= pend ) {
            break;                      // end of parms
        }
    }
    m = k + (k < funcinfo->parm_cnt);   // mandatory parm count

    if( m < funcinfo->parm_cnt ) {
        xx_line_err_exit_c( ERR_FUNC_PARM_MISS, p - 1 );
        /* never return */
    }

    // collect the optional parm(s)
    if( pchar >= pend ) {               // no optional parms
        k = 0;
    } else {
        for( k = 0; k < funcinfo->opt_parm_cnt; k++ ) {
            multiletter_function = false;
            var_in_parm = false;
            parms[m + k].arg.s = find_start_of_parm( pchar );
            pchar = find_end_of_parm( parms[m + k].arg.s, pend );

            if( multiletter_function
              || var_in_parm ) {
                parms[m + k].redo = true;
            } else {
                parms[m + k].redo = false;
            }
            parms[m + k].arg.e = pchar;
            parms[m + k + 1].arg.s = pchar + 1;

            if( pchar >= pend ) {
                break;                  // end of parms
            }
        }
        k += (k < funcinfo->opt_parm_cnt);
    }
    parmcount = m + k;                  // total parmcount
    parms[parmcount].arg.s = NULL;      // end of parms indicator

    /* Now resolve those parm that need it */

    for( k = 0; k < parmcount; k++ ) {
        while( parms[k].redo ) {
            resbuf = alloc_resbuf( &in_wk );
            strcpy( resbuf, parms[k].arg.s );// copy parm

            ps = resbuf + (parms[k].arg.e - parms[k].arg.s);
            *ps = '\0';

            parms[k].arg.e = ps;
            parms[k].arg.s = resbuf;
            if( (input_cbs->fmflags & II_research)
              && GlobalFlags.firstpass ) {
                out_msg( " Function %s parm %s found\n", funcname, resbuf );
            }

            if( !resolve_symvar_functions( resbuf, false ) )
                break; // no change, we're done

            pend = parms[k].arg.s + strlen( parms[k].arg.s );
            multiletter_function = false;
            var_in_parm = false;
            pend = find_end_of_parm( parms[k].arg.s, pend );

            if( multiletter_function
              || var_in_parm ) {
                parms[k].redo = true;
            } else {
                parms[k].redo = false;
            }

            parms[k].arg.e = resbuf + strlen( parms[k].arg.s );
            if( (input_cbs->fmflags & II_research)
              && GlobalFlags.firstpass ) {
                out_msg( " Function      parm %s return\n", resbuf );
            }
        }
    }

    ProcFlags.suppress_msg = multiletter_function;

    cc = funcinfo->fun( parms, parmcount, result, ressize );

    ProcFlags.suppress_msg = false;

    free_lines( in_wk );

    if( cc != CC_pos ) {                // error in function
        *(*result)++ = '&';             // result is & to preserve the input
        **result = '\0';

        ProcFlags.unresolved = true;

        return( (char *)args );
    }

    ProcFlags.substituted = true;
    return( (char *)pret + 1 );         // all OK new scan position
}


#if 0
/*
+++++++++++++++++++++++++++++++  used functions in OW documentation +++++++++
used single letter functions: others are unused
&e'  exist 0 or 1
&l'  length, if undefined length of name
&s'  subscript   low
&S'  superscript high
&u'  upper

&w'  width          implemented for internal use

used other functions:

&'delstr(
&'d2c(
&'index(
&'insert(
&'left(
&'length(
&'lower(
&'min(
&'pos(
&'right(
&'strip(
&'substr(
&'subword(
&'translate(
&'upper(
&'veclastpos(
&'vecpos(
&'word(
&'wordpos(
&'words(

&'c2x(           implemented for internal use
&'width(         implemented for internal use
*/
#endif
