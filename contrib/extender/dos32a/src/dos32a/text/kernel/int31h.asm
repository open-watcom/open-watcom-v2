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

;=============================================================================
; INT 31h INTERFACE
;=============================================================================

		evendata
;=============================================================================
int31h_tab	label word
	dw	0300h		; simulate real mode int
	dw	int31h_0300
	dw	0301h		; call rm proc RETF
	dw	int31h_0301
	dw	0302h		; call rm proc IRET
	dw	int31h_0302
;---------------------------------------------------------------------
	dw	0000h		; allocate descriptor
	dw	int31h_0000
	dw	0001h		; free descriptor
	dw	int31h_0001
	dw	0002h		; map seg to sel
	dw	int31h_0002
	dw	0003h		; get sel increment value
	dw	int31h_0003
	dw	0006h		; get sel base addr
	dw	int31h_0006
	dw	0007h		; set sel base addr
	dw	int31h_0007
	dw	0008h		; set sel limit
	dw	int31h_0008
	dw	0009h		; set sel access rights
	dw	int31h_0009
	dw	000Ah		; create alias sel
	dw	int31h_000A
	dw	000Bh		; get descriptor
	dw	int31h_000B
	dw	000Ch		; set descriptor
	dw	int31h_000C
	dw	000Eh		; get multi descriptors
	dw	int31h_000E
	dw	000Fh		; set multi descriptors
	dw	int31h_000F
;---------------------------------------------------------------------
	dw	0100h		; alloc DOS memory
	dw	int31h_0100
	dw	0101h		; free DOS memory
	dw	int31h_0101
	dw	0102h		; resize DOS memory
	dw	int31h_0102
;---------------------------------------------------------------------
	dw	0200h		; get real mode int
	dw	int31h_0200
	dw	0201h		; set real mode int
	dw	int31h_0201
	dw	0202h		; get pm exception vector
	dw	int31h_0202
	dw	0203h		; set pm exception vector
	dw	int31h_0203
	dw	0204h		; get pm int
	dw	int31h_0204
	dw	0205h		; set pm int
	dw	int31h_0205
;---------------------------------------------------------------------
	dw	0303h		; alloc callback
	dw	int31h_0303
	dw	0304h		; free callback
	dw	int31h_0304
	dw	0305h		; get state save/restore addr
	dw	int31h_0305
	dw	0306h		; get raw mode switch addr
	dw	int31h_0306
;---------------------------------------------------------------------
	dw	0400h		; get DPMI version
	dw	int31h_0400
;---------------------------------------------------------------------
	dw	0500h		; get free mem info
	dw	int31h_0500
	dw	0501h		; alloc mem
	dw	int31h_0501
	dw	0502h		; free mem
	dw	int31h_0502
	dw	0503h		; resize mem
	dw	int31h_0503
	dw	050Ah		; get linear mem block and size
	dw	int31h_050A
;---------------------------------------------------------------------
	dw	0600h		; lock linear region			*VM*
	dw	int31h_0600
	dw	0601h		; unlock linear region			*VM*
	dw	int31h_0601
	dw	0602h		; Mark Real Mode Region as Pageable	*VM*
	dw	int31h_0602
	dw	0603h		; Relock Real Mode Region		*VM*
	dw	int31h_0603
	dw	0604h		; get page size				*VM*
	dw	int31h_0604
;---------------------------------------------------------------------
	dw	0702h		; mark page				*VM*
	dw	int31h_0702
	dw	0703h		; discard page				*VM*
	dw	int31h_0703
;---------------------------------------------------------------------
	dw	0800h		; physical mem mapping			*VM*
	dw	int31h_0800
	dw	0801h		; free mapped mem			*VM*
	dw	int31h_0801
;---------------------------------------------------------------------
	dw	0900h		; get/disable VIS
	dw	int31h_0900
	dw	0901h		; get/enable VIS
	dw	int31h_0901
	dw	0902h		; get VIS
	dw	int31h_0902
;---------------------------------------------------------------------
	dw	0A00h		; vendor specific
	dw	int31h_0A00
;---------------------------------------------------------------------
;	dw	0B00h		; Set Debug Watchpoint
;	dw	int31h_0B00
;	dw	0B01h		; Clear Debug Watchpoint
;	dw	int31h_0B01
;	dw	0B02h		; Get State of Debug Watchpoint
;	dw	int31h_0B02
;	dw	0B03h		; Reset Debug Watchpoint
;	dw	int31h_0B03
;---------------------------------------------------------------------
	dw	0E00h		; get FPU status
	dw	int31h_0E00
	dw	0E01h		; set FPU emulation
	dw	int31h_0E01
;---------------------------------------------------------------------
	dw	0EEFFh		; PMODE/W compatible call
	dw	int31h_EEFF
;---------------------------------------------------------------------
int31h_end	label word




;=============================================================================
int31h_pm:
	cli
	cld
	push	ds es fs gs			; push registers on stack
	pushad

	push	bx
	mov	ds,cs:seldata			; DS -> KERNEL

	cmp	ax,int31h_cache[0]		; check if function # is in cache
	mov	bx,int31h_cache[2]		; retrieve cached target addr
	je	@@2				; if cached, jump

	xor	bx,bx				; do a linear search
@@0:	cmp	ax,int31h_tab[bx]		; found function # ?
	je	@@1				; if yes, jump

	add	bx,4				; advance to next entry
	cmp	bx,(offs int31h_end - offs int31h_tab)	; end of table?
	jb	@@0				; loop until end of table

	pop	bx				; no function found
	jmp	int31fail8001			; exit with error 8001h

@@1:	mov	bx,int31h_tab[bx+2]		; get address of the appropriate handler
	mov	int31h_cache[0],ax		; store function # in cache
	mov	int31h_cache[2],bx		; store function addr in cache

@@2:	mov	ds,selzero			; DS -> 0 (beginning of memory)
	xchg	bx,[esp]			; store target addr & restore BX
	ret


;-----------------------------------------------------------------------------
int31fail8001:					; INT 31h return fail with error 8001h
	mov	al,01h
	jmp	int31failx
int31fail8010:					; INT 31h return fail with error 8010h
	mov	al,10h
	jmp	int31failx
int31fail8011:					; INT 31h return fail with error 8011h
	mov	al,11h
	jmp	int31failx
int31fail8012:					; INT 31h return fail with error 8012h
	mov	al,12h
	jmp	int31failx
int31fail8013:					; INT 31h return fail with error 8013h
	mov	al,13h
	jmp	int31failx
int31fail8015:					; INT 31h return fail with error 8015h
	mov	al,15h
	jmp	int31failx
int31fail8016:					; INT 31h return fail with error 8016h
	mov	al,16h
	jmp	int31failx
int31fail8021:					; INT 31h return fail with error 8021h
	mov	al,21h
	jmp	int31failx
int31fail8022:					; INT 31h return fail with error 8022h
	mov	al,22h
	jmp	int31failx
int31fail8023:					; INT 31h return fail with error 8023h
	mov	al,23h
	jmp	int31failx
int31fail8024:					; INT 31h return fail with error 8024h
	mov	al,24h
	jmp	int31failx
int31fail8025:					; INT 31h return fail with error 8025h
	mov	al,25h

int31failx:
	mov	ah,80h
	mov	[esp+28],ax			; set AX on stack to 8010h for POPAD
	jmp	int31fail



;-----------------------------------------------------------------------------
int31failbx:					; INT 31h return fail with BX,AX
	mov	wptr [esp+16],bx		; put BX onto stack for POPAD
	jmp	int31failax
int31failcx:					; INT 31h return fail with CX,AX
	mov	wptr [esp+24],cx		; put CX onto stack for POPAD
int31failax:					; INT 31h return fail with AX
	mov	wptr [esp+28],ax		; put AX onto stack for POPAD


;-----------------------------------------------------------------------------
int31fail:					; INT 31h return fail, pop all regs
	popad
	pop	gs fs es ds
int31failnopop:					; INT 31h return fail with carry set
	or	bptr [esp+8],01h		; set carry in EFLAGS on stack
	iretd


