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

        modstart dosdrive

        defp    _dos_getdrive
        xdefp   "C",_dos_getdrive
;
;       unsigned _dos_getdrive( unsigned *drive );
;
        push    BX              ; save BX
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        push    DS              ; save DS
        mov     DS,DX           ; point to drive
endif
        mov     BX,AX           ; get address of drive
        mov     AH,19h          ; get current drive
        int     21h             ; ...
        inc     AX              ; increment drive #
        mov     AH,0            ; zero high part
        mov     [BX],AX         ; store drive #
        sub     AX,AX           ; set return code
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        pop     DS              ; restore DS
endif
        pop     BX              ; restore BX
        ret                     ; return to caller
        endproc _dos_getdrive

        defp    _dos_setdrive
        xdefp   "C",_dos_setdrive
;
;       void _dos_setdrive( unsigned drivenum, unsigned *drives )
;
        push    BX              ; save BX
        push    DX              ; save DX
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        push    DS              ; save DS
        mov     DS,CX           ; point to drive
else
        mov     BX,DX           ; get address of drive
endif
        mov     DX,AX           ; get drive number
        dec     DX              ; ...
        mov     AH,0Eh          ; set current drive
        int     21h             ; ...
        mov     AH,0            ; zero high part of return
        mov     [BX],AX         ; store total # of drives
        sub     AX,AX           ; set return code
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        pop     DS              ; restore DS
endif
        pop     DX              ; restore DX
        pop     BX              ; restore BX
        ret                     ; return to caller
        endproc _dos_setdrive

        endmod
        end
