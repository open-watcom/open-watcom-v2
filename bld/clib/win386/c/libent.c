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


#include <windows.h>
#include <dos.h>
#include <malloc.h>
#include <stdarg.h>

typedef struct libent {
    void        *proc;
    short       num;
    short       bytes;
    short       lens[1];
} libent;

#define MAX_DLL_ENTRIES 512

extern  DWORD far *_DLLEntryAddr;
libent  *__DLLEntries[MAX_DLL_ENTRIES]; // DLL entry points

/*
 * DefineDLLEntry - define an entry point to a 32-bit dll
 *                  variable parms are either DLL_CHAR, DLL_WORD, DLL_DWORD,
 *                  or DLL_ENDLIST
 */
int DefineDLLEntry( int entnum, void *routine, ... )
{
    va_list     al;
    libent      *curr;
    int         bytecount=0,num=0,i=0,len;

    if( entnum >= MAX_DLL_ENTRIES ) {
        return( -1 );
    }
    va_start( al, routine );
    for(;;) {
        len = va_arg( al, int );
        if( len == DLL_ENDLIST ) break;
        if( len == DLL_WORD || len == DLL_DWORD ) {
            if( len == DLL_DWORD ) bytecount += 4;
            else bytecount += 2;
            num++;
        } else {
            return( -1 );
        }
        if( num > 512 ) {
            return( NULL ); /* ya, so its arbitrary */
        }

    }
    va_end( al );

    curr = malloc( sizeof( libent ) + num * sizeof( short ) );
    if( curr == NULL ) {
        return( -1 );
    }
    curr->num = num;
    curr->bytes = bytecount;
    curr->proc = routine;

    /*
     * add all lengths
     */
    va_start( al, routine );
    while( num > 0 ) {
        len = va_arg( al, short );
        curr->lens[i++] = len;
        num--;
    }
    free( __DLLEntries[entnum] );       // free old entry
    __DLLEntries[entnum] = curr;        // set new entry
    *_DLLEntryAddr = (DWORD) &__DLLEntries[0];

    va_end( al );
    return( 0 );

} /* DefineDLLEntry */
