include mdef.inc
include struct.inc
include xinit.inc

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


        extrn   __init_8087_emu : near
        name    initemu

extrn   __int34         : near
extrn   __int3c         : near
;========================================================

        xinit   __init_87_emulator,1
        xfini   __fini_87_emulator,1

DGROUP  group   _DATA
        assume  ds:DGROUP
_DATA   segment word public 'DATA'
        extrn   __8087 : byte
        extrn   __real87 : byte
        extrn   __no87 : word

i34off  dw      0
i34seg  dw      0
i35off  dw      0
i35seg  dw      0
i36off  dw      0
i36seg  dw      0
i37off  dw      0
i37seg  dw      0
i38off  dw      0
i38seg  dw      0
i39off  dw      0
i39seg  dw      0
i3aoff  dw      0
i3aseg  dw      0
i3boff  dw      0
i3bseg  dw      0
i3coff  dw      0
i3cseg  dw      0
i3doff  dw      0
i3dseg  dw      0
_DATA   ends

_TEXT segment word public 'CODE'

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void _init_87_emulator( void )
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

assume   cs:_TEXT,ds:DGROUP
xchg_vects      proc near
        push    bx                      ; save regs
        push    cx                      ; ...
        push    dx                      ; ...
        push    si                      ; ...
        push    di                      ; ...
        push    ds                      ; ...
        push    es                      ; ...
if _MODEL and _BIG_DATA                 ; get addressability
        mov     di,DGROUP               ; ...
else                                    ; ...
        mov     di,ds                   ; ...