;-----------------------------------------------------------------------------
int31okedx:					; INT 31h return ok with EDX,CX,AX
	mov	[esp+20],edx			; put EDX onto stack for POPAD
	jmp	int31okcx
int31okdx:					; INT 31h return ok with DX,CX,AX
	mov	[esp+20],dx			; put DX onto stack for POPAD
	jmp	int31okcx
int31oksinoax:					; INT 31h return ok SI,DI,BX,CX
	mov	ax,[esp+28]			; get old value of AX for restore
int31oksi:					; INT 31h return ok SI,DI,BX,CX,AX
	mov	[esp+4],si			; put SI onto stack for POPAD
	mov	[esp+0],di			; put DI onto stack for POPAD
int31okbx:
	mov	[esp+16],bx			; put BX onto stack for POPAD
int31okcx:					; INT 31h return ok with CX,AX
	mov	[esp+24],cx			; put CX onto stack for POPAD
int31okax:					; INT 31h return ok with AX
	mov	[esp+28],ax			; put AX onto stack for POPAD


;-----------------------------------------------------------------------------
int31ok:					; INT 31h return ok, pop all regs
	popad
	pop	gs fs es ds
int31oknopop:					; INT 31h return ok with carry clear
	and	bptr [esp+8],0FEh		; clear carry in EFLAGS on stack
	iretd





;=============================================================================
; Helper functions
;=============================================================================

;-----------------------------------------------------------------------------
int31testsel:					; test for valid selector BX
	pop	bp				; pop return address
	cmp	bx,cs:gdtlimit			; selector BX out of range?
	ja	int31fail8022			; if yes, fail with error 8022h
	mov	edi,cs:gdtbase			; get base of GDT
	and	ebx,0FFF8h			; mask offset table index and RPL
	test	bptr ds:[edi+ebx+6],10h		; is descriptor used?
	jz	int31fail8022			; if descriptor not used, fail 8022h
	jmp	bp				; return ok

;-----------------------------------------------------------------------------
int31testaccess:				; test access bits in CX
	pop	bp				; pop return address
	test	ch,20h				; test MUST BE 0 bit in CH
	jnz	int31fail8021			; if not 0, error 8021h
	test	cl,90h				; test present and MUST BE 1 bits
	jz	int31fail8021			; if both 0, error 8021h
	jpo	int31fail8021			; if unequal, error 8021h
	test	cl,60h				; test DPL
	jnz	int31fail8021			; if not 0, error 8021h
	test	cl,8				; if code, more tests needed
	jz	@@0				; if data, skip code tests
	test	cl,2				; readable?
	jz	int31fail8021
	test	cl,4				; non-conform?
	jnz	int31fail8021
@@0:	jmp	bp				; return ok

;-----------------------------------------------------------------------------
int31testint:
	movzx	ebx,bl				; EBX = interrupt number
	mov	al,bl
	mov	ah,bl
	and	ax,0F807h
	movzx	esi,al
	cmp	ah,picmaster			; if 1st PIC, offset = buffer + 0
	jz	@@done
	add	si,8
	cmp	ah,picslave			; if 2nd PIC, offset = buffer + 8
	jz	@@done
	or	si,-1
@@done:	ret					; return: ZF = 0 if BL is IRQ, otherwise ZF = 1





;=============================================================================
; DESCRIPTOR FUNCTIONS
;=============================================================================

;=============================================================================
; Allocate Descriptors
;
int31h_0000:
	test	cx,cx				; if CX = 0, error 8021h
	jz	int31fail8021

	mov	edx,cs:gdtbase			; get base of GDT
	movzx	eax,cs:gdtlimit			; EAX = last selector index
	and	al,0F8h

	mov	bx,cx				; BX = number of selectors to find
@@l0:	test	bptr [edx+eax+6],10h		; is descriptor used?
	jnz	@@f0
	dec	bx				; found free descriptor, dec counter
	jnz	@@f1				; continue if need to find more

	mov	ebx,eax				; found all descriptors requested
@@l1:	mov	dptr [edx+ebx],0		; set entire new descriptor
	mov	dptr [edx+ebx+4],109200h
	add	bx,8				; increment selector index
	loop	@@l1				; dec counter of descriptors to mark
	jmp	int31okax			; return ok, with AX

@@f0:	mov	bx,cx				; reset number of selectors to find
@@f1:	sub	ax,8				; dec current selector counter
	cmp	ax,8*SYSSELECTORS		; more descriptors to go?
	jae	@@l0				; if yes, loop
	jmp	int31fail8011			; did not find descriptors


;=============================================================================
; Free Descriptor
;
int31h_0001:
	mov	ax,cs
	cmp	ax,bx
	jz	int31fail8022			; cannot free CS selector
	mov	ax,ss
	cmp	ax,bx
	jz	int31fail8022			; cannot free SS selector
	call	int31testsel			; test for valid selector BX
	xor	eax,eax
	mov	[edi+ebx+0],eax			; mark descriptor as free
	mov	[edi+ebx+4],eax

	mov	cx,4				; zero any segregs loaded with BX
	lea	ebp,[esp+32]			; EBP -> selectors on stack
@@l0:	cmp	wptr [ebp],bx			; selector = BX?
	jne	@@f0				; if no, continue loop

	mov	[ebp],ax			; zero selector on stack

@@f0:	add	ebp,2				; increment selector ptr
	loop	@@l0				; loop
	jmp	int31ok				; return ok


;=============================================================================
; Map Real-Mode Segment to Descriptor
;
int31h_0002:
	mov	ds,cs:seldata

	mov	cx,16				; max 16 selectors
	mov	si,offs segmentbases		; check, if segment already mapped
@@0:	mov	ax,[si+0]			; is selector zero (free entry)
	test	ax,ax
	jz	@@1				; if yes, loop
	cmp	bx,[si+2]			; compare segment values
	jz	int31okax			; if already mapped, done
@@1:	add	si,4
	loop	@@0

	mov	cl,16
	mov	si,offs segmentbases		; search for a free entry
@@2:	cmp	wptr [si],0			; this field free?
	jz	@@3				; if yes, use it
	add	si,4
	loop	@@2
	jmp	int31fail8010			; no entry free

@@3:	mov	[si+2],bx			; store segment
	movzx	edi,bx				; convert to linear address
	shl	edi,4
	mov	cl,1
	xor	ax,ax
	int	31h				; allocate selector
	jc	int31failax
	mov	[si+0],ax			; store selector

	mov	bx,ax
	xor	cx,cx
	mov	dx,-1
	mov	ax,8
	int	31h				; set descriptor limit 64k

	mov	dx,di
	shr	edi,16
	mov	cx,di
	mov	ax,7
	int	31h				; set descriptor base

	mov	cx,0092h
	mov	ax,9
	int	31h				; set access rights

	mov	ax,bx				; return selector
	jmp	int31okax


;=============================================================================
; Get Selector Increment Value
;
int31h_0003:
	mov	ax,8				; selector increment value is 8
	jmp	int31okax			; return ok, with AX


;=============================================================================
; Get Segment Base Address
;
int31h_0006:
	call	int31testsel			; test for valid selector BX

	mov	dx,wptr ds:[edi+ebx+2]		; low word of 32bit linear address
	mov	cl,bptr ds:[edi+ebx+4]		; high word of 32bit linear address
	mov	ch,bptr ds:[edi+ebx+7]
	jmp	int31okdx			; return ok, with DX, CX, AX


;=============================================================================
; Set Segment Base Address
;
int31h_0007:
	call	int31testsel			; test for valid selector BX

	mov	wptr ds:[edi+ebx+2],dx		; low word of 32bit linear address
	mov	bptr ds:[edi+ebx+4],cl		; high word of 32bit linear address
	mov	bptr ds:[edi+ebx+7],ch
	jmp	int31ok				; return ok


;=============================================================================
; Set Segment Limit
;
int31h_0008:
	call	int31testsel			; test for valid selector BX
	cmp	cx,0Fh				; is limit greater than 1M?
	jbe	@@1				; if not, jump

	or	dx,0FFFh			; auto-adjust limit
	shrd	dx,cx,12			; DX = low 16 bits of page limit
	shr	cx,12				; CL = high 4 bits of page limit
	or	cl,80h				; set granularity bit in CL

