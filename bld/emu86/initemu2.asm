include struct.inc
include mdef.inc

.8087
public  FJSRQQ
FJSRQQ  equ             0000H
public  FISRQQ
FISRQQ  equ             0000H
public  FIERQQ
FIERQQ  equ             0000H
public  FIDRQQ
FIDRQQ  equ             0000H
public  FIWRQQ
FIWRQQ  equ             0000H
public  FJCRQQ
FJCRQQ  equ             0000H
public  FJARQQ
FJARQQ  equ             0000H
public  FICRQQ
FICRQQ  equ             0000H
public  FIARQQ
FIARQQ  equ             0000H

        extrn   __init_8087_emu : near
        extrn   __hook8087_     : far
        extrn   __unhook8087_   : far
        name    initemu

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
        push    ds                      ; save DS
if _MODEL and _BIG_DATA                 ; get addressability
        mov     ax,DGROUP               ; ...
        mov     ds,ax
endif                                   ; ...
        call    __hook8087_             ; hook into int7 if 80x87 not present
        mov     byte ptr __real87,al    ; set whether real 80x87 present
        call    __init_8087_emu         ; initialize the 80x87
        mov     byte ptr __8087,al      ; at this point we can't tell the real
                                        ; thing from the fake since emulator is
                                        ; hooked in
        pop     ds                      ; restore DS
        ret                             ; return to caller
__init_87_emulator endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void _fini_87_emulator( void )
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

public __fini_87_emulator
__fini_87_emulator proc near
        call    __unhook8087_           ; unhook from int7
        ret
__fini_87_emulator endp


_TEXT   ends

        end
