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
        modstart dosattr

        defp    _dos_getfileattr
        if __WASM__ ge 100
            xdefp   "C",_dos_getfileattr
        else
            xdefp   <"C",_dos_getfileattr>
        endif
;
;       unsigned _dos_getfileattr( char *path, unsigned *attr );
;
        push    BP              ; save BP
        mov     BP,SP           ; get access to stack
        push    BX              ; save BX
        push    CX              ; save CX
        push    DX              ; save DX
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        push    DS              ; save DS
        mov     DS,DX           ; get FP_SEG(path)
        push    CX              ; save FP_SEG(attr)
else
        mov     BX,DX           ; save pointer to attributes
endif
        mov     DX,AX           ; get path
        mov     AX,4300h        ; get file attributes
        int     21h             ; ...
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        pop     DS              ; get segment part of attr
endif
        _if     nc              ; if no error
          mov   [BX],CX         ; - store file attributes
        _endif                  ; endif
        call    __doserror_     ; set return code
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        pop     DS              ; restore DS
endif
        pop     DX              ; restore DX
        pop     CX              ; restore CX
        pop     BX              ; restore BX
        pop     BP              ; restore BP
        ret                     ; return to caller
        endproc _dos_getfileattr
        defp    _dos_setfileattr
        if __WASM__ ge 100
            xdefp   "C",_dos_setfileattr
        else
            xdefp   <"C",_dos_setfileattr>
        endif
;
;       unsigned _dos_setfileattr( char *path, unsigned attr );
;
        push    CX              ; save CX
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        push    DS              ; save DS
        mov     DS,DX           ; get FP_SEG(path)
        mov     DX,AX           ; get FP_OFF(path)
        mov     CX,BX           ; get attr
else
        mov     CX,DX           ; get attr
        mov     DX,AX           ; get path
endif
        mov     AX,4301h        ; set file attributes
        int     21h             ; ...
        call    __doserror_     ; set return code
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        pop     DS              ; restore DS
endif
        pop     CX              ; restore CX
        ret                     ; return to caller
        endproc _dos_setfileattr

        endmod
        end