@@1:	mov	wptr ds:[edi+ebx],dx		; put low word of limit
	and	bptr ds:[edi+ebx+6],50h		; mask off G and high nibble of limit
	or	bptr ds:[edi+ebx+6],cl		; put high nibble of limit
	jmp	int31ok				; return ok


;=============================================================================
; Set Descriptor Access Rights
;
int31h_0009:
	call	int31testsel			; test for valid selector BX
	call	int31testaccess			; test access bits in CX

	or	ch, 10h				; set AVL bit, descriptor used
	and	ch,0D0h				; mask off low nibble of CH
	and	bptr ds:[edi+ebx+6],0Fh		; mask off high nibble access rights
	or	bptr ds:[edi+ebx+6],ch		; or in high access rights byte
	mov	bptr ds:[edi+ebx+5],cl		; put low access rights byte
	jmp	int31ok				; return ok


;=============================================================================
; Create Alias Descriptor
;
int31h_000A:
	call	int31testsel			; test for valid selector BX

	xor	ax,ax				; allocate descriptor
	mov	cx,1
	int	31h
	jc	int31fail8011			; if failed, descriptor unavailable

	push	ax				; preserve allocated selector
	push	ds				; copy descriptor and set type data
	pop	es
	movzx	edi,ax				; EDI = target selector
	mov	esi,cs:gdtbase			; ESI -> GDT
	add	edi,esi				; adjust to target descriptor in GDT
	add	esi,ebx				; adjust to source descriptor in GDT

	movs	dptr es:[edi],ds:[esi]		; copy descriptor
	lods	dptr ds:[esi]
	mov	ah,92h				; set descriptor type - R/W up data
	stos	dptr es:[edi]
	pop	ax				; restore allocated selector
	jmp	int31okax			; return ok, with AX


;=============================================================================
; Get Descriptor
;
int31h_000B:
	call	int31testsel			; test for valid selector BX
	lea	esi,[edi+ebx]			; ESI -> descriptor in GDT
	mov	edi,[esp]			; get EDI buffer ptr from stack
	movs	dptr es:[edi],ds:[esi]		; copy descriptor
	movs	dptr es:[edi],ds:[esi]
	jmp	int31ok				; return ok


;=============================================================================
; Set Descriptor
;
int31h_000C:
	call	int31testsel			; test for valid selector BX
	mov	esi,[esp]			; ESI = EDI buffer ptr from stack
	mov	cx,es:[esi+5]			; get access rights from descriptor
	call	int31testaccess			; test access bits in CX

	push	ds es				; swap DS and ES, target and source
	pop	ds es

	add	edi,ebx				; adjust EDI to descriptor in GDT
	movs	dptr es:[edi],ds:[esi]		; copy descriptor
	lods	dptr ds:[esi]
	or	al,10h				; set descriptor AVL bit
	stos	wptr es:[edi]
	jmp	int31ok				; return ok


;=============================================================================
; Get Multiple Descriptors
;
int31h_000E:
	mov	ax,000Bh			; function 000Bh, get descriptor
	jmp	int31h_000EF			; go to common function


;=============================================================================
; Set Multiple Descriptors
;
int31h_000F:
	mov	ax,000Ch			; function 000Ch, set descriptor

int31h_000EF:					; common to functions 000Eh and 000Fh
	test	cx,cx				; if CX = 0, return ok immediately
	jz	int31ok

	mov	dx,cx				; DX = number of descriptors
	xor	cx,cx				; CX = successful counter
@@l0:	mov	bx,es:[edi]			; BX = selector to get
	add	edi,2
	int	31h				; get/set descriptor
	jc	int31failcx			; if error, fail with AX and CX

	add	edi,8				; increment descriptor ptr
	inc	cx				; increment successful copy counter
	dec	dx				; decrement loop counter
	jnz	@@l0				; if more descriptors to go, loop
	jmp	int31ok				; return ok






;=============================================================================
; DOS MEMORY FUNCTIONS
;=============================================================================

;=============================================================================
; Allocate DOS Memory Block
;
int31h_0100:
	mov	ah,48h				; DOS alloc memory function
	call	int31010x_f2			; allocate memory
	jc	int31failbx			; if fail, exit with AX=err, BX=maxmem

	mov	dx,ax				; DX=segment of DOS memory block
	xor	ax,ax				; allocate descriptor
	mov	cx,1
	int	31h
	jnc	@@1
	mov	ah,49h				; if error allocating descriptor
	call	int31010x_f2			; free what was allocated
	jmp	int31fail8011			; and exit with error 8011h

@@1:	mov	[esp+14h],ax			; set selector in DX
	mov	[esp+1Ch],dx			; set base address in AX
	mov	bx,ax
	mov	cx,dx
	shl	dx,4
	shr	cx,12
	mov	ax,0007h			; set selector base
	int	31h

	mov	cx,0092h			; set access rights
	mov	al,09h
	int	31h
	jmp	int31010x			; set selector size


;=============================================================================
; Free DOS Memory Block
;
int31h_0101:
	mov	ah,49h
	mov	si,dx				; preserve DX = selector
	call	int31010x_f1
	jc	int31failax
	mov	bx,si				; restore selector in BX
	jmp	int31h_0001


;=============================================================================
; Resize DOS Memory Block
;
int31h_0102:
	mov	ah,4Ah
	mov	si,dx				; preserve DX = selector
	call	int31010x_f1
	jc	int31failbx
	mov	bx,si				; restore selector in BX

int31010x:
	movzx	edx,wptr [esp+10h]		; get original size
	shl	edx,4				; convert para to bytes
	dec	edx				; limit=size-1
	shld	ecx,edx,16
	mov	ax,0008h			; set limit
	int	31h
	jmp	int31ok


;-----------------------------------------------------------------------------
int31010x_f1:
	pop	bp
	push	ax bx
	mov	bx,dx				; BX = selector
	mov	ax,0006h			; get base
	int	31h
	pop	bx ax
	jc	int31failax
	shrd	dx,cx,4				; adjust CX:DX to segment value
	push	bp

int31010x_f2:
	xor	cx,cx
	push	cx				; set real mode SS:SP
	push	cx
	sub	esp,10
	push	dx				; set real mode ES
	push	cx				; set real mode flags
	pushad					; set real mode registers
	push	ss
	pop	es
	mov	edi,esp
	mov	bl,21h
	mov	ax,0300h
	int	31h
	mov	bx,[esp+10h]			; get BX from structure
	mov	ax,[esp+1Ch]			; get AX from structure
	lea	esp,[esp+32h]
	pop	bp
	jc	int31failax			; if error, fail
	bt	wptr [esp-14h],0
	jmp	bp






;=============================================================================
; INTERRUPT FUNCTIONS
;=============================================================================

;=============================================================================
; Get Real Mode Interrupt Vector
;
int31h_0200:
	movzx	ebx,bl				; EBX = BL (interrupt number)
	mov	dx,[ebx*4+0]			; load real mode vector offset
	mov	cx,[ebx*4+2]			; load real mode vector segment
	jmp	int31okdx			; return ok, with AX, CX, DX


;=============================================================================
; Set Real Mode Interrupt Vector
;
int31h_0201:
	mov	ebp,dr7
	xor	eax,eax				; temporarily disable null-ptr protection
	mov	dr7,eax

	movzx	ebx,bl				; EBX = BL (interrupt number)
	mov	[ebx*4+0],dx			; set real mode vector offset
	mov	[ebx*4+2],cx			; set real mode vector segment

	mov	dr7,ebp
	jmp	int31ok				; return ok


;=============================================================================
; Get Processor Exception Handler Vector
;
int31h_0202:
	mov	ds,cs:seldata
	cmp	bl,20h				; must be in range 00..1Fh
	jae	int31fail8021			; invalid value

	xor	cx,cx
	xor	edx,edx
	cmp	bl,10h				; if not in range 00..0Fh
	jae	@@done				; return 0:0

	movzx	ebx,bl
	mov	cx,wptr exctab_pm[ebx*8+4]	; get user-defined exception handler CS
	mov	edx,dptr exctab_pm[ebx*8+0]	; get user-defined exception handler EIP

