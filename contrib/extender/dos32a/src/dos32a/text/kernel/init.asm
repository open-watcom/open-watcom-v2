;
; Copyright (C) 1996-2002 Supernar Systems, Ltd. All rights reserved.
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

;=============================================================================
; Initialize Protected Mode
;===========================
; In:	BX = client version
;	DX = offset of Client's Safe Exit Routine (used by exception handler)
;	ES = real mode segment for protected mode data (ignored if not needed)
;
; Out:	AX = return code:
;	 0005h = DPMI - could not enter 32bit protected mode
;	 0006h = DPMI - could not allocate needed selectors
;	 0007h = could not enable A20 gate
;	BX = kernel code selector
;	ECX = amount of allocated memory (in bytes)
;	EDX = pointer to allocated memory (from 0)
;	SI = current process ID (when spawned)
;	DI = version of the previously installed DOS/32A (0 if none)
;	CF = set on error, if no error:
;	 ESP = high word clear
;	 CS = 16bit selector for real mode CS with limit of 64k
;	 SS = selector for real mode SS with limit of 64k
;	 DS = selector for real mode DS with limit of 64k
;	 ES = selector for PSP with limit of 100h
;	 FS = 0 (NULL selector)
;	 GS = 0 (NULL selector)
;=============================================================================

pm32_init:
	cld
	pushad
	push ds
	push cs				; DS = _KERNEL
	pop ds

	xor eax,eax
	mov kernel_code,cs
	mov client_version,bx
	mov client_call[0],dx
	mov ax,cs			; set base addx of _KERNEL
	shl eax,4
	mov codebase,eax
	add vcpi_gdtaddx,eax		; adjust addresses for VCPI structure
	add vcpi_idtaddx,eax
	add vcpistrucaddx,eax
	btr pm32_maxextmem,31		; limit extended memory to 2GB

	push es
	push cs
	pop es
	mov di,offs @area1_db
	mov cx,(offs @area1_end - offs @area1_db) /2
	xor ax,ax
	rep stosw
	pop es

	mov bp,sp
	mov [bp+02h],ax			; set DI to 0 (prev D32A version)
	mov ax,0FF88h			; detect if DOS/32A is present
	int 21h
	cmp eax,'ID32'			; check if we were spawned
	jnz @@1				; if not, jump
	mov [bp+02h],bx			; set DI to prev. version of D32A
	cmp bx,client_version		; check versions
	jnz @@1				; if not equal, jump
	mov id32_mem_free,ecx
	mov id32_mem_ptr,edx
	mov id32_mem_vcpi,edi
	shr esi,16			; get previous process id
	inc si				; increment (make it this process id)
	mov id32_process_id,si		; store the new, current process id
	mov pm32_maxextmem,0		; use already allocated memory

@@1:	movzx bx,pmodetype		; jump to appropriate init code
	add bx,bx
	jmp @@init[bx]

@@init	dw r_init
	dw x_init
	dw v_init
	dw d_init


;=============================================================================
dvxr_init:				; DPMI/VCPI/XMS/raw common init tail
	xor ax,ax			; allocate selector for return code
	mov cx,1
	int 31h
	jnc @@0
@@err:	mov ax,4CFFh			; could not allocate selector
	int 21h				; terminate immediately

@@0:	mov bp,sp
	mov bx,ax			; new code descriptor for return
	mov ax,0007h			; set base address of calling segment
	mov dx,[bp+36]
	mov cx,dx
	shl dx,4
	shr cx,12
	int 31h
	jc @@err

	inc ax				; set selector limit to 64k
	xor cx,cx
	mov dx,0FFFFh
	int 31h
	jc @@err

	inc ax				; set selector type and access rights
	mov dx,cs			; get DPL from current CPL, and access
	lar cx,dx			;  rights and type from current CS
	shr cx,8			; type is already 16bit code segment
	int 31h
	jc @@err

	mov [bp+36],bx			; store selector in return address

	cmp cs:pmodetype,3		; if DPMI, no need in client_addx
	jz @@1				;  DPMI will handle all the exceptions
	push ds				; store client's code selector
	mov ds,cs:seldata
	mov client_call[2],bx
	pop ds

