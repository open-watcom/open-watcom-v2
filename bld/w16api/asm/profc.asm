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
;* Description:  Interface to the Windows profiling API
;*               
;*****************************************************************************

.387

PROF_FN macro   func
        mov     ax,4500h+func
        int     2fh
        endm

PubProc macro	x
PUBLIC	x
x	proc	far
	endm

EndProc macro	x, p
ifb <p>
	ret
else
	ret	p
endif
x	endp
	endm

DGROUP	GROUP	_DATA

_TEXT	SEGMENT	BYTE PUBLIC USE16 'CODE'
	ASSUME CS:_TEXT, DS:DGROUP, SS:DGROUP

PubProc PROFINSCHK
        PROF_FN 0
        xor     ah,ah
EndProc PROFINSCHK

PubProc PROFSETUP
        push    bp
        mov     bp,sp
        mov     bx,8H[bp]
        mov     cx,6H[bp]
        PROF_FN 1
        pop     bp
EndProc PROFSETUP, 4

PubProc PROFSAMPRATE
        push    bp
        mov     bp,sp
        mov     bx,8H[bp]
        mov     cx,6H[bp]
        PROF_FN 2
        pop     bp
EndProc PROFSAMPRATE, 4

PubProc PROFSTART
        PROF_FN 3
EndProc PROFSTART

PubProc PROFSTOP
        PROF_FN 4
EndProc PROFSTOP

PubProc PROFCLEAR
        PROF_FN 5
EndProc PROFCLEAR

PubProc PROFFLUSH
        PROF_FN 6
EndProc PROFFLUSH

PubProc PROFFINISH
        PROF_FN 7
EndProc PROFFINISH

_TEXT	ENDS

_DATA	SEGMENT	WORD PUBLIC USE16 'DATA'
_DATA	ENDS

        end
