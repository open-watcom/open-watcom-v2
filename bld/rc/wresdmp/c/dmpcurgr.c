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


#include <stdio.h>
#include "wresall.h"
#include "dmpcurgr.h"
#include "rcrtns.h"

extern void PrintIconCurDirHeader( IconCurDirHeader * head )
/**********************************************************/
{
    printf( "\tReserved: %d   Type: %d   Count: %d\n", head->Reserved,
                    head->Type, head->ResCount );
}

static void PrintCurDirEntry( CurDirEntry * entry )
/*************************************************/
{
    printf( "Width: %d   Height: %d   Planes: %d   BitCount: %d\n",
            entry->Width, entry->Height, entry->Planes, entry->BitCount );
    printf( "\tLength: %ld   Resource Name: %d\n", entry->Length, entry->CurID );
}

extern int DumpCursorGroup( uint_32 offset, uint_32 length, WResFileID handle )
/*****************************************************************************/
{
    IconCurDirHeader    head;
    CurDirEntry         entry;
    long                prevpos;
    int                 error;
    int                 currentry;

    length = length;
    prevpos = RCSEEK( handle, offset, SEEK_SET );

    error = ResReadIconCurDirHeader( &(head), handle );
    PrintIconCurDirHeader( &head );

    for (currentry = 0; !error && currentry < head.ResCount; currentry++ ) {
        error = ResReadCurDirEntry( &entry, handle );
        if (!error) {
            printf( "    %2d. ", currentry + 1 );
            PrintCurDirEntry( &entry );
        }
    }

    RCSEEK( handle, prevpos, SEEK_SET );

    return( error );
}
