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
        modstart dosfile

        defp    _dos_close
        if __WASM__ ge 100
            xdefp       "C",_dos_close
        else
            xdefp       <"C",_dos_close>
        endif
;
;       unsigned _dos_close( int handle );
;
ifdef __STACK__
        mov     EAX,4[ESP]      ; get handle
endif
        push    EBX             ; save BX
        mov     BX,AX           ; get file handle
        mov     AH,3Eh          ; close file
        int21h                  ; ...
        call    __doserror_     ; set return code
        pop     EBX             ; restore BX
        ret                     ; return to caller
        endproc _dos_close
        defp    _dos_commit
        if __WASM__ ge 100
            xdefp       "C",_dos_commit
        else
            xdefp       <"C",_dos_commit>
        endif
;
;       unsigned _dos_commit( int handle );
;
ifdef __STACK__
        mov     EAX,4[ESP]      ; get handle
endif
        push    EBX             ; save BX
        mov     BX,AX           ; get file handle
        mov     AH,68h          ; commit file
        clc
        int21h                  ; ...
        call    __doserror_     ; set return code
        pop     EBX             ; restore BX
        ret                     ; return to caller
        endproc _dos_commit
        defp    _dos_open
        if __WASM__ ge 100
            xdefp       "C",_dos_open
        else
            xdefp       <"C",_dos_open>
        endif
;
;       unsigned _dos_open( char *path, unsigned mode, int *handle )
;
ifdef __STACK__
        push    EBX
        push    EDX
        mov     EAX,12[ESP]     ; get path
        mov     EDX,16[ESP]     ; mode
        mov     EBX,20[ESP]     ; handle
endif
        xchg    EAX,EDX         ; AX = mode, EDX = path
        mov     AH,3Dh          ; open the file
        int21h                  ; ...
        _if     nc              ; if no error
          mov   [EBX],EAX       ; - store handle
        _endif                  ; endif
        call    __doserror_     ; set return code
ifdef __STACK__
        pop     EDX
        pop     EBX
endif
        ret                     ; return to caller
        endproc _dos_open

        endmod
        end
