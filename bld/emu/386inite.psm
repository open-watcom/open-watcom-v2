.386p
.387
include mdef.inc
include xinit.inc

        name    386inite
    
extrn   __sys_init_387_emulator : near
extrn   __sys_fini_387_emulator : near

    xinit   __init_387_emulator,1
    xfini   __fini_387_emulator,1

_TEXT segment word public 'CODE'

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void _init_387_emulator( int )
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

assume   cs:_TEXT,ds:DGROUP

public          __init_387_emulator
__init_387_emulator proc near
            jmp __sys_init_387_emulator
__init_387_emulator endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void _fini_387_emulator( void )
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

public          __fini_387_emulator
__fini_387_emulator proc near
            jmp __sys_fini_387_emulator
__fini_387_emulator endp

_TEXT   ends

        end
