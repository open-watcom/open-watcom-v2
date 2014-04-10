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
;* Description:  RDOS helper routines for TLS. 
;*
;*****************************************************************************


.387
.386p
                PUBLIC  __tls_index
                PUBLIC  __tls_start
                PUBLIC  __tls_alloc
                PUBLIC  __tls_free
                PUBLIC  __tls_set_value
                PUBLIC  __tls_get_value
                PUBLIC  __create_thread

; Offsets within FS

__pv_arbitrary = 14h
__tls_bitmap = 28h
__tls_array = 2Ch

; RDOS gate definitions

wait_milli_nr	            = 25
create_thread_nr            = 28
terminate_thread_nr         = 29

UserGate macro gate_nr
	db 67h
	db 9Ah
	dd gate_nr
	dw 3
	    endm

DGROUP          GROUP   CONST,CONST2,_DATA,_BSS

_TEXT           SEGMENT BYTE PUBLIC USE32 'CODE'
                ASSUME CS:_TEXT, DS:DGROUP, SS:DGROUP

; OUT: EAX = TLS index

__tls_alloc:
	mov ecx,fs:__tls_bitmap
	bsf eax, dword ptr [ecx]
	jnz __tls_alloc_ok

	bsf eax, dword ptr [ecx+4]
	lea eax, [eax+32]
	jnz __tls_alloc_ok

	mov eax,-1
	ret

__tls_alloc_ok:
	btr dword ptr [ecx], eax
	ret

; IN:  ECX = TLS index

__tls_free:
	cmp ecx, 64
	sbb eax, eax
	jnc __tls_free_ok

	mov eax,fs:__tls_bitmap
	bts dword ptr [eax],ecx

__tls_free_ok:
	ret

; IN:  ECX = TLS index
; OUT: EAX = Value

__tls_get_value:
	xor eax, eax
	cmp ecx, 64
	jnc __tls_get_done

	mov edx, fs:__tls_array
	mov eax, [edx + ecx * 4]

__tls_get_done:
    ret

; IN:  ECX = TLS index
; IN:  EAX = Value

__tls_set_value:
	cmp ecx, 64
	jnc __tls_set_done

	mov edx, fs:__tls_array
	mov [edx + ecx * 4], eax

__tls_set_done:
    ret

__task_end:
    UserGate terminate_thread_nr   ; won't return

__task_start:
	mov ax,ds
	mov es,ax
	mov esi,fs:__pv_arbitrary
	push OFFSET __task_end
	push edx
	ret

; IN: EDX   Task callback
; IN: EDI   Task name
; IN: EAX   Task data
; IN: EBX   Task prio
; IN: ECX   Stack size

__create_thread:
	push ds
	pushad
;
    push ebx
	mov bx,cs
	mov ds,bx
	mov esi,OFFSET __task_start
	mov fs:__pv_arbitrary,eax
	mov bx,fs
	pop eax
	UserGate create_thread_nr
;
	mov eax,10
	UserGate wait_milli_nr
;
	popad
	pop ds
	ret

_TEXT           ENDS

CONST           SEGMENT DWORD PUBLIC USE32 'DATA'
CONST           ENDS

CONST2          SEGMENT DWORD PUBLIC USE32 'DATA'
CONST2          ENDS

_DATA           SEGMENT DWORD PUBLIC USE32 'DATA'
                DD      0
                DD      0
                DD      0

_DATA           ENDS

_BSS            SEGMENT DWORD PUBLIC USE32 'BSS'
_BSS            ENDS

                END
