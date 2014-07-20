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

        xrefp   __doserror_
        modstart dosio

        defp    _dos_read
        xdefp   "C",_dos_read
;
;       unsigned _dos_read( int handle,
;                           void far *buf,
;                           unsigned count,
;                           unsigned *bytes );
;
        push    3F00h           ; push function code
_do_io:

ifdef __STACK__
        push    ECX
        push    EBX
        push    EDX
        push    DS              ; save DS
        mov     EBX,24[ESP]     ; get handle
        lds     EDX,28[ESP]     ; get address of buffer
        mov     ECX,36[ESP]     ; get count
        mov     EAX,16[ESP]     ; get function code
else
        push    DS              ; save DS
        mov     DS,CX           ; get segment of buffer
        mov     ECX,EDX         ; get count
        mov     EDX,EBX         ; get offset of buffer
        mov     EBX,EAX         ; get file handle
        mov     EAX,4[ESP]      ; get function code
endif
        int21h                  ; do the operation
        pop     DS              ; restore DS
        push    DS              ; save it again
        _if     nc              ; if no error
ifdef __STACK__
          mov   EBX,40[ESP]     ; - get pointer to number of bytes
else
          mov   EBX,12[ESP]     ; - get pointer to number of bytes
endif
          mov   [EBX],EAX       ; - store number of bytes transferred
        _endif                  ; endif
        call    __doserror_     ; set return code
        pop     DS              ; restore DS
ifdef __STACK__
        pop     EDX
        pop     EBX
        pop     ECX
endif
        add     ESP,4           ; remove function code
ifdef __STACK__
        ret                     ; return to caller
else
        ret     4               ; return to caller
endif
        endproc _dos_read

        defp    _dos_write
        xdefp   "C",_dos_write
;
;       unsigned _dos_write( int handle,
;                           void far *buf,
;                           unsigned count,
;                           unsigned *bytes );
;
        push    4000h           ; push function code
        jmp     _do_io          ; do the io
        endproc _dos_write

        endmod
        end
