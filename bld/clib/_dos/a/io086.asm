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
        modstart dosio

get_parms macro
        push    BP              ; save BP
        mov     BP,SP           ; get access to parms
        push    DS              ; save DS
        mov     DS,CX           ; get FP_SEG(buf)
        mov     CX,DX           ; get count
        mov     DX,BX           ; get FP_OFF(buf)
        mov     BX,AX           ; get handle
if _MODEL and _BIG_CODE
        bytes = 6
else
        bytes = 4
endif
        endm

        defp    _dos_read
        xdefp   "C",_dos_read
;
;       unsigned _dos_read( int handle,
;                           void far *buf,
;                           unsigned count,
;                           unsigned *bytes );
;
        get_parms
        mov     AH,3Fh          ; read file
_do_io: int     21h             ; ...
        pop     DS              ; restore DS
        push    DS              ; save it again
        _if     nc              ; if no error
 if _MODEL and (_BIG_DATA or _HUGE_DATA)
          lds   BX,bytes[BP]    ; - get pointer to number of bytes
 else
          mov   BX,bytes[BP]    ; - get pointer to number of bytes
 endif
          mov   [BX],AX         ; - store number of bytes transferred
        _endif                  ; endif
        pop     DS              ; restore DS
        call    __doserror_     ; set return code
        pop     BP              ; restore BP
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        ret     4               ; return to caller
else
        ret     2               ; return to caller
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
        get_parms
        mov     AH,40h          ; write file
        jmp     _do_io          ; ...
        endproc _dos_write

        endmod
        end
