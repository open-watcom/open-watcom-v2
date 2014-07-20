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


include mdef.inc

ifdef __DOS__
ifndef __386__
__DOS_086__ = 1
endif
endif

        modstart _old8087

datasegment
        xrefp   __8087
ifdef __DOS_086__
        xrefd   __dos87emucall, word
        xrefd   __dos87real, byte
endif
enddata

        xdefp    __old_8087
        defp     __old_8087
ifdef __DOS_086__
        cmp     __dos87real,0
        jz      l1
endif
        fldz
        fldz
        fldz
        fldz
ifdef __DOS_086__
l1:     cmp     __dos87emucall,0
        jz      l2
        mov     ax,2
        call    __dos87emucall
l2:
endif
        ret
        endproc __old_8087
        endmod

include xinit.inc

        xinit   __old_8087,INIT_PRIORITY_FPU + 4

        end
