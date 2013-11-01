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

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/*    SCR multi letter functions                                           */
/***************************************************************************/

#define pick( name, length, parms, optparms, routine ) \
            { #name, length, parms, optparms, routine },

static  const   scrfunc scr_functions[] = {

#include "gsfuncs.h"

    { " ", 0, 0, 0, NULL }              // end
};

#define SCR_FUNC_MAX (sizeof( scr_functions ) / sizeof( scr_functions[0] ) - 1)

static const char   ampchar = '&';
static bool         multiletter_function;   // parm contains function
static bool         var_in_parm;        // parm contains variable



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
 *      string delimiters are several chars
 *
 */

static  char    * find_end_of_parm( char * pchar, char * pend )
{
#define max_paren 50                    // should be enough
    char    quotechar[max_paren];
    bool    instring[max_paren];
    int     paren_level;
    int     delta_paren;
    char    c;
    char    cm1;
    char    cm2;
    bool    finished;
    bool    test_for_quote;             // only at first char after ( or ,

    paren_level = 0;
    delta_paren = 0;
    quotechar[paren_level] ='\0';
    instring[paren_level] = false;
    instring[1] = false;
    finished = false;
    test_for_quote = true;
    cm1 = '\0';
    cm2 = '\0';
    c = '\0';
    for(  ; *pchar != '\0' ; pchar++ ) {
        cm2 = cm1;
        cm1 = c;
        c = *pchar;
        if( cm1 == ampchar ) {
            if( c == '\'' ) {
                multiletter_function = true;// parm contains a function
                delta_paren = 1;
                instring[paren_level + 1] = false;
            } else {
                var_in_parm = true;     // parm contains variable
            }
        }
        if( instring[paren_level] ) {
            if( c == quotechar[paren_level] ) {
                instring[paren_level] = false;
            }
        } else {
            if( test_for_quote && is_quote_char( c ) ) {
                if( (cm1 == ampchar) || // &' sequence
                    ((cm2 == ampchar) && isalpha( cm1 )) ) {// &X' sequence
                            /* no instring change */
                } else {
                    instring[paren_level] = true;
                    quotechar[paren_level] = c;
                }
                test_for_quote = false;
            } else {
                switch( c ) {
                case    '(' :
                    paren_level += 1 + delta_paren;
                    delta_paren = 0;
                    test_for_quote = true;
                    if( paren_level < max_paren ) {
                        instring[paren_level] = false;
                    } else {
                        paren_level--;
                        finished = true;// error msg ??? TBD
                    }
                    break;
                case    ')' :
                    if( paren_level <= 1 ) {
                        finished = true;
                    }
                    paren_level--;
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
        if( finished || (pchar >= pend)) {
            break;
        }
    }
    return( pchar );
#undef max_paren
}


/***************************************************************************/
/*  find start of parm                                                     */
/*     leading blanks are skipped                                          */
/***************************************************************************/

static  char    * find_start_of_parm( char * pchar )
{
    ++pchar;                            // over ( or ,
    if( *pchar == ' ' ) {
        while( *pchar == ' ' ) {        // over unquoted blanks
            ++pchar;
        }
    }
    return( pchar );
}


/***************************************************************************/
/*  err_info  give some info about error                                   */
/***************************************************************************/

static  void    err_info( char * * result )
{
    char                linestr[MAX_L_AS_STR];

    if( input_cbs->fmflags & II_macro ) {
        ultoa( input_cbs->s.m->lineno, linestr, 10 );
        g_info( inf_mac_line, linestr, input_cbs->s.m->mac->name );
    } else {
        ultoa( input_cbs->s.f->lineno, linestr, 10 );
        g_info( inf_file_line, linestr, input_cbs->s.f->filename );
    }
    err_count++;
    show_include_stack();

    **result = '&';                 // result is & to preserve the input
    *result += 1;
    **result = '\0';
}


/***************************************************************************/
/*  scr_multi_funcs  isolate function name, lookup name in table           */
/*                   scan function arguments                               */
/*                   call corresponding function                           */
/***************************************************************************/

char  * scr_multi_funcs( char * in, char * end, char ** result, int32_t valsize )
{
    char            *   pchar;
    int                 rc;
    size_t              fnlen;
    int                 funcind;
    int                 k;
    int                 m;
    char                fn[FUN_NAME_LENGTH + 1];
    bool                found;
    parm                parms[MAX_FUN_PARMS];
    int                 parmcount;
    condcode            cc;
    char            *   ps;
    char            *   resbuf;
    inp_line        *   in_wk;

    in_wk = NULL;                       // no result buffer yet
    rc = 0;
    fnlen = 0;
    pchar = in + 2;                     // over &' to function name

    // collect function name
    while( *pchar && pchar <= end && is_function_char( *pchar ) ) {
        fn[fnlen] = *pchar++;
        if( fnlen < FUN_NAME_LENGTH ) {
            fnlen++;
        } else {
            break;
        }
    }
    fn[fnlen] = '\0';

    if( *pchar != '(' ) {         // open paren does not follow function name
        g_err( err_func_parm_miss );
        err_info( result );
        return( in + 1 );               // avoid endless loop
    }

    // test for valid functionname
    found = false;
    for( k = 0; k < SCR_FUNC_MAX; k++ ) {
        if( fnlen == scr_functions[k].length
            && !stricmp( fn, scr_functions[k].fname ) ) {

            found = true;
            if( input_cbs->fmflags & II_research && GlobalFlags.firstpass ) {
                out_msg( " Function %s found\n", scr_functions[k].fname );
                add_multi_func_research( fn );
            }
            break;
        }
    }
    if( !found ) {
        g_err( err_func_name, fn );
        err_info( result );
        return( in + 1 );               // avoid endless loop
    }
    funcind = k;

    // collect the mandatory parm(s)

    for( k = 0; k < scr_functions[funcind].parm_cnt; k++ ) {

        multiletter_function = false;
        var_in_parm = false;
        parms[k].incomplete = false;

        parms[k].a = find_start_of_parm( pchar );
        pchar = find_end_of_parm( parms[k].a, end );

        if( multiletter_function || var_in_parm ) {
            parms[k].incomplete = true;
        }
        if( parms[k].incomplete ) {

            resbuf = alloc_resbuf( &in_wk );
            strcpy_s( resbuf, buf_size, parms[k].a);// copy parm

            ps = resbuf + (pchar - parms[k].a);
            *ps = '\0';
            parms[k].e = ps - 1;
            parms[k].a = resbuf;
            if( input_cbs->fmflags & II_research && GlobalFlags.firstpass ) {
                out_msg( " Function %s parm %s found\n",
                         scr_functions[funcind].fname, resbuf );
            }

            resolve_symvar_functions( resbuf );

            parms[k].e = resbuf + strnlen_s( resbuf, buf_size ) - 1;
            if( input_cbs->fmflags & II_research && GlobalFlags.firstpass ) {
                out_msg( " Function      parm %s return\n", resbuf );
            }
        } else {
            parms[k].e = pchar - 1;
        }
        parms[k + 1].a = pchar + 1;

        if( (*pchar == ')') || (pchar >= end) ) {
            break;                      // end of parms
        }
    }
    m = k + (k < scr_functions[funcind].parm_cnt);// mandatory parm count

    if( m < scr_functions[funcind].parm_cnt ) {
        g_err( err_func_parm_miss );
        err_info( result );
        return( in + 1 );               // avoid endless loop
    }

    // collect the optional parm(s)
    if( (*pchar == ')') || (pchar >= end) ) {   // no optional parms
        k = 0;
    } else {
        for( k = 0; k < scr_functions[funcind].opt_parm_cnt; k++ ) {
            multiletter_function = false;
            var_in_parm = false;
            parms[m + k].incomplete = false;
            parms[m + k].a = find_start_of_parm( pchar );
            pchar = find_end_of_parm( parms[m + k].a, end );

            if( multiletter_function || var_in_parm ) {
                parms[m + k].incomplete = true;
            }
            if( parms[m + k].incomplete ) {

                resbuf = alloc_resbuf( &in_wk );
                strcpy_s( resbuf, buf_size, parms[m + k].a);// copy parm

                ps = resbuf + (pchar - parms[m + k].a);
                *ps = '\0';

                parms[m + k].e = ps - 1;
                parms[m + k].a = resbuf;
                if( input_cbs->fmflags & II_research && GlobalFlags.firstpass ) {
                    out_msg( " Function %s parm %s found\n",
                             scr_functions[funcind].fname, resbuf );
                }

                resolve_symvar_functions( resbuf );

                parms[m + k].e = resbuf + strnlen_s( resbuf, buf_size ) - 1;
                if( input_cbs->fmflags & II_research && GlobalFlags.firstpass ) {
                    out_msg( " Function      parm %s return\n", resbuf );
                }
            } else {
                parms[m + k].e = pchar - 1;
            }
            parms[m + k + 1].a = pchar + 1;

            if( (*pchar == ')') || (pchar >= end) ) {
                break;                  // end of parms
            }
        }
        k += (k < scr_functions[funcind].opt_parm_cnt);
    }
    parmcount = m + k;                  // total parmcount
    parms[parmcount].a = NULL;          // end of parms indicator

    if( *pchar != ')' ) {
        g_err( err_func_parm_end );
        err_info( result );
        return( in + 1 );               // avoid endless loop
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

        return( in + 1 );
    }

    ProcFlags.substituted = true;
    return( pchar + 1 );                // all OK new scan position
}


#if 0

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

#endif
