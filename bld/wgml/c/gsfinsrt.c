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
* Description:  WGML implement multi letter function &'insert( )
*
****************************************************************************/


#include "wgml.h"


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
/***************************************************************************/

condcode    scr_insert( parm parms[MAX_FUN_PARMS], unsigned parmcount, char **result, unsigned ressize )
{
    tok_type        new;
    tok_type        target;
    int             n;
    int             length;
    char            padchar;
    condcode        cc;
    int             k;
    int             new_len;
    int             target_len;
    getnum_block    gn;

    if( parmcount < 2
      || parmcount > 5 )
        return( CC_neg );

    new = parms[0].arg;
    new_len = unquote_arg( &new );

    target = parms[1].arg;
    target_len = unquote_arg( &target );

    n = 0;                  // default start pos
    if( parmcount > 2 ) {   // evalute startpos
        if( parms[2].arg.s < parms[2].arg.e ) {
            gn.arg = parms[2].arg;
            gn.ignore_blanks = false;
            cc = getnum( &gn );
            if( cc != CC_pos ) {
                if( !ProcFlags.suppress_msg ) {
                    xx_source_err_exit_c( ERR_FUNC_PARM, "3 (startpos)" );
                    /* never return */
                }
                return( cc );
            }
            n = gn.result;
        }
    }

    length = new_len;       // default length
    if( parmcount > 3 ) {   // evalute length
        if( parms[3].arg.s < parms[3].arg.e ) {
            gn.arg = parms[3].arg;
            gn.ignore_blanks = false;
            cc = getnum( &gn );
            if( cc != CC_pos ) {
                if( !ProcFlags.suppress_msg ) {
                    xx_source_err_exit_c( ERR_FUNC_PARM, "4 (length)" );
                    /* never return */
                }
                return( cc );
            }
            length = gn.result;
        }
    }

    padchar = ' ';          // default pad character
    if( parmcount > 4 ) {   // evalute length
        tok_type pad = parms[3].arg;
        if( unquote_arg( &pad ) > 0 ) {
            padchar = *pad.s;
        }
    }

    k = 0;
    /*
     * copy target up to startpos
     */
    while( (k < n) && (target.s < target.e) && (ressize > 0) ) {
        *(*result)++ = *target.s++;
        k++;
        ressize--;
    }
    /*
     * pad up to startpos (if needed)
     */
    while( (k < n) && (ressize > 0) ) {
        *(*result)++ = padchar;
        k++;
        ressize--;
    }
    /*
     * insert new string up to length
     */
    while( (k < n + length) && (new.s < new.e) && (ressize > 0) ) {
        *(*result)++ = *new.s++;
        k++;
        ressize--;
    }
    /*
     * pad up to length (if needed)
     */
    while( (k < n + length) && (ressize > 0) ) {
        *(*result)++ = padchar;
        k++;
        ressize--;
    }
    /*
     * copy rest of target (if any)
     */
    while( (target.s < target.e) && (ressize > 0) ) {
        *(*result)++ = *target.s++;
        ressize--;
    }

    **result = '\0';

    return( CC_pos );
}
