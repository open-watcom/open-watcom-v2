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
        modstart dosfind

        defp    _dos_findfirst
        if __WASM__ ge 100
            xdefp   "C",_dos_findfirst
        else
            xdefp   <"C",_dos_findfirst>
        endif
;
;       unsigned _dos_findfirst( char *path, unsigned attr, struct find_t *buf)
;       struct find_t {
;               char reserved[21];              reserved for use by DOS
;               char attribute;                 attribute byte for matched path
;               unsigned short wr_time;         time of last write to file
;               unsigned short wr_date;         date of last write to file
;               unsigned long  size;            length of file in bytes
;               char name[13];                  null-terminated name of file
;       };
;
        push    BP              ; save BP
        mov     BP,SP           ; get access to stack
        push    BX              ; save BX
        push    CX              ; save CX
        push    DX              ; save DX
  if _MODEL and (_BIG_DATA or _HUGE_DATA)
        push    DS              ; save DS
        push    DX              ; get FP_SEG(path)
        push    AX              ; get FP_OFF(path)
        mov     CX,BX           ; get attr
    if _MODEL and _BIG_CODE
          lds   DX,6[BP]        ; - get pointer to buffer
    else
          lds   DX,4[BP]        ; - get pointer to buffer
    endif
  else
        push    DS              ; push address of path
        push    AX              ; ...
        mov     CX,DX           ; get attr
        mov     DX,BX           ; get address of buffer
  endif
        mov     AH,1Ah          ; set Disk Transfer Address
        int     21h             ; ...
        pop     DX              ; get pointer to path
        pop     DS              ; ...
        mov     AH,4Eh          ; findfirst file
        int     21h             ; ...
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        pop     DS              ; restore DS
endif
        call    __doserror_     ; set return code
        pop     DX              ; restore DX
        pop     CX              ; restore CX
        pop     BX              ; restore BX
        pop     BP              ; restore BP
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        ret     4               ; return to caller
else
        ret                     ; return to caller
endif
        endproc _dos_findfirst
        defp    _dos_findnext
        if __WASM__ ge 100
            xdefp   "C",_dos_findnext
        else
            xdefp   <"C",_dos_findnext>
        endif
;
;       unsigned _dos_findnext( struct find_t *buf)
;
        push    DX              ; save DX
  if _MODEL and (_BIG_DATA or _HUGE_DATA)
        push    DS              ; save DS
        mov     DS,DX           ; get FP_SEG(buf)
  endif
        mov     DX,AX           ; get FP_OFF(buf)
        mov     AH,1Ah          ; set Disk Transfer Address
        int     21h             ; ...
        mov     AH,4Fh          ; findnext file
        int     21h             ; ...
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        pop     DS              ; restore DS
endif
        call    __doserror_     ; set return code
        pop     DX              ; restore DX
        ret                     ; return to caller
        endproc _dos_findnext
        defp    _dos_findclose
        if __WASM__ ge 100
            xdefp   "C",_dos_findclose
        else
            xdefp   <"C",_dos_findclose>
        endif
;
;       unsigned _dos_findclose( struct find_t *buf)
;
        xor     AX,AX           ; stub program under MS-DOS
        ret                     ; return to caller
        endproc _dos_findclose

        endmod
        end
