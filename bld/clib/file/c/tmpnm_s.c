/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
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
* Description:  Safer C Library tmpnam_s() - safer tmpnam()
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include "saferlib.h"
#include <string.h>
#include <stdio.h>


_WCRTLINK errno_t __F_NAME(tmpnam_s,_wtmpnam_s)( CHAR_TYPE *s, rsize_t maxsize )
/******************************************************************************/
{
    errno_t     rc= -1;
    size_t      len;
    CHAR_TYPE   workname[ L_tmpnam_s ];
    char       *msg = NULL;

    /* runtime-constraints */
    /* s not  NULL */
    /* maxsize <= RSIZE_MAX */
    /* maxsize >  generated tmpname */
    /* maxsize > 0  */
    if( __check_constraint_nullptr_msg( msg, s ) &&
        __check_constraint_zero_msg( msg, maxsize ) &&
        __check_constraint_maxsize_msg( msg, maxsize ) ) {

        __F_NAME(tmpnam,_wtmpnam)( workname );
        len = __F_NAME(strlen,wcslen)( workname );
        if ( __check_constraint_a_gt_b_msg( msg, len, maxsize ) ) {
            __F_NAME(strcpy,wcscpy)(s, workname );
            rc = 0;
        }
    }
    if( msg != NULL ) {
    /* Runtime-constraints violated, set tmpname empty */
        if( (s != NULL) && (maxsize > 0) && __lte_rsizmax( maxsize ) ) {
            s[0] = NULLCHAR;
        }
        /* Now call the handler */
        __rtct_fail( __func__, msg, NULL );
    }
    return( rc );
}

