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


#include "dbgdefn.h"
#include "dbgdata.h"
#include <malloc.h>

extern void             MemExpand(void);

extern int _d16ReserveExt( int );
#pragma aux _d16ReserveExt =    "mov cx,ax" \
                                "shr eax,16" \
                                "mov bx,ax" \
                                "mov dx,1400H" \
                                "mov ax,0ff00H" \
                                "int 21H" \
                                "ror eax,16" \
                                "mov ax,dx" \
                                "ror eax,16" \
                                parm [ eax ] \
                                value [ eax ] \
                                modify [ ebx ecx edx ]

void SysSetMemLimit()
{
#if !defined(__OSI__)
    _d16ReserveExt( MemSize + 1*1024UL*1024UL );
#endif
    MemExpand();
    if( _IsOff( SW_REMOTE_LINK ) && _IsOff( SW_KEEP_HEAP_ENABLED ) ) {
        _heapenable( 0 );
    }
}
