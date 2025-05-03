/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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

static const scrfunc    scr_functions[] = {
    #define pick( name, length, parms, optparms, routine )  { #name, length, parms, optparms, routine },
    #include "gsfuncs.h"
    #undef pick
    { " ", 0, 0, 0, NULL }              // end
};

#define SCR_FUNC_MAX (sizeof( scr_functions ) / sizeof( scr_functions[0] ) - 1)

static bool         multiletter_function;   // parm contains function
static bool         var_in_parm;            // parm contains variable


/****************************************************/
/*allocate a buffer for resolving symvars+functions */
/****************************************************/
static  char    * alloc_resbuf( inp_line ** in_wk )
{
    inp_line  * wk;

    if( *in_wk == NULL ) {
        *in_wk =  mem_alloc( sizeof( inp_line ) + buf_size );
        (*in_wk)->next = NULL;
    } else {
        wk = mem_alloc( sizeof( inp_line ) + buf_size );
        wk->next = *in_wk;
        *in_wk = wk;
    }
    return( (*in_wk)->value );
}


/*  find end of parm for multi letter functions
 *     end of parm is either , or )
 *     but only if outside of string and not in deeper paren level
 *     string delimiters are several chars
 *
 */

static  char    * find_end_of_parm( char * pchar, char * pend )
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
                if( test_for_quote && is_quote_char( c ) ) {
                    if( (cm1 == ampchar) || // &' sequence
                        ((cm2 == ampchar) && my_isalpha( cm1 )) ) {// &X' sequence
                                /* no instring change */
                    } else {
                        instring[paren_level] = true;
                        quotechar[paren_level] = c;
                    }
                    test_for_quote = false;
                } else {
                    switch( c ) {
                    case    '(' :
                        paren_level += 1;
                        test_for_quote = true;
                        if( paren_level < MAX_PAREN ) {
                            instring[paren_level] = false;
                        } else {
                            paren_level--;
                            finished = true;// error msg ??? TBD
                        }
                        break;
                    case    ')' :
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
                    case    ',' :
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
        if( finished || (pchar >= pend)) {
            break;
        }
        pchar++;
    }
    return( pchar );
}


/***************************************************************************/
/*  find start of parm                                                     */
/*     leading blanks are skipped                                          */
/***************************************************************************/

static  char    * find_start_of_parm( char * pchar )
{
    ++pchar;                            // over ( or ,
    SkipSpaces( pchar );                // over unquoted blanks
    return( pchar );
}


/***************************************************************************/
/*  scr_multi_funcs  isolate function name, lookup name in table           */
/*                   scan function arguments                               */
/*                   call corresponding function                           */
/***************************************************************************/

