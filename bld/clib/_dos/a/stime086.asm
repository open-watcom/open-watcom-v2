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

        xrefp   "C",__set_EINVAL
        modstart dosstime

        defp    _dos_setdate
        xdefp   "C",_dos_setdate
;
;       unsigned _dos_setdate( struct dosdate_t *date );
;       struct dosdate_t {
;               unsigned char day;      1-31
;               unsigned char month;    1-12
;               unsigned int year;      1980-2099
;               unsigned char dayofweek; 0-6 (0=Sunday)
;       };
;
        push    DX              ; save DX
        push    CX              ; save CX
        push    BX              ; save BX
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        push    DS              ; save DS
        mov     DS,DX           ; point to struct
endif
        mov     BX,AX           ; get address of dosdate_t structure
        mov     CX,2[BX]        ; get the year
        mov     DH,1[BX]        ; get the month
        mov     DL,[BX]         ; get the day
        mov     AL,4[BX]        ; get the day of the week
        mov     AH,2Bh          ; set date
        int     21h             ; ...
        mov     AH,AL           ; set return code
        cmp     AL,0FFh         ; if error
        _if     e               ; then
          call  __set_EINVAL    ; - set errno = EINVAL
        _endif                  ; endif
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        pop     DS              ; restore DS
endif
        pop     BX              ; restore BX
        pop     CX              ; restore CX
        pop     DX              ; restore DX
        ret                     ; return to caller
        endproc _dos_setdate

        defp    _dos_settime
        xdefp   "C",_dos_settime
;
;       unsigned _dos_settime( struct dostime_t *time );
;       struct dostime_t {
;               unsigned char hour;     0-23
;               unsigned char minute;   0-59
;               unsigned char second;   0-59
;               unsigned char hsecond;  1/100 second; 0-99
;       };
;
        push    DX              ; save DX
        push    CX              ; save CX
        push    BX              ; save BX
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        push    DS              ; save DS
        mov     DS,DX           ; point to struct
endif
        mov     BX,AX           ; get address of dostime_t structure
        mov     CH,[BX]         ; get the hour
        mov     CL,1[BX]        ; get the minute
        mov     DH,2[BX]        ; get the second
        mov     DL,3[BX]        ; get the hundredths of seconds
        mov     AH,2Dh          ; set time
        int     21h             ; ...
        mov     AH,AL           ; set return code
        cmp     AL,0FFh         ; if error
        _if     e               ; then
          call  __set_EINVAL    ; - set errno = EINVAL
        _endif                  ; endif
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        pop     DS              ; restore DS
endif
        pop     BX              ; restore BX
        pop     CX              ; restore CX
        pop     DX              ; restore DX
        ret                     ; return to caller
        endproc _dos_settime

        endmod
        end
