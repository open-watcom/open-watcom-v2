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
        name    initemuq

;========================================================

include xinit.inc

        xinit   __init_87_emulator,1
        xfini   __fini_87_emulator,1

_DATA segment word public 'DATA'
        extrn   __8087 : byte
_DATA ends

_TEXT segment word public 'CODE'

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void _init_87_emulator( void )
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

assume   cs:_TEXT,ds:DGROUP

public  __init_87_emulator
__init_87_emulator proc near
        push    ds
if _MODEL and _BIG_DATA                 ; get addressability
        mov     ax,DGROUP               ; ...
        mov     ds,ax
endif                                   ; ...
        call    __init_8087_emu         ; initialize the 80x87
        mov     byte ptr __8087,al      ; pretend we have a 387 if emulating
        pop     ds                      ; ...
        ret                             ; return to caller
__init_87_emulator endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void _fini_87_emulator( void )
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

public __fini_87_emulator
__fini_87_emulator proc near
        ret
__fini_87_emulator endp


_TEXT   ends

        end