char * scr_multi_funcs( char * in, char * pstart, char ** result, int32_t valsize )
{
    bool                found;
    char                fn[FUN_NAME_LENGTH + 1];
    char            *   p;              // points into input buffer
    char            *   pchar;          // points into input buffer
    char            *   pend;           // points into resbuf
    char            *   pret;           // end of function in input buffer
    char            *   ps;             // points into resbuf
    char            *   resbuf;         // buffer into which parm.a and parm.e point
    condcode            cc;
    inp_line        *   in_wk;          // stack of resbufs, one for each parameter
    int                 funcind;
    int                 k;
    int                 m;
    int                 parmcount;
    int                 p_level;
    int                 rc;
    parm                parms[MAX_FUN_PARMS];
    size_t              fnlen;

    /* pstart points to the outer open parenthesis ('(') */

    ProcFlags.unresolved = false;
    in_wk = NULL;                       // no result buffer yet
    rc = 0;
    pchar = in + 2;                     // over &' to function name

    // find true end of function
    p = pstart;
    p_level = 0;
    while( *p != '\0' ) {               // to end of buffer
        if( *p == '(' ) {
            p_level++;
        } else if( *p == ')' ) {
            p_level--;
            if( p_level == 0 ) {
                pend = p;               // pend points to outermost ')'
                break;
            }
        }
        p++;
    }

    pret = p;                           // save for return (points to final ')')

    // collect function name
    fnlen = 0;
    for( ; pchar < pstart; pchar++  ) {
        fn[fnlen] = *pchar;
        if( fnlen < FUN_NAME_LENGTH ) {
            fnlen++;
        } else {
            break;
        }
    }
    fn[fnlen] = '\0';

    // test for valid functionname
    found = false;
    for( k = 0; k < SCR_FUNC_MAX; k++ ) {
        if( fnlen == scr_functions[k].length
            && !stricmp( fn, scr_functions[k].fname ) ) {

            found = true;
            if( (input_cbs->fmflags & II_research) && GlobalFlags.firstpass ) {
                out_msg( " Function %s found\n", scr_functions[k].fname );
                add_multi_func_research( fn );
            }
            break;
        }
    }

    if( !found ) {
        /* this is not an error in wgml 4.0 */
        ProcFlags.unresolved = true;
        return( pstart );
    }
    funcind = k;

    /* Missing ')' is only a problem if the function was found */

    if( p_level > 0 ) {      // at least one missing ')'
        xx_line_err_c( err_func_parm_end, p - 1 );
    }

    // collect the mandatory parm(s)

    for( k = 0; k < scr_functions[funcind].parm_cnt; k++ ) {

        multiletter_function = false;
        var_in_parm = false;

        parms[k].a = find_start_of_parm( pchar );
        pchar = find_end_of_parm( parms[k].a, pend );

        if( multiletter_function || var_in_parm ) {
            parms[k].redo = true;
        } else {
            parms[k].redo = false;
        }
        parms[k].e = pchar - 1;
        parms[k + 1].a = pchar + 1;

        if( pchar >= pend ) {
            break;                      // end of parms
        }
    }
    m = k + (k < scr_functions[funcind].parm_cnt);// mandatory parm count

    if( m < scr_functions[funcind].parm_cnt ) {
        xx_line_err_c( err_func_parm_miss, p - 1 );
    }

    // collect the optional parm(s)
    if( pchar >= pend ) {               // no optional parms
        k = 0;
    } else {
        for( k = 0; k < scr_functions[funcind].opt_parm_cnt; k++ ) {
            multiletter_function = false;
            var_in_parm = false;
            parms[m + k].a = find_start_of_parm( pchar );
            pchar = find_end_of_parm( parms[m + k].a, pend );

            if( multiletter_function || var_in_parm ) {
                parms[m + k].redo = true;
            } else {
                parms[m + k].redo = false;
            }
            parms[m + k].e = pchar - 1;
            parms[m + k + 1].a = pchar + 1;

            if( pchar >= pend ) {
                break;                  // end of parms
            }
        }
        k += (k < scr_functions[funcind].opt_parm_cnt);
    }
    parmcount = m + k;                  // total parmcount
    parms[parmcount].a = NULL;          // end of parms indicator

    /* Now resolve those parm that need it */

    for( k = 0; k < parmcount; k++ ) {
        while( parms[k].redo ) {
            resbuf = alloc_resbuf( &in_wk );
            strcpy( resbuf, parms[k].a );// copy parm

            ps = resbuf + (parms[k].e - parms[k].a) + 1;
            *ps = '\0';

            parms[k].e = ps - 1;
            parms[k].a = resbuf;
            if( (input_cbs->fmflags & II_research) && GlobalFlags.firstpass ) {
                out_msg( " Function %s parm %s found\n", scr_functions[funcind].fname, resbuf );
            }

            if( !resolve_symvar_functions( resbuf, false ) ) break; // no change, we're done

            pend = parms[k].a + strlen( parms[k].a );
            multiletter_function = false;
            var_in_parm = false;
            pend = find_end_of_parm( parms[k].a, pend );

            if( multiletter_function || var_in_parm ) {
                parms[k].redo = true;
            } else {
                parms[k].redo = false;
            }

            parms[k].e = resbuf + strlen( parms[k].a ) - 1;
            if( (input_cbs->fmflags & II_research) && GlobalFlags.firstpass ) {
                out_msg( " Function      parm %s return\n", resbuf );
            }
        }
    }

    ProcFlags.suppress_msg = multiletter_function;

    cc = scr_functions[funcind].fun( parms, parmcount, result, valsize );

    ProcFlags.suppress_msg = false;

    free_lines( in_wk );

    if( cc != pos ) {                   // error in function
        **result = '&';                 // result is & to preserve the input
        *result += 1;
        **result = '\0';

        ProcFlags.unresolved = true;

        return( pstart );
    }

    ProcFlags.substituted = true;
    return( pret + 1 );                 // all OK new scan position
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
