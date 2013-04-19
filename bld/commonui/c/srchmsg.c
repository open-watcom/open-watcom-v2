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
* Description:  Search a list of messages for given message.
*
****************************************************************************/


#include "precomp.h"
#include <string.h>
#include "mem.h"
#include "srchmsg.h"
#include "ldstr.h"

/*
 * SrchMsg - searchs tbl for a message corresponding to msgno
 *         - if one exists a pointer to it is returned
 *           otherwise a pointer to dflt is returned
 */
char *SrchMsg( unsigned msgno, msglist *tbl, char *dflt )
{
    msglist             *curmsg;

    curmsg = tbl;
    for( ;; ) {
        if( curmsg->msg == NULL ) {
            return( dflt );
        }
        if( curmsg->msgno == msgno ) {
            return( curmsg->msg );
        }
        curmsg++;
    }

} /* SrchMsg */

/*
 * InitSrchTable - load the strings for a search table from the resource
 *                 file
 *               - WARNING: this function must not be called more
 *                 than once for each table
 *               - buf must be large enough to hold any message in the table
 */
BOOL InitSrchTable( HANDLE inst, msglist *tbl )
{
    msglist             *curmsg;

    curmsg = tbl;
    inst = inst;
    for( ;; ) {
        if( curmsg->msg == (char *)-1L ) {
            break;
        }
        curmsg->msg = AllocRCString( (unsigned)curmsg->msg );
        if( curmsg->msg == NULL ) {
            return( FALSE );
        }
        curmsg++;
    }
    curmsg->msg = NULL;
    return( TRUE );

} /* InitSrchTable */