@@1:	xor bx,bx			; init successful, carry clear

;-----------------------------------------------------------------------------
init_done:				; return with return code
	mov [bp+1Eh],bx			; set AX=return code
	jc @@1
	mov eax,cs:mem_free
	mov edx,cs:mem_ptr
	mov [bp+1Ah],eax		; set ECX=amount of allocated memory
	mov [bp+16h],edx		; set EDX=pointer to allocated memory
	or eax,edx			; if allocated some memory, exit
	jnz @@1
	call init_done_id32
@@1:	pop ds
	popad
	mov bx,cs			; return BX=kernel code selector
	mov si,cs:id32_process_id	; return SI=current process ID
	cld
	retf

init_done_id32:
	cmp cs:pmodetype,3		; if under DPMI, exit
	jz @@done
	mov ds,cs:seldata
	cmp id32_process_id,0		; if this is the first process (mom)
	jz @@done			; then we're done

	mov eax,id32_mem_free
	mov edx,id32_mem_ptr
	mov [bp+1Ah],eax		; set ECX=amount of allocated memory
	mov [bp+16h],edx		; set EDX=pointer to allocated memory
	mov mem_free,eax
	mov mem_ptr,edx
	lea ecx,[eax+10h]		; size of memory + 16 bytes
	lea eax,[ecx+edx]		; top of memory
	mov mem_top,eax

	mov al,pmodetype		; now check if running under VCPI
	cmp al,0
	jz @@done
	cmp al,1
	jz @@done
	push es
	mov esi,id32_mem_vcpi		; import pagetables from prev process
	mov edi,pagetablefree
	mov es,selzero
	mov ds,selzero
	shr ecx,12
	rep movs dword ptr es:[edi],[esi]
	pop es
@@done:	ret





;=============================================================================
;******	NOTE:	DPMI host will provide all INT 31h functions and
;		will be responsible for any memory allocation.

d_init:	pop ds				; get original caller DS from stack
	mov ax,1			; enter DPMI protected mode
	call cs:dpmiepmode
	push ds				; put DS back onto stack
	jnc dvxr_init			; error? if not, go on with init
	mov bx,6			; error entering protected mode, set
	cmp ax,8011h			;  error code and abort
	stc
	jz init_done
	dec bx				; error code 5, not 6
	jmp init_done





	Align 16
@kernel_beg	label byte		;** Begin of kernel code
@callback_data	label byte		;** CALLBACK DATA Structure

;=============================================================================
v_init:	xor eax,eax
	mov ax,es			; align data area on page boundary
	add ax,00FFh
	xor al,al
	mov es,ax
	mov dx,ax			; set base and top of page table area

	shl eax,4
	add eax,1000h			; skip Page Directory
	mov pagetablebase,eax		; 0th PageTable linear address
	add eax,1000h			; skip 0th PageTable
	movzx ecx,pagetables
	shl ecx,12
	add eax,ecx
	mov pagetabletop,eax		; Nth PageTable linear adress
	mov phystablebase,eax		; set base and top of physical pages
	movzx ecx,pm32_maxfpages
	shl ecx,12
	add eax,ecx
	mov phystabletop,eax

	xor di,di
	xor eax,eax			; clear PageDir and 0th PageTable
	mov cx,0800h
	rep stos dword ptr es:[di]

	mov gs,dx                       ; GS = segment of Page Directory
	mov ax,dx
	add ax,0100h
	mov es,ax                       ; ES = segment of 0th Page Table
	mov fs,ax                       ; FS = segment of 0th Page Table

	sub sp,8*3			;***NOTE: stack will be restored later
	mov si,sp			; DS:SI = ptr to VCPI structure
	xor di,di			; ES:DI = ptr to imported 0th PageTab
	push ds
	push ss
	pop ds
	mov ax,0DE01h			; get VCPI protected mode interface
	int 67h
	pop ds

	push di				;** store DI = ptr to free entry
	mov vcpi_calleip,ebx		; store protected mode VCPI call EIP
