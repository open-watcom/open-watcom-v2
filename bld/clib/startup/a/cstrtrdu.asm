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
;* Description:  Startup code for RDOS, 32-bit usermode application.
;*
;*****************************************************************************

        name    cstrtrdu.asm

.387
.386p

include xinit.inc

        assume  nothing

        extrn   __RdosMain              : near
        extrn   __FiniRtns              : near

        extrn   _edata                  : byte  ; end of DATA (start of BSS)
        extrn   _end                    : byte  ; end of BSS (start of STACK)

        extrn   ___begtext              : near

        assume  nothing

; RDOS gate definitions

get_exception_nr			= 14
set_exception_nr			= 15
unload_exe_nr				= 46
notify_pe_exception_nr		= 70
show_exception_text_nr      = 375

UserGate macro gate_nr
	db 67h
	db 9Ah
	dd gate_nr
	dw 3
	    endm

; exception event status codes

STATUS_BREAKPOINT		        EQU	080000003h
STATUS_SINGLE_STEP		        EQU	080000004h
STATUS_ACCESS_VIOLATION	  	    EQU	0C0000005h
STATUS_NO_MEMORY		        EQU	0C0000017h
STATUS_ILLEGAL_INSTRUCTION      EQU	0C000001Dh
STATUS_NONCONTINUABLE_EXCEPTION	EQU	0C0000025h
STATUS_ARRAY_BOUNDS_EXCEEDED	EQU	0C000008Ch
STATUS_INTEGER_DIVIDE_BY_ZERO	EQU	0C0000094h
STATUS_INTEGER_OVERFLOW		    EQU	0C0000095h
STATUS_PRIVILEGED_INSTRUCTION	EQU	0C0000096h
STATUS_STACK_OVERFLOW		    EQU	0C00000FDh
STATUS_CONTROL_C_EXIT		    EQU	0C000013Ah
STATUS_FLOAT_DENORMAL_OPERAND   EQU 0C000008Dh
STATUS_FLOAT_DIVIDE_BY_ZERO     EQU	0C000008Eh
STATUS_FLOAT_INEXACT_RESULT	    EQU	0C000008Fh
STATUS_FLOAT_INVALID_OPERATION	EQU	0C0000090h
STATUS_FLOAT_OVERFLOW		    EQU	0C0000091h
STATUS_FLOAT_STACK_CHECK	    EQU	0C0000092h
STATUS_FLOAT_UNDERFLOW		    EQU	0C0000093h

; context types

CONTEXT_X86			EQU	10000h
CONTEXT_CONTROL		EQU	1
CONTEXT_INTEGER		EQU	2
CONTEXT_SEGMENTS	EQU	4
CONTEXT_FPU			EQU	8
CONTEXT_DREGS		EQU	10h

OUR_CNT	EQU	CONTEXT_X86+CONTEXT_CONTROL+CONTEXT_INTEGER+CONTEXT_SEGMENTS

; context record

ContextRecord	STRUC
		
ContextFlags	dd ?
DRSpace		dd 6 dup (?)	; not filled
FPUSpace	dd 28 dup (?)	; not filled
CntSegGs	dd ?
CntSegFs	dd ?
CntSegEs	dd ?
CntSegDs	dd ?
CntEdi		dd ?
CntEsi		dd ?
CntEbx		dd ?
CntEdx		dd ?
CntEcx		dd ?
CntEax		dd ?
CntEbp		dd ?
CntEip		dd ?
CntSegCs	dd ?
CntEFlags	dd ?
CntEsp		dd ?
CntSegSs	dd ?

ContextRecord	ENDS

; dispatch record

DispatchRecord	STRUC
pPrevious	dd ?	; DispatchRecord *pPrevious
pEhandler	dd ?	; EHandler *pEhandler
DispatchRecord	ENDS

CONTINUABLE			    EQU	0
NOT_CONTINUABLE			EQU	1
UNWINDING			    EQU	2
UNWINDING_FOR_EXIT		EQU	4
UNWIND_IN_PROGRESS		EQU	6

