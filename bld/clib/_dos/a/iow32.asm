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
        modstart iow32

        defp    _dos_read
        if __WASM__ ge 100
            xdefp       "C",_dos_read
        else
            xdefp       <"C",_dos_read>
        endif
;
;       unsigned _dos_read( int handle,         EAX
;                           void *buf,          EDX
;                           unsigned count,     EBX
;                           unsigned *bytes );  ECX
;
        push    3F00h           ; push function code
_do_io:

        push    ECX
ifdef __STACK__
        push    EBX
        push    EDX
        mov     EBX,20[ESP]     ; get handle
        mov     EDX,24[ESP]     ; get address of buffer
        mov     ECX,28[ESP]     ; get count
        mov     EAX,12[ESP]     ; get function code
else
        mov     ECX,EBX         ; get count
        mov     EBX,EAX         ; get file handle
        mov     EAX,4[ESP]      ; get function code
endif
        int21h                  ; do the operation
        _if     nc              ; if no error
ifdef __STACK__
          mov   EBX,32[ESP]     ; - get pointer to number of bytes
else
          mov   EBX,[ESP]       ; - get pointer to number of bytes
endif
          mov   [EBX],EAX       ; - store number of bytes transferred
        _endif                  ; endif
        call    __doserror_     ; set return code
ifdef __STACK__
        pop     EDX
        pop     EBX
endif
        pop     ECX
        add     ESP,4           ; remove function code
        ret                     ; return to caller
        endproc _dos_read
        defp    _dos_write
        if __WASM__ ge 100
            xdefp       "C",_dos_write
        else
            xdefp       <"C",_dos_write>
        endif
;
;       unsigned _dos_write( int handle,
;                           void *buf,
;                           unsigned count,
;                           unsigned *bytes );
;
        push    4000h           ; push function code
        jmp     _do_io          ; do the io
        endproc _dos_write

        endmod
        end
