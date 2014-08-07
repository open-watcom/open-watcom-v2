;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  Init 16-bit DOS FPU emulation
;*
;*****************************************************************************

; !!!!! must be compiled with -fpi87 option !!!!!

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


        xinit   __init_87_emulator,INIT_PRIORITY_FPU
        xfini   __fini_87_emulator,INIT_PRIORITY_FPU

DGROUP  group   _DATA
        assume  ds:DGROUP

_DATA   segment word public 'DATA'
        extrn   __8087          : byte
        extrn   __real87        : byte
        extrn   __no87          : byte
        extrn   __dos87emucall  : word
        extrn   __dos87real     : byte

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

        extrn   __init_8087_emu : near

        extrn   __int34         : near
        extrn   __int3c         : near
        extrn   __x87id         : near
        extrn   ___dos87emucall : near

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void _init_87_emulator( void )
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xchg_vects      proc near
        push    bx                      ; save regs
        push    cx                      ; ...
        push    dx                      ; ...
        push    si                      ; ...
        push    di                      ; ...
        push    ds                      ; ...
        push    es                      ; ...
        mov     di,ds                   ; ...
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
__init_87_emulator proc
        push    bx                      ; save bx
        call    __x87id
        mov     __dos87real,al          ; set installed 80x87
        mov     __real87,al             ; set real 80x87 used
        mov     __8087,al               ; set 80x87
        mov     bl,__no87               ; get state of NO87 environment var
        test    al,al                   ; coprocessor is present
        _if     e                       ; if no coprocessor
          inc   bl                      ; - pretend NO87 was set
        _endif                          ; endif
        test    bl,bl                   ; if no 80x87 or no87 set
        _if     ne                      ; then
          mov   __dos87emucall, ___dos87emucall ; set pointer for DOS EMU control
          mov   __real87,0              ; - no real 80x87
          mov   __8087,3                ; - set 80387
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
        call    __init_8087_emu         ; initialize real 80x87 and 80x87 EMU
        pop     bx                      ; restore bx
        ret                             ; return to caller
__init_87_emulator endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      void _fini_87_emulator( void )
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

public __fini_87_emulator
__fini_87_emulator proc
        mov     word ptr __dos87emucall,0
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
