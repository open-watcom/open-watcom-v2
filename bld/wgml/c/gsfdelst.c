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
* Description:  WGML implement multi letter function &'delstr( )
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/*  script string function &'delstr(                                       */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/* &'delstr(string,n<,length>):  The  Delete String function  deletes the  */
/*    part of 'string' starting at character  number 'n'.   The number of  */
/*    characters can be specified in 'length' and defaults from character  */
/*    'n' to the end of the string.  If 'string' is less than 'n' charac-  */
/*    ters long, then nothing is deleted.                                  */
/*      &'delstr('abcdef',3) ==> ab                                        */
/*      &'delstr('abcdef',3,2) ==> abef                                    */
/*      &'delstr('abcdef',10) ==> abcdef                                   */
/***************************************************************************/

condcode    scr_delstr( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    char            *   pval;
    char            *   pend;
    condcode            cc;
    int                 k;
    int                 n;
    int                 len;
    getnum_block        gn;
    char                linestr[MAX_L_AS_STR];

    if( (parmcount < 2) || (parmcount > 3) ) {
        cc = neg;
        return( cc );
    }

    pval = parms[0].a;
    pend = parms[0].e;

    unquote_if_quoted( &pval, &pend );

    len = pend - pval + 1;              // default length

    if( len <= 0 ) {                    // null string nothing to do
        **result = '\0';
        return( pos );
    }

    n   = 0;                            // default start pos
    gn.ignore_blanks = false;

    if( parms[1].e >= parms[1].a ) {// start pos
        gn.argstart = parms[1].a;
        gn.argstop  = parms[1].e + 1;
        cc = getnum( &gn );
        if( (cc != pos) || (gn.result == 0) ) {
            if( !ProcFlags.suppress_msg ) {
                g_err( err_func_parm, "2 (startpos)" );
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

    if( parmcount > 2 ) {               // evalute length
        if( parms[2].e >= parms[2].a ) {// length specified
            gn.argstart = parms[2].a;
            gn.argstop  = parms[2].e + 1;
            cc = getnum( &gn );
            if( (cc != pos) || (gn.result == 0) ) {
                if( !ProcFlags.suppress_msg ) {
                    g_err( err_func_parm, "3 (length)" );
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
    }

    k = 0;
    while( (k < n) && (pval <= pend) && (ressize > 0) ) {// copy unchanged before startpos
        **result = *pval++;
        *result += 1;
        k++;
        ressize--;
    }

    k = 0;
    while( (k < len) && (pval <= pend) ) {  // delete
        pval++;
        k++;
    }

    while( (pval <= pend) && (ressize > 0) ) {// copy unchanged
        **result = *pval++;
        *result += 1;
        ressize--;
    }

    **result = '\0';

    return( pos );
}
