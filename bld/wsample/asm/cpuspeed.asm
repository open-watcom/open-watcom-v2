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

		name	getcpuspeed
_TEXT segment use32 dword public 'CODE'
		assume	cs:_TEXT

;
;	GetCurrentTime is specific to NetWare
;

		extrn	GetCurrentTime : near

; ----------------------------------------------------------------------------
		public	cpuspeed
;
;	unsigned long cpuspeed(void)
;

		cpuspeed proc near

		push			ebx
$00:
		pushfd
		sti

		call			GetCurrentTime
		mov				ebx, eax
$10:
		call			GetCurrentTime
		cmp				eax, ebx
		je				$10

		call			GetCurrentTime
		mov				ebx, eax
$20:
		call			GetCurrentTime
		cmp				eax, ebx
		je				$20
		
		call			GetCurrentTime
		mov				ebx, eax
		rdtsc

		mov				ecx, eax
$30:
		call			GetCurrentTime
		cmp				eax, ebx
		je				$30

		rdtsc

		popfd			; restore interrupt state

		sub				eax, ecx
		jb				$00	; do it again if negative

		;
		;	eax now holds the cycle count difference over one 
		;	tick (approx 55mS). this code may fail when CPUs
		;	pass ~39GHz ;-)
		;	This is because the count difference will exceed
		;	2^31 on processors of this speed and above
		;

		xor				edx, edx
		mov				ebx, 0000471FH		;	18207
		mul				ebx
		add				eax, 1DCD6500H		;	5x10^8
		adc				edx, 0
		mov				ebx, 3B9ACA00H		;	1x10^9
		div				ebx

		pop				ebx
		ret

		cpuspeed endp

; ----------------------------------------------------------------------------

_TEXT	ends

		end

