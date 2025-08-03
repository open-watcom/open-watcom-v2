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
* Description:  WGML implement multi letter functions  &'vecpos( )
*                                                  &'veclastpos( )
*
*               The optional parms START and CASE are not implemented
*               as they are not used in OW doc build system
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*  script string functions &'vecpos(                                      */
/*                          &'lastvecpos(                                  */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'vecpos(needle,haystack<,<start><,case>>):  The Vector Position func-  */
/*    tion returns the subscript number of  the first occurrence of 'nee-  */
/*    dle' in 'haystack'  set symbol.   The search for a  match starts at  */
/*    the first element  of 'haystack' but may be overridden  by adding a  */
/*    'start'  subscript in  'haystack'.    The default  case of 'M'  or  */
/*    'Mixed' searches for  an element which matches exactly,   a case of  */
/*    'U' or  'Upper' will match the  'needle' and 'haystack'  element in  */
/*    uppercase.    If the  'needle'  string is  not  found,   a zero  is  */
/*    returned.                                                            */
/*      .se rodent(1) = 'Gerbil'                                           */
/*      .se rodent(2) = 'Hamster'                                          */
/*      .se rodent(3) = 'Mouse'                                            */
/*      .se rodent(4) = 'Rabbit'                                           */
/*      &'vecpos('Hamster',rodent) ==> 2                                   */
/*      &'vecpos(Dog,rodent) ==> 0                                         */
/*      &'vecpos(Hamster,rodent,3) ==> 0                                   */
/*      &'vecpos(HAMSTER,rodent) ==> 0                                     */
/*      &'vecpos(HAMSTER,rodent,1,U) ==> 2                                 */
/*      &'vecpos(Mouse) ==> error, too few operands                        */
/*                                                                         */
/* ! start and case are NOT implemented                                    */
/*                                                                         */
/***************************************************************************/


/***************************************************************************/
/* &'veclastpos(needle,haystack<,<start><,case>>):  The Vector Last Posi-  */
/*    tion function returns  the subscript number of  the last occurrence  */
/*    of 'needle'  in 'haystack'  set symbol.    The search  for a  match  */
/*    starts at the first element of  'haystack' but may be overridden by  */
/*    adding a 'start' subscript in 'haystack'.   The default case of 'M'  */
/*    or 'Mixed' searches for an element which matches exactly, a case of  */
/*    'U' or  'Upper' will match the  'needle' and 'haystack'  element in  */
/*    uppercase.    If the  'needle'  string is  not  found,   a zero  is  */
/*    returned.                                                            */
/*                                                                         */
/* ! start and case are NOT implemented                                    */
/*                                                                         */
/***************************************************************************/

static  condcode    get_vector_pos( parm parms[MAX_FUN_PARMS], unsigned parmcount,
                               char **result, unsigned ressize, bool first )
{
    tok_type        needle;
    tok_type        haystack;
#if 0
    int             start;
    char                        casechar;
#endif
    int             rc;
    int             index;
    int             haystack_len;
    int             needle_len;
    char            c;
    sub_index       var_ind;
    symvar          symvar_entry;
    symsub          *symsubval;
    symvar          *psymvar;
    bool            suppress_msg;

    (void)ressize;

    if( parmcount < 2
      || parmcount > 4 )
        return( CC_neg );

    index = 0;

    needle = parms[0].arg;
    needle_len = unquote_arg( &needle );

    /*
     * haystack should be a symbol
     * scan_sym need include quotes for correct symbol parsing
     * don't use unquote_arg for haystack
     */
    haystack = parms[1].arg;
    haystack_len = parms[1].arg.e - parms[1].arg.s;

    if( (haystack_len > 0)              // not null string
      || (needle_len > 0) ) {           // needle not null
#if 0
        start = 0;
        if( parmcount > 2 ) {
            if( parms[2].arg.s < parms[2].arg.e ) {
                gn.arg = parms[2].arg;
                gn.ignore_blanks = false;
                cc = getnum( &gn );
                if( (cc != CC_pos) ) {
                    if( !ProcFlags.suppress_msg ) {
                        xx_source_err_exit_c( ERR_FUNC_PARM, "3 (startpos)" );
                        /* never return */
                    }
                    return( cc );
                }
                start = gn.result;
            }
        }
        casechar = 'M';
        if( parmcount > 3 ) {
            tok_type tmp = parms[3].arg;
            if( unquote_arg( &tmp ) > 0 ) {
                casechar = my_toupper( *tmp.s );
            }
        }
#endif
        g_scan_err = false;
        suppress_msg = ProcFlags.suppress_msg;
        ProcFlags.suppress_msg = true;
        scan_sym( haystack.s, &symvar_entry, &var_ind, NULL, false );
        ProcFlags.suppress_msg = suppress_msg;;
        if( !g_scan_err ) {
            rc = find_symvar_sym( &symvar_entry, var_ind, &symsubval );
            if( rc > 0 ) {              // variable found
                psymvar = symsubval->base;
                if( psymvar->flags & SF_subscripted ) {
                    c = *needle.e;
                    *needle.e = '\0';   // make nul delimited
                    for( symsubval = psymvar->subscripts; symsubval != NULL; symsubval = symsubval->next ) {
                        if( strcmp( symsubval->value, needle.s ) == 0 ) {
                           index = symsubval->subscript;
                           /*
                            * finished for first position search
                            * go for last position search
                            */
                           if( first ) {
                               break;
                           }
                        }
                    }
                    *needle.e = c;
                }
            }
        }
    }

    *result += sprintf( *result, "%d", index );

    return( CC_pos );
}


/*
 * &'vecpos(
 *
 */

condcode    scr_vecpos( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    return( get_vector_pos( parms, parmcount, result, ressize, true ) );
}


/*
 * &'veclastpos(
 *
 */

condcode    scr_veclastpos( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    return( get_vector_pos( parms, parmcount, result, ressize, false ) );
}
