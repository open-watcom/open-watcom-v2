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
* Description: utility functions for tags:
*         get_att_value        --- get the value for an attribute
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "gvars.h"

/***************************************************************************/
/* set the attribute value and report tag-end ('.') if found               */
/*     [<white space>]=[<white space>]<value>                              */
/***************************************************************************/

char * get_att_value( char * p )
{
    char        quote;

    ProcFlags.tag_end_found = false;
    val_start = NULL;
    while( *p == ' ' ) {                // over WS to '='
        p++;
    }
    if( *p == '=' ) {
        p++;
        while( *p == ' ' ) {            // over WS to value
            p++;
        }
    } else {
        if( *p == '.' ) {
            ProcFlags.tag_end_found = true;
        }
        xx_line_err( err_eq_missing, p );
        scan_start = scan_stop + 1;
        return( p );
    }
    if( (*p == '\0') || (*p == '.') ) { // value is missing
        if( *p == '.' ) {
            ProcFlags.tag_end_found = true;
        }
        xx_line_err( err_att_val_missing, p );
        scan_start = scan_stop + 1;
        return( p );
    }
    if( *p == '"' || *p == '\'' ) {
        quote = *p;
        ++p;
        val_start = p;
        while( *p ) {
            if( *p == quote ) {
                if( *(p + 1) != quote ) {
                    break;
                }
                { // this should almost never be used
                    char    *   q;
                    char    *   r;
                    q = p;
                    r = p + 1;
                    while( *r ) {
                        *q = *r;
                        q++;
                        r++;
                    }
                }
            }
            ++p;
        }
        val_len = p - val_start;    // up to (not including) final quote
        if( *p != quote ) {         // terminating quote not found
            xx_line_err( err_att_val_open, val_start - 1 );
            scan_start = scan_stop + 1;
            return( p );
        }
        ++p;                        // over final quote
    } else {
        val_start = p;
        while( *p && *p != ' ' && *p != '.' ) {
            ++p;
        }
        val_len = p - val_start;
    }
    if( *p == '.' ) {
        ProcFlags.tag_end_found = true;
    }
    return( p );
}

