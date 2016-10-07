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
#include <string.h>
#include "watcom.h"
#include "types.h"
#include "bincmp.h"
#include "rcrtns.h"

#define BUFFER_SIZE  1024

static char     Buffer1[BUFFER_SIZE];
static char     Buffer2[BUFFER_SIZE];

int BinaryCompare( int handle1, uint_32 offset1, int handle2, uint_32 offset2, uint_32 length )
/*********************************************************************************************/
{
    int             numread;
    WResFileOffset  currpos1;
    WResFileOffset  currpos2;
    int             rc;

    /* seek to the start of the places to compare */
    currpos1 = RCSEEK( handle1, offset1, SEEK_SET );
    if( currpos1 == -1 ) {
        return( -1 );
    }
    currpos2 = RCSEEK( handle2, offset2, SEEK_SET );
    if( currpos2 == -1 ) {
        RCSEEK( handle1, currpos1, SEEK_SET );
        return( -1 );
    }

    /* compare the parts that fill the buffer */
    rc = 0;
    while( length >= BUFFER_SIZE ) {
        numread = RCREAD( handle1, Buffer1, BUFFER_SIZE );
        if( numread != BUFFER_SIZE ) {
            rc = -1;
            break;
        }
        numread = RCREAD( handle2, Buffer2, BUFFER_SIZE );
        if( numread != BUFFER_SIZE ) {
            rc = -1;
            break;
        }
        if( memcmp( Buffer1, Buffer2, BUFFER_SIZE ) != 0 ) {
            rc = 1;
            break;
        }
        length -= BUFFER_SIZE;
    }

    if( rc == 0 && length > 0 ) {
        numread = RCREAD( handle1, Buffer1, length );
        if( numread != length ) {
            rc = -1;
        } else {
            numread = RCREAD( handle2, Buffer2, length );
            if( numread != length ) {
                rc = -1;
            } else {
                if( memcmp( Buffer1, Buffer2, length ) != 0 ) {
                    rc = 1;
                }
            }
        }
    }

    RCSEEK( handle2, currpos2, SEEK_SET );
    RCSEEK( handle1, currpos1, SEEK_SET );
    return( rc );
}