endif                                   ; ...
        lea     si,i34off               ; point to vector table
        mov     al,34H                  ; for( v = 34H; v <= 3dH; ) {
        mov     cx,10                   ; - ...
grab:   mov     ah,35H                  ; - get old interrupt
        int     21H                     ; - ... in es:bx
        mov     ds,di                   ; - point ds:di at interrupt table
        mov     dx,[si]                 ; - pick up new interrupt vector offset
        mov     [si],bx                 ; - stash old interrupt vector offset
        add     si,2                    ; - point to segment
        mov     bx,[si]                 ; - pick up new segment
        mov     [si],es                 ; - stash old segment
        add     si,2                    ; - point to segment
        mov     ds,bx                   ; - new vector in ds:dx
        mov     ah,25H                  ; - set new interrupt
        int     21H                     ; - ...
        inc     al                      ; - ++v
        loop    grab                    ; endfor
        pop     es                      ; ...
        pop     ds                      ; ...
        pop     di                      ; ...
        pop     si                      ; ...
        pop     dx                      ; ...
        pop     cx                      ; ...
        pop     bx                      ; save regs
        ret                             ; return to caller
xchg_vects      endp


;       __no87 is not 0 if NO87 environment variable is present

public  __init_87_emulator
__init_87_emulator proc near
        push    ds                      ; save ds
        push    bx                      ; save bx
if _MODEL and _BIG_DATA                 ; get addressability
        mov     ax,DGROUP               ; ...
        mov     ds,ax
endif                                   ; ...
        fninit                          ; initialize math coprocessor
        push    ax                      ; ...
        mov     bx,sp                   ; ...
        fnstcw  ss:[bx]                 ; store control word in memory
        pop     ax                      ; get CW into ax
        mov     bx,__no87               ; get state of NO87 environment var
        cmp     ah,03h                  ; coprocessor is present
        _if     ne                      ; if no coprocessor
          inc   bx                      ; - pretend NO87 was set
        _endif                          ; endif
        test    bx,bx                   ; if no 80x87 or no87 set
        _if     ne                      ; then
          mov   ax,offset __int34       ; - emulate instructions
          mov   i3coff,offset __int3c   ; - ...
          mov   i3doff,offset __int3d   ; - ...
        _else                           ; else
          mov   ax,offset __patch34     ; - patch instructions
          mov   i3coff,offset __patch3c ; - ...
          mov   i3doff,offset __patch3d ; - ...
        _endif                          ; endif
        mov     i34seg,cs               ; set up rest of table
        mov     i34off,ax               ; ...
        mov     i35seg,cs               ; ...
        mov     i35off,ax               ; ...
        mov     i36seg,cs               ; ...
        mov     i36off,ax               ; ...
        mov     i37seg,cs               ; ...
        mov     i37off,ax               ; ...
        mov     i38seg,cs               ; ...
        mov     i38off,ax               ; ...
        mov     i39seg,cs               ; ...
        mov     i39off,ax               ; ...
        mov     i3aseg,cs               ; ...
        mov     i3aoff,ax               ; ...
        mov     i3bseg,cs               ; ...
        mov     i3boff,ax               ; ...
        mov     i3cseg,cs               ; ...
        mov     i3dseg,cs               ; ...
        call    xchg_vects              ; set up vectors
        call    __init_8087_emu         ; initialize the 8087
        mov     byte ptr __8087,al      ; pretend we have a 387 if emulating
        test    bx,bx                   ; if no 80x87 or no87 set
        _if     ne                      ; then
          mov   byte ptr __real87,0     ; - say we do not have a real 80x87
        _else                           ; else
          mov   byte ptr __real87,al    ; - we have a real 80x87 of type AL
        _endif                          ; endif
        pop     bx                      ; restore bx
        pop     ds                      ; restore ds
        ret                             ; return to caller
__init_87_emulator endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void _fini_87_emulator( void )
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

public __fini_87_emulator
__fini_87_emulator proc near
        call    xchg_vects
        ret
__fini_87_emulator endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       interrupt int3d()               FWAIT
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

__int3d proc    near
        sti                             ; interrupts back on
        push    si                      ; save some registers
        push    ds                      ; ...
        push    bp                      ; ...
        mov     bp,sp                   ; set up stack frame
        lds     si,6[bp]                ; point es:si at return address
        dec     si                      ; point at the 3D
        dec     si                      ; point to the INT instruction
        mov     word ptr [si],09090H    ; zap it with NOPs
        pop     bp                      ; restore some registers
        pop     ds                      ; ...
        pop     si                      ; ...
        iret                            ; return from interrupt
__int3d endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void patch34()
;;      - turn an int 34H instruction back into a real 8087 instruction
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

__patch34 proc  near
        sti                             ; interrupts back on
        push    si                      ; save some registers
        push    ds                      ; ...
        push    bp                      ; ...
        mov     bp,sp                   ; set up stack frame
        lds     si,6[bp]                ; point ds:si at return address
        dec     si                      ; back up one byte
        add     byte ptr [si],0A4H      ; turn int number into opcode
        dec     si                      ; point to int instruction
        mov     byte ptr [si],09BH      ; insert FWAIT instruction
        mov     6[bp],si                ; zap our return address
        pop     bp                      ; restore some registers
        pop     ds                      ; ...
        pop     si                      ; ...
        iret                            ; return from interrupt
__patch34 endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void patch3c()
;;      - Turn an int 3CH instruction into an 8087 instruction with an
;;        appropriate segment override
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
__patch3c proc near
        sti                             ; interrupts back on
        push    si                      ; save some registers
        push    bx                      ; ...
        push    ds                      ; ...
        push    bp                      ; ...
        mov     bp,sp                   ; set up stack frame
        lds     si,8[bp]                ; point ds:si at return address
        mov     bl,[si]                 ; get opcode into bl
        or      byte ptr [si],0C0H      ; fix high two bits of opcode
        not     bl                      ; not the opcode
        and     bl,0C0H                 ; keep original top two bits
        shr     bl,1                    ; move into seg override posn
        shr     bl,1                    ; ...
        shr     bl,1                    ; ...
        or      bl,026H                 ; turn into a segment override
        dec     si                      ; point to the 3C
        mov     [si],bl                 ; zap it with segment override
        dec     si                      ; point to the INT instruction
        mov     byte ptr [si],09BH      ; zap it with an FWAIT
        mov     8[bp],si                ; zap our return address
        pop     bp                      ; restore some registers
        pop     ds                      ; ...
        pop     bx                      ; ...
        pop     si                      ; ...
        iret                            ; return from interrupt
__patch3c endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void patch3d()
;;      - Turn an int 3DH instruction into FWAIT // NOP
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
__patch3d proc near
        sti                             ; interrupts back on
        push    si                      ; save some registers
        push    ds                      ; ...
        push    bp                      ; ...
        mov     bp,sp                   ; set up stack frame
        lds     si,6[bp]                ; point ds:si at return address
        dec     si                      ; point at the 3D
        dec     si                      ; point to the INT instruction
        mov     word ptr [si],0909BH    ; zap it with an FWAIT // NOP
        mov     6[bp],si                ; zap our return address
        pop     bp                      ; restore some registers
        pop     ds                      ; ...
        pop     si                      ; ...
        iret                            ; return from interrupt
__patch3d endp


_TEXT   ends

        end
