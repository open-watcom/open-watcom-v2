;
; Copyright (C) 1996-2006 by Narech K. All rights reserved.
;
; Redistribution  and  use  in source and  binary  forms, with or without
; modification,  are permitted provided that the following conditions are
; met:
;
; 1.  Redistributions  of  source code  must  retain  the above copyright
; notice, this list of conditions and the following disclaimer.
;
; 2.  Redistributions  in binary form  must reproduce the above copyright
; notice,  this  list of conditions and  the  following disclaimer in the
; documentation and/or other materials provided with the distribution.
;
; 3. The end-user documentation included with the redistribution, if any,
; must include the following acknowledgment:
;
; "This product uses DOS/32 Advanced DOS Extender technology."
;
; Alternately,  this acknowledgment may appear in the software itself, if
; and wherever such third-party acknowledgments normally appear.
;
; 4.  Products derived from this software  may not be called "DOS/32A" or
; "DOS/32 Advanced".
;
; THIS  SOFTWARE AND DOCUMENTATION IS PROVIDED  "AS IS" AND ANY EXPRESSED
; OR  IMPLIED  WARRANTIES,  INCLUDING, BUT  NOT  LIMITED  TO, THE IMPLIED
; WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED.  IN  NO  EVENT SHALL THE  AUTHORS  OR  COPYRIGHT HOLDERS BE
; LIABLE  FOR  ANY DIRECT, INDIRECT,  INCIDENTAL,  SPECIAL, EXEMPLARY, OR
; CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF
; SUBSTITUTE  GOODS  OR  SERVICES;  LOSS OF  USE,  DATA,  OR  PROFITS; OR
; BUSINESS  INTERRUPTION) HOWEVER CAUSED AND  ON ANY THEORY OF LIABILITY,
; WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
; OTHERWISE)  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
; ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
;

BUILDING_KERNEL = 1
BUILDING_CLIENT = 0

;*****************************************************************************
; DOS/32 Advanced DOS Extender master Kernel file, includes kernel definitions
; data structures, and files from .\TEXT\KERNEL\ implementing the kernel.
;
;	Created on:	Oct-30-1996
;
;*****************************************************************************

		.386p
		.387
		LOCALS
		NOJUMPS

SELCODE		= 08h			; KERNEL code selector
SELDATA		= 10h			; KERNEL data selector
SELZERO		= 18h			; ZERO data selector
SELVCPITSS	= 20h			; TSS selector for VCPI
SELVCPICODE	= 28h			; VCPI call code selector
SELVCPICOD2	= 30h			; VCPI internal selector #2
SELVCPICOD3	= 38h			; VCPI internal selector #3
SELBIOSDATA	= 40h			; BIOS DATA Area 40h selector
SELCALLBACK	= 48h			; Callback DS selector

SYSSELECTORS	= 10			; number of system selectors in GDT

INCLUDE	TEXT\include.asm
PUBLIC	pm32_info
PUBLIC	pm32_init
PUBLIC	pm32_data
PUBLIC	@kernel_beg
PUBLIC	@kernel_end


_KERNEL	segment para public use16 'CODE1'
	assume	cs:_KERNEL, ds:_KERNEL
;=============================================================================
pm32_data	label byte	; data area 12 bytes
pm32_mode	db	-1	; mode bits:
				;  bit0: 0=test DPMI/VCPI, 1=VCPI/DPMI
				;  bit1: 0=exception control off, 1=on
				;  bit2: 0=VCPI smart alloc pages off, 1=on
				;  bit3: 0=VCPI+XMS alloc sheme off, 1=on
				;  bit4: 0=trap software INTs off, 1=on
				;  bit5: 0=check ext blocks off, 1=on
				;  bit6: 0=invalid limit check off, 1=on
				;  bit7: reserved
pm32_maxpages	db	-1	; maximum number of VCPI page tables
pm32_maxfpages	db	-1	; max. of physical mem. map pages
pm32_callbacks	db	-1	; number of real mode callbacks
pm32_selectors	dw	-1	; max selectors under VCPI/XMS/raw
pm32_rmstacks	db	-1	; real mode stack nesting
pm32_pmstacks	db	-1	; protected mode stack nesting
pm32_rmstacklen	dw	-1	; real mode stack length, in para
pm32_pmstacklen	dw	-1	; protected mode stack length, in para
pm32_maxextmem	dd	-1	; maximum extended memory to allocate



;=============================================================================
; GLOBAL DATA:
;=============================================================================

