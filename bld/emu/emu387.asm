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


.386p
        name    emu387

_PLDT = 1 ;     _Phar Lap Development Tools
_NW   = 2 ;     Netware 386
_QNX  = 3 ;     QNX/386

ifdef __WIN387__
        include VMM.Inc

VxD_LOCKED_CODE_SEG

else
_DATA segment dword public 'DATA'
_DATA ends
DGROUP  group   _DATA
        assume  ds:DGROUP

_TEXT   segment dword public 'CODE'
        assume  cs:_TEXT
endif

modstart        macro   modname
                endm

xdefp           macro   xsym
                ifdef _DEBUG
                public  xsym
                endif
                endm

defp            macro   dsym
         dsym   proc    near
                endm

xref            macro   dsym
                endm

endproc         macro   dsym
         dsym   endp
                endm

ifdef __WIN387__
startdata       macro
VxD_LOCKED_DATA_SEG
                endm
enddata         macro
VxD_LOCKED_DATA_ENDS
                endm
endmod          macro
VxD_LOCKED_CODE_ENDS
                endm
else
endmod          macro
_TEXT           ends
                endm
startdata       macro
_DATA   segment dword public 'DATA'
                endm
enddata         macro
_DATA   ends
                endm
endif

include struct.inc
include xception.inc
include fstatus.inc
include fpconsts.inc
include 386fpemu.inc
include 386flda.inc
include 386fldc.inc
include 386fldd.inc
include 386fldm.inc
include 386ldi4.inc
;;include 386ldi8.inc
include 386i4ld.inc
include 386fdld.inc
include 386ldfs.inc
include 386ldfd.inc
include 386fsld.inc
;;include 386i8ld.inc
include 386round.inc
include 386atan.inc
include 386fprem.inc
include 386fxam.inc
include 386log.inc
include 386sind.inc
include 386trig.inc
include 386f2xm1.inc
include 386sqrt.inc
include 386poly.inc

        endmod
        end