@@done:	mov	ax,[esp+28]
	jmp	int31okedx			; return ok, with AX, CX, EDX


;=============================================================================
; Set Processor Exception Handler Vector
;
int31h_0203:
	xchg	bx,cx				; swap INT number with INT selector
	call	int31testsel			; test for valid selector BX
	xchg	bx,cx

	mov	ds,cs:seldata
	cmp	bl,20h				; must be in range 00..1Fh
	jae	int31fail8021			; invalid value

	cmp	bl,10h				; if not in range 00..0Fh
	jae	@@done				; then done

	movzx	ebx,bl
	mov	wptr exctab_pm[ebx*8+4],cx	; set user-defined exception handler CS
	mov	dptr exctab_pm[ebx*8+0],edx	; set user-defined exception handler EIP

@@done:	jmp	int31ok				; return ok


;=============================================================================
; Get Protected Mode Interrupt Vector
;
int31h_0204:
	mov	ds,cs:seldata
	call	int31testint			; check if one of IRQs
	jz	@@1				; if yes, read from buffer

@@0:	shl	ebx,3				; adjust for location in IDT
	add	ebx,idtbase			; add base of IDT
	mov	ds,selzero
	mov	edx,dptr [ebx+4]		; get high word of offset
	mov	dx,wptr [ebx+0]			; get low word of offset
	mov	cx,wptr [ebx+2]			; get selector
	jmp	@@done

@@1:	bt	irqset_pm,si			; check if IRQ is installed
	jnc	@@2				; if not, return built-in handler
	test	bl,0F0h				; check if IRQ is above INT 0Fh
	jnz	@@0				; if yes, don't buffer: read from IDT

	mov	cx,wptr irqtab_pm[esi*8+4]	; get user-defined IRQ handler CS
	mov	edx,dptr irqtab_pm[esi*8+0]	; get user-defined IRQ handler EIP
	jmp	@@done

@@2:	mov	cx,SELCODE			; load built-in IRQ handler CS
	lea	edx,std_matrix[esi*4]		; load built-in IRQ handler EIP

@@done:	mov	ax,[esp+28]
	jmp	int31okedx			; return ok, with AX, CX, EDX


;=============================================================================
; Set Protected Mode Interrupt Vector
;
int31h_0205:
	xchg	bx,cx				; swap int number with int selector
	call	int31testsel			; test for valid selector BX
	xchg	bx,cx

	mov	ds,cs:seldata
	mov	es,selzero
	movzx	ecx,cx				; ECX = CX (selector)

	call	int31testint			; check if one of IRQs
	jz	@@1				; if yes, install in buffer
	cmp	bl,1Bh				; process special interrupts
	jz	@@1Bh
	cmp	bl,1Ch
	jz	@@1Ch
	cmp	bl,23h
	jz	@@23h
	cmp	bl,24h
	jz	@@24h

@@0:	shl	ebx,3				; adjust for location in IDT
	add	ebx,idtbase			; add base of IDT
	mov	wptr es:[ebx+0],dx		; set low word of offset
	shr	edx,16
	mov	wptr es:[ebx+6],dx		; set high word of offset
	mov	wptr es:[ebx+2],cx		; set selector
	jmp	@@done

@@1:	cmp	cx,SELCODE			; check if restoring IRQ
	jnz	@@2				; if not, jump

	btr	irqset_rm,si			; reset IRQ installed bit (RM)
	btr	irqset_pm,si			; reset IRQ installed bit (PM)
	mov	eax,irqtab_rm[esi*4]		; restore real mode interrupt
	mov	es:[ebx*4],eax
	cmp	bl,10h				; check if INT vector is above INT 0Fh
	jae	@@0				; if yes, install into IDT

	jmp	@@done

@@2:	bts	irqset_rm,si			; set IRQ installed bit (RM)
	bts	irqset_pm,si			; set IRQ installed bit (PM)
	mov	eax,es:[ebx*4]			; get CS:IP of real mode INT vector
	mov	irqtab_rm[esi*4],eax		; save CS:IP of real mode INT vector
	lea	eax,back_matrix[esi*4]		; get address of real mode IRQ callback
	mov	wptr es:[ebx*4+0],ax		; set real mode IRQ callback IP
	mov	wptr es:[ebx*4+2],_KERNEL	; set real mode IRQ callback CS
	cmp	bl,10h				; check if INT vector is above INT 0Fh
	jae	@@0				; if yes, install into IDT

	mov	wptr irqtab_pm[esi*8+4],cx	; install CS into IRQ buffer
	mov	dptr irqtab_pm[esi*8+0],edx	; install EIP into IRQ buffer

@@done:	jmp	int31ok				; return ok

@@1Bh:	cmp	cx,SELCODE			; install real mode INT 1Bh callback
	mov	eax,newint1Bh
	jnz	@@1Bh0
	mov	eax,oldint1Bh
@@1Bh0:	mov	es:[4*1Bh],eax
	jmp	@@0

@@1Ch:	cmp	cx,SELCODE			; install real mode INT 1Ch callback
	mov	eax,newint1Ch
	jnz	@@1Ch0
	mov	eax,oldint1Ch
@@1Ch0:	mov	es:[4*1Ch],eax
	jmp	@@0

@@23h:	cmp	cx,SELCODE			; install real mode INT 23h callback
	mov	eax,newint23h
	jnz	@@23h0
	mov	eax,oldint23h
@@23h0:	mov	es:[4*23h],eax
	jmp	@@0

@@24h:	cmp	cx,SELCODE			; install real mode INT 24h callback
	mov	eax,newint24h
	jnz	@@24h0
	mov	eax,oldint24h
@@24h0:	mov	es:[4*24h],eax
	jmp	@@0


;=============================================================================
; Get and Disable Virtual Interrupt State
;
int31h_0900:
	add	esp,26h				; adjust stack
	pop	ds				; restore DS
	btr	wptr [esp+8],9			; test and clear IF bit in EFLAGS
	setc	al				; set AL = carry (IF flag from EFLAGS)
	jmp	int31oknopop			; return ok, dont pop registers


;=============================================================================
; Get and Enable Virtual Interrupt State
;
int31h_0901:
	add	esp,26h				; adjust stack
	pop	ds				; restore DS
	bts	wptr [esp+8],9			; test and set IF bit in EFLAGS
	setc	al				; set AL = carry (IF flag from EFLAGS)
	jmp	int31oknopop			; return ok, dont pop registers


;=============================================================================
; Get Virtual Interrupt State
;
int31h_0902:
	add	esp,26h				; adjust stack
	pop	ds				; restore DS
	bt	wptr [esp+8],9			; just test IF bit in EFLAGS
	setc	al				; set AL = carry (IF flag from EFLAGS)
	jmp	int31oknopop			; return ok, dont pop registers






;=============================================================================
; REAL/PROTECTED MODE TRANSLATION FUNCTIONS
;=============================================================================

;=============================================================================
; Call Real Mode Procedure with Far Return Frame
; Call Real Mode Procedure with Interrupt Return Frame
;
int31h_0301:
int31h_0302:
	mov	ebp,dptr es:[edi+2Ah]		; get target CS:IP from structure
	jmp	int3103				; go to common code


;=============================================================================
; Simulate Real Mode Interrupt
;
int31h_0300:
	movzx	ebx,bl				; get real mode INT CS:IP
	mov	ebp,dptr ds:[ebx*4]		; read from real mode interrupt table

int3103:					; common to 0300h, 0301h, and 0302h
	mov	gs,cs:seldata
	movzx	ebx,wptr es:[edi+2Eh]		; EBX = SP from register structure
	movzx	edx,wptr es:[edi+30h]		; EDX = SS from register structure
	mov	ax,bx				; check if caller provided stack
	or	ax,dx
	jnz	@@f3				; if yes, go on to setup stack

	mov	dx,cs:rmstacktop		; DX = SS for real mode redirection
	mov	bx,cs:rmstacklen		; get size of real mode stack
	sub	dx,bx				; adjust DX to next stack location
	cmp	dx,cs:rmstackbase		; exceeded real mode stack space?
	jb	int31fail8012			; if yes, error 8012h
	mov	gs:rmstacktop,dx		; update ptr for possible reenterancy
	shl	bx,4				; adjust BX from paragraphs to bytes

