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
include int21.inc

        modstart dosdrive

        defp    _dos_getdrive
        xdefp   "C",_dos_getdrive
;
;       unsigned _dos_getdrive( unsigned *drive );
;
ifdef __STACK__
        mov     EAX,4[ESP]      ; get handle
endif
        push    EBX             ; save BX
        mov     EBX,EAX         ; save address of drive
        sub     EAX,EAX         ; zero EAX
        mov     AH,19h          ; get current drive
        int21h                  ; ...
        inc     AX              ; increment drive #
        mov     AH,0            ; zero high part
        mov     [EBX],EAX       ; store drive #
        sub     AX,AX           ; set return code
        pop     EBX             ; restore BX
        ret                     ; return to caller
        endproc _dos_getdrive

        defp    _dos_setdrive
        xdefp   "C",_dos_setdrive
;
;       void _dos_setdrive( unsigned drivenum, unsigned *drives )
;
ifdef __STACK__
        push    EDX
        mov     EAX,8[ESP]      ; get drivenum
        mov     EDX,12[ESP]     ; drives
endif
        push    EBX             ; save BX
        mov     EBX,EDX         ; save pointer to drives
        mov     EDX,EAX         ; get drive number
        dec     EDX             ; ...
        sub     EAX,EAX         ; zero EAX
        mov     AH,0Eh          ; set current drive
        int21h                  ; ...
        mov     AH,0            ; zero high part of return
        mov     [EBX],EAX       ; store total # of drives
        sub     EAX,EAX         ; set return code
        pop     EBX             ; restore BX
ifdef __STACK__
        pop     EDX
endif
        ret                     ; return to caller
        endproc _dos_setdrive

        endmod
        end
