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
;* Description:  DOS memory management routines for 16-bit DOS.
;*
;*****************************************************************************


;
;
include mdef.inc
include struct.inc

        xref    __doserror_
        modstart dosmem

        defp    _dos_allocmem
        xdefp   "C",_dos_allocmem
;
;       unsigned _dos_allocmem( unsigned size, unsigned *segment );
;
        push    BX              ; save BX
        push    DX              ; save DX
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        push    DS              ; save DS
        mov     DS,CX           ; get segment part of pointer to segment
        mov     DX,BX           ; get offset part of pointer
endif
        mov     BX,AX           ; get # of paragraphs wanted
        mov     AH,48h          ; allocate memory
        int     21h             ; ...
        xchg    BX,DX           ; get pointer for result
        _if     nc              ; if no error
          mov   DX,AX           ; - get segment of allocated memory
          sub   AX,AX           ; - indicate no error
        _else                   ; else
          call  __doserror_     ; - set error code
        _endif                  ; endif
        mov     [BX],DX         ; store size of largest block or segment
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        pop     DS              ; restore DS
endif
        pop     DX              ; restore DX
        pop     BX              ; restore BX
        ret                     ; return to caller
        endproc _dos_allocmem

        defp    _dos_freemem
        xdefp   "C",_dos_freemem
;
;       unsigned _dos_freemem( unsigned segment );
;
        push    ES              ; save ES
        mov     ES,AX           ; get segment to be freed
        mov     AH,49h          ; free allocated memory
        int     21h             ; ...
        call    __doserror_     ; set return code
        pop     ES              ; restore ES
        ret                     ; return to caller
        endproc _dos_freemem

        defp    _dos_setblock
        xdefp   "C",_dos_setblock
;
;       unsigned _dos_setblock( unsigned size,
;                               unsigned segment, unsigned *maxsize );
;
;
        push    ES              ; save ES
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        push    DS              ; save DS
        mov     DS,CX           ; get segment part of pointer to segment
endif
        push    BX              ; save pointer to maxsize
        mov     ES,DX           ; get segment to be modified
        mov     BX,AX           ; get new size
        mov     AH,4Ah          ; modify allocated memory
        int     21h             ; ...
        mov     DX,BX           ; get maximum block size if failure
        pop     BX              ; restore pointer to maxsize
        _if     c               ; if error
          mov   [BX],DX         ; - store the maxsize
        _endif                  ; endif
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        pop     DS              ; restore DS
endif
        call    __doserror_     ; set return code
        pop     ES              ; restore ES
        ret                     ; return to caller
        endproc _dos_setblock

        endmod
        end