; exception record

ExceptionRecord	STRUC

ExceptionCode	    dd ?	; Number of Exception
ExceptionFlags	    dd ?
pOuterException	    dd ?	; ExceptionRecord *pOuterException
ExceptionAddress    dd ?
NumParams	        dd ?	; Number of parameters following
ExceptionInfo	    dd ?

ExceptionRecord	ENDS

_DATA   segment use32 public 'DATA'

Cntx            ContextRecord <>
Erec            ExceptionRecord <>
OldExcHandlers  dq 32 DUP (?)
EAddress        dd 0

_DATA   ends

_TEXT   segment use32 word public 'CODE'

        assume  cs:_TEXT
        assume  ds:_DATA
        assume  fs:_DATA
        assume  gs:_DATA
        assume  ss:_DATA

; Init exception handling before starting code. 
; This will not be done by kernel32.dll if there is no references to it
; in executable, which will be the case for OW applications.


RaiseException Proc near
	push ebp
	mov ebp, esp
;
; Build ExceptionRecord on Stack
;
	mov edx, [ebp+20] ; pointer to arguments
	mov ecx, [ebp+16] ; number of arguments
	mov eax, ecx
	jecxz ReNoCpy

ReDoCpy:
	push DWORD PTR [ecx*4-4+edx]
	loop short ReDoCpy

ReNoCpy:
	push eax  ; Num args
	push Cntx.CntEip
	push 0
	push DWORD PTR [ebp+12] ; flags
	push DWORD PTR [ebp+8] ; code

	; traverse handler chain

	mov edx, fs: [ecx]

ReCallHandler:
	cmp edx, -1
	jz ReNoHandler ; should never happen

	mov eax, esp
	push edx
	push edx
	push offset Cntx
	push edx
	push eax
	call pEhandler[edx]
	add esp, 16
	pop edx
	mov edx, pPrevious[edx]
	test eax, eax ; did the handler do something?
	jnz short ReCallHandler

ReNoHandler:
	mov esp, ebp
	pop ebp
	ret 16
RaiseException Endp

CPUExceptionHandler:
	push offset Erec.ExceptionInfo
	push Erec.Numparams
	push NOT_CONTINUABLE
	push Erec.ExceptionCode
	call RaiseException
;
    mov eax,-1
    UserGate unload_exe_nr

ueSs    EQU 36
ueEsp   EQU 32
ueFlags	EQU 28
ueCs	EQU 24
ueEip	EQU 20
ueExtra EQU 16
ueCode	EQU 4

UnwindException:
	push ebp
	mov ebp,esp
	push ds
	push eax
;
	mov eax, cs
	cmp ax, [ebp].ueCs
	jne short ChainDebugger
;
	mov eax,[ebp].ueCode
	cmp eax,1
	jz short ChainDebugger

	cmp eax, 3
	jnz TestDebugger
;
	dec dword ptr [ebp].ueEip
	jmp short ChainDebugger

TestDebugger:
    UserGate show_exception_text_nr
	or eax,eax
	jz ChainDebugger
;
    mov eax,fs:[0]
    cmp eax,-1
    jnz NoDebugger

ChainDebugger:
	mov eax,[ebp].ueCode
	mov eax,[4*eax].ECodeTab
    UserGate notify_pe_exception_nr
	pop eax
	pop ds
	pop ebp
	xchg eax, [esp]
	push dword ptr cs:[8*eax].OldExcHandlers
	mov eax, dword ptr cs:[8*eax+4].OldExcHandlers
	xchg eax, [esp+4]
	retf

NoDebugger:
;
; Save general registers
;
	mov eax, [esp]
	mov Cntx.CntEax, eax
	mov Cntx.CntEbx, ebx
	mov Cntx.CntEcx, ecx
	mov Cntx.CntEdx, edx
	mov Cntx.CntEsi, esi
	mov Cntx.CntEdi, edi
    mov eax, 8[esp]	
	mov Cntx.CntEbp, eax