;=============================================================================
;*** MEMORY DATA ***
mem_ptr		equ @area1_dd+00h	;dd 0; pointer to next free mem block
mem_free	equ @area1_dd+04h	;dd 0; size of largest free mem block
mem_top		equ @area1_dd+08h	;dd 0
mem_used	equ @area1_dw+0Ch	;dw 0
segmentbases	equ @area1_dw+0Eh	;dw 16*2 dup(0); for function 0002h


;=============================================================================
;*** Interrupt Redirection DATA ***
temp_int	equ @area1_db+4Fh	;db 0
irqset_rm	equ @area1_dw+50h	;dw 0		; installed Real Mode IRQ Vectors
irqset_pm	equ @area1_dw+52h	;dw 0		; installed Prot. Mode IRQ Vectors
irqtab_rm	equ @area1_dd+56h	;dd 16 dup(0)	; 16 Real Mode IRQ Vectors
irqtab_pm	equ @area1_dd+96h	;dd 32 dup(0)	; 16 Prot. Mode IRQ Vectors
exctab_pm	equ @area1_dd+116h	;dd 32 dup(0)	; 16 Exception Vectors


;=============================================================================
;*** Misc. ***
tempw0		equ @area1_dw+198h	;dw 0
tempw1		equ @area1_dw+19Ah	;dw 0
tempd1		equ @area1_dd+19Ch	;label dword
tempw2		equ @area1_dw+19Ch	;dw 0
tempw3		equ @area1_dw+19Eh	;dw 0

oldcr0		equ @area1_dd+1A0h	;dd 0	; preserved CR0
oldint15h	equ @area1_dd+1A4h	;dd 0	; preserved INT 15h vector
oldint1Bh	equ @area1_dd+1A8h	;dd 0
oldint1Ch	equ @area1_dd+1ACh	;dd 0
oldint21h	equ @area1_dd+1B0h	;dd 0
oldint23h	equ @area1_dd+1B4h	;dd 0
oldint24h	equ @area1_dd+1B8h	;dd 0
oldint2Fh	equ @area1_dd+1BCh	;dd 0
newint1Bh	equ @area1_dd+1C0h	;dd 0
newint1Ch	equ @area1_dd+1C4h	;dd 0
newint23h	equ @area1_dd+1C8h	;dd 0
newint24h	equ @area1_dd+1CCh	;dd 0
id32_mem_free	equ @area1_dd+1D0h
id32_mem_ptr	equ @area1_dd+1D4h
id32_mem_vcpi	equ @area1_dd+1D8h
id32_process_id	equ @area1_dw+1DCh	; current process id when spawn
id32_spawned_id	equ @area1_db+1E0h	; spawn mode flag (1=spawned)
id32_tsrmode_id	equ @area1_db+1E2h	; TSR mode flag (1=TSR)
pagetablebase	equ @area1_dd+1E4h	;dd 0; base of page table area
pagetabletop	equ @area1_dd+1E8h	;dd 0; top of page table area
pagetablefree	equ @area1_dd+1ECh	;dd 0; base of available pagetab area
phystablebase	equ @area1_dd+1F0h	;dd 0
phystabletop	equ @area1_dd+1F4h	;dd 0

;=============================================================================
; *** Performance Counters ***
_pc_base	equ @area1_db+200h
_pc_intrmtopm	equ @area1_dd+200h	;dd 0; INT RM->PM switch counter
_pc_intpmtorm	equ @area1_dd+204h	;dd 0; INT PM->RM switch counter
_pc_irqrmtopm	equ @area1_dd+208h	;dd 0; IRQ RM->PM switch counter
_pc_irqpmtorm	equ @area1_dd+20Ch	;dd 0; IRQ PM->RM switch counter
_pc_irqcbrmtopm	equ @area1_dd+210h	;dd 0; IRQ Callback RM->PM counter
_pc_irqcbpmtorm	equ @area1_dd+214h	;dd 0; IRQ Callback PM->RM counter
_pc_cbrmtopm	equ @area1_dd+218h	;dd 0; Callback RM->PM counter
_pc_cbpmtorm	equ @area1_dd+21Ch	;dd 0; Callback PM->RM counter
pmstacklen	equ @area1_dd+220h	;dd 0; prot mode stack length in bytes
pmstackbase	equ @area1_dd+224h	;dd 0; bottom of prot mode stack area
pmstacktop	equ @area1_dd+228h	;dd 0; top of prot mode stack area
pmstacktop2	equ @area1_dd+22Ch	;dd 0
rmstacklen	equ @area1_dw+230h	;dw 0; real mode stack size in para
rmstackbase	equ @area1_dw+232h	;dw 0; bottom of real mode stack area
rmstacktop	equ @area1_dw+234h	;dw 0; top of real mode stack area
rmstacktop2	equ @area1_dw+236h	;dw 0
rmstackesp	equ @area1_dd+240h	;dd 0; for functions 0300h,0301h,0302h
rmstackss	equ @area1_dw+244h	;dw 0
callbackbase	equ @area1_dd+248h	;dd 0; base of real mode callbacks
callbackseg	equ @area1_dw+24Ch	;dw 0; segment of callbacks
irqcallbackptr	equ @area1_dw+250h	;dw 0; ptr to IRQ callback ESP buffer

		evendata
