;*****************************************************************************
;*
;*                         Open Watcom Project
;*
;* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;* Copyright (c) 1987-1992 Rational Systems, Incorporated. All Rights Reserved.
;*
;*****************************************************************************
;*
;*  mem32.asm -- support for debugger interface library
;*
;*****************************************************************************

        .CODE
        .386

;
; void peek32(unsigned long off32, unsigned sel, size_t count, void FarPtr dest)
;
; input:
;   offset32  dx:ax
;   sel       bx
;   count     cx
;   dest      on stack
;
; output:
;
peek32 proc WATCOM_C public uses es ds esi edi, offs32_lo:word, offs32_hi:word, sel:word, count:word, dest:dword
        movzx   ecx,count
        jcxz short nodata1
        mov     si,offs32_hi
        shl     esi,16
        mov     si,offs32_lo
        les     di,dest
        movzx   edi,di
        mov     ds,sel
        rep     movs byte ptr[edi],[esi]
nodata1:
        ret
peek32 endp

;
; bool poke32(unsigned long offset32, unsigned sel, size_t count, const void FarPtr src)
;
; input:
;   offset32  dx:ax
;   sel       bx
;   count     cx
;   src       on stack
;
; output:
;   bool      ax
;
poke32 proc WATCOM_C public uses es ds esi edi, offs32_lo:word, offs32_hi:word, sel:word, count:word, src:dword
        movzx   ecx,count
        jcxz short nodata2
        mov     di,offs32_hi
        shl     edi,16
        mov     di,offs32_lo
        xor     ax,ax
        verw    sel
        jz short writeable
        ;
        ; create segment alias descriptor (writable)
        ;
        mov     ax,0AH
        int 31H
        jc short finish
        mov     sel,ax
writeable:
        lds     si,src
        movzx   esi,si
        mov     es,sel
        rep     movs byte ptr[edi],[esi]
        or      ax,ax
        je short finish
        ;
        ; free code segment alias descriptor
        ;
        mov     ax,01H
        int 31H
        clc
finish:
nodata2:
        sbb     ax,ax
        and     ax,1
        ret
poke32 endp

        end
