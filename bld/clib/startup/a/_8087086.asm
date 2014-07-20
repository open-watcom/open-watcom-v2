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
include xinit.inc

        modstart    _8087

        xrefp   "C",__chk8087

datasegment
        public  __8087
        public  __real87
ifdef __DOS__
        public  __dos87emucall
        public  __dos87real
endif

__8087          db  0   ; 0 => no real 80x87 and no EMU, otherwise real 80x87 or EMU present
__real87        db  0   ; 0 => no real 80x87 is used, otherwise real 80x87 is used
ifdef __DOS__
__dos87emucall  dw  0   ; (DOS) 0 => no 80x87 EMU, otherwise 80x87 EMU control routine
__dos87real     db  0   ; (DOS) 0 => no real 80x87 installed, otherwise real 80x87 installed
endif
enddata

        xinit   __chk8087,INIT_PRIORITY_FPU + 3

        endmod
        end