;
; Save segment registers
;
	mov eax, 4[esp]
	mov Cntx.CntSegDs, eax
	mov Cntx.CntSegEs, es
	mov Cntx.CntSegFs, fs
	mov Cntx.CntSegGs, gs
;
; Get things pushed on stack by the DPMI host
;
	mov eax,[ebp].ueEip
	mov Cntx.CntEip, eax
	mov Erec.ExceptionAddress, eax
	mov eax,[ebp].ueCs
	mov Cntx.CntSegCs, eax
	mov eax,[ebp].ueFlags
	mov Cntx.CntEflags, eax
	mov eax,[ebp].ueEsp
	mov Cntx.CntEsp, eax
	mov eax,[ebp].ueSs
	mov Cntx.CntSegSs, eax
;
; Get exception number
;
	mov eax, [esp+12]

; translate into Win32 exception code

	mov eax, ECodeTab[eax*4]
	mov Erec.ExceptionCode, eax

	mov Erec.NumParams, 1
	mov eax,[ebp].ueExtra
	mov Erec.ExceptionInfo, eax

ExcNocode:
    mov [ebp].ueEip, offset CPUExceptionHandler
    pop eax
    pop ds
    pop ebp
    add esp, 16
	retf


EcodeTab label dword

	dd STATUS_INTEGER_DIVIDE_BY_ZERO
	dd STATUS_SINGLE_STEP
	dd STATUS_NONCONTINUABLE_EXCEPTION
	dd STATUS_BREAKPOINT
	dd STATUS_INTEGER_OVERFLOW
	dd STATUS_ARRAY_BOUNDS_EXCEEDED
	dd STATUS_ILLEGAL_INSTRUCTION
	dd STATUS_NONCONTINUABLE_EXCEPTION
	dd STATUS_NONCONTINUABLE_EXCEPTION
	dd STATUS_NONCONTINUABLE_EXCEPTION 
	dd STATUS_NONCONTINUABLE_EXCEPTION
	dd STATUS_NONCONTINUABLE_EXCEPTION
	dd STATUS_STACK_OVERFLOW
	dd STATUS_PRIVILEGED_INSTRUCTION
	dd STATUS_ACCESS_VIOLATION

BackTrans label dword
	dd STATUS_INTEGER_DIVIDE_BY_ZERO
	dd STATUS_SINGLE_STEP
	dd STATUS_NONCONTINUABLE_EXCEPTION
	dd STATUS_BREAKPOINT
	dd STATUS_INTEGER_OVERFLOW
	dd STATUS_ARRAY_BOUNDS_EXCEEDED
	dd STATUS_ILLEGAL_INSTRUCTION
	dd STATUS_STACK_OVERFLOW
	dd STATUS_PRIVILEGED_INSTRUCTION
	dd STATUS_ACCESS_VIOLATION
	dd STATUS_NO_MEMORY
	dd STATUS_CONTROL_C_EXIT
	dd STATUS_FLOAT_DENORMAL_OPERAND
	dd STATUS_FLOAT_DIVIDE_BY_ZERO
	dd STATUS_FLOAT_INEXACT_RESULT
	dd STATUS_FLOAT_INVALID_OPERATION
	dd STATUS_FLOAT_OVERFLOW
	dd STATUS_FLOAT_STACK_CHECK
	dd STATUS_FLOAT_UNDERFLOW
	dd 0

BackString label near
	dd offset ExcStr0
	dd offset ExcStr1
	dd offset ExcStr2
	dd offset ExcStr3
	dd offset ExcStr4
	dd offset ExcStr5
	dd offset ExcStr6
	dd offset ExcStr7
	dd offset ExcStr8
	dd offset ExcStr9
	dd offset ExcStr10
	dd offset ExcStr11
	dd offset ExcStr12
	dd offset ExcStr13
	dd offset ExcStr14
	dd offset ExcStr15
	dd offset ExcStr16
	dd offset ExcStr17
	dd offset ExcStr18
	dd offset ExcStrUnavail