@@1:	and byte ptr es:[di+1],0F1h     ; clear bits 9-11 in imported PageTab
	sub di,4
	jnc @@1

	mov cx,dx			; get physical address of PageDir
	shr cx,8
	mov ax,0DE06h
	int 67h
	and dx,0F000h
	mov vcpi_cr3,edx		; set VCPI CR3 register

;
; Map VCPI 0th PageTable into our PageDir
;
	mov cx,es			; CX = 0th PageTable addr
	shr cx,8			; convert to PageTable number
	mov ax,0DE06h			; VCPI get pagetable Physical Addr
	int 67h
	and dh,0F0h
	mov dl,07h
	mov gs:[0000h],edx		; store address in PageDir

;
; Map the rest of our custom PageTables into our PageDir
;
	mov ax,es			; advance to the 1st PageTable
	add ax,0100h
	mov es,ax

	mov si,ax			; SI = segment of 1st PageTable
	mov al,pagetables		; AL = PageTables to alloc
	mov ebx,1			; EBX = index ptr into PageDir
	call vcpi_setup_pagetables	; initialize VCPI PageDir and PageTabs

;
; Map PageTables for phys. mapping into PageDir
;
	mov ax,gs			; advance PageDir to 8000_0000h addr
	add ax,80h
	mov gs,ax

	mov al,pm32_maxfpages		; phystables to allocate
	xor ebx,ebx			; EBX = index ptr into PageDir
	call vcpi_setup_pagetables	; initialize VCPI PageDir and PageTabs
	pop di				;** restore DI = ptr to free entry

	xor eax,eax
	test pm32_mode,00000100b	; if smart pagetables is off
	jnz @@2				;  then align pagetablefree to
	mov di,1000h			;  first allocated free pagetable
@@2:	mov ax,di			; set base of usable page table area
	add eax,pagetablebase
	mov pagetablefree,eax

	push si es
	call vcpi_alloc_ems		; allocate VCPI memory
	pop es si

	push si				; SI = segment of TSS (for later use)
	xor di,di			; clear TSS with all 0, not really
	mov cx,34h			;  needed, but just to be safe
	xor ax,ax
	rep stos word ptr es:[di]
	mov eax,vcpi_cr3		; set CR3 in TSS
	mov es:[1Ch],eax
	mov dword ptr es:[64h],680000h	; set offset of I/O permission bitmap
					;  and clear T bit
	add si,7			; increment next data area ptr
	mov es,si
	mov rmtopmswrout,offs v_rmtopmsw	; set VCPI mode switch addresses
	mov pmtormswrout,offs v_pmtormsw
	jmp vxr_init			; go to VCPI/XMS/raw continue init


vcpi_setup_pagetables:
	push bp
	movzx bp,al			; BP = PageTables to alloc
	test bp,bp
	jz @@done
@@1:	mov cx,si			; CX = PageTable segment
	shr cx,8			; convert to PageTable number
	mov ax,0DE06h			; VCPI get PageTable Physical Addr
	int 67h
	and dh,0F0h
	mov dl,07h
	mov gs:[ebx*4],edx		; store address in PageDir
	add si,0100h			; increment pagetable segment (4K)
	mov es,si
	xor di,di			; clear PageTable
	xor eax,eax
	mov cx,0400h
	rep stos dword ptr es:[di]
	inc bx				; increment index in PageDir
	dec bp				; decrement PageTable counter
	jnz @@1				; if no, loop
@@done:	pop bp
	ret

vcpi_alloc_ems:
	push fs
	pop es				; ES:DI ptr to free entry in 0th page
	movzx eax,pagetables		; calculate free linear space
	shl eax,22			; convert PageTables to bytes (*4M)
	mov ecx,1000h
	sub cx,di			; ECX = unused space in 0th PageTable
	and cl,0FCh
	shl ecx,10			; convert 4K pages to bytes
	add eax,ecx			; EAX = available linear space (bytes)
	mov ecx,pm32_maxextmem		; ECX = memory to allocate (bytes)
	cmp ecx,eax			; choose smallest value
	jbe @@0
	mov ecx,eax
