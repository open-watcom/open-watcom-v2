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
;* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
;*               DESCRIBE IT HERE!
;*
;*****************************************************************************


                NAME    SERINT

                EXTRN   _dos_setvect_:NEAR
                EXTRN   _dos_getvect_:NEAR

                EXTRN   _Ticks:WORD
                EXTRN   _IntVector:WORD
                EXTRN   _IOBase:WORD
                EXTRN   _ErrorFlag:WORD
                EXTRN   _BreakFlag:WORD

DGROUP          GROUP   _BSS

_BSS            SEGMENT PUBLIC BYTE 'BSS'
_BSS            ENDS

_TEXT           SEGMENT PUBLIC BYTE 'CODE'

                ASSUME  CS:_TEXT,DS:DGROUP

                PUBLIC  InitInts_
                PUBLIC  FiniInts_
                PUBLIC  GetBufferByte_
                PUBLIC  ClearBuffer_


I8259           EQU     20H
EOI             EQU     20H
LSR_FLG         EQU     00FFH
BUFF_SIZE       EQU     0200H   ; must be a power of two

HW_INT_BASE     EQU     8
NO_DATA         equ     0ffffH

rbuf            db      BUFF_SIZE dup(?)
rbuf_gptr       dw      0
rbuf_rptr       dw      0
_ds_value       DW      ?
io_base         dw      ?
int_vect        db      ?

InitInts_       proc    near
                push    bx                      ; save registers
                push    cx
                push    dx
                push    ds
                mov     ax,seg DGROUP
                mov     ds,ax
                mov     cs:_ds_value,ax         ; save DS value for int handlers
                mov     ax,ds:_IOBase           ; save com port base
                mov     cs:io_base,ax
                mov     ax,ds:_IntVector        ; get old com handler
                mov     cs:int_vect,al          ; save int_vect for int handler
                add     ax,HW_INT_BASE
                call    _dos_getvect_
                mov     cx,ax
                or      cx,dx                   ; is it null?
                jne     _ok
                  mov     ax,offset null_handler  ; inset default handler
                  mov     dx,cs
_ok:
                mov     word ptr cs:old_com+0,ax; save old handler
                mov     word ptr cs:old_com+2,dx

                mov     ax,1cH                  ; get old timer handler
                call    _dos_getvect_
                mov     word ptr cs:old_timer+0,ax; save old handler
                mov     word ptr cs:old_timer+2,dx

                mov     ax,ds:_IntVector
                add     ax,HW_INT_BASE
                mov     bx,offset com_int
                mov     cx,cs
                call    _dos_setvect_

                mov     ax,1cH
                mov     bx,offset timer_int
                mov     cx,cs
                call    _dos_setvect_

                pop     ds                      ; restore registers
                pop     dx
                pop     cx
                pop     bx
                ret
InitInts_       endp


FiniInts_       proc    near
                push    bx
                push    cx
                push    ds
                mov     bx,seg DGROUP
                mov     ds,bx
                mov     bx,word ptr cs:old_com+0
                mov     cx,word ptr cs:old_com+2
                mov     ax,cs
                cmp     ax,cx
                jne     _not_null
                  xor     cx,cx
                  xor     bx,bx
_not_null:
                mov     ax,ds:_IntVector
                add     ax,HW_INT_BASE
                call    _dos_setvect_

                mov     bx,word ptr cs:old_timer+0
                mov     cx,word ptr cs:old_timer+2
                mov     ax,1cH
                call    _dos_setvect_
                pop     ds
                pop     cx
                pop     bx
                ret
FiniInts_       endp


GetBufferByte_  proc    near
                push    bx
                mov     ax,NO_DATA
                cli
                mov     bx,cs:rbuf_gptr
                cmp     bx,cs:rbuf_rptr
                je      done
                  mov   al,cs:rbuf[bx]
                  sub   ah,ah
                  inc   bx
                  and   bx,BUFF_SIZE-1
                  mov   cs:rbuf_gptr,bx
done:
                sti
                pop     bx
                ret
GetBufferByte_  endp

ClearBuffer_    proc    near
                push    dx
                cli
                mov     dx,cs:io_base               ; dx = IOBase
                in      al,dx
                push    cs:rbuf_rptr
                pop     cs:rbuf_gptr
                sti
                pop     dx
                ret
ClearBuffer_    endp


null_handler:
                push    ax
                mov     al,EOI
                out     I8259,al                    ; reset I8259
                pop     ax
                iret

pass_on:
                pop     dx          ; restore registers
                pop     ax
                db      0eaH        ; far jump
old_com         dd      0           ; filled in by InitInts_

public com_int
com_int         proc    far
                push    ax
                push    dx

                mov     dx,cs:io_base               ; dx = IOBase
                add     dx,0002H                    ; dx = IIR
                in      al,dx                       ; al = iir_val
                test    al,01H                      ; IS a com: interrupt?
                jne     pass_on                     ;   if not, pass it on
                push    bx
                mov     bx,cs:rbuf_rptr             ; bx = buffer index
new_data:
                sub     dx,0002H                    ; dx = IOBase
                cmp     al,04H                      ; new data? (iir_val=4?)
                jne     com_error                   ;   no, see if line error
                in      al,dx                       ; al = character received
                mov     byte ptr cs:rbuf[bx],al     ; save char into buffer
                inc     bx                          ; bx = next free location
                and     bx,BUFF_SIZE-1              ; (assume power of two)
check_iir:
                add     dx,0002H                    ; dx = IIR
                in      al,dx                       ; al = iir_val
                test    al,01H                      ; more com: interrupt?
                je      new_data                    ;   yes: loop

                mov     cs:rbuf_rptr,bx
                pop     bx
                sti
                mov     al,60H
                add     al,cs:int_vect              ; al = specific level EOI
                out     I8259,al                    ; reset I8259

                pop     dx
                pop     ax
                iret

com_error:      cmp     al,06H                      ; recv line status int?
                jne     check_iir                   ;  no, see if more ints
                add     dx,0005H                    ; dx = LSR
                in      al,dx                       ; al = lsr value
                sub     dx,0005H                    ; dx = IOBase
                sub     ah,ah                       ; high bits are 0
                push    ds
                mov     ds,word ptr cs:_ds_value
                test    ax,0EH
                je      next
                mov     word ptr ds:_ErrorFlag,1
next:           test    ax,10H
                je      pop_ds
                mov     word ptr ds:_BreakFlag,1
pop_ds:         pop     ds
                jmp     short check_iir
com_int         endp


public timer_int
timer_int       proc    far
                sti
                push    ds
                mov     ds,word ptr cs:_ds_value

                ASSUME  DS:DGROUP

                inc     word ptr ds:_Ticks
                pop     ds

                ASSUME  DS:NOTHING
                db      0eaH        ; far jump
old_timer       dd      0           ; filled in by InitInts_
timer_int       endp

_TEXT           ENDS

                END
