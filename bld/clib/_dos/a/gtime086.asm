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

        modstart dosgtime

        defp    _dos_getdate
        if __WASM__ ge 100
            xdefp   "C",_dos_getdate
        else
            xdefp   <"C",_dos_getdate>
        endif
;
;       void _dos_getdate( struct dosdate_t *date );
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
        mov     AH,2Ah          ; get date
        int     21h             ; ...
        mov     2[BX],CX        ; store the year
        mov     1[BX],DH        ; store the month
        mov     [BX],DL         ; store the day
        mov     4[BX],AL        ; store the day of the week
        sub     AX,AX           ; set return code
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        pop     DS              ; restore DS
endif
        pop     BX              ; restore BX
        pop     CX              ; restore CX
        pop     DX              ; restore DX
        ret                     ; return to caller
        endproc _dos_getdate
        defp    _dos_gettime
        if __WASM__ ge 100
            xdefp   "C",_dos_gettime
        else
            xdefp   <"C",_dos_gettime>
        endif
;
;       void _dos_gettime( struct dostime_t *time );
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
        mov     AH,2Ch          ; get time
        int     21h             ; ...
        mov     [BX],CH         ; store the hour
        mov     1[BX],CL        ; store the minute
        mov     2[BX],DH        ; store the second
        mov     3[BX],DL        ; store the hundredths of seconds
        sub     AX,AX           ; set return code
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        pop     DS              ; restore DS
endif
        pop     BX              ; restore BX
        pop     CX              ; restore CX
        pop     DX              ; restore DX
        ret                     ; return to caller
        endproc _dos_gettime

        endmod
        end
