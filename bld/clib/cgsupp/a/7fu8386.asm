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
        modstart        _7fu8386

        xdefp   __FSU87
        xdefp   __FDU87

defpe __FSU87
        sub     esp,12
        mov     [esp],eax
        fld     dword ptr [esp]
        jmp     short convert

defpe __FDU87
        sub     esp,12
        mov     [esp],eax
        mov     4[esp],edx
        fld     qword ptr [esp]

        ; fall through

convert:
        push    ecx
        fstp    tbyte ptr 4[esp]        ; get number out in memory
        mov     ax,12[esp]              ; pick up sign/exponent
        and     ax,7fffh                ; isolate exponent
        sub     ax,16383                ; remove bias
        jl      ret_zero                ; if less than .5, return zero
        cmp     ax,64                   ; are we too big?
        jae     ret_inf                 ; if so, return infinity
        mov     cl,63                   ; calculate shift count
        sub     cl,al                   ; ...
        mov     eax,4[esp]              ; pick up mantissa
        mov     edx,8[esp]              ; ...
        je      negate                  ; skip out if no shifting
L2:
        shr     edx,1                   ; shift down one bit
        rcr     eax,1                   ; ...
        dec     cl                      ; are we done?
        jne     L2                      ; do it again if not
negate:
        test    byte ptr 13[esp],80h    ; is number negative?
        jns     done                    ; if not, we're done
        neg     edx                     ; negate number
        neg     eax                     ; ...
        sbb     edx,0                   ; ...
done:
        pop     ecx                     ; ...
        add     esp,12                  ; outta here
        ret                             ; ...

ret_zero:
        xor     edx,edx
        xor     eax,eax
        jmp     done

ret_inf:
        mov     edx,0ffffffffh
        mov     eax,edx
        jmp     done
endproc __FDU87
endproc __FSU87

        endmod
        end
