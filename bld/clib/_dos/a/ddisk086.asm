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

        xref    "C",__set_EINVAL
        modstart dosdisk

        defp    _dos_getdiskfree
        xdefp   "C",_dos_getdiskfree
;
;       unsigned _dos_getdiskfree( unsigned drive,
;                                  struct diskfree_t *diskspace)
;       struct diskfree_t {
;               unsigned total_clusters;
;               unsigned avail_clusters;
;               unsigned sectors_per_cluster;
;               unsigned bytes_per_sector;
;       };
;
        push    DI              ; save DI
        push    CX              ; save CX
        push    BX              ; save BX
        push    DX              ; save DX
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        push    DS              ; save DS
        mov     DS,CX           ; get FP_SEG(diskspace)
        mov     DI,BX           ; get FP_OFF(diskspace)
else
        mov     DI,DX           ; get address of diskfree_t structure
endif
        mov     DX,AX           ; get drive number
        mov     AH,36h          ; get disk free space
        int     21h             ; ...
        cmp     AX,0FFFFh       ; if valid drive number
        _if     ne              ; then
          mov   [DI],DX         ; - save total # of clusters
          mov   2[DI],BX        ; - save available number of clusters
          mov   4[DI],AX        ; - save number of sectors per cluster
          mov   6[DI],CX        ; - save number of bytes per sector
          sub   AX,AX           ; - indicate no errors
        _else                   ; else
          call  __set_EINVAL    ; - set errno = EINVAL
        _endif                  ; endif
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        pop     DS              ; restore DS
endif
        pop     DX              ; restore DX
        pop     BX              ; restore BX
        pop     CX              ; restore CX
        pop     DI              ; restore DI
        ret                     ; return to caller
        endproc _dos_getdiskfree

        endmod
        end