@@0:	xor ebx,ebx			; EBX = counter of allocated pages
	jecxz @@done			; if no memory to be allocated, done

@@1:	cmp di,1000h			; if NOT overflowing page (DX>4096)
	jb @@2				;  then jump
	mov ax,es			; else adjust ES:DI pointer to next PG
	add ax,0100h
	mov es,ax
	xor di,di

@@2:	mov ax,0DE04h			; allocate 4K VCPI page
	int 67h
	test ah,ah			; check if error
	jnz @@3				; if yes, then we are done
	and dh,0F0h			; clear avail bits
	mov dl,07h			; set page as user/writeable/present
	mov es:[di],edx			; store page addr in PageTable
	add di,4			; increment ptr into PageTable
	inc bx				; increment allocated page counter
	sub ecx,4096
	ja @@1				; loop until no more pages to allocate

@@3:	mov vcpi_allocmem,bx		; store alloc pages for deallocation
	cmp ecx,4096			; is there memory left to allocate
	jb @@4				; no, then done
	test pm32_mode,00001000b	; check if VCPI+XMS allocation scheme
	jz @@4				;  enabled, if not jump
	call vcpi_alloc_xms		; try XMS memory allocation

@@4:	shl ebx,12			; convert allocated pages to bytes
	jz @@done			; allocated any memory? if no, jump
	mov mem_free,ebx		; store amount of allocated memory
	mov eax,pagetablefree		; figure out address of memory
	sub eax,pagetablebase
	shl eax,10
	mov mem_ptr,eax
@@done:	ret

vcpi_alloc_xms:
	push ebx			; save EBX counter
	shr ecx,10			; convert bytes to KB
	and cl,0FCh			; mask ECX to match 4KB pages

@@1:	mov edx,ecx
	jecxz @@done			; if EDX=0, no mem to be allocated
	call xms_allocmem		; XMS allocate extended memory
	dec ax
	jz @@2				; if got memory, jump
	sub ecx,4			; try less memory, subtract 4K page
	jnc @@1				; loop til there is no memory to alloc
	jmp @@done			; no memory allocated, done

@@2:	mov xms_handle,dx		; store handle
	mov ah,0Ch			; XMS lock extended memory
	call dword ptr xms_call
	dec ax
	jz @@3				; if locked memory, jump
	xor dx,dx
	xchg dx,xms_handle		; reset xms_handle: no mem allocated
	mov ah,0Ah			; free allocated XMS memory
	call dword ptr xms_call
	jmp @@done

@@3:	shl edx,16			; convert DX:BX to EDX pointer
	mov dx,bx
	shr ecx,2			; ECX=memory allocated in 4K blocks
	movzx eax,cx
	pop ebx
	add ebx,eax			; adjust allocated page number
	push ebx

@@4:	cmp di,1000h			; if NOT overflowing page (DX>4096)
	jb @@5				;  then jump
	mov ax,es			; else adjust ES:DI pointer to next PG
	add ax,0100h
	mov es,ax
	xor di,di

@@5:	and dh,0F0h			; clear avail bits
	mov dl,07h			; set page as user/writeable/present
	mov es:[di],edx			; set linear memory addr
	add di,4			; increment pointer
	add edx,4096			; increment address
	loop @@4

@@done:	pop ebx				; restore EBX counter
	ret





;=============================================================================
x_init:	mov ah,07h			; query A20
	call dword ptr xms_call
	mov A20_state,al
	mov ah,03h			; global enable A20
	call dword ptr xms_call
	mov bx,0007h			; error code 0007h in case of error
	dec ax				; error enabling A20?
	stc
	jnz init_done			; if yes, exit with error 0007h

	mov eax,xms_data		; get KB of free XMS memory
	mov edx,pm32_maxextmem		; get requested amount of memory
	shr edx,10			; convert to KB
	cmp edx,eax
	jbe @@1
	mov edx,eax

