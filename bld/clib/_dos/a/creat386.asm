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

        xref    __doserror_
        modstart doscreat

        defp    _dos_creat
        if __WASM__ ge 100
            xdefp       "C",_dos_creat
        else
            xdefp       <"C",_dos_creat>
        endif
;
;       unsigned _dos_creat( char *path, unsigned attr, int *handle )
;
ifdef __STACK__
        push    EBX
        push    EDX
        mov     EAX,12[ESP]     ; get path
        mov     EDX,16[ESP]     ; attr
        mov     EBX,20[ESP]     ; handle
endif
        push    ECX             ; save CX
        mov     ECX,EDX         ; get attribute
        mov     EDX,EAX         ; DX = path
        mov     AH,3Ch          ; creat file
        int21h                  ; ...
        _if     nc              ; if no error
          mov   [EBX],EAX       ; - store handle
        _endif                  ; endif
        call    __doserror_     ; set return code
        pop     ECX             ; restore CX
ifdef __STACK__
        pop     EDX
        pop     EBX
endif
        ret                     ; return to caller
        endproc _dos_creat
        defp    _dos_creatnew
        if __WASM__ ge 100
            xdefp       "C",_dos_creatnew
        else
            xdefp       <"C",_dos_creatnew>
        endif
;
;       unsigned _dos_creatnew( char *path, unsigned attr, int *handle )
;
ifdef __STACK__
        push    EBX
        push    EDX
        mov     EAX,12[ESP]     ; get path
        mov     EDX,16[ESP]     ; attr
        mov     EBX,20[ESP]     ; handle
endif
        push    ECX             ; save CX
        mov     ECX,EDX         ; get attribute
        mov     EDX,EAX         ; DX = path
        mov     AH,5Bh          ; create a new file
        int21h                  ; ...
        _if     nc              ; if no error
          mov   [EBX],EAX       ; - store handle
        _endif                  ; endif
        call    __doserror_     ; set return code
        pop     ECX             ; restore CX
ifdef __STACK__
        pop     EDX
        pop     EBX
endif
        ret                     ; return to caller
        endproc _dos_creatnew

        endmod
        end
