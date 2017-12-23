.386p
.387

include langenv.inc
include mdef.inc

        modstart    386inite

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void _init_387_emulator( int )
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        public  __init_387_emulator
__init_387_emulator proc near
        ret
__init_387_emulator endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void _fini_387_emulator( void )
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        public  __fini_387_emulator
__fini_387_emulator proc near
        ret
__fini_387_emulator endp

        endmod

include xinit.inc

        xinit   __init_387_emulator,INIT_PRIORITY_FPU
        xfini   __fini_387_emulator,INIT_PRIORITY_FPU

        end
