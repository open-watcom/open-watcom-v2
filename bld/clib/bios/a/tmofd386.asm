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
;* Description:  BIOS time of day service, 32-bit version.
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
;       unsigned __ibm_bios_timeofday( int service, long *timeval );
;
ifdef __STACK__
        mov     EAX,4[ESP]      ; get service
        mov     EDX,8[ESP]      ; get pointer to structure
endif
        push    EBX             ; save BX
        push    ECX             ; save CX
        push    EDX             ; save DX
        mov     EBX,EDX         ; get pointer to diskinfo
        _guess                  ; guess: _TIME_GETCLOCK
          or    EAX,EAX         ; - quit if not _TIME_GETCLOCK
          _quif ne              ; - ...
          int   1Ah             ; - get current time
          mov   [EBX],DX        ; - store time
          mov   2[EBX],CX       ; - ...
          mov   AH,0            ; - zero high part of overflow
        _admit                  ; guess: _TIME_SETCLOCK
          dec   EAX             ; - quit if not _TIME_SETCLOCK
          mov   EAX,0FFFFFFFFh  ; - set error code
          _quif ne              ; - ...
          mov   DX,[EBX]        ; - get time to be set
          mov   CX,2[EBX]       ; - ...
          mov   AH,1            ; - indicate set time
          int   1Ah             ; - set the time
          sub   EAX,EAX         ; - set return code
        _endguess               ; endguess
        pop     EDX             ; restore BX
        pop     ECX             ; restore BX
        pop     EBX             ; restore BX
        ret                     ; return to caller
        endproc __ibm_bios_timeofday

        endmod
        end
