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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <limits.h>
#include "layer0.h"
#include "util.h"
#include "reserr.h"
#include "wresrtns.h"


WResHelpID *WResHelpIDFromNum( long num )
/****************************************
 * allocate an ID and fill it in
 */
{
    WResHelpID      *help_id;

    if( num < 0 || num > SHRT_MAX  ) {
        help_id = NULL;
        WRES_ERROR( WRS_BAD_PARAMETER );
    } else {
        help_id = AllocWResHelpIDNum( offsetof( WResHelpID, ID.Num ) );
        if( help_id == NULL ) {
            WRES_ERROR( WRS_MALLOC_FAILED );
        } else {
            WResInitHelpIDFromNum( num, help_id );
        }
    }
    return( help_id );
} /* WResHelpIDFromNum */
