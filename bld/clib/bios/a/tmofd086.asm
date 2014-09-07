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
;* Description:  BIOS time of day service, 16-bit version.
;*
;*****************************************************************************


;
;
include mdef.inc
include struct.inc

        modstart b_timofd

        defp    __ibm_bios_timeofday
        xdefp  "C",__ibm_bios_timeofday
;
;       int     __ibm_bios_timeofday( int service, long *timeval );
;
        push    BX              ; save BX
        push    CX              ; save CX
        push    DX              ; save DX
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        push    DS              ; save DS
        mov     DS,CX           ; point to diskinfo struct
else
        mov     BX,DX           ; get pointer to diskinfo
endif
        _guess                  ; guess: _TIME_GETCLOCK
          or    AX,AX           ; - quit if not _TIME_GETCLOCK
          _quif ne              ; - ...
          int   1Ah             ; - get current time
          mov   [BX],DX         ; - store time
          mov   2[BX],CX        ; - ...
          xor   AH,AH           ; - zero high part of overflow
        _admit                  ; guess: _TIME_SETCLOCK
          dec   AX              ; - quit if not _TIME_SETCLOCK
          mov   AX,0FFFFh       ; - set error code
          _quif ne              ; - ...
          mov   DX,[BX]         ; - get time to be set
          mov   CX,2[BX]        ; - ...
          mov   AH,1            ; - indicate set time
          int   1Ah             ; - set the time
          sub   AX,AX           ; - set return code
        _endguess               ; endguess
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        pop     DS              ; restore DS
endif
        pop     DX              ; restore BX
        pop     CX              ; restore BX
        pop     BX              ; restore BX
        ret                     ; return to caller
        endproc __ibm_bios_timeofday

        endmod
        end
