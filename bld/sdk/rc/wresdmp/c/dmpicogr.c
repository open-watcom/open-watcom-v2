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
#include <io.h>
#include "wresall.h"
#include "dmpicogr.h"

/* from dmpcurgr.c */
extern void PrintIconCurDirHeader( IconCurDirHeader * head );

static void PrintIconDirEntry( IconDirEntry * entry, int entrynum )
/*****************************************************************/
{
    printf( "    %2d.  Width: %d   Height: %d   Colour count: %d\n", entrynum,
            entry->Info.Width, entry->Info.Height, entry->Info.ColourCount );
    printf( "\tReserved: %d   Planes: %d   Bit count: %d\n",
            entry->Info.Reserved, entry->Info.Planes, entry->Info.BitCount );
    printf( "\tLength: %ld   Resource name: %d\n", entry->Info.Length,
            entry->IconID );
}

#pragma off (unreferenced)
extern int DumpIconGroup( uint_32 offset, uint_32 length, WResFileID handle )
#pragma on (unreferenced)
/***************************************************************************/
{
    IconCurDirHeader    head;
    IconDirEntry        entry;
    long                prevpos;
    int                 error;
    int                 currentry;

    prevpos = lseek( handle, offset, SEEK_SET );

    error = ResReadIconCurDirHeader( &(head), handle );
    PrintIconCurDirHeader( &head );

    for (currentry = 0; !error && currentry < head.ResCount; currentry++ ) {
        error = ResReadIconDirEntry( &entry, handle );
        if (!error) {
            PrintIconDirEntry( &entry, currentry + 1 );
        }
    }

    lseek( handle, prevpos, SEEK_SET );

    return( error );
}
