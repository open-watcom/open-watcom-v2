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


;
;
include mdef.inc
include struct.inc

        name    harderr

_TEXT   segment word public 'CODE'
_TEXT   ends

_DATA   segment word public 'DATA'
_DATA   ends

DGROUP  group   _DATA
        assume  cs:_TEXT, ds: DGROUP, ss: DGROUP, es:nothing

_DATA   segment
int24ip dw      0
int24cs dw      0
oldsp   dw      0
errors  dw      1, 0f308h, 1054h, 1200h
_DATA   ends

_TEXT   segment

        defp    _harderr
        xdefp   "C",_harderr
;
;       unsigned _harderr( void (far *fptr)() );
;
        push    DS              ; save DS
    ifndef __WINDOWS__
        if _MODEL and (_BIG_DATA or _HUGE_DATA)
        push    AX
        mov     AX,seg int24ip  ; get DS
        mov     DS,AX
        pop     AX
        endif
    endif
        mov     int24ip,AX      ; save address of users int 24 handler
        mov     int24cs,DX      ; ...
        mov     AX,CS           ; set DS=CS
        mov     DS,AX           ; ...
        mov     DX,offset int24rtn ; point to int 24 interrupt handler
        mov     AX,2524h        ; set interrupt handler
        int     21h             ; ...
        sub     AX,AX           ; set success
        pop     DS              ; restore DS
        ret                     ; return to caller
        endproc _harderr

        xdefp   "C",_hardresume

        defp    int24rtn
        push    ES              ; save registers
        push    DS              ; ...
        push    BP              ; ...
        push    DI              ; ...
        push    SI              ; ...
        push    DX              ; ...
        push    CX              ; ...
        push    BX              ; ...
        push    AX              ; ...
        mov     BX,DGROUP       ; point to DGROUP
        mov     DS,BX           ; ...
        mov     oldsp,SP        ; save the stack pointer
        mov     DX,DI           ; get error code
        mov     CX,BP           ; get address of device header
        mov     BX,SI           ; ...
        call    dword ptr int24ip; invoke user's int 24 handler


_hardresume:
;
;       AX - contains an action code
;       0 - IGNORE
;       1 - RETRY
;       2 - ABORT
;       3 - FAIL
;
        mov     SP,oldsp        ; restore SP
_hardretn_return:
        pop     BX              ; skip over saved AX
        pop     BX              ; restore registers
        pop     CX              ; ...
        pop     DX              ; ...
        pop     SI              ; ...
        pop     DI              ; ...
        pop     BP              ; ...
        pop     DS              ; ...
        pop     ES              ; ...
        iret                    ; return from interrupt

        endproc int24rtn

        xdefp   _hardretn_
;
;       void _hardretn( int errcode );
;
        defp    _hardretn_
        mov     SP,oldsp        ; restore sp
        add     SP,12*2         ; skip over saved registers
        mov     BP,SP           ; get access to stack
        mov     BX,[BP]         ; get function code
        cmp     BH,38h          ; if func code >= 38h
        _if     ae              ; then
          or    byte ptr 22[BP],1; - turn on carry bit in saved flags
          jmp   _hardretn_return; - and return
        _endif                  ; endif
        mov     DX,BX           ; get function code
        mov     AX,BX           ; get function code
        mov     CL,4            ; get shift count
        shr     DX,CL           ; isolate top 2 bits of func code in DH
        shr     DL,CL           ; get bottom 4 bits of func code in DL
        mov     CL,DL           ; move to CL for shift count
        inc     CL              ; +1
        mov     BL,DH           ; get top 2 bits for index
        xor     BH,BH           ;
        shl     BX,1            ; times 2
        mov     DX,errors[BX]   ; get bit mask of funcs that set error codes
        shl     DX,CL           ;
        _if     c               ; if function returns an error
          mov   AL,0FFh         ; - indicate error
        _endif                  ; endif
        jmp     _hardretn_return; and return
        endproc _hardretn_

        endmod
        end
