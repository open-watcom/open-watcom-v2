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

make32  macro   hi,lo,elo
        shl     elo,16          ; put lo into hi
        mov     lo,hi           ; get hi
        rol     elo,16          ; swap lo/hi
        endm

DGROUP group _DATA

                assume  cs:_TEXT, ds:DGROUP

extrn _FlatCS: word
extrn _FlatDS: word

_DATA segment word public 'DATA' use16
p_offset        dd      0
p_seg           dw      0
                dw      0
s_offset        dd      0
s_seg           dw      0
                dw      0
_DATA ends

public  DoCall_
public  DoReadWord_
public  DoWriteWord_
public  DoReturn_
public  Return16

_TEXT segment word public 'CODE' use16
;*
;*** DoCall - parms     [dx ax] - debug function (32-bit near address)
;***                    [cx bx] - buffer (32-bit near address)
;***                    [di si] - return addr (32-bit near) - points
;***                              to DoReturn
;*
DoCall_ proc    near
        push    ds
        push    bp
        mov     bp,seg DGROUP
        mov     ds,bp
        mov     bp,sp
        and     sp,0fffch
        push    bp

        push    gs
        push    di
        push    si

        ;*
        ;*** eax - Dos32Debug
        ;*** ebx - buffer parameter
        ;*** esi - return address
        ;*
        make32  dx,ax,eax
        make32  cx,bx,ebx
        make32  di,si,esi
        mov     dx,cs
        movzx   edx,dx
        mov     cx,offset Return16
        movzx   ecx,cx

        mov     ds:p_offset,eax
        mov     ax,ds:_FlatCS
        mov     ds:p_seg,ax
        mov     ax,ds
        mov     gs,ax

        mov     ax,sp                   ; this will
        push    ss                      ;   get us back
        push    ax                      ;     from 32-bit

        movzx   esp,sp                  ; turn ss:sp into a flat esp
        mov     ax,ss
        shr     ax,3
        rol     eax,16
        mov     ax,sp

        mov     ds:s_offset,eax         ; get new ss:esp
        mov     ax,ds:_FlatDS
        mov     ds:s_seg,ax
        lss     esp,fword ptr ds:s_offset

        mov     ds,ax                   ; point to 32-bit data
        mov     es,ax

        push    edx                     ; seg
        push    ecx                     ; off

        push    ebx                     ; 32-bit buffer
        push    esi                     ; 32-bit return address
        assume gs:_data
        jmp     fword ptr gs:p_offset   ; jump to function
DoReturn_:
        db      083h,0c4h,004h          ; add esp,4
        retf
Return16:
        lss     sp,[esp]                ; get old ss:sp

        pop     si
        pop     di
        pop     gs
        mov     bp,seg DGROUP
        mov     ds,bp
        pop     bp
        mov     sp,bp
        pop     bp
        pop     ds

        mov     edx,eax
        ror     edx,16

        ret
DoCall_ endp

DoReadWord_ proc        near
        mov     ax,word ptr gs:[ebx]
        int     3
DoReadWord_ endp

DoWriteWord_ proc       near
        mov     word ptr gs:[ebx],ax
        int     3
DoWriteWord_ endp
_TEXT ends

        end
