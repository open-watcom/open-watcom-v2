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
ifdef __STACK__
        push    EDX
        mov     EAX,8[ESP]      ; get drivenum
        mov     EDX,12[ESP]     ; drives
endif
        push    EDI             ; save DI
        push    ECX             ; save CX
        push    EBX             ; save BX
        mov     EDI,EDX         ; get address of diskfree_t structure
        mov     EDX,EAX         ; get driver number
        mov     AH,36h          ; get disk free space
        int21h                  ; ...
        cmp     AX,0FFFFh       ; if valid drive number
        _if     ne              ; then
          and   EDX, 0000ffffh
          mov   [EDI],EDX       ; - save total # of clusters
          and   EBX, 0000ffffh
          mov   4[EDI],EBX      ; - save available number of clusters
          and   EAX, 0000ffffh
          mov   8[EDI],EAX      ; - save number of sectors per cluster
          and   ECX, 0000ffffh
          mov   12[EDI],ECX     ; - save number of bytes per sector
          sub   EAX,EAX         ; - indicate no errors
        _else                   ; else
          call  __set_EINVAL    ; - set errno = EINVAL
        _endif                  ; endif
        pop     EBX             ; restore BX
        pop     ECX             ; restore CX
        pop     EDI             ; restore DI
ifdef __STACK__
        pop     EDX
endif
        ret                     ; return to caller
        endproc _dos_getdiskfree

        endmod
        end