@@1:	mov esi,edx
	test edx,edx			; check if no extended memory is to
	jz @@done			;  be allocated, if so, then jump

	call xms_allocmem		; allocate EDX KB memory
	dec ax
	jnz @@done			; no memory has been allocated
	mov xms_handle,dx		; store handle

	mov ah,0Ch			; lock extended memory
	call dword ptr xms_call
	dec ax
	jz @@2				; if no error, jump
	xor dx,dx
	xchg dx,xms_handle		; reset handle: no mem allocated
	mov ah,0Ah			; free allocated extended memory
	call dword ptr xms_call
	jmp @@done

@@2:	mov word ptr mem_ptr[0],bx	; store linear pointer to memory
	mov word ptr mem_ptr[2],dx
	shl esi,10			; convert to bytes
	mov dword ptr mem_free,esi	; store amount of memory allocated
@@done:	jmp xr_init                     ; go to XMS/raw continue init





;=============================================================================
r_init:	call enable_A20			; enable A20
	mov bx,0007h			; error code 0007h
	jc init_done			; exit if error enabling A20

	xor eax,eax
	mov ah,88h			; how much extended memory free
	int 15h
	test ax,ax			; if none, done with raw init
	jz xr_init

	shl eax,10			; EAX = size of memory (bytes)
	lea edx,[eax+100000h]		; EDX = base of memory

	cmp eax,pm32_maxextmem		; check how much memory to alloc
	jbe @@1				; pick lowest value
	mov eax,pm32_maxextmem

@@1:	add eax,000003FFh		; align memory to KB
	and eax,0FFFFC00h
	sub edx,eax
	mov mem_ptr,edx			; store extended memory base
	mov mem_free,eax		; store size of extended memory
	shr eax,10			; convert to KB
	mov mem_used,ax			; set used memory

xr_init:                                ; XMS/raw common init tail
	mov word ptr picslave,0870h
	mov rmtopmswrout,offs xr_rmtopmsw; set XMS/raw mode switch addresses
	mov pmtormswrout,offs xr_pmtormsw




;=============================================================================
vxr_init:				; VCPI/XMS/raw common init tail
	call install_ints		; install interrupts and except handl.

	xor eax,eax
	mov ax,es
	mov idtseg,ax			; set IDT segment base address
	mov ebx,eax
	shl ebx,4
	mov idtbase,ebx			; set IDT linear base address

	add ax,80h			; size of IDT
	mov rmstackbase,ax

	movzx bx,pm32_rmstacks		; set top and base of real mode stack
	mov cx,pm32_rmstacklen
	mov rmstacklen,cx
	imul bx,cx			;  area for interrupt redirection
	add ax,bx
	mov rmstacktop,ax
	mov rmstacktop2,ax

	shl eax,4
	mov pmstackbase,eax		; set next data area to end of RM stk

	movzx ebx,pm32_pmstacks		; set protected mode stack area top
	movzx ecx,pm32_pmstacklen	;  for callbacks
	shl ecx,4
	mov pmstacklen,ecx		; protected mode stack size in bytes
	imul ebx,ecx
	add eax,ebx
	mov pmstacktop,eax		; protected mode stack area top
	mov pmstacktop2,eax

	mov callbackbase,eax		; top of stacks is base of callbacks
	shr eax,4			; BX = seg of callback area
	mov callbackseg,ax
	mov es,ax			; ES = seg of callback area

	call allocate_callbacks		; allocate callbacks

	xor eax,eax
	mov ax,es			; set GDT base address
	mov gdtseg,ax			; store segment of GDT
	shl eax,4
	mov gdtbase,eax
	movzx ecx,pm32_selectors	; set GDT limit
	lea ecx,[8*ecx+8*SYSSELECTORS-1]
	mov gdtlimit,cx
	xor di,di			; clear GDT with all 0
	inc cx
	shr cx,1
	xor eax,eax
	rep stos word ptr es:[di]

	cmp pmodetype,2			; if under VCPI, do VCPI GDT set up
	jne @@f0
	pop ax				; restore TSS seg from stack
	shl eax,4			; set up TSS selector in GDT
	mov es:[SELVCPITSS+2],eax
	mov byte ptr es:[SELVCPITSS],67h	; limit
	mov byte ptr es:[SELVCPITSS+5],89h	; access rights
	add eax,64h-4*9			; unused part of TSS is also
	mov vcpiswitchstack,eax		;  temporary switch stack
	mov di,SELVCPICODE		; copy 3 VCPI descriptors from stack
	mov si,sp			;  to GDT
	mov cl,4*3
	rep movs word ptr es:[di],ss:[si]
	add sp,8*3                      ;***NOTE: (VCPI) adjust stack

