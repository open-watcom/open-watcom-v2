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


#include "layer0.h"
#include "resutil.h"
#include "read.h"
#include "reserr.h"
#include "wresrtns.h"

WResSeekReturn ResSeek( WResFileID handle, long offset, int origin )
/******************************************************************/
/* cover function for seek */
{
    WResSeekReturn  posn;

    posn = WRESSEEK( handle, offset, origin );
    if( posn == -1 ) {
        WRES_ERROR( WRS_SEEK_FAILED );
    }
    return( posn );
}

bool ResPadDWord( WResFileID handle )
/***********************************/
/* advances in the file to the next DWORD boundry */
{
    long        curr_pos;
    long        padding;
    bool        error;

    curr_pos = WRESTELL( handle );
    if( curr_pos == -1 ) {
        WRES_ERROR( WRS_TELL_FAILED );
        error = true;
    } else {
        padding = RES_PADDING( curr_pos, sizeof(uint_32) );
        error = ( WRESSEEK( handle, padding, SEEK_CUR ) == -1 );
        if( error ) {
            WRES_ERROR( WRS_SEEK_FAILED );
        }
    }
    return( error );
}
