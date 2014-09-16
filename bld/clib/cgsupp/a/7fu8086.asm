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


include mdef.inc
include struct.inc

.8087
        modstart        _7fu8086

        xdefp   __FSU87
        xdefp   __FDU87

defpe __FSU87
        push    bp
        mov     bp,sp
        sub     sp,8
        push    dx
        push    ax
        fld     dword ptr -12[bp]
        jmp     short convert

defpe __FDU87
        push    bp
        mov     bp,sp
        sub     sp,4
        push    ax
        push    bx
        push    cx
        push    dx
        fld     qword ptr -12[bp]
        ; fall through

convert:
        push    si
        fstp    tbyte ptr -12[bp]       ; get number out in memory
        mov     ax,-4[bp]               ; pick up sign/exponent
        and     ax,7fffh                ; isolate exponent
        sub     ax,16383                ; remove bias
        jl      ret_zero                ; if less than .5, return zero
        cmp     ax,64                   ; are we too big?
        jae     ret_inf                 ; if so, return infinity
        mov     si,63                   ; calculate shift count
        sub     si,ax                   ; ...
        mov     ax,-6[bp]               ; pick up mantissa
        mov     bx,-8[bp]               ; ...
        mov     cx,-10[bp]              ; ...
        mov     dx,-12[bp]              ; ...
        je      negate                  ; skip out if no shifting
L2:
        shr     ax,1                    ; shift down one bit
        rcr     bx,1                    ; ...
        rcr     cx,1                    ; ...
        rcr     dx,1                    ; ...
        dec     si                      ; are we done?
        jne     L2                      ; do it again if not
negate:
        test    byte ptr -3[bp],80h     ; is number negative?
        jns     done                    ; done if not
        mov     si,dx                   ; negate number
        xor     dx,dx
        sub     dx,si
        mov     si,cx
        mov     cx,0
        sbb     cx,si
        mov     si,bx
        mov     bx,0
        sbb     bx,si
        mov     si,ax
        xor     ax,ax
done:
        pop     si                      ; outta here
        mov     sp,bp                   ; ...
        pop     bp                      ; ...
        ret                             ; ...

ret_zero:
        xor     ax,ax
        xor     bx,bx
        xor     cx,cx
        xor     dx,dx
        jmp     done

ret_inf:
        mov     ax,0ffffh
        mov     bx,ax
        mov     cx,ax
        mov     dx,ax
        jmp     done
endproc __FDU87
endproc __FSU87

        endmod
        end