@@f0:	mov ax,0FFFFh
	mov word ptr es:[SELZERO],ax		; set SELZERO limit
	mov word ptr es:[SELCALLBACK],ax	; set callback DS limit
	mov ax,0DF92h
	mov word ptr es:[SELZERO+5],ax		; set SELZERO rights
	mov word ptr es:[SELCALLBACK+5],ax	; set callback DS rights

	mov ax,cs			; AX = base
	mov bx,SELCODE			; BX = index to SELCODE descriptor
	mov cx,0FFFFh			; CX = limit (64k)
	mov dx,109Ah			; DX = access rights
	call vxr_initsetdsc
	mov bx,SELDATA			; BX = index to SELDATA descriptor
	mov dx,1092h			; DX = access rights
	call vxr_initsetdsc
	mov ax,0040h
	mov bx,SELBIOSDATA
	call vxr_initsetdsc

	mov bx,8*SYSSELECTORS		; BX = base of free descriptors
	push bx                         ; store selector
	mov ax,ss			; set caller SS descriptor
	mov dx,5092h
	call vxr_initsetdsc
	mov ax,[bp]                     ; set caller DS descriptor
	mov [bp],bx                     ; put DS selector on stack for exit
	call vxr_initsetdsc

	push bx				; get PSP segment
	mov ah,51h
	int 21h
	mov si,bx			; SI = PSP segment
	pop bx

	push ds
	mov ds,si			; set caller environment descriptor
	mov ax,ds:[002Ch]
	test ax,ax			; is environment seg 0?
	jz @@f1				; if yes, dont convert to descriptor
	mov ds:[002Ch],bx		; store selector value in PSP
	call vxr_initsetdsc
	mov ax,si                       ; set caller PSP descriptor
@@f1:	mov cx,0FFh			; limit is 100h bytes
	call vxr_initsetdsc
	pop ds

	sub bx,8
	mov cx,bx			; CX = ES descriptor, PSP
	pop dx                          ; DX = SS descriptor, from stack
	mov ax,SELZERO                  ; AX = DS descriptor, SELZERO
	movzx ebx,sp                    ; EBX = SP, current SP - same stack
	mov si,SELCODE                  ; target CS is SELCODE, same segment
	mov edi,offs @@swpm		; target EIP
	jmp rmtopmswrout                ; jump to mode switch routine
;
; now we are in protected mode
;
@@swpm:	cli
	mov edi,cs:codebase		; EDI = offset of _KERNEL from 0
	mov eax,cs:vcpi_cr3		; EAX = CR3
	mov cr3,eax			; reload CR3 register to flush TLB
	xor eax,eax
	mov cr2,eax			; reset CR2 page fault addr register
	mov eax,cr0
	mov oldcr0[edi],eax		; preserve original CR0

	cmp cs:fputype,0
	setnz virtualfpu[edi]		; set client fpu status
	mov eax,ds:[4*1Bh]		; preserve INT 1Bh - (CTRL-Break)
	mov oldint1Bh[edi],eax
	mov eax,ds:[4*1Ch]		; preserve INT 1Ch - (timer ticks)
	mov oldint1Ch[edi],eax
	mov eax,ds:[4*21h]		; preserve INT 21h - (DOS API)
	mov oldint21h[edi],eax
	mov eax,ds:[4*23h]		; preserve INT 23h - (DOS CTRL-C)
	mov oldint23h[edi],eax
	mov eax,ds:[4*24h]		; preserve INT 24h - (DOS Critical)
	mov oldint24h[edi],eax
	mov eax,ds:[4*2Fh]		; preserve INT 2Fh - (Multiplex)
	mov oldint2Fh[edi],eax
	mov ax,cs:kernel_code		; install real mode INT 21h
	shl eax,16
	mov ax,offs int21h
	mov ds:[4*21h],eax

	mov eax,ds:[4*15h]		; get INT 15h vector
	mov oldint15h[edi],eax		; store old INT 15h vector
	cmp cs:pmodetype,0		; is system raw
	jnz @@1				; if not, we are done
	cmp cs:id32_process_id,0
	jnz @@1
	mov ax,cs:kernel_code
	shl eax,16
	mov ax,offs int15h
	mov ds:[4*15h],eax		; install new INT 15h handler

