;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
;*               DESCRIBE IT HERE!
;*
;*****************************************************************************


                NAME    SETJMP
_TEXT           SEGMENT PUBLIC WORD 'CODE'
                ASSUME  CS:_TEXT
                PUBLIC  _setjmp_
                PUBLIC  longjmp_
_setjmp_:       xchg    ax,si
                push    ds
                mov     ds,dx
                pop     dx
                mov     word ptr ds:[si],bx
                mov     word ptr ds:+2H[si],cx
                mov     word ptr ds:+6H[si],ax  ; save SI
                mov     word ptr ds:+8H[si],di
                mov     word ptr ds:+0aH[si],bp
                mov     word ptr ds:+0eH[si],es
                mov     word ptr ds:+10H[si],dx ; save DS
                pop     word ptr ds:+14H[si]
                mov     word ptr ds:+16H[si],ss
                mov     word ptr ds:+0cH[si],sp
                mov     word ptr ds:+12H[si],cs
                push    word ptr ds:+14H[si]
                mov     ds,dx                   ; restore DS
                mov     si,ax                   ; restore SI
                sub     ax,ax
                ret
longjmp_:       mov     si,ax
                mov     ds,dx
                mov     ss,word ptr ds:+16H[si]
                mov     sp,word ptr ds:+0cH[si]
                push    word ptr ds:+14H[si]
                or      bx,bx
                jne     L1
                inc     bx
L1:             push    bx
                mov     bx,word ptr ds:[si]
                mov     cx,word ptr ds:+2H[si]
                mov     dx,word ptr ds:+4H[si]
                mov     di,word ptr ds:+8H[si]
                mov     bp,word ptr ds:+0aH[si]
                mov     es,word ptr ds:+0eH[si]
                mov     ax,word ptr ds:+10H[si]
                mov     si,word ptr ds:+6H[si]
                mov     ds,ax
                pop     ax
                ret
_TEXT           ENDS

                END
