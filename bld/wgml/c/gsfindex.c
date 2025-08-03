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
* Description:  WGML implement multi letter function &'index( ) and &'pos( )
*               They are only different in the parameter order:
*                   &'index( haystack, needle,   ... )
*                   &'pos  ( needle,   haystack, ... )
*               and &'lastpos()
****************************************************************************/


#include "wgml.h"


static condcode    get_pos( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize, bool last, bool index )
{
    tok_type        needle;
    tok_type        haystack;
    int             start;
    condcode        cc;
    int             result_index;
    int             haystack_len;
    int             needle_len;
    getnum_block    gn;
    char            *ph;
    char            *pn;

    (void)ressize;

    if( parmcount < 2
      || parmcount > 3 )
        return( CC_neg );

    result_index = 0;

    if( index ) {
        haystack = parms[0].arg;
        needle = parms[1].arg;
    } else {
        needle = parms[0].arg;
        haystack = parms[1].arg;
    }

    needle_len = unquote_arg( &needle );
    haystack_len = unquote_arg( &haystack );
    if( haystack_len > 0
      && needle_len > 0
      && haystack_len >= needle_len ) {
        start = 0;      /* default start pos */
        if( parmcount > 2 ) {
            /*
             * evalute start pos (optional)
             */
            if( parms[2].arg.s < parms[2].arg.e ) {
                gn.arg = parms[2].arg;
                gn.ignore_blanks = false;
                cc = getnum( &gn );
                if( (cc != CC_pos) || (gn.result == 0) ) {
                    if( !ProcFlags.suppress_msg ) {
                        xx_source_err_exit_c( ERR_FUNC_PARM, "3 (startpos)" );
                        /* never return */
                    }
                    return( cc );
                }
                start = gn.result - 1;
            }
        }

        if( haystack_len >= start + needle_len ) {
            if( last ) {
                /*
                 * search last
                 */
                for( ph = haystack.e - 1; ph >= haystack.s + start; ph-- ) {
                    pn = needle.e - 1;
                    while( (*ph == *pn) && (pn >= needle.s)) {
                        ph--;
                        pn--;
                    }
                    if( pn < needle.s ) {
                        result_index = ph - haystack.s + 2;    // found, set index
                        break;
                    }
                }
            } else {
                /*
                 * search first
                 */
                for( ph = haystack.s + start; ph < haystack.e - (needle_len - 1); ph++ ) {
                    pn = needle.s;
                    while( (*ph == *pn) && (pn < needle.e)) {
                        ph++;
                        pn++;
                    }
                    if( pn >= needle.e ) {
                        result_index = ph - haystack.s - needle_len + 1; // found, set index
                        break;
                    }
                }
            }
        }
    }

    *result += sprintf( *result, "%d", result_index );

    return( CC_pos );
}

/***************************************************************************/
/*                                                                         */
/* &'index(haystack,needle<,start>):   The  Index  function  returns  the  */
/*    character position of the string 'needle' in the string 'haystack'.  */
/*    If not found, the function returns zero.  The first character posi-  */
/*    tion to be searched in 'haystack' may be specified with the 'start'  */
/*    number and  it defaults to the  first character position  in 'hays-  */
/*    tack'.                                                               */
/*      &'index('abcdef','c') ==> 3                                        */
/*      &'index('abcdef','cd') ==> 3                                       */
/*      &'index('abcdef','yz') ==> 0                                       */
/*      &'index('ab','abcdef') ==> 0                                       */
/*      &'index('ababab','ab',2) ==> 3                                     */
/*      &'index('ababab','ab',6) ==> 0                                     */
/*                                                                         */
/***************************************************************************/

condcode    scr_index( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    return( get_pos( parms, parmcount, result, ressize, false, true ) );
}

/***************************************************************************/
/*                                                                         */
/* &'pos(needle,haystack<,start>):   The  Position function  returns  the  */
/*    character position  of the first  occurrence of 'needle'  in 'hays-  */
/*    tack'.   The  search for a  match starts  at the first  position of  */
/*    'haystack' but  may be  overridden by  adding a  'start' column  in  */
/*    'haystack'.    If the  'needle' string  is  not found,   a zero  is  */
/*    returned.                                                            */
/*      &'pos('c','abcde') ==> 3                                           */
/*      &'pos(x,abcde) ==> 0                                               */
/*      &'pos(abcde,abcde) ==> 1                                           */
/*      &'pos(a,aaaaa) ==> 1                                               */
/*      &'pos('a','aaaaa') ==> 1                                           */
/*      &'pos(a,aaaaa,3) ==> 3                                             */
/*      &'pos(12345678,abc) ==> 0                                          */
/*      &'pos(a) ==> error, too few operands                               */
/*      &'pos(a,abcd,junk) ==> error, 'start' not numeric                  */
/*      &'pos(a,abcd,3,'.') ==> error, too many operands                   */
/*                                                                         */
/***************************************************************************/

condcode    scr_pos( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    return( get_pos( parms, parmcount, result, ressize, false, false ) );
}

/***************************************************************************/
/*                                                                         */
/* &'lastpos(needle,haystack<,start>):    The  Last   Position   function  */
/*    returns the starting  character of the last  occurrence of 'needle'  */
/*    in 'haystack'.  The first position to search from in 'haystack' may  */
/*    be specified  with 'start' and  this defaults  to the start  of the  */
/*    'haystack' string.  If no match for 'needle' can be found in 'hays-  */
/*    tack' then zero is returned.                                         */
/*      &'lastpos('c','abcde') ==> 3                                       */
/*      &'lastpos(x,abcde) ==> 0                                           */
/*      &'lastpos(abcde,abcde) ==> 1                                       */
/*      &'lastpos(a,aaaaa) ==> 5                                           */
/*      &'lastpos('a','aaaaa') ==> 5                                       */
/*      &'lastpos(a,aaaaa,3) ==> 5                                         */
/*      &'lastpos(a,aaaaa,10) ==> 0                                        */
/*      &'lastpos(a) ==> error, too few operands                           */
/*                                                                         */
/***************************************************************************/

condcode    scr_lpos( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    return( get_pos( parms, parmcount, result, ressize, true, false ) );
}
