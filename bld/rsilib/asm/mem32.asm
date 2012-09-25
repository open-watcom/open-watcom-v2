;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;                         Open Watcom Project
;
;   Copyright (c) 2002-2010 Open Watcom Contributors. All Rights Reserved.
;   Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;   Copyright (c) 1987-91, 1992 Rational Systems, Incorporated. All Rights Reserved.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   mem32.asm -- support for debugger interface library
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .CODE
        .386

;
; void __cdecl peek32(long off32, int sel, char far *dest, unsigned short count)
;
peek32 proc C public uses es ds esi edi ecx, off32:dword, sel:word, dest:dword, count:word
        mov     esi,off32
        mov     ds,sel
        les     di,dest
        movzx   edi,di
        mov     cx,count
        jcxz nocopy1
        movzx   ecx,cx
        rep     movs byte ptr[edi],[esi]
nocopy1:
        ret
peek32 endp

;
; int __cdecl poke32(long offset32, int sel, char far *dest, unsigned short count)
;
poke32 proc C public uses es ds bx ecx esi edi, off32:dword, sel:word, src:dword, count:word
        mov     bx,sel
        xor     ax,ax
        verw    bx
        jz short writeable
        ;
        ; create code segment alias descriptor (writable)
        ;
        mov     ax,0AH
        int 31H
        jc short finish
        mov     bx,ax
writeable:
        mov     es,bx
        mov     edi,off32
        lds     si,src
        movzx   esi,si
        mov     cx,count
        jcxz nocopy2
        movzx   ecx,cx
        rep     movs byte ptr[edi],[esi]
nocopy2:
        or      ax,ax
        je short finish
        ;
        ; free code segment alias descriptor
        ;
        mov     ax,01H
        int 31H
        xor     ax,ax
finish:
        ret
poke32 endp

        end