ExcStr0 db 'Integer divide by zero',0
ExcStr1 db 'Single step',0
ExcStr2 db 'Noncontiunuable exception',0
ExcStr3 db 'Breakpoint',0
ExcStr4 db 'Integer overflow',0
ExcStr5 db 'Array bounds exceeded',0
ExcStr6 db 'Illegal instruction',0
ExcStr7 db 'Stack overflow',0
ExcStr8 db 'Privileged instruction',0
ExcStr9 db 'Access violation',0
ExcStr10 db 'No memory',0
ExcStr11 db 'Control C exit',0
ExcStr12 db 'Float denormal operand',0
ExcStr13 db 'Float divide by zero',0
ExcStr14 db 'Float inexact result',0
ExcStr15 db 'Float invalid operation',0
ExcStr16 db 'Float overflow',0
ExcStr17 db 'Float stack check',0
ExcStr18 db 'Float underflow',0
ExcStrUnavail db '  N/A',0

Exc00:
	push 0
	jmp short UnwindException

Exc01:
	push 1
	jmp short UnwindException

Exc02:
	push 2
	jmp short UnwindException

Exc03:
	push 3
	jmp short UnwindException

Exc04:
	push 4
	jmp short UnwindException

Exc05:
	push 5
	jmp short UnwindException

Exc06:
	push 6
	jmp short UnwindException

Exc07:
	push 7
	jmp short UnwindException

Exc08:
	push 8
	jmp short UnwindException

Exc09:
	push 9
	jmp short UnwindException

Exc0A:
	push 0Ah
	jmp short UnwindException

Exc0B:
	push 0Bh
	jmp short UnwindException

Exc0C:
	push 0Ch
	jmp short UnwindException

Exc0D:
	push 0Dh
	jmp short UnwindException

Exc0E:
	push 0Eh
	jmp short UnwindException

Exc0F:
	push 0Fh
	jmp short UnwindException

Exc10:
	push 10h
	jmp short UnwindException

Exc11:
	push 11h
	jmp short UnwindException

Exc12:
	push 12h
	jmp short UnwindException

Exc13:
	push 13h
	jmp short UnwindException

Exc14:
	push 14h
	jmp short UnwindException

Exc15:
	push 15h
	jmp short UnwindException

Exc16:
	push 16h
	jmp short UnwindException

Exc17:
	push 17h
	jmp short UnwindException

Exc18:
	push 18h
	jmp short UnwindException

Exc19:
	push 19h
	jmp short UnwindException

Exc1A:
	push 1Ah
	jmp short UnwindException

Exc1B:
	push 1Bh
	jmp short UnwindException

Exc1C:
	push 1Ch
	jmp short UnwindException

Exc1D:
	push 1Dh
	jmp short UnwindException

Exc1E:
	push 1Eh
	jmp short UnwindException

Exc1F:
	push 1Fh
	jmp short UnwindException

InitException   proc
	push es
	mov edx, OFFSET OldExcHandlers
	xor al,al

GetOldExc:
    UserGate get_exception_nr
	mov [edx], edi
	mov [edx+4], es
	add edx, 8
	inc al
	cmp al, 01fh
	jna GetOldExc
;
	mov ax,cs
	mov es,ax
	xor al,al
	mov edi,OFFSET Exc00

SetNewExc:
    UserGate set_exception_nr
	inc al
	add edi,OFFSET Exc01 - OFFSET Exc00
	cmp al, 01Fh
	jna SetNewExc
;
	pop es
	ret
InitException Endp


dstart:
    call InitException
    int 3
    jmp short _cstartd

;
; This is for debugging. This code must reside immediately before _cstart_
;

    dd OFFSET dstart
         
public _cstart_

_cstart_ proc  far
    call InitException

_cstartd:
    sub ebp,ebp                 ; ebp=0 indicate end of ebp chain
    call __RdosMain
    dd ___begtext              ; make sure dead code elimination
;
; copyright message
;
include msgrt32.inc
include msgcpyrt.inc

_cstart_ endp

_TEXT   ends

        end     _cstart_