@@f3:	lea	edi,[edx*4]			; EDI -> top of real mode stack
	lea	edi,[edi*4+ebx]

	mov	ax,ss
	xchg	ax,gs:rmstackss			; preserve and set new top of stack
	push	ax				; parms for possible reenterancy
	lea	eax,[esp-4]
	xchg	eax,gs:rmstackesp
	push	eax

	movzx	ecx,cx
	mov	ax,cx				; EAX = length of stack parms
	add	ax,ax				; convert words to bytes
	sub	bx,2Eh				; adjust real mode SP for needed vars
	sub	bx,ax				; adjust real mode SP for stack parms

	push	ds es				; swap DS and ES
	pop	ds es

	std					; string copy backwards
	sub	edi,2				; copy stack parms from protected mode
	lea	esi,[ecx*2+esp+38h]		; stack to real mode stack
	rep	movs wptr es:[edi],ss:[esi]

	mov	esi,[esp+06h]			; ESI = offset of structure from stack
	mov	ax,ds:[esi+20h]			; AX = FLAGS from register structure
	cmp	bptr [esp+22h],1		; check AL on stack for function code
	jz	@@f4				; if function 0301h, go on
	and	ah,0FCh				; 0300h or 0302h, clear IF and TF flag
	stos	wptr es:[edi]			; put flags on real mode stack
	sub	bx,2

@@f4:	cld					; string copy forward
	lea	edi,[edx*4]			; EDI -> bottom of stack
	lea	edi,[edi*4+ebx]
	mov	cl,8				; copy general regs to real mode stack
	rep	movs dptr es:[edi],ds:[esi]
	add	esi,6				; copy FS and GS to real mode stack
	movs	dptr es:[edi],ds:[esi]

	mov	wptr es:[edi+8],_KERNEL		; return address from call
	mov	wptr es:[edi+6],offs @@f1
	mov	wptr es:[edi+4],ax		; store FLAGS for real mode IRET maybe
	mov	dptr es:[edi],ebp		; put call address to real mode stack
	mov	ax,[esi-6]			; real mode DS from register structure
	mov	cx,[esi-8]			; real mode ES from register structure
	mov	si,_KERNEL			; real mode target CS:IP
	mov	di,offs @@f0
	db 66h					; JMP DWORD PTR, as in 32bit offset,
	jmp	wptr cs:pmtormswrout		;  not seg:16bit offset

@@f0:	popad					; load regs with call values
	pop	fs gs
	iret					; go to call address

@@f1:	push	gs fs ds es			; store registers on stack
	pushf					; store flags on stack
	cli
	pushad

	xor	eax,eax
	mov	ax,ss				; EAX = linear ptr to SS
	xor	ebp,ebp
	shl	eax,4
	mov	bp,sp				; EBP = SP
	add	ebp,eax				; EBP -> stored regs on stack

	mov	dx,cs:rmstackss			; get protected mode SS:ESP from stack
	mov	ebx,cs:rmstackesp
	mov	ax,SELZERO			; DS selector value for protected mode
	mov	cx,SELDATA			; ES selector value for protected mode
	mov	si,SELCODE			; target CS:EIP in protected mode
	mov	edi,offs @@f2
	jmp	cs:rmtopmswrout			; go back to protected mode

@@f2:	push	es
	pop	gs
	pop	es:rmstackesp
	pop	es:rmstackss
	mov	esi,ebp				; copy return regs from real mode
	mov	edi,[esp]			; get structure offset from stack
	mov	es,[esp+24h]
	mov	ecx,15h				;  stack to register structure
	cld
	rep	movs wptr es:[edi],ds:[esi]

	cmp	dptr es:[edi+4],0		; stack provided by caller?
	jne	int31ok				; if yes, done now
	mov	ax,cs:rmstacklen		; restore top of real mode stack
	add	gs:rmstacktop,ax
	jmp	int31ok				; return ok


;=============================================================================
; Allocate Real Mode Callback Address
;
int31h_0303:
	mov	bl,cs:pm32_callbacks		; CL = total number of callbacks
	test	bl,bl				; are there any?
	jz	int31fail8015			; if no, error 8015h

	mov	edx,cs:callbackbase		; EDX -> base of callbacks
	mov	ecx,edx				; for later use

@@l0:	cmp	wptr [edx+3],0			; is this callback free?
	jz	@@f0				; if yes, allocate
	add	edx,25				; increment ptr to callback
	dec	bl				; decrement loop counter
	jnz	@@l0				; if more callbacks to check, loop
	jmp	int31fail8015			; no free callback, error 8015h

@@f0:	mov	bx,[esp+38]			; BX = caller DS from stack
	mov	[edx+3],bx			; store callback parms in callback
	mov	[edx+7],esi
	mov	[edx+12],es
	mov	[edx+16],edi
	sub	edx,ecx				; DX = offset of callback
	shr	ecx,4				; CX = segment of callback
	jmp	int31okdx			; return ok, with DX, CX, AX


;=============================================================================
; Free Real Mode Callback Address
;
int31h_0304:
	cmp	cx,cs:callbackseg		; valid callback segment?
	jne	int31fail8024			; if no, error 8024h

	movzx	ebx,dx				; EBX = offset of callback
	xor	ax,ax				; check if valid offset
	xchg	dx,ax
	mov	cx,25
	div	cx
	test	dx,dx				; is there a remainder
	jnz	int31fail8024			; if yes, not valid, error 8024h
	test	ah,ah				; callback index too big?
	jnz	int31fail8024			; if yes, not valid, error 8024h
	cmp	al,cs:pm32_callbacks		; callback index out of range?
	jae	int31fail8024			; if yes, not valid, error 8024h

	add	ebx,cs:callbackbase		; EBX -> callback
	mov	wptr [ebx+3],0			; set callback as free
	jmp	int31ok				; return ok


;=============================================================================
; Get State Save/Restore Addresses
;
int31h_0305:					; get state save/restore addresses
	add	esp,26h				; adjust stack
	pop	ds				; restore DS
	xor	ax,ax				; size needed is none
	mov	bx,cs:kernel_code		; real mode seg of same RETF
	mov	cx,offs vxr_saverestorerm	; same offset of 16bit RETF
	mov	si,cs				; selector of routine is this one
	mov	edi,offs vxr_saverestorepm	; offset of simple 32bit RETF
	jmp	int31oknopop			; return ok, dont pop registers


;=============================================================================
; Get Raw CPU Mode Switch Addresses
;
int31h_0306:					; get raw mode switch addresses
	add	esp,26h				; adjust stack
	pop	ds				; restore DS
	mov	si,cs				; selector of pmtorm rout is this one
	mov	edi,cs:pmtormswrout		; offset in this seg of rout
	mov	bx,cs:kernel_code		; real mode seg of rmtopm rout
	mov	cx,cs:rmtopmswrout		; offset of rout in real mode
	jmp	int31oknopop			; return ok, dont pop registers






;=============================================================================
; MISC FUNCTIONS
;=============================================================================

;=============================================================================
; Get DPMI Version
;
int31h_0400:
	add	esp,26h				; adjust stack
	pop	ds				; restore DS
	mov	ax,005Ah			; return version 0.9
	mov	bx,0003h			; capabilities
	cmp	cs:pmodetype,2
	jnz	@@1
	mov	bl,1
@@1:	mov	cl,cs:cputype			; processor type
	mov	dx,wptr cs:picslave		; master and slave PIC values
	jmp	int31oknopop			; return ok, don't pop registers


