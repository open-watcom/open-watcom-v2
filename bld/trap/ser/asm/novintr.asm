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


.386p
                name    serint

                extrn   WaitCount : dword
                extrn   IntVector : dword
                extrn   IOBase : dword
                extrn   LSRVal : dword
                extrn   LSRNew : dword

_text           segment public byte 'code'

                public  GetByte
                public  ClearCom


I8259           equ     20H
EOI             equ     20H
LSR_FLG         equ     00FFH
BUFF_SIZE       equ     0200H   ; must be a power of two

NO_DATA         equ     0ffffffffH

_data segment byte public 'data'
rbuf            db      BUFF_SIZE dup(?)
rbuf_gptr       dd      0
rbuf_rptr       dd      0
_data ends

dgroup          group   _data
assume          ds:dgroup, cs:_text

GetByte         proc    near                    ; GetByte -- get byte from buff
                cli                             ; interrupts of for a sec
                push    ebx                     ; save a register
                mov     eax,NO_DATA             ; return no data present
                mov     ebx,rbuf_gptr           ; if gptr != rptr
                cmp     ebx,rbuf_rptr           ; - then
                je      done                    ; - ...
                  xor   eax,eax                 ; - char = rbuf[ gptr ]
                  mov   al,rbuf[ebx]            ; - ...
                  inc   ebx                     ; - increment gptr mod BUFF_SIZE
                  and   ebx,BUFF_SIZE-1         ; - ...
                  mov   rbuf_gptr,ebx           ; - ...
done:                                           ; endif
                pop     ebx                     ; restore a register
                sti                             ; back on
                ret                             ; return to caller
GetByte         endp                            ;

ClearCom        proc    near                    ; ClearCom -- empty the buffer
                push    edx                     ; save edx
                cli                             ; interrupts off
                mov     eax,rbuf_rptr           ; rbug_gptr = rbuf_rptr
                mov     rbuf_gptr,eax           ; ...
                mov     edx,IOBase              ; clear pending interrupts
                in      al,dx                   ; ...
                sti                             ; interrupts back on
                pop     edx                     ; restore edx
                ret                             ; return to caller
ClearCom        endp                            ;


retfromint      macro
                ret
                endm
pass_on:
                pop     edx                     ; restore registers
                xor     eax,eax                 ; pass it on
                dec     eax                     ; ...
                pop     eax                     ; ...
                retfromint                              ; ... return to interrupted code

public ComInt
ComInt          proc    near
                push    eax                     ; save a register
                push    edx                     ; ...

                mov     edx,IOBase              ; edx = IOBase
                add     edx,0002H               ; edx = IIR
                in      al,dx                   ; al = iir_val
                test    al,01H                  ; if not a com port interrupt
                jne     pass_on                 ; - pass it on
                push    ebx                     ; save another register
                mov     ebx,rbuf_rptr           ; ebx = buffer index
new_data:                                       ; do
                sub     edx,0002H               ; - dx = IOBase
                cmp     al,04H                  ; - if no new data
                jne     com_error               ; - - see if line error
                in      al,dx                   ; - save char into buffer
                mov     byte ptr rbuf[ebx],al   ; - ...
                inc     ebx                     ; - increment rptr mod BUFF_SIZE
                and     ebx,BUFF_SIZE-1         ; - ...
check_iir:
                add     edx,0002H               ; - dx = IIR
                in      al,dx                   ; - al = iir_val
                test    al,01H                  ;
                je      new_data                ; while( more com data )

                mov     rbuf_rptr,ebx           ; save back rptr
                pop     ebx                     ; restore a register
                sti                             ; interrupts off a sec
                mov     al,60H                  ; tell I8259 we're done
                add     eax,IntVector           ; al = specific level EOI
                out     I8259,al                ; reset I8259

                pop     edx                     ; restore regsters
                xor     eax,eax                 ; indicate success
                pop     eax                     ; ...
                retfromint                              ; return to interrupted code

com_error:      cmp     al,06H                  ; recv line status int?
                jne     check_iir               ;  no, see if more ints
                add     edx,0005H               ; dx = LSR
                xor     eax,eax                 ; zero high bits
                in      al,dx                   ; al = lsr value
                sub     edx,0005H               ; dx = IOBase
                mov     LSRVal,eax              ; save value to LSRVal
                mov     LSRNew,LSR_FLG          ; set flag to show new val
                jmp     short check_iir
ComInt          endp

_text           ends

                end
