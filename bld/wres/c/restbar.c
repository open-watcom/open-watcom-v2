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


#include "write.h"
#include "restbar.h"
#include "reserr.h"
#include "wresrtns.h"

int ResWriteToolBarHeader( WResFileID handle, uint_16 item1, uint_16 item2,
                           uint_16 cnt )
/************************************************************************/
{
    uint_16     tmp;
    int         ret;

    // don't know why but MS RC puts out a 1 at the start of the resourece
    // perhaps this is a version ????
    tmp = 1;
    ret =  ResWriteUint16( &tmp, handle );
    if( !ret ) {
        ret =  ResWriteUint16( &item1, handle );
    }
    if( !ret ) {
        ret =  ResWriteUint16( &item2, handle );
    }
    if( !ret ) {
        ret =  ResWriteUint16( &cnt, handle );
    }
    return( ret );
}

int ResWriteToolBarItems( WResFileID handle, uint_16 *items, unsigned cnt ) {
/****************************************************************************/

    int         numwrite;
    unsigned    size;

    size = cnt * sizeof( uint_16 );
    numwrite = (* WRESWRITE) ( handle, items, size );
    if( numwrite != size ) {
        WRES_ERROR( WRS_WRITE_FAILED );
        return( TRUE );
    } else {
        return( FALSE );
    }
}
