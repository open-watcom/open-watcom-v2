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

#include "wgml.h"

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

    if( (parmcount < 2) || (parmcount > 3) ) {
        cc = neg;
        return( cc );
    }

    pval = parms[0].start;
    pend = parms[0].stop;

    unquote_if_quoted( &pval, &pend );

    if( pend == pval ) {                // null string nothing to do
        **result = '\0';
        return( pos );
    }

    n   = 0;                            // default start pos
    gn.ignore_blanks = false;

    if( parms[1].stop > parms[1].start ) {// start pos
        gn.argstart = parms[1].start;
        gn.argstop  = parms[1].stop;
        cc = getnum( &gn );
        if( (cc != pos) || (gn.result == 0) ) {
            if( !ProcFlags.suppress_msg ) {
                g_err( err_func_parm, "2 (startpos)" );
                g_info_inp_pos();
                err_count++;
                show_include_stack();
            }
            return( cc );
        }
        n = gn.result - 1;
    }

    if( parmcount > 2 ) {               // evalute length
        if( parms[2].stop > parms[2].start ) {// length specified
            gn.argstart = parms[2].start;
            gn.argstop  = parms[2].stop;
            cc = getnum( &gn );
            if( (cc != pos) || (gn.result == 0) ) {
                if( !ProcFlags.suppress_msg ) {
                    g_err( err_func_parm, "3 (length)" );
                    g_info_inp_pos();
                    err_count++;
                    show_include_stack();
                }
                return( cc );
            }
            len = gn.result;
        }
    }

    k = 0;
    while( (k < n) && (pval < pend) && (ressize > 0) ) {// copy unchanged before startpos
        **result = *pval++;
        *result += 1;
        k++;
        ressize--;
    }

    k = 0;
    while( (k < len) && (pval < pend) ) {  // delete
        pval++;
        k++;
    }

    while( (pval < pend) && (ressize > 0) ) {// copy unchanged
        **result = *pval++;
        *result += 1;
        ressize--;
    }

    **result = '\0';

    return( pos );
}
