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


#if _HOST == 8086
/* unsigned DS_Reg() */
#pragma aux     DS_Reg = \
        0x8C 0xD8       /* mov ax,ds */         \
        value [ax];

/* movedata( fromseg, fromoff, toseg, tooff, len ) */
#pragma aux     movedata = \
        0x1E            /* push ds */           \
        0x06            /* push es */           \
        0x8E 0xD8       /* mov  ds,ax */        \
        0x8E 0xC2       /* mov  es,dx */        \
        0xF3 0xA4       /* rep  movsb */        \
        0x07            /* pop  es    */        \
        0x1F            /* pop  ds    */        \
        parm    caller  [ax] [si] [dx] [di] [cx]\
        modify  [si di];

#if _OS == _DOS
/* SegAlloc( size ) */
#pragma aux     SegAlloc = \
        0xB4 0x48       /* mov ah,48h  */       \
        0xCD 0x21       /* int 21h     */       \
        0x73 0x02       /* if  c       */       \
        0x29 0xC0       /*  sub ax,ax  */       \
                        /* endif       */       \
        parm    caller  [bx]                    \
        value   [ax];
extern unsigned SegAlloc( unsigned );

/* SegFree( segment ) */
#pragma aux     SegFree = \
        0x06            /* push es      */      \
        0x8E 0xC0       /* mov es,ax    */      \
        0xB4 0x49       /* mov ah,49h   */      \
        0xCD 0x21       /* int 21h      */      \
        0x07            /* pop es       */      \
        parm    caller  [ax];
extern void SegFree( unsigned );
#endif
#endif
