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
;* Description:  Implementation of _dos_get/setftime() for 32-bit DOS. 
;*
;*****************************************************************************


;
;
include mdef.inc
include struct.inc
include int21.inc

        xrefp   __doserror_
        modstart dosftime

        defp    _dos_getftime
        xdefp   "C",_dos_getftime
;
;       unsigned _dos_getftime( int handle,
;                               unsigned *date,
;                               unsigned *time );
;
ifdef __STACK__
        push    EBX
        push    EDX
        mov     EAX,12[ESP]     ; get handle
        mov     EDX,16[ESP]     ; pointer to date
        mov     EBX,20[ESP]     ; pointer to time
endif
        push    EDI             ; save DI
        push    EDX             ; save DX
        push    ECX             ; save CX
        push    EBX             ; save BX
        mov     EDI,EDX         ; get pointer to date
        mov     EBX,EAX         ; get file handle
	xor     ECX,ECX         ; clear junk in high bits of ECX
	xor     EDX,EDX         ; and EDX
        mov     AX,5700h        ; get file's date and time
        int21h                  ; ...
        pop     EBX             ; get pointer to time
        _if     nc              ; if no error
          mov   [EDI],EDX       ; - store date
          mov   [EBX],ECX       ; - store time
        _endif                  ; endif
        call    __doserror_     ; set return code
        pop     ECX             ; restore CX
        pop     EDX             ; restore DX
        pop     EDI             ; restore DI
ifdef __STACK__
        pop     EDX
        pop     EBX
endif
        ret                     ; return to caller
        endproc _dos_getftime

        defp    _dos_setftime
        xdefp   "C",_dos_setftime
;
;       unsigned _dos_setftime( int handle,
;                               unsigned date,
;                               unsigned time );
;
ifdef __STACK__
        push    EBX
        push    EDX
        mov     EAX,12[ESP]     ; get handle
        mov     EDX,16[ESP]     ; date
        mov     EBX,20[ESP]     ; time
endif
        push    ECX             ; save ECX
        mov     ECX,EBX         ; get time
        mov     EBX,EAX         ; get file handle
        mov     AX,5701h        ; set file's date and time
        int21h                  ; ...
        call    __doserror_     ; set return code
        pop     ECX             ; restore ECX
ifdef __STACK__
        pop     EDX
        pop     EBX
endif
        ret                     ; return to caller
        endproc _dos_setftime

        endmod
        end
