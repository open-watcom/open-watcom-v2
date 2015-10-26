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


//
// WINAPI :     Windows API functions that could not be defined with pragmas
//

#include "ftnstd.h"

#include <stddef.h>

#define GMEM_MOVEABLE   0x0002
#define LMEM_MOVEABLE   0x0002

#define DWORD           unsigned long
#define WORD            unsigned int

typedef WORD            HANDLE;

extern  HANDLE          __far __pascal GlobalReAlloc(HANDLE,DWORD,WORD);
extern  HANDLE          __far __pascal LocalReAlloc(HANDLE,WORD,WORD);
extern  HANDLE          __far __pascal LockSegment(WORD);
extern  HANDLE          __far __pascal UnLockSegment(WORD);


HANDLE          GlobalDiscard( HANDLE h ) {
//=========================================

    return( GlobalReAlloc( h, 0, GMEM_MOVEABLE ) );
}


HANDLE          LocalDiscard( HANDLE h ) {
//========================================

    return( LocalReAlloc( h, 0, LMEM_MOVEABLE ) );
}


#pragma off (unreferenced);
HANDLE          LockData( HANDLE dummy ) {
#pragma on (unreferenced);
//========================================

    return( LockSegment( 0xffff ) );
}


#pragma off (unreferenced);
HANDLE          UnLockData( HANDLE dummy ) {
#pragma on (unreferenced);
//==========================================

    return( UnLockSegment( 0xffff ) );
}