;=============================================================================
; Get Vendor-Specific API Entry Point
;
int31h_0A00:
	add	esp,26h				; adjust stack
	pop	ds				; restore DS

	push	es edi ecx esi			; save regs that will be modified
	push	cs				; ES = CS
	pop	es

	mov	ecx,15				; search for vendor1 string
	mov	edi,offs @@str1
	push	esi
	repe	cmps bptr ds:[esi],es:[edi]
	pop	esi
	mov	edi,offs @@ent1			; ES:EDI = sel:offset of entry SUNSYS
	jz	@@0				; if found, jump
	test	cs:pm32_mode,10000000b		; check if to ignore DOS/4G extensions
	jnz	@@err				; if not, we are done
	mov	cl,16				; search for vendor2 string
	mov	edi,offs @@str2
	repe	cmps bptr ds:[esi],es:[edi]
	jnz	@@err				; if not found, done
	mov	edi,offs @@ent2			; ES:EDI = sel:offset of entry DOS/4G
	pop	esi ecx
	add	esp,6
	jmp	int31oknopop

@@0:	add	esp,14
	xor	eax,eax				; clear high words
	mov	ebx,eax
	mov	ecx,eax
	mov	edx,eax
	mov	ax,cs:client_version		; AX = DOS Extender Version Number
	mov	bl,cs:pm32_mode			; BL = kernel configuration
	mov	bh,cs:pmodetype			; BH = system software type
	mov	cl,cs:cputype			; CL = processor type
	mov	ch,cs:fputype			; CH = FPU type
	mov	dx,wptr cs:picslave		; DX = PIC values (unremapped)
	jmp	int31oknopop

@@err:	pop	esi ecx edi es			; if none of the strings were idetified
	mov	ax,8001h			; return with AX=8001h
	jmp	int31failnopop

@@str1	db 'SUNSYS DOS/32A',0			; vendor1 API ID-string (DOS/32A)
@@str2	db 'RATIONAL DOS/4G',0			; vendor2 API ID-string (DOS/4G)

@@ent2:	mov	ax,8500h			; vendor2 API entry point (DOS/4G)
	jmp	dptr cs:client_call

@@ent1:	test	al,al				; vendor1 API entry point (DOS/32A)
	jz	API_func00
	cmp	al,01h
	jz	API_func01
	cmp	al,02h
	jz	API_func02
	cmp	al,03h
	jz	API_func03
	cmp	al,04h
	jz	API_func04
	cmp	al,05h
	jz	API_func05
	cmp	al,06h
	jz	API_func06
	cmp	al,07h
	jz	API_func07
	cmp	al,08h
	jz	API_func08
	cmp	al,09h
	jz	API_func09
	stc
	db 66h
	retf

;---------------------------------------------------------------------
API_func00:					; API function 00h: get access to IDT & GDT
	mov	bx,SELZERO
	movzx	ecx,cs:gdtlimit			; ECX = GDT limit
	movzx	edx,cs:idtlimit			; EDX = IDT limit
	mov	esi,cs:gdtbase			; BX:ESI = pointer to GDT
	mov	edi,cs:idtbase			; BX:EDI = pointer to IDT
	jmp	API_funcok

;---------------------------------------------------------------------
API_func01:					; API function 01h: get access to PageTables
	mov	bx,SELZERO
	movzx	ecx,cs:pagetables		; ECX = number of allocated pagetables
	movzx	edx,cs:pm32_maxfpages		; EDX = number of allocated phystables
	mov	esi,cs:pagetablebase		; BX:ESI = pointer to 0th pagetable
	mov	edi,cs:phystablebase		; BX:EDI = pointer to phystable
	inc	cx
	jmp	API_funcok

;---------------------------------------------------------------------
API_func02:					; API function 02: get access to INT tables
	mov	bx,SELDATA
	mov	esi,offs irqset_rm		; BX:ESI = pointer to INT switches
	mov	edi,offs irqtab_rm		; BX:EDI = pointer to INT tables
	jmp	API_funcok

;---------------------------------------------------------------------
API_func03:					; API function 03: get access to EXT memory
	mov	bx,SELZERO
	mov	ecx,cs:mem_free			; ECX = size of allocated memory
	mov	edx,cs:mem_ptr			; EDX = pointer to allocated memory
	mov	esi,cs:mem_top			; ESI = top of allocated memory
	jmp	API_funcok

;---------------------------------------------------------------------
API_func04:					; API function 04: get access to rm-stacks
	mov	bx,SELZERO
	movzx	ecx,cs:rmstacklen		; ECX = size of one stack
	movzx	edx,cs:rmstacktop		; EDX = pointer to top of stack
	movzx	esi,cs:rmstackbase		; ESI = base of stack area
	movzx	edi,cs:rmstacktop2		; EDI = default top of stack
	jmp	API_funcok

;---------------------------------------------------------------------
API_func05:					; API function 05: get access to pm-stacks
	mov	bx,SELZERO
	mov	ecx,cs:pmstacklen		; ECX = size of one stack
	mov	edx,cs:pmstacktop		; EDX = pointer to top of stack
	mov	esi,cs:pmstackbase		; ESI = base of stack area
	mov	edi,cs:pmstacktop2		; EDI = default top of stack
	jmp	API_funcok

;---------------------------------------------------------------------
API_func06:					; API function 06: get kernel selectors
	mov	bx,SELCODE			; BX = Kernel code selector
	mov	cx,SELDATA			; CX = Kernel data selector
	mov	dx,SELZERO			; DX = Kernel zero selector
	movzx	esi,wptr cs:kernel_code		; ESI = Kernel code segment
	mov	di,wptr cs:client_call[2]	; DI = Client code selector
	jmp	API_funcok

;---------------------------------------------------------------------
API_func07:					; API function 07: get critical handler entry
	mov	cx,cs:client_call[2]		; CX = default 16bit selector
	mov	dx,cs:client_call[0]		; DX = default 16bit offset
	jmp	API_funcok

;---------------------------------------------------------------------
API_func08:					; API function 08: set critical handler entry
	push	ds
	mov	ds,cs:seldata
	mov	client_call[2],cx		; CX = custom 16bit selector
	mov	client_call[0],dx		; DX = custom 16bit offset
	pop	ds
	jmp	API_funcok

;---------------------------------------------------------------------
API_func09:					; API function 09: get access to p. counters
	mov	cx,SELDATA			; CX = Kernel data selector
	mov	edx,offs _pc_base		; EDX = base of performance counters

;---------------------------------------------------------------------
API_funcok:
	clc
	db 66h
	retf






;=============================================================================
; MEMORY FUNCTIONS
;=============================================================================

;=============================================================================
; Get Free Memory Information
;
int31h_0500:
	or	eax,-1
	mov	ecx,0Ch
	push	edi
	rep	stos dptr es:[edi]
	pop	edi
	mov	eax,cs:mem_ptr
	or	eax,cs:mem_free
	jz	@@1
	call	int31_checkblocks
	call	int31_getfreemem

@@1:	mov	ebx,eax
	mov	edx,eax				; EDX = largest free block
	mov	eax,cs:mem_free
	shr	eax,12				; EAX = total memory pages
	shr	ebx,12				; EBX = free pages left
	shr	ecx,12				; ECX = total allocated memory
	push	es
	pop	ds
	mov	[edi+00h],edx			; 00h - largest free block
	mov	[edi+04h],ebx			; 04h - max unlocked pages
	mov	[edi+08h],ebx			; 08h - max locked pages
	mov	[edi+0Ch],eax			; 0Ch - total linear space
	mov	[edi+10h],ebx			; 10h -
	mov	[edi+14h],ecx			; 14h -
	mov	[edi+18h],eax			; 18h - total pages
	mov	[edi+1Ch],ecx			; 1Ch - total free mem in pages
	jmp	int31ok


;=============================================================================
; Allocate Memory Block
;
int31h_0501:
	call	int31_checkifmemavail
	call	int31_testbxcxtoebx		; convert BX:CX to EBX
	call	int31_checkblocks
	mov	esi,cs:mem_ptr			; get pointer to memory
@@1:	mov	eax,[esi+04h]			; get block size
	btr	eax,31				; check if memory block is used
	jc	@@2				; if yes, jump
	cmp	eax,ebx				; check if block is large enough
	jae	int31_allocblock		; if yes, allocate block
@@2:	lea	esi,[esi+eax+10h]		; load address of next memory block
	cmp	esi,cs:mem_top			; check if at top of memory
	jb	@@1				; if not, loop
	jmp	int31fail8013			; fail: not enough memory


