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


        name    emustub

_TEXT segment word public 'CODE'

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void __init_87_emulator(void)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

assume   cs:_TEXT

;========================================================

public  __init_87_emulator
__init_87_emulator proc near
__init_87_emulator endp

_TEXT   ends

        end