client_call	dw	0,0		; client's critical handler offset
client_version	dw	0		; extender version
kernel_code	dw	0		; kernel CS: segment
cputype		db	0		; CPU type
fputype		db	0		; FPU type
pmodetype	db	0		; protected mode type
pagetables	db	0		; number of page tables under VCPI
picslave	db	0		; PIC slave base interrupt
picmaster	db	0		; PIC master base interrupt
__reserved_1	db	0		; reserved
A20_state	db	0		; old A20 gate state
cpuidlvl	dd	0		; CPUID level

		evendata
codebase	dd	0		; _KERNEL linear address
dpmiepmode	dd	0		; DPMI enter pmode addx

selzero		dw	SELZERO		; for immediate segreg loading
seldata		dw	SELDATA		; for immediate segreg loading
selcallback	dw	SELCALLBACK	; for immediate segreg loading
gdtseg		dw	0		; segment of GDT
gdtlimit	dw	0		; GDT limit
gdtbase		dd	0		; GDT base
idtseg		dw	0		; segment of IDT
idtlimit	dw	7FFh		; IDT limit
idtbase		dd	0		; IDT base
rmidtlimit	dw	3FFh		; real mode IDT limit
rmidtbase	dd	0		; real mode IDT base
rmtopmswrout	dw	offs v_rmtopmsw	; addx of real to protected routine
pmtormswrout	dd	offs v_pmtormsw	; addx of protected to real routine


;=============================================================================
;*** XMS DATA ***
		evendata
xms_call	dw	0,0		; XMS driver offset, segment
xms_data	dd	0		; XMS 3.0 available memory
xms_handle	dw	0		; memory handle


;=============================================================================
;*** VCPI DATA ***
		evendata		; VCPI structure, DO NOT MODIFY
vcpi_cr3	dd	0		; VCPI CR3 value for protected mode
vcpi_gdtaddx	dd	offs gdtlimit	; linear addx of GDT limit and base
vcpi_idtaddx	dd	offs idtlimit	; linear addx of IDT limit and base
vcpi_selldt	dw	0		; LDT selector for protected mode
vcpi_seltss	dw	SELVCPITSS	; TSS selector for protected mode
vcpi_eip	dd	offs v_rmtopmswpm; destination EIP in protected mode
vcpi_cs		dw	SELCODE		; destination CS in protected mode

vcpi_calleip	dd	0		; VCPI protected mode call offset
vcpi_callcs	dw	SELVCPICODE	; VCPI protected mode call selector
vcpi_allocmem	dw	0		; VCPI number of allocated pages
vcpistrucaddx	dd	offs vcpi_cr3	; VCPI switch structure linear address
vcpiswitchstack	dd	0		; VCPI temporary mode switch stack


;=============================================================================
;*** DPMI DATA ***
		evendata
int31h_cache	label word
	dw	0EEFFh			; last DPMI function #
	dw	int31h_EEFF		; last DPMI function target addr





;=============================================================================
; PROTECTED MODE INIT/EXIT CODE

include	TEXT\KERNEL\detect.asm
include	TEXT\KERNEL\init.asm
include	TEXT\KERNEL\exit.asm
include	TEXT\KERNEL\misc.asm


;=============================================================================
; PROTECTED MODE KERNEL CODE

include	TEXT\KERNEL\mode.asm
include	TEXT\KERNEL\intr.asm
include	TEXT\KERNEL\int31h.asm


;=============================================================================
; BETA test code

If EXEC_TYPE eq 2
include	TEXT\testbeta.asm
Endif

	Align 16
@kernel_end	label byte

_KERNEL      ends
end