;=============================================================================
; Free Memory Block
;
int31h_0502:
	shl	esi,16				; convert SI:DI to ESI
	mov	si,di
	call	int31_checkifmemavail
	call	int31_checkblocks
	call	int31_checkhandle
	btr	dptr [esi+04h],31		; set block as free
	call	int31_linkfreeblocks
	jmp	int31ok


;=============================================================================
; Resize Memory Block
;
int31h_0503:
	shl	esi,16				; convert SI:DI to ESI
	mov	si,di
	call	int31_checkifmemavail
	call	int31_testbxcxtoebx		; convert BX:CX to EBX
	call	int31_checkblocks
	call	int31_checkhandle

	mov	eax,[esi+04h]			; get size of this block
	btr	eax,31				; check if block is used
	jnc	int31fail8023			; if block is free, fail
	cmp	eax,ebx				; check if enough memory
	jae	int31_allocblock		; if yes, reallocate block
	mov	[esi+04h],eax			; set this block as free
	lea	edi,[esi+eax+10h]		; get address of next block
	cmp	edi,cs:mem_top			; check if at top of memory
	jae	@@0
	mov	edx,[edi+04h]			; get size of next block
	btr	edx,31				; check if block next to us is free
	jc	@@0				; if not, jump
	lea	edx,[eax+edx+10h]		; calculate total size (this+hdr+next)
	cmp	edx,ebx				; check if enough
	jb	@@0				; if not, jump
	mov	eax,edx				; set this size = (this + next)
	mov	[esi+04h],eax			; link this and next blocks
	jmp	int31_allocblock		; and go to the allocation routine

@@0:	mov	edi,cs:mem_ptr			; get pointer to memory
@@1:	mov	edx,[edi+04h]			; get block size
	btr	edx,31				; check if memory block is used
	jc	@@2				; if yes, jump
	cmp	edx,ebx				; check if block is large enough
	jae	@@3				; if yes, allocate block
@@2:	lea	edi,[edi+edx+10h]		; load address of next memory block
	cmp	edi,cs:mem_top			; check if at top of memory
	jb	@@1				; if not, loop
	bts	eax,31				; set this block as used
	mov	[esi+04h],eax			; restore state of this block
	jmp	int31fail8013			; fail: not enough memory

@@3:	push	esi edi
	mov	ecx,eax
	shr	ecx,2
	add	esi,10h
	add	edi,10h
	rep	movs dptr es:[edi],ds:[esi]
	mov	cl,al
	and	cl,3
	rep	movs bptr es:[edi],ds:[esi]
	pop	edi esi
	call	int31_linkfreeblocks
	mov	esi,edi
	mov	eax,edx
	jmp	int31_allocblock		; and go to the allocation routine


;=============================================================================
; Get Memory Block Size and Base
;
int31h_050A:
	shl	esi,16				; convert SI:DI to ESI
	mov	si,di
	call	int31_checkifmemavail
	call	int31_checkblocks
	call	int31_checkhandle
	mov	ebx,[esi+04h]			; check if block is used
	btr	ebx,31
	jnc	int31fail8023
	add	esi,10h
	xchg	ebx,esi
	mov	cx,bx
	shr	ebx,16
	mov	di,si
	shr	esi,16
	jmp	int31oksinoax



;-----------------------------------------------------------------------------
int31_checkifmemavail:				; check if memory had been allocated
	pop	bp
	push	eax
	mov	eax,cs:mem_ptr
	or	eax,cs:mem_free
	pop	eax
	jz	int31fail8013
	jmp	bp

;-----------------------------------------------------------------------------
int31_checkblocks:				; check if memory had been overwritten
	push	eax esi
	mov	esi,cs:mem_ptr
@@1:	test	si,000Fh			; blocks must be para aligned
	jnz	@@err
	mov	eax,12345678h			; header id
	cmp	eax,[esi+00h]			; if no header_id at block start
	jnz	@@err				; then signal error
	cmp	eax,[esi+0Ch]			; if no header_id at block end
	jnz	@@err				; then signal error
	mov	eax,[esi+04h]			; get block size
	btr	eax,31				; reset the used flag
	lea	esi,[esi+eax+10h]		; load address of next memory block
	cmp	esi,cs:mem_ptr
	jb	@@err
	cmp	esi,cs:mem_top			; check if at top of memory
	ja	@@err
	jb	@@1				; if not, loop
	pop	esi eax
@@done:	ret

@@err:	mov	ds,cs:seldata
	xor	eax,eax
	mov	mem_ptr,eax			; set to zero to prevent looping error
	mov	mem_free,eax			; when exiting
	mov	ax,8400h
	jmp	dptr client_call

;-----------------------------------------------------------------------------
int31_checkhandle:
	pop	bp				; check for valid handle in ESI
	cmp	esi,cs:mem_ptr
	jb	@@1
	cmp	esi,cs:mem_top
	ja	@@1
	mov	eax,12345678h
	cmp	eax,[esi+00h]
	jnz	@@1
	cmp	eax,[esi+0Ch]
	jnz	@@1
	jmp	bp
@@1:	jmp	int31fail8023			; fail: invalid handle

;-----------------------------------------------------------------------------
int31_testbxcxtoebx:				; convert BX:CX to EBX
	pop	bp
	shl	ebx,16
	mov	bx,cx
	test	ebx,ebx
	jz	int31fail8021			; BX:CX cannot be zero
	add	ebx,0Fh				; align EBX on para boundary
	and	bl,0F0h
	bt	ebx,31
	jc	int31fail8021			; cannot allocate that much memory
	jmp	bp

;-----------------------------------------------------------------------------
int31_getfreemem:
	xor	eax,eax				; reset free memory size
	xor	ecx,ecx
	mov	esi,cs:mem_ptr			; get pointer to memory
@@1:	mov	edx,[esi+04h]			; get block size
	btr	edx,31				; check if memory block is used
	jc	@@2				; if yes, jump
	add	ecx,edx
	cmp	eax,edx				; pick largest value
	ja	@@2
	mov	eax,edx
@@2:	lea	esi,[esi+edx+10h]		; load addres of next memory block
	cmp	esi,cs:mem_top			; check if at top of memory
	jb	@@1				; if not, loop
	ret

;-----------------------------------------------------------------------------
int31_allocblock:
	mov	ecx,12345678h
	movzx	edx,cs:id32_process_id
	sub	eax,ebx				; nextsize=actualsize-allocsize
	sub	eax,10h				; is nextsize<16 (header size)
	jb	@@1				; if yes, do not create next block
	lea	edi,[esi+ebx+10h]		; EDI = ptr to next block
	mov	[edi+00h],ecx			; header ID1
	mov	[edi+04h],eax			; set next block as free/size
	mov	[edi+08h],edx			; process_ID
	mov	[edi+0Ch],ecx			; header ID2

@@1:	bts	ebx,31				; set this block as used
	mov	[esi+00h],ecx			; header ID1
	mov	[esi+04h],ebx			; store this block size in header
	mov	[esi+08h],edx			; process_ID
	mov	[esi+0Ch],ecx			; header ID2
	call	int31_linkfreeblocks

	lea	ebx,[esi+10h]			; EBX = ptr to memory block  (-header)
	mov	cx,bx
	shr	ebx,16
	mov	di,si
	shr	esi,16
	jmp	int31oksinoax

;-----------------------------------------------------------------------------
int31_linkfreeblocks:
	pushad
	mov	edi,cs:mem_ptr			; EDI = ptr to 1st block handle
	mov	ebp,cs:mem_top

	mov	eax,[edi+04h]			; get 1st block size
	btr	eax,31
	lea	esi,[edi+eax+10h]		; ESI = ptr to 2nd block handle
	cmp	esi,ebp				; check if at top of memory
	jae	@@done				; if yes, done
	mov	esi,edi				; ESI=EDI = ptr to 1st block

@@1:	mov	eax,[esi+04h]			; get block size
	btr	eax,31				; check if block is used
	jc	@@4				; if yes, jump

	xor	ebx,ebx				; reset amount of free block memory
	xor	ecx,ecx				; reset number of free blocks in raw
	mov	edi,esi				; remember addr of first free block
	jmp	@@3

