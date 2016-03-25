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
;* Description:  8087 emulator code
;*
;*****************************************************************************


.286

ifdef __SMALL__
_SMALL_CODE = 1
else ifdef __COMPACT__
_SMALL_CODE = 1
else
_SMALL_CODE = 0
endif

_TEXT   segment word public 'CODE'

modstart        macro   modname
                endm

xdefp           macro   xsym
ifndef NDEBUG
                public  xsym
endif
                endm

defp            macro   dsym
         dsym   proc    near
                endm

xrefp           macro   dsym
                endm

endproc         macro   dsym
         dsym   endp
                endm

endmod          macro
_TEXT           ends
                endm

lcall           macro   dest
if _SMALL_CODE
                call    dest
else
                push    cs
                call    near ptr dest
endif
                endm

include struct.inc
include fstatus.inc
include cmpconst.inc
include fpe86.inc
include shiftmac.inc

include normdw.asm
include normqw.asm
include flda086.asm
include fldc086.asm
include fldd086.asm
include fldm086.asm
include ldi4086.asm
include ldi8086.asm
include i4ld086.asm
include i8ld086.asm
include fdld086.asm
include ldfs086.asm
include ldfd086.asm
include fsld086.asm
include e86round.inc
include e86atan.inc
include e86fprem.inc
include e86fxam.inc
include e86log.inc
include sindata.inc
include e86sin.inc
include e86f2xm1.inc
include sqrt086.asm
include e86poly.inc

        endmod
        end
