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
* Description:  WGML implement multi letter function &'left( )
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/*  script string function &'left(                                         */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'left(string,length<,pad>):   To  generate  a   character  string  of  */
/*    'length' characters  with 'string' in  its start.   If  'length' is  */
/*    longer than 'string' then 'pad' the result.                          */
/*      &'left('ABC D',8) ==> "ABC D   "                                   */
/*      &'left('ABC D',8,'.') ==> "ABC D..."                               */
/*      &'left('ABC  DEF',7) ==> "ABC  DE"                                 */
/*                                                                         */
/* ! optional parm PAD is NOT implemented                                  */
/*                                                                         */
/***************************************************************************/

condcode    scr_left( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    char            *   pval;
    char            *   pend;
    condcode            cc;
    int                 k;
    int                 len;
    getnum_block        gn;
    char                linestr[MAX_L_AS_STR];

    if( parmcount != 2 ) {
        cc = neg;
        return( cc );
    }

    pval = parms[0].start;
    pend = parms[0].stop - 1;

    unquote_if_quoted( &pval, &pend );

    len = pend - pval + 1;              // default length

    if( len <= 0 ) {                    // null string nothing to do
        **result = '\0';
        return( pos );
    }

    if( parms[1].stop > parms[1].start ) {// length specified
        gn.argstart = parms[1].start;
        gn.argstop  = parms[1].stop;
        cc = getnum( &gn );
        if( cc != pos ) {
            if( !ProcFlags.suppress_msg ) {
                g_err( err_func_parm, "2 (length)" );
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
        len = gn.result;
    }

    for( k = 0; k < len; k++ ) {        // copy from start
        if( (pval > pend) || (ressize <= 0) ) {
            break;
        }
        **result = *pval++;
        *result += 1;
        ressize--;
    }

    for( ; k < len; k++ ) {             // pad to length
        if( ressize <= 0 ) {
            break;
        }
        **result = ' ';
        *result += 1;
        ressize--;
    }

    **result = '\0';

    return( pos );
}

