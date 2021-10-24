/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include <dos.h>
#include <malloc.h>
#include <stdarg.h>
#include <windows.h>

typedef struct libent {
    void        *proc;
    short       num;
    short       bytes;
    short       lens[1];
} libent;

#define MAX_DLL_ENTRIES 512

extern  DWORD __far *_DLLEntryAddr;
libent  *__DLLEntries[MAX_DLL_ENTRIES]; // DLL entry points

/*
 * DefineDLLEntry - define an entry point to a 32-bit dll
 *                  variable parms are either DLL_CHAR, DLL_WORD, DLL_DWORD,
 *                  or DLL_ENDLIST
 */
int DefineDLLEntry( int entnum, void *routine, ... )
{
    va_list     args;
    libent      *curr;
    int         bytecount = 0;
    int         num = 0;
    int         i = 0;
    int         len;
    int         ret;

    if( entnum >= MAX_DLL_ENTRIES ) {
        return( -1 );
    }
    ret = -1;
    va_start( args, routine );
    for( ;; ) {
        len = va_arg( args, int );
        if( len == DLL_ENDLIST )
            break;
        if( len == DLL_WORD || len == DLL_DWORD ) {
            if( len == DLL_DWORD ) {
                bytecount += 4;
            } else {
                bytecount += 2;
            }
            num++;
        } else {
            break;
        }
        if( num > 512 ) {
            ret = 0;
            break; /* ya, so its arbitrary */
        }

    }
    va_end( args );
    if( len != DLL_ENDLIST ) {
        return( ret );
    }

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
    va_start( args, routine );
    while( num > 0 ) {
        len = va_arg( args, short );
        curr->lens[i++] = len;
        num--;
    }
    va_end( args );
    free( __DLLEntries[entnum] );       // free old entry
    __DLLEntries[entnum] = curr;        // set new entry
    *_DLLEntryAddr = (DWORD)&__DLLEntries[0];

    return( 0 );

} /* DefineDLLEntry */
