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
        modstart doscreat

create  macro   func_code
        push    BP              ; save BP
        mov     BP,SP           ; get access to stack
        push    BX              ; save BX
        push    CX              ; save CX
        push    DX              ; save DX
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        push    DS              ; save DS
        mov     DS,DX           ; get FP_SEG(path)
        mov     DX,AX           ; get FP_OFF(path)
        mov     CX,BX           ; get attr
else
        mov     CX,DX           ; get attr
        mov     DX,AX           ; get path
endif
        mov     AH,func_code    ; get function code
        int     21h             ; create the file
        _if     nc              ; if no error
if _MODEL and (_BIG_DATA or _HUGE_DATA)
 if _MODEL and _BIG_CODE
          lds   BX,6[BP]        ; - get pointer to handle
 else
          lds   BX,4[BP]        ; - get pointer to handle
 endif
endif
          mov   [BX],AX         ; - store handle
        _endif                  ; endif
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        pop     DS              ; restore DS
endif
        call    __doserror_     ; set return code
        pop     DX              ; restore DX
        pop     CX              ; restore CX
        pop     BX              ; restore BX
        pop     BP              ; restore BP
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        ret     4               ; return to caller
else
        ret                     ; return to caller
endif
        endm


        defp    _dos_creat
        if __WASM__ ge 100
            xdefp   "C",_dos_creat
        else
            xdefp   <"C",_dos_creat>
        endif
;
;       unsigned _dos_creat( char *path, unsigned attr, int *handle )
;
        create  3Ch
        endproc _dos_creat
        defp    _dos_creatnew
        if __WASM__ ge 100
            xdefp   "C",_dos_creatnew
        else
            xdefp   <"C",_dos_creatnew>
        endif
;
;       unsigned _dos_creatnew( char *path, unsigned attr, int *handle )
;
        create  5Bh
        endproc _dos_creatnew

        endmod
        end
