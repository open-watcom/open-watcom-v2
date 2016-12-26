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
#include "wio.h"
#include "watcom.h"
#include "types.h"
#include "wressetr.h"
#include "rcrtns.h"
#include "bincmp.h"


#define BUFFER_SIZE  1024

static char     Buffer1[BUFFER_SIZE];
static char     Buffer2[BUFFER_SIZE];

int BinaryCompare( WResFileID fid1, uint_32 offset1, WResFileID fid2, uint_32 offset2, uint_32 length )
/*****************************************************************************************************/
{
    size_t          numread;
    WResFileOffset  currpos1;
    WResFileOffset  currpos2;
    int             rc;

    /* seek to the start of the places to compare */
    currpos1 = RESSEEK( fid1, offset1, SEEK_SET );
    if( currpos1 == -1 ) {
        return( -1 );
    }
    currpos2 = RESSEEK( fid2, offset2, SEEK_SET );
    if( currpos2 == -1 ) {
        RESSEEK( fid1, currpos1, SEEK_SET );
        return( -1 );
    }

    /* compare the parts that fill the buffer */
    rc = 0;
    for( numread = BUFFER_SIZE; length > 0; length -= numread ) {
        if( numread > length )
            numread = length;
        if( RESREAD( fid1, Buffer1, numread ) != numread ) {
            rc = -1;
            break;
        }
        if( RESREAD( fid2, Buffer2, numread ) != numread ) {
            rc = -1;
            break;
        }
        if( memcmp( Buffer1, Buffer2, numread ) != 0 ) {
            rc = 1;
            break;
        }
    }

    RESSEEK( fid2, currpos2, SEEK_SET );
    RESSEEK( fid1, currpos1, SEEK_SET );
    return( rc );
}
