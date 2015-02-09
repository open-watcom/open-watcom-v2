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
* Description:  WGML implement multi letter function &'insert( )
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/*  script string function &'insert(                                       */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* &'insert(new,target<,<n><,<length><,pad>>>):   To  Insert   the  'new'  */
/*    string with  length 'length' into  the 'target' string  after char-  */
/*    acter 'n'.   If 'n' is omitted then  'new' is inserted at the start  */
/*    of 'target'.   The 'pad' character may  be used to extend the 'tar-  */
/*    get' string to length 'n' or the 'new' string to length 'length'.    */
/*      "&'insert(' ','abcdef',3)" ==> "abc def"                           */
/*      "&'insert('123','abc',5,6)" ==> "abc  123   "                      */
/*      "&'insert('123','abc',5,6,'+')" ==> "abc++123+++"                  */
/*      "&'insert('123','abc')" ==> "123abc"                               */
/*      "&'insert('123','abc',5,,'-')" ==> "abc--123"                      */
/*      "&'insert('123','abc',,,'-')" ==> "123abc"                         */
/*                                                                         */
/* ! optional parms LENGTH and PAD are NOT implemented                     */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

condcode    scr_insert( parm parms[MAX_FUN_PARMS], size_t parmcount, char * * result, int32_t ressize )
{
    char            *   pval;
    char            *   pend;
    condcode            cc;
    int                 k;
    int                 n;
    int                 len;
    getnum_block        gn;
    char            *   ptarget;
    char            *   ptargetend;
    char                linestr[MAX_L_AS_STR];

    if( (parmcount < 2) || (parmcount > 3) ) {
        cc = neg;
        return( cc );
    }

    pval = parms[0].a;                // string to insert
    pend = parms[0].e;

    unquote_if_quoted( &pval, &pend );

    len = pend - pval + 1;              // length to insert


    ptarget    = parms[1].a;          // string to be modified
    ptargetend = parms[1].e;

    unquote_if_quoted( &ptarget, &ptargetend );


    if( len <= 0 ) {                    // null string insert nothing to do
        **result = '\0';
        return( pos );
    }

    n = 0;                              // default start pos
    gn.ignore_blanks = false;

    if( parmcount > 2 ) {               // evalute startpos
        if( parms[2].e >= parms[2].a ) {
            gn.argstart = parms[2].a;
            gn.argstop  = parms[2].e + 1;
            cc = getnum( &gn );
            if( cc != pos ) {
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
            n = gn.result;
        }
    }

    k = 0;
    while( (k < n) && (ptarget <= ptargetend) && (ressize > 0) ) { // copy up to startpos
        **result = *ptarget++;
        *result += 1;
        k++;
        ressize--;
    }
    if( n > k ) {         // startpos > target length, insert one extra blank
        **result = ' ';
        *result += 1;
        ressize--;
    }

    while( (pval <= pend) && (ressize > 0) ) { // insert new string
        **result = *pval++;
        *result += 1;
        ressize--;
    }

    while( (ptarget <= ptargetend) && (ressize > 0) ) { // copy rest (if any)
        **result = *ptarget++;
        *result += 1;
        ressize--;
    }

    **result = '\0';

    return( pos );
}

