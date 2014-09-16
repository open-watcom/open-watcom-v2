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
;* Description:	NetWare specific code to calculate the speed of a Pentium+
;*				class processor.	
;*
;*****************************************************************************

.586
.model small

		name	pitstuff
_TEXT segment use32 dword public 'CODE'
		assume	cs:_TEXT


; ----------------------------------------------------------------------------

		public count_pit0
;
;	unsigned long count_pit0(void)
;
		count_pit0 proc near

		; clear interrupts to try and be more accurate
		pushfd
		cli

$a0:
		; issue a timer 0 latch command
		xor al, al
		out 43H, al
		in al, 40H
		mov	dl, al
		in al, 40H
		mov dh, al
		movzx	eax, dx
		cmp eax, 1
		jne $a0
$a10:
		xor al, al
		out 43H, al
		in al, 40H
		mov	dl, al
		in al, 40H
		mov dh, al
		movzx	eax, dx
		cmp eax, 1
		je $a10

		popfd
		ret

		count_pit0 endp

; ----------------------------------------------------------------------------

_TEXT	ends

		end