@@1:	push ds es edi
	push cs
	pop ds				; DS = code base (executable)
	mov es,seldata			; ES = code base (writeable)
	mov ax,0303h
	mov esi,offs int1Bh		;** NOTE: EDI points to init_code
	mov edi,offs @callback_data+00h	;** EDI = callback data structure
	int 31h				;** which will be used only once
	push cx dx			;** at startup.
	mov si,offs int1Ch
	mov di,offs @callback_data+32h
	int 31h
	push cx dx
	mov si,offs int23h
	mov di,offs @callback_data+64h
	int 31h
	push cx dx
	mov si,offs int24h
	mov di,offs @callback_data+96h
	int 31h
	push cx dx
	mov ds,seldata			; DS = data selector
	pop newint24h
	pop newint23h
	pop newint1Ch
	pop newint1Bh
	pop edi es ds

	xor eax,eax
	mov ebx,cs:mem_ptr		; EBX = base of extended memory
	mov ecx,cs:mem_free		; ECX = size of extended memory
	mov edx,ebx			; align ptr to memory on para boundary
	add ebx,0Fh
	and bl,0F0h
	mov esi,ebx
	sub esi,edx			; get the difference
	add esi,10h			; plus 16 bytes for 1st block header
	sub ecx,esi			; reduce size of free memory
	ja @@2				; if no error, jump
	mov mem_ptr[edi],eax		; not enough memory even for one
	mov mem_free[edi],eax		; memory block header, done now
	jmp @@done

@@2:	mov eax,12345678h		; header id
	mov [ebx+00h],eax
	mov [ebx+0Ch],eax
	movzx eax,id32_process_id
	mov [ebx+08h],eax		; set current process id
	mov [ebx+04h],ecx		; set first block unused/currentsize
	lea edx,[ebx+ecx+10h]		; get pointer to next block (mem_top)
	mov mem_top[edi],edx		; set top of memory
	mov mem_ptr[edi],ebx		; set base of memory
	mov mem_free[edi],ecx		; set size of memory

@@done:	sti
	jmp dvxr_init			; go to DPMI/VCPI/XMS/raw init tail







;-----------------------------------------------------------------------------
install_ints:
	xor di,di                       ; set up IDT
	xor ecx,ecx
	mov dx,word ptr picslave

@@1:	lea eax,[SELCODE*10000h+ecx*4+offs int_matrix]
	stos dword ptr es:[di]
	mov eax,8E00h			; interrupt gate type (IF=0)
	mov bl,cl			; isolate high 5 bits of int num
	and bl,0F8h
	test cl,0F0h			; one of the low 16 interrupts?
	jz @@2				; if yes, store as interrupt gate
	cmp bl,dl			; one of the high IRQs?
	je @@2				; if yes, store as interrupt gate
	cmp bl,dh			; one of the low IRQs?
	je @@2				; if yes, store as interrupt gate
	mov ax,8F00h			; set to trap gate type (IF=unchanged)
@@2:	stos dword ptr es:[di]
	inc cl                          ; increment interrupt number
	jnz @@1				; loop if more interrupts to go

	mov word ptr es:[8*21h],offs int21 ; protected mode INT 21h
	mov word ptr es:[8*31h],offs int31 ; protected mode INT 31h

	push ds es			; copy IRQ vectors into table
	push ds
	pop es
	xor ax,ax
	mov ds,ax
	mov di,offs irqtab_rm
	movzx si,dh
	shl si,2
	mov cx,8
	rep movs dword ptr es:[di],ds:[si]
	movzx si,dl
	shl si,2
	mov cl,8
	rep movs dword ptr es:[di],ds:[si]
	pop es ds

	test pm32_mode,00000010b	; if exception handling is off
	jz @@done			; we are done
