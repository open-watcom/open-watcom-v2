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


#include <stddef.h>
#include "types.h"
#include "wresall.h"
#include "bincpy.h"
#include "rcrtns.h"

#define BUFFER_SIZE  1024

static char     Buffer[ BUFFER_SIZE ];

int BinaryCopy( WResFileID inhandle, WResFileID outhandle, unsigned long length )
/*******************************************************************************/
{
    int     nummoved;

    /* read the parts that fill the buffer */
    while (length >= BUFFER_SIZE) {
        nummoved = RCREAD( inhandle, Buffer, BUFFER_SIZE );
        if (nummoved != BUFFER_SIZE) {
            return( TRUE );
        }
        nummoved = RCWRITE( outhandle, Buffer, BUFFER_SIZE );
        if (nummoved != BUFFER_SIZE) {
            return( TRUE );
        }

        length -= BUFFER_SIZE;
    }

    if (length > 0) {
        nummoved = RCREAD( inhandle, Buffer, length );
        if (nummoved != (int)length) {
            return( TRUE );
        }
        nummoved = RCWRITE( outhandle, Buffer, length );
        if (nummoved != (int)length) {
            return( TRUE );
        }
    }

    return( FALSE );
}
