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


.386p


                extrn   __nullarea:WORD
                extrn   "C", __saved_CS:WORD
                extrn   DosSelToFlat:NEAR

_text           segment word use32 'code'


__do_thunk:

                mov     edi,eax                 ; offset to function to call
                mov     ax,seg __saved_CS
                mov     ds,ax
                mov     es,ax
                mov     ax,ss
                shl     eax,010h
                mov     ax,sp
                and     esp,0ffffh
                call    DOSSELTOFLAT
                push    ds
                push    eax
                lss     esp,pword ptr [esp]

                mov     ax,word ptr __saved_CS
                push    eax
                push    edi

                retf

_text           ends


_text16         segment word public use16 'code'


SAVED_JUNK      equ     10

                public  __Far32Func
__Far32Func     proc    far export
                push    bp
                push    ds
                push    es
                push    si
                push    di
                mov     bp,sp

                mov     ax,sp
                add     ax,cx
                test    al,02h
                jz      no_adjust
                sub     sp,2
no_adjust:
                test    cx,cx
                je      no_parms
                mov     si,cx
next_parm:
                mov     ax,SAVED_JUNK+6[bp+si]
                push    ax
                sub     si,02
                jnz     next_parm
no_parms:

                mov     si,ss

                mov     ax,SAVED_JUNK+2[bp]
                mov     es,ax
                mov     bx,SAVED_JUNK[bp]
                add     bx,3
                mov     eax,dword ptr es:[bx]

                call    far ptr __do_thunk

                push    si
                mov     edx,eax         ; return value is in eax - we want it in dx:ax
                push    bp
                shr     edx,010H        ; if it is long - otherwise we stomp dx - BFD
                lss     sp,[esp]
                pop     di
                pop     si
                pop     es
                pop     ds
                pop     bp
                retf
__Far32Func     endp
_text16         ends

end
