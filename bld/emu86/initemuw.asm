include struct.inc
include mdef.inc

.8087
public  FJSRQQ
FJSRQQ  equ             08000H
public  FISRQQ
FISRQQ  equ             00632H
public  FIERQQ
FIERQQ  equ             01632H
public  FIDRQQ
FIDRQQ  equ             05C32H
public  FIWRQQ
FIWRQQ  equ             0A23DH
public  FJCRQQ
FJCRQQ  equ             0C000H
public  FJARQQ
FJARQQ  equ             04000H
public  FICRQQ
FICRQQ  equ             00E32H
public  FIARQQ
FIARQQ  equ             0FE32H

        extrn   __fpmath        : far
        extrn   GETWINFLAGS     : far
        extrn   __init_8087_emu : near
        extrn   __raise_fpe_    : near

        name    initemuw

;========================================================

include xinit.inc

        xinit   __init_87_emulator,1
        xfini   __fini_87_emulator,1

DGROUP  group   _DATA
        assume  ds:DGROUP
_DATA segment word public 'DATA'
        extrn   __8087 : byte
        extrn   __real87 : byte
_DATA ends

_TEXT segment word public 'CODE'

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void _init_87_emulator( void )
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

assume   cs:_TEXT,ds:DGROUP

public  __init_87_emulator
__init_87_emulator proc near
        push    ds                      ; save ds
        push    dx                      ; save dx
        push    cx                      ; save cx
        push    bx                      ; save bx
if _MODEL and _BIG_DATA                 ; get addressability
        mov     ax,DGROUP               ; ...
        mov     ds,ax
endif                                   ; ...
        sub     bx,bx                   ; initialize Windows FPMATH support
        mov     ax,offset _TEXT:_userfpe        ; ...
        mov     dx,cs                   ; ...
        call    __fpmath                ; ...
        mov     ax,offset _TEXT:_userfpe        ; ...
        mov     dx,cs                   ; ...
        mov     bx,3                    ; ...
        call    __fpmath                ; ...

        call    __init_8087_emu         ; initialize the 80x87
        mov     byte ptr __8087,al      ; at this point we can't tell the real
                                        ; thing from the fake since emulator is
                                        ; hooked in, ask windows if there is
                                        ; a coprocessor
        call    far ptr GETWINFLAGS
        test    ah,04H
        je      no_copro
        mov     al, byte ptr __8087     ; get coprocessor value
        jmp     set_real
no_copro:
        mov     al, 0
set_real:
        mov     byte ptr __real87,al    ; assume we have a real 80x87 present
        pop     bx                      ; restore bx
        pop     cx                      ; restore cx
        pop     dx                      ; restore dx
        pop     ds                      ; restore ds
        ret                             ; return to caller
__init_87_emulator endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void _fini_87_emulator( void )
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

public __fini_87_emulator
__fini_87_emulator proc near
        push    dx                      ; save dx
        push    cx                      ; save cx
        push    bx                      ; save bx
        mov     bx,2                    ; finalize Windows FPMATH support
        call    __fpmath                ; ...
        pop     bx                      ; restore bx
        pop     cx                      ; restore cx
        pop     dx                      ; restore dx
        ret                             ; return
__fini_87_emulator endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void _userfpe(void)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

public _userfpe
_userfpe proc near
        push    bp              ; DS resides at (SP + 4) - Grab a copy
        mov     bp, sp
        mov     ax, [bp + 6]
        pop     bp              ; Clean up BP
        push    ds              ; Save DS
        mov     ds, ax          ; Restore app's DS
        call    __raise_fpe_    ; Call __raise_fpe()
        pop     ds
        retf
_userfpe endp

_TEXT   ends

        end
