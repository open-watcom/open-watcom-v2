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
* Description:  WGML implement multi letter function &'index( ) and &'pos( )
*               They are only different in the parameter order:
*                   &'index( haystack, needle,   ... )
*                   &'pos  ( needle,   haystack, ... )
*               and &'lastpos()
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

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

condcode    scr_index( parm parms[MAX_FUN_PARMS], size_t parmcount, char **result, int32_t ressize )
{
    char            *   pneedle;
    char            *   pneedlend;
    char            *   phay;
    char            *   phayend;
    condcode            cc;
    int                 index;
    int                 n;
    int                 hay_len;
    int                 needle_len;
    getnum_block        gn;
    char            *   ph;
    char            *   pn;
    char                linestr[MAX_L_AS_STR];

    ressize = ressize;
    if( (parmcount < 2) || (parmcount > 3) ) {
        cc = neg;
        return( cc );
    }

    phay = parms[0].start;
    phayend = parms[0].stop - 1;

    unquote_if_quoted( &phay, &phayend );
    hay_len = phayend - phay + 1;       // haystack length

    pneedle = parms[1].start;
    pneedlend = parms[1].stop - 1;

    unquote_if_quoted( &pneedle, &pneedlend );
    needle_len = pneedlend - pneedle + 1;   // needle length

    n   = 0;                            // default start pos
    gn.ignore_blanks = false;

    if( parmcount > 2 ) {               // evalute start pos
        if( parms[2].stop > parms[2].start ) {// start pos specified
            gn.argstart = parms[2].start;
            gn.argstop  = parms[2].stop;
            cc = getnum( &gn );
            if( (cc != pos) || (gn.result == 0) ) {
                if( !ProcFlags.suppress_msg ) {
                    g_err( err_func_parm, "3 (startpos)" );
                    if( input_cbs->fmflags & II_macro ) {
                        ultoa( input_cbs->s.m->lineno, linestr, 10 );
                        g_info( inf_mac_line, linestr, input_cbs->s.m->mac->name );
                    } else {
                        ultoa( input_cbs->s.f->lineno, linestr, 10 );
                        g_info( inf_file_line, linestr, input_cbs->s.f->filename );
                    }
                    err_count++;
                    show_include_stack();
                }
                return( cc );
            }
            n = gn.result - 1;
        }
    }

    if( (hay_len <= 0) ||               // null string nothing to do
        (needle_len <= 0) ||            // needle null nothing to do
        (needle_len > hay_len) ||       // needle longer haystack
        (n + needle_len > hay_len) ) {  // startpos + needlelen > haystack
                                        // ... match impossible

        **result = '0';                 // return index zero
        *result += 1;
        **result = '\0';
        return( pos );
    }

    ph = phay + n;                      // startpos in haystack
    pn = pneedle;
    index = 0;

    for( ph = phay + n; ph <= phayend - needle_len + 1; ph++ ) {
        pn = pneedle;
        while( (*ph == *pn) && (pn <= pneedlend)) {
            ph++;
            pn++;
        }
        if( pn > pneedlend ) {
            index = ph - phay - needle_len + 1; // found, set index
            break;
        }
    }

    *result += sprintf( *result, "%d", index );

    return( pos );
}

/*
 * scr_pos : swap parm1 and parm2, then call scr_index
 *
 */

condcode    scr_pos( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    char            *   pwk;

    if( parmcount < 2 ) {
        return( neg );
    }

    pwk = parms[0].start;
    parms[0].start = parms[1].start;
    parms[1].start = pwk;

    pwk = parms[0].stop;
    parms[0].stop = parms[1].stop;
    parms[1].stop = pwk;

    return( scr_index( parms, parmcount, result, ressize ) );
}

/***************************************************************************/
/*  lastpos                                                                */
/***************************************************************************/

condcode    scr_lpos( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    char            *   pneedle;
    char            *   pneedlend;
    char            *   phay;
    char            *   phayend;
    condcode            cc;
    int                 index;
    int                 n;
    int                 hay_len;
    int                 needle_len;
    getnum_block        gn;
    char            *   ph;
    char            *   pn;
    char                linestr[MAX_L_AS_STR];

    ressize = ressize;
    if( (parmcount < 2) || (parmcount > 3) ) {
        cc = neg;
        return( cc );
    }

    pneedle = parms[0].start;
    pneedlend = parms[0].stop - 1;

    unquote_if_quoted( &pneedle, &pneedlend );
    needle_len = pneedlend - pneedle + 1;   // needle length

    phay = parms[1].start;
    phayend = parms[1].stop - 1;

    unquote_if_quoted( &phay, &phayend );
    hay_len = phayend - phay + 1;       // haystack length

    n   = 0;                            // default start pos
    gn.ignore_blanks = false;

    if( parmcount > 2 ) {               // evalute start pos
        if( parms[2].stop > parms[2].start ) {// start pos specified
            gn.argstart = parms[2].start;
            gn.argstop  = parms[2].stop;
            cc = getnum( &gn );
            if( (cc != pos) || (gn.result == 0) ) {
                if( !ProcFlags.suppress_msg ) {
                    g_err( err_func_parm, "3 (startpos)" );
                    if( input_cbs->fmflags & II_macro ) {
                        ultoa( input_cbs->s.m->lineno, linestr, 10 );
                        g_info( inf_mac_line, linestr, input_cbs->s.m->mac->name );
                    } else {
                        ultoa( input_cbs->s.f->lineno, linestr, 10 );
                        g_info( inf_file_line, linestr, input_cbs->s.f->filename );
                    }
                    err_count++;
                    show_include_stack();
                }
                return( cc );
            }
            n = gn.result - 1;
        }
    }

    if( (hay_len <= 0) ||               // null string nothing to do
        (needle_len <= 0) ||            // needle null nothing to do
        (needle_len > hay_len) ||       // needle longer haystack
        (n + needle_len > hay_len) ) {  // startpos + needlelen > haystack
                                        // ... match impossible

        **result = '0';                 // return index zero
        *result += 1;
        **result = '\0';
        return( pos );
    }

    ph = phay + n;                      // startpos in haystack
    pn = pneedle;
    index = 0;

    for( ph = phayend; ph >= phay + n ; ph-- ) {
        pn = pneedlend;
        while( (*ph == *pn) && (pn >= pneedle)) {
            ph--;
            pn--;
        }
        if( pn < pneedle ) {
            index = ph - phay + 2;          // found, set index
            break;
        }
    }

    *result += sprintf( *result, "%d", index );

    return( pos );
}
