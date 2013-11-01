;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;                         Open Watcom Project
;
; Copyright (c) 2002-2010 The Open Watcom Contributors. All Rights Reserved.
; Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
; Copyright (c) 1987-1992 Rational Systems, Incorporated. All Rights Reserved.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   fixtrap.asm -- support for debugger interface library
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.386p

DGROUP GROUP _DATA

_DATA segment word public 'DATA' use16
oldint1         DF      ?
trapping        DB      1
_DATA ends

_TEXT segment dword public 'CODE' use16

        ;
        ; This interrupt handler assumes DOS/4G interrupt handling
        ; (since it's installed with INT 21h/2501h, both interrupt
        ; 1 and exception 1 will be directed to this handler; since
        ; the CS that installs the handler is 16-bit, DOS/4G will
        ; arrange for a 16-bit stack frame).
        ;

        ASSUME  ds:DGROUP

traptest:
        push    bp
        mov     bp, sp
        and     BYTE PTR ss:[bp+7], NOT 1h
        push    ds
        mov     bp, DGROUP
        mov     ds, bp
        mov     trapping, 1
        pop     ds
        pop     bp
        iret

;
; int init_fixtrap( void )
;
init_fixtrap proc "C" public
        mov     ax, 1600h
        int     2Fh
        ;
        ; check for Windows 95 (version 4)
        ;
        cmp     al, 4
        mov     ax, 0
        jne short not_windows_95
        push    es
        push    ebx
        mov     ax, 3501h
        int     21h
        mov     DWORD PTR oldint1, ebx
        mov     WORD PTR oldint1+4, es
        ;
        ; return ring bits of INT 1 handler selector
        ;
        mov     ax,es
        and     ax, 3
        pop     ebx
        pop     es
not_windows_95:
        ret
init_fixtrap endp

;
; void fixtrap( void )
;
fixtrap proc "C" public uses AX BX ECX EDX
        pushf
        pop     bx              ; get flags into BX
        sub     ecx, ecx        ; zero loop counter for below
        or      bh, 1           ; OR in trap flag
        mov     trapping, cl
        ;
        ; install INT 1 hook
        ;
        push    ds
        push    cs
        pop     ds

        ASSUME  ds:NOTHING

        sub     edx, edx
        mov     ax, 2501h
        mov     dx, OFFSET traptest
        int     21h
        pop     ds

        ASSUME  ds:DGROUP

        ;
        ; Loop indefinitely until popping the trap flag into the
        ; flags register causes us to see an INT 1 (traptest to get
        ; called).  This code is a little risky because it assumes
        ; that the only reason we might not get INT 1 under Windows
        ; is Windows 95's use of single-stepping to fix programs that
        ; use popf to restore the interrupt flag.  Repeated popf's
        ; will eventually cause Windows 95's 32-bit internal counter
        ; to reach zero and Windows 95 to let the INT 1 through.  
        ; However, if something else is preventing the trap flag 
        ; from causing an INT 1, our counter could wrap all the way
        ; around to zero (a slow process!) and we might still not
        ; fix the trap flag.
        ;

ALIGN 4

pushnpop:
        push    bx
        popf
        dec     ecx
        jz short give_up
        cmp     BYTE PTR trapping, 0
        jz short pushnpop
        ;
        ; restore original INT 1 vector
        ;
        push    ds
        mov     ax, 2501h
        mov     edx, DWORD PTR oldint1
        mov     ds, WORD PTR oldint1+4
        int     21h
        pop     ds
give_up:
        ;
        ; next attempt to pop trap flag should work
        ;
        ret
fixtrap endp

_TEXT ends

        end
