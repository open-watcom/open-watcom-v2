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
        modstart dosfile

        defp    _dos_close
        if __WASM__ ge 100
            xdefp   "C",_dos_close
        else
            xdefp   <"C",_dos_close>
        endif
;
;       unsigned _dos_close( int handle );
;
        push    BX              ; save BX
        mov     BX,AX           ; get file handle
        mov     AH,3Eh          ; close file
        int     21h             ; ...
        call    __doserror_     ; set return code
        pop     BX              ; restore BX
        ret                     ; return to caller
        endproc _dos_close
        defp    _dos_commit
        if __WASM__ ge 100
            xdefp   "C",_dos_commit
        else
            xdefp   <"C",_dos_commit>
        endif
;
;       unsigned _dos_commit( int handle );
;
        push    BX              ; save BX
        mov     BX,AX           ; get file handle
        mov     AH,68h          ; commit file
        clc
        int     21h             ; ...
        call    __doserror_     ; set return code
        pop     BX              ; restore BX
        ret                     ; return to caller
        endproc _dos_commit
        defp    _dos_open
        if __WASM__ ge 100
            xdefp   "C",_dos_open
        else
            xdefp   <"C",_dos_open>
        endif
;
;       unsigned _dos_open( char *path, unsigned mode, int *handle )
;
        push    BP              ; save BP
        mov     BP,SP           ; get access to stack
        push    BX              ; save BX
        push    CX              ; save CX
        push    DX              ; save DX
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        push    DS              ; save DS
        mov     DS,DX           ; get FP_SEG(path)
        mov     DX,AX           ; get FP_OFF(path)
        mov     AX,BX           ; get mode
else
        xchg    DX,AX           ; AX = mode, DX = address of path
endif
        mov     AH,3Dh          ; open the file
        int     21h             ; ...
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
        endproc _dos_open

        endmod
        end