@@2:	add	ecx,10h				; increment number of free blocks
	mov	eax,[esi+04h]			; get block size
	btr	eax,31				; check if block is free
	lea	ebx,[eax+ebx]			; amount of free memory encountered
	jnc	@@3				; if yes, jump
	sub	ebx,eax
	sub	ecx,10h
	add	ebx,ecx
	add	[edi+04h],ebx
	jmp	@@4

@@3:	lea	esi,[esi+eax+10h]		; calculate address of next block
	cmp	esi,ebp				; check if at top of memory
	jb	@@2				; if not, loop
	add	ebx,ecx
	add	[edi+04h],ebx
	jmp	@@done

@@4:	lea	esi,[esi+eax+10h]		; calculate address of next block
	cmp	esi,ebp				; check if at top of memory
	jb	@@1				; if not, loop

@@done:	popad
	ret






;=============================================================================
; VIRTUAL MEMORY FUNCTIONS (not supported)
;=============================================================================

;=============================================================================
; Lock Linear Region
; Unlock Linear Region
; Mark Real Mode Region as Pageable
; Relock Real Mode Region
; Mark Page as Demand Paging Candidate
; Discard Page Contents
;
int31h_0600:
int31h_0601:
int31h_0602:
int31h_0603:
int31h_0702:
int31h_0703:
	jmp	int31ok


;=============================================================================
; Get Page Size
;
int31h_0604:
	xor	bx,bx
	mov	cx,1000h
	jmp	int31okbx





;=============================================================================
; PHYSICAL MEMORY MAPPING FUNCTIONS
;=============================================================================

;=============================================================================
; Physical Address Mapping
;
int31h_0800:
	shl	ebx,16				; convert BX:CX to EBX
	shl	esi,16				; convert SI:DI to ESI
	mov	bx,cx
	mov	si,di
	cmp	ebx,100000h			; check if mapping under 1MB limit
	jb	int31fail8021			; if yes, error
	test	esi,esi				; check if size is zero
	jz	int31fail8021			; if yes, error
	cmp	cs:pmodetype,2			; check if system is VCPI
	jz	@@vcpi				; if yes, do VCPI memory mapping
	jmp	int31ok				; if under raw/XMS, do nothing

@@vcpi:	cmp	cs:pm32_maxfpages,0		; check if any linear memory avail.
	jz	int31fail8012			; if not, signal error 8012h

	push	ds
	pop	es
	mov	edi,cs:phystablebase		; get base of pagetables
@@0:	mov	eax,ebx				; EAX = physical address
	lea	ecx,[esi+0FFFh]			; ECX = size of memory to map
	and	ax,0F000h
	shr	ecx,12
	mov	edx,edi
@@1:	cmp	edi,cs:phystabletop		; are there any pages mapped left
	jae	@@3				; no, go on with mapping

	mov	ebp,[edi]
	and	bp,0F000h
	cmp	eax,ebp				; check if page already mapped
	jz	@@2				; if yes, go to check next page
	add	edi,4				; increment pointer into pagetables
	jmp	@@0				; loop

@@2:	add	edi,4				; increment pointer into pagetables
	add	eax,4096
	dec	ecx				; decrement amount of pages checked
	jnz	@@1				; if there are more left, loop
	mov	eax,ebx
	and	ax,0F000h
	sub	ebx,eax
	jmp	@@done				; region already mapped, we are done

@@3:	mov	edi,cs:phystablebase		; get base of pagetables
	mov	ecx,cs:phystabletop		; get number of available pagetables
	sub	ecx,edi
	shr	ecx,2				; convert to 4KB pages
	mov	eax,ebx
	and	ax,0F000h
	lea	ebp,[ebx+esi+0FFFh]		; EBP = number of required 4KB pages
	sub	ebp,eax
	shr	ebp,12

@@4:	test	ecx,ecx				; check if no linear space left
	jz	int31fail8021			; if yes, error
	xor	eax,eax
	repne	scas dptr es:[edi]		; scan for first free page
	lea	edx,[edi-4]			; EDX = first free page address
	repe	scas dptr es:[edi]		; scan for amount of free pages
	mov	eax,edi
	sub	eax,edx
	shr	eax,2				; EAX = free pages available
	cmp	eax,ebp				; check if enough free pages
	jb	@@4				; no, must loop

	mov	eax,ebx				; EAX = physical address
	and	ax,0F000h
	sub	ebx,eax
	mov	edi,edx				; EDI = address of first free page
	mov	ecx,ebp
	mov	al,07h				; set page as user/writeable/present
	cmp	cs:cputype,3			; check if CPU is 486+
	jbe	@@loop				; if not, jump
	mov	al,1Fh				; set PCD and PWT bits (no page cache)

@@loop:	stos	dptr es:[edi]			; map one 4KB page
	add	eax,4096			; go for next page
	loop	@@loop				; loop until no pages left
	or	bptr [edx+1],2			; mark start of mapped block
	or	bptr [edi-3],4			; mark end of mapped block

@@done:	sub	edx,cs:phystablebase
	shl	edx,10
	add	edx,ebx

	mov	[esp+18h],dx
	shr	edx,16
	or	dx,8000h
	mov	[esp+10h],dx

	mov	eax,cs:vcpi_cr3
	mov	cr3,eax
	xor	eax,eax
	mov	cr2,eax
	jmp	int31ok


;=============================================================================
; Free Physical Address Mapping
;
int31h_0801:
	cmp	cs:pmodetype,2
	jz	@@vcpi
	jmp	int31ok				; if under raw/XMS, do nothing

@@vcpi:	cmp	cs:pm32_maxfpages,0		; check if any linear memory avail.
	jz	int31fail8012			; if not, signal error 8012h

	and	bx,7FFFh
	shl	ebx,16
	mov	bx,cx
	shr	ebx,10
	and	bl,0FCh

	add	ebx,cs:phystablebase
	cmp	ebx,cs:phystablebase		; check if addr is in range
	jb	int31fail8025
	cmp	ebx,cs:phystabletop
	jae	int31fail8025
	test	bptr [ebx+1],2
	jz	int31fail8025

@@loop:	xor	eax,eax				; clear page table entries
	xchg	eax,[ebx]
	add	ebx,04h
	test	ah,04h
	jz	@@loop

@@1:	mov	eax,cs:vcpi_cr3
	mov	cr3,eax
	xor	eax,eax
	mov	cr2,eax
	jmp	int31ok






;=============================================================================
; FPU RELATED FUNCTIONS
;=============================================================================

;=============================================================================
; Get Coprocessor Status
;
int31h_0E00:
	movzx	ax,cs:fputype			; get FPU type
	shl	ax,4				; put FPU type in bits 4..7

	smsw	dx				; get MSW in DX
	and	dl,06h				; mask MP & EM bits
	shr	dl,1				; map MP & EM bits to MPv & EMv bits
	or	al,dl				; put MPv & EMv bits into AL
	shl	dl,2				; map MP & EM bits to MPr & EMr bits
	or	al,dl				; put MPr & EMr bits into AL

	jmp	int31okax


;=============================================================================
; Set Coprocessor Emulation
;
int31h_0E01:
	mov	ds,cs:seldata
	and	bl,00000011b			; mask MPv & EMv bits
	shl	bl,1				; map MPv & EMv bits to MP & EM bits

	smsw	ax				; get MSW in AX
	or	al,bl				; set MP & EM bits
	lmsw	ax				; reload MSW

	jmp	int31ok






;=============================================================================
; NON-STANDARD FUNCTIONS
;=============================================================================

;=============================================================================
; PMODE/W id function
;
int31h_EEFF:
	add	esp,26h				; adjust stack
	pop	ds				; restore DS
	push	cs
	pop	es				; ES = CS
	mov	eax,'D32A'			; load id-string
	mov	ebx,offs $+2			; ES:EBX points to itself ('\0' within opcode)
	mov	ch,cs:pmodetype
	mov	cl,cs:cputype
	mov	dx,cs:client_version
	jmp	int31oknopop