;
; install INT 00h-0Eh exceptions
;
	mov ax,offs irq_fail		; offset of fail routine
	sub ax,offs int_matrix+4	; calculate offset displacement
	mov di,offs int_matrix+2	; DI = pointer into CALL matrix
	mov cl,15
@@l1:	mov ds:[di],ax			; modify CALL address
	sub ax,4			; PUSH + CALL = 4 byte
	add di,4
	loop @@l1
;
; install IRQ 0-15 (PIC dependent)
;
	movzx dx,picmaster		; install IRQ 0-7 (INT 08-0Fh)
	call setup_irqs
	movzx dx,picslave		; install IRQ 8-15 (INT 70-77h)
	call setup_irqs

	cmp picmaster,10h		; setup IRQ 7 (damn it!)
	jae @@done
	mov ax,(offs irq_normal) - (offs int_matrix+4) - 0Fh*4
	mov word ptr int_matrix[0Fh*4+2],ax	; restore INT 0Fh (IRQ 7)

@@done:	ret


setup_irqs:
	cmp dl,10h
	mov ax,offs irq_tester		; if DX is in range INT 00h - 0Fh
	jb @@4				;  install as IRQ check handler
	mov ax,offs irq_normal		; else no IRQ check is needed
@@4:	shl dx,2
	sub ax,offs int_matrix+4
	sub ax,dx
	mov di,offs int_matrix+2
	add di,dx
	mov cl,8			; 8 IRQs to go
@@l3:	mov ds:[di],ax
	sub ax,4			; PUSH + CALL is 4 bytes
	add di,4			; next entry in IDT
	loop @@l3			; loop
	ret


;-----------------------------------------------------------------------------
allocate_callbacks:
	movzx cx,pm32_callbacks		; CL = number of callbacks
	jcxz @@done			; if no, done with this part
	xor di,di			; location within callback seg
	mov ax,6866h
	mov dx,kernel_code

	push ds
	push es
	pop ds
@@1:	mov word ptr ds:[di],6066h	; PUSHAD instruction
	mov byte ptr ds:[di+2],ah	; PUSH WORD instruction
	mov word ptr ds:[di+3],0	; immediate 0 used as free flag
	mov word ptr ds:[di+5],ax	; PUSH DWORD instruction
	mov byte ptr ds:[di+11],0B9h	; MOV CX,? instruction
	mov word ptr ds:[di+14],ax	; PUSH DWORD instruction
	mov byte ptr ds:[di+20],0EAh	; JMP FAR PTR ?:? intruction
	mov word ptr ds:[di+21],offs callback
	mov word ptr ds:[di+23],dx
	add di,25			; increment ptr to callback
	loop @@1			; if more callbacks to do, loop
	pop ds

	add di,0Fh			; align next data area on paragraph
	shr di,4
	mov ax,es
	add ax,di
	mov es,ax			; set ES to base of next data area
@@done:	ret


;-----------------------------------------------------------------------------
vxr_initsetdsc:                         ; set descriptor for VCPI/XMS/raw init
	push ax
	movzx eax,ax                    ; EAX = base of segment
	shl eax,4
	mov word ptr es:[bx],cx         ; limit = CX
	mov dword ptr es:[bx+2],eax     ; base address = EAX
	mov word ptr es:[bx+5],dx       ; access rights = DX
	add bx,8                        ; increment descriptor index
	pop ax
	ret


;-----------------------------------------------------------------------------
xms_allocmem:
	push edi
	mov edi,edx
	mov ah,89h
	call dword ptr xms_call
	cmp ax,0001h
	jz @@ok
	mov dx,di
	mov ah,09h
	call dword ptr xms_call
@@ok:	pop edi
	ret


