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
;* Description:  Stack checking for 16-bit QNX.
;*
;*****************************************************************************


include mdef.inc
include struct.inc
include exitwmsg.inc

        .286C

        extrn   "C",_STACKLOW      : word

        modstart        stk

CONST   segment word public 'DATA'
CONST   ends

_DATA   segment word public 'DATA'
_DATA   ends

        DGROUP  group   CONST, _DATA
        assume  ds:DGROUP

dgroupp dw      DGROUP

        xdefp   __STK
        xdefp   "C",__STKOVERFLOW

msg     db      "Stack Overflow at "
msg_end label byte
hextab  db      "0123456789ABCDEF"

        defpe   __STK
        _guess                  ; guess: no overflow
          cmp   ax,sp           ; - quit if user asking for too much
          _quif ae              ; - . . .
          sub   ax,sp           ; - calculate new low point
          neg   ax              ; - calc what new SP would be
if _MODEL and (_BIG_DATA or _HUGE_DATA)
          push  ds              ; - save ds
          mov   ds,cs:dgroupp   ; - load ds from DGROUP
endif
          cmp   ax,ds:_STACKLOW ; - quit if too much
if _MODEL and (_BIG_DATA or _HUGE_DATA)
          pop   ds              ; - restore ds
endif
          _quif be              ; - . . .
          ret                   ; - return
        _endguess               ; endguess

;* modified stack overflow code to print the return address of the
;* caller.

__STKOVERFLOW:

        pop     ax
if _MODEL and _BIG_CODE
        pop     dx
else
        mov     dx,cs
endif                           ; dx:ax points at retaddr
        add     sp,0100h        ; cream last 256 bytes of stack
        mov     bp,sp

        push    ax              ; push return address, segment first
        push    dx              ; to make it easier to print it


        push    ss              ; setup string instrs to copy
        pop     es              ; exit message onto stack
        cld
        mov     di,bp

        mov     si,offset msg
        mov     cx,offset msg_end
        sub     cx,si
        push    cs
        pop     ds
        rep     movsb

        pop     bx              ; get segment
        call    _putw
        mov     al,':'
        stosb
        pop     bx              ; get offset
        call    _putw
        xor     al,al
        stosb
        mov     ds,cs:dgroupp
        mov     dx,ss           ; ds:ax  points at
        mov     ax,bp           ; exit code in bx
        mov     bx,1
        call    __fatal_runtime_error

        endproc __STK

_putw proc near
        mov     dx,bx           ; save value
        mov     cl,12           ; setup shift count
_lup:
        mov     bx,dx           ; get value
        shr     bx,cl           ; put in bottom 4 bits.
        and     bx,0fh
        mov     al,cs:hextab[bx]
        stosb
        sub     cl,4
        jns     _lup
        ret
_putw   endp

        endmod

        end
