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

        xref    __doserror_
        modstart dosftime

        defp    _dos_getftime
        if __WASM__ ge 100
            xdefp   "C",_dos_getftime
        else
            xdefp   <"C",_dos_getftime>
        endif
;
;       unsigned _dos_getftime( int handle,
;                               unsigned short *date,
;                               unsigned short *time );
;
        push    BP              ; save BP
        mov     BP,SP           ; get access to parms
        push    DI              ; save DI
        push    DX              ; save DX
        push    CX              ; save CX
        push    BX              ; save BX
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        push    DS              ; save DS
        mov     DS,CX           ; get FP_SEG(date)
        mov     DI,BX           ; get FP_OFF(date)
else
        push    BX              ; push address of time
        mov     DI,DX           ; get address of date
endif
        mov     BX,AX           ; get handle
        mov     AX,5700h        ; get file's date and time
        int     21h             ; ...
if _MODEL and (_BIG_DATA or _HUGE_DATA)
else
        pop     BX              ; get pointer to time
endif
        _if     nc              ; if no error
          mov   [DI],DX         ; - store date
if _MODEL and (_BIG_DATA or _HUGE_DATA)
 if _MODEL and _BIG_CODE
          lds   BX,6[BP]        ; - get pointer to time
 else
          lds   BX,4[BP]        ; - get pointer to time
 endif
endif
          mov   [BX],CX         ; - store time
        _endif                  ; endif
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        pop     DS              ; restore DS
endif
        call    __doserror_     ; set return code
        pop     BX              ; restore BX
        pop     CX              ; restore CX
        pop     DX              ; restore DX
        pop     DI              ; restore DI
        pop     BP              ; restore BP
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        ret     4               ; return to caller
else
        ret                     ; return to caller
endif
        endproc _dos_getftime
        defp    _dos_setftime
        if __WASM__ ge 100
            xdefp   "C",_dos_setftime
        else
            xdefp   <"C",_dos_setftime>
        endif
;
;       unsigned _dos_setftime( int handle,
;                               unsigned short date,
;                               unsigned short time );
;
        push    CX              ; save CX
        mov     CX,BX           ; get time
        mov     BX,AX           ; get file handle
        mov     AX,5701h        ; set file's date and time
        int     21h             ; ...
        call    __doserror_     ; set return code
        pop     CX              ; restore CX
        ret                     ; return to caller
        endproc _dos_setftime

        endmod
        end
