.386

;*****************************
;* Equates                   *
;*****************************

SWAPPERERROR	EQU	9999
WEDGEOFFSET	EQU	180h

;*****************************
;* Public declarations       *
;*****************************

PUBLIC	CWSwap
;PUBLIC	SWPRUNCMD
;PUBLIC	OverLay
;PUBLIC	O_RUNX

;PUBLIC	Done

;*****************************
;* External data             *
;*****************************

;EXTRN	__retni:FAR
;EXTRN	__parclen:FAR
;EXTRN	__parc:FAR

;*****************************
;* SPAWN Structures          *
;*****************************

;
;	--- Version 3.3 92-03-30 21:39 ---
;
;	SPAWN.ASM - Main function for memory swapping spawn call.
;
;	Public Domain Software written by
;		Thomas Wagner
;		Ferrari electronic GmbH
;		Beusselstrasse 27
;		D-1000 Berlin 21
;		Germany
;
;
; Assemble with
;
; tasm  /DPASCAL spawn,spawnp  		- Turbo Pascal (Tasm only), near
; tasm  /DPASCAL /DFARCALL spawn,spawnp	- Turbo Pascal (Tasm only), far
; ?asm  spawn;		  		- C, default model (small)
; ?asm  /DMODL=large spawn  		- C, large model
;
;	NOTE:	For C, change the 'model' directive below according to your
;		memory model, or define MODL=xxx on the command line.
;
;		For Turbo C Huge model, you must give /DTC_HUGE on the
;		command line, or define it here.
;
;
; Main function:
;

;   C:
;   	int do_spawn (int swapping,
;		      char *execfname,
;		      char *cmdtail,
;		      unsigned envlen,
;		      char *envp,
;		      char *stdin,
;		      char *stdout,
;		      char *stderr)
;
;   Parameters:
;
;	swapping - swap/spawn/exec function:
;			< 0: Exec, don't swap
;		  	  0: Spawn, don't swap
;			> 0: Spawn, swap
;			     in this case, prep_swap must have
;			     been called beforehand (see below).
;
;	cmdtail - command tail for EXEC.
;
;	execfname - name and path of file to execute.
;
;	envlen - length of environment copy (may be 0).
;
;	envp -  pointer to environment block (must be aligned on
;		paragraph boundary). Unused if envlen is 0.
;
;	'cmdtail' and 'execfname' must be zero terminated, even when
;	calling from Pascal. For Pascal, the length byte of the string
;	is ignored.
;
;   Returns:
;	0000..00ff:	Returncode of EXECed program
;	03xx:		DOS-Error xx calling EXEC
;	0500:		Swapping requested, but prep_swap has not
;			been called or returned an error
;	0501:		MCBs don't match expected setup
;	0502:		Error while swapping out
;	06xx:		DOS-Error xx on redirection
;
;
; For swapping, the swap method must be prepared before calling do_spawn.
;
;   C:
;	int prep_swap (unsigned method, char *swapfname)
;
;   Parameters:
;
;	method	- bit-map of allowed swap devices:
;			01 - Allow EMS
;			02 - Allow XMS
;			04 - Allow File swap
;			10 - Try XMS first, then EMS
;			40 - Create file as "hidden"
;			80 - Use "create temp" call for file swap
;		       100 - Don't preallocate file
;		       200 - Check for Network, don't preallocate if net
;		      4000 - Environment block will not be swapped
;
;	swapfname - swap file name (may be undefined if the
;		    "method" parameters disallows file swap).
;		    The string must be zero terminated, even
;		    when calling from Pascal. For Pascal, the
;		    length byte of the string is ignored.
;
;   Returns:
;
;   	A positive integer on success:
;		1 - EMS swap initialized
;		2 - XMS swap initialized
;		4 - File swap initialized
;	A negative integer on failure:
;		-1 - Couldn't allocate swap space
;		-2 - The spawn module is located too low in memory
;
;--------------------------------------------------------------------------
;
;
;	Set NO_INHERIT to FALSE if you don't want do_exec to mess with
;	the handle table in the PSP, and/or you do want the child process
;	to inherit all open files.
;	If NO_INHERIT is TRUE, only the first five handles (the standard
;	ones) will be inherited, all others will be hidden. This allows
;	the child to open more files, and also protects you from the child
;	messing with any open handles.
;	NO_INHERIT should always be TRUE if you use functions to extend
;	the handle table (for more than 20 open files).
;
;	Set REDIRECT to FALSE if you do not want do_spawn to support redirection.
;
;
;
FALSE		=	0
TRUE		=	NOT FALSE
;
NO_INHERIT	=	TRUE
REDIRECT	=	FALSE
;
;
;
;	IFNDEF	MODL
;	.model	small,c
;	%out	small model
;	ELSE
;%	.model	MODL,c
;%	%out	MODL model
;	ENDIF
;
;ptrsize	=	@DataSize
ptrsize	=	1
;
;	extrn	_psp: word
;
;	public	do_spawn
;	public	prep_swap
;	public	swap_prep
;
stacklen	=	256		; local stack
;
;	"ems_size" is the EMS block size: 16k.
;
ems_size	=	16 * 1024	; EMS block size
ems_parasize	=	ems_size / 16	; same in paragraphs
ems_shift	=	10		; shift factor for paragraphs
ems_paramask	=	ems_parasize-1	; block mask
;
;	"xms_size" is the unit of measurement for XMS: 1k
;
xms_size	=	1024		; XMS block size
xms_parasize	=	xms_size / 16	; same in paragraphs
xms_shift	=	6		; shift factor for paragraphs
xms_paramask	=	xms_parasize-1	; block mask
;
;	Method flags
;
USE_EMS		=	01h
USE_XMS		=	02h
USE_FILE	=	04h
XMS_FIRST	=	10h
HIDE_FILE	=	40h
CREAT_TEMP	=	80h
NO_PREALLOC	=	100h
CHECK_NET	=	200h
DONT_SWAP_ENV	=	4000h
;
;	Return codes
;
RC_TOOLOW	=	0102h
RC_BADPREP	=	0500h
RC_MCBERROR	=	0501h
RC_SWAPERROR	=	0502h
RC_REDIRFAIL	=	0600h
;
EMM_INT		=	67h
;
;	The EXEC function parameter block
;
exec_block	struc
envseg	dw	?		; environment segment
ppar	dw	?		; program parameter string offset
pparseg	dw	?		; program parameter string segment
fcb1	dw	?		; FCB offset
fcb1seg	dw	?		; FCB segment
fcb2	dw	?		; FCB offset
fcb2seg	dw	?		; FCB segment
exec_block	ends
;
;	Structure of an XMS move control block
;
xms_control	struc
lenlo		dw	?	; length to move (doubleword)
lenhi		dw	?
srchnd		dw	?	; source handle (0 for standard memory)
srclo		dw	?	; source address (doubleword or seg:off)
srchi		dw	?
desthnd		dw	?	; destination handle (0 for standard memory)
destlo		dw	?	; destination address (doubleword or seg:off)
desthi		dw	?
xms_control	ends
;
;	The structure of the start of an MCB (memory control block)
;
mcb		struc
id		db	?
owner		dw	?
paras		dw	?
mcb		ends
;
;	The structure of an internal MCB descriptor.
;	CAUTION: This structure is assumed to be no larger than 16 bytes
;	in several places in the code, and to be exactly 16 bytes when
;	swapping in from file. Be careful when changing this structure.
;
mcbdesc		struc
addr		dw	?	; paragraph address of the MCB
msize		dw	?	; size in paragraphs (excluding header)
swoffset	dw	?	; swap offset (0 in all blocks except first)
swsize		dw	?	; swap size (= msize + 1 except in first)
num_follow	dw	?	; number of following MCBs
		dw	3 dup(?) ; pad to paragraph (16 bytes)
mcbdesc		ends
;
;	The variable block set up by prep_swap
;
prep_block	struc
xmm		dd	?		; XMM entry address
first_mcb	dw	?		; Segment of first MCB
psp_mcb		dw	?		; Segment of MCB of our PSP
env_mcb		dw	?		; MCB of Environment segment
noswap_mcb	dw	?		; Env MCB that may not be swapped
noswap2_mcb	dw	?		; Wedge MCB that may not be swapped
noswap3_mcb	dw	?		; File handle MCB that may not be swapped
ems_pageframe	dw	?		; EMS page frame address
handle		dw	?		; EMS/XMS/File handle
total_mcbs	dw	?		; Total number of MCBs
swapmethod	db	?		; Method for swapping
sp_swapfilename	db	81 dup(?)	; Swap file name if swapping to file
prep_block	ends
;
;----------------------------------------------------------------------
;
;	Since we'll be moving code and data around in memory,
;	we can't address locations in the resident block with
;	normal address expressions. MASM does not support
;	defining variables with a fixed offset, so we have to resort
;	to a kludge, and define the shrunk-down code as a structure.
;	It would also be possible to use an absolute segment for the
;	definition, but this is not supported by the Turbo Pascal linker.
;
;	All references to low-core variables from low-core itself
;	are made through DS, so we define a text macro "lmem" that
;	expands to "ds:". When setting up low core from the normal
;	code, ES is used to address low memory, so this can't be used.
;
lmem	equ	<ds:>
;
;	The memory structure for the shrunk-down code, excluding the
;	code itself. The code follows this block.
;	The start of this block is the PSP.
;
parseg		struc
		db	18h dup(?)
psp_handletab	db	20 dup(?)	; Handle Table
psp_envptr	dw	?		; Environment Pointer
		dd	?
psp_handlenum	dw	?		; Number of Handles (DOS >= 3.3)
psp_handleptro	dw	?		; Handle Table Pointer (DOS >= 3.3)
psp_handleptrs	dw	?		; Handle Table Pointer Segment
		db	5ch-38h dup(?)	; start after PSP
;
save_ss		dw	?		; 5C - saved global ss
save_sp		dw	?		; 5E - saved global sp
xfcb1		db	16 dup(?)	; 60..6F - default FCB
xfcb2		db	16 dup(?)	; 70..7F - default FCB
zero		dw	?		; 80 Zero command tail length (dummy)
;
expar		db	TYPE exec_block dup (?) ; exec-parameter-block
spx		dw	?		; saved local sp
div0_off	dw	?		; divide by zero vector save
div0_seg	dw	?
lhandlesave	db	26 dup(?)	; saved handle table and pointer
		IF	REDIRECT
lredirsav	db	6 dup(?)	; saved redirection handles
lstdinsav	dw	3 dup(?)	; duped redirection handles
		ENDIF
filename	db	82 dup(?)	; exec filename
progpars	db	128 dup(?)	; command tail
		db	stacklen dup(?)	; local stack space
mystack		db	?
lprep		db	TYPE prep_block dup(?)	; the swapping variables
lcurrdesc	db	TYPE mcbdesc dup(?)	; the current MCB descriptor
lxmsctl		db	TYPE xms_control dup(?)
eretcode	dw	?		; EXEC return code
retflags	dw	?		; EXEC return flags
cgetmcb		dw	?		; address of get_mcb
dtasave		DD	?		; address of original DTA
GoToWedge	DD	?		; address of wedge code
;
parseg	ends
;
param_len	=	((TYPE parseg + 1) / 2) * 2	; make even
codebeg		=	param_len
;

;*****************************
;* Code begins               *
;*****************************

SWAP_TEXT	SEGMENT PARA PRIVATE USE16 'CODE'

ASSUME	cs:SWAP_TEXT

CodeStart	=	$

;lookie	DW	keep_paras

;
;------------------------------------------------------------------------
;
lowcode_begin:
;
;       The following parts of the program code will be moved to
;	low core and executed there, so there must be no absolute
;	memory references.
;	The call to get_mcb must be made indirect, since the offset
;	from the swap-in routine to get_mcb will not be the same
;	after moving.
;
;
;	get_mcb allocates a block of memory by modifying the MCB chain
;	directly.
;
;	On entry, lcurrdesc has the mcb descriptor for the block to
;		  allocate.
;
;	On exit,  Carry is set if the block couldn't be allocated.
;
;	Uses 	AX, BX, CX, ES
;	Modifies lprep.first_mcb
;
get_mcb	proc	near
;
	mov	ax,lmem lprep.first_mcb
	mov	bx,lmem lcurrdesc.addr
;
getmcb_loop:
	mov	es,ax
	cmp	ax,bx
	ja	gmcb_abort		; halt if MCB > wanted
	je	mcb_found		; jump if same addr as wanted
	add	ax,es:paras		; last addr
	inc	ax			; next mcb
	cmp	ax,bx
	jbe	getmcb_loop		; Loop if next <= wanted
;
;
;	The wanted MCB starts within the current MCB. We now have to
;	create a new MCB at the wanted position, which is initially
;	free, and shorten the current MCB to reflect the reduced size.
;
	cmp	es:owner,0
	jne	gmcb_abort		; halt if not free
	mov	bx,es			; current
	inc	bx			; + 1 (header doesn't count)
	mov	ax,lmem lcurrdesc.addr
	sub	ax,bx			; paragraphs between MCB and wanted
	mov	bx,es:paras		; paras in current MCB
	sub	bx,ax			; remaining paras
	dec	bx			; -1 for header
	mov	es:paras,ax		; set new size for current
	mov	cl,es:id		; old id
	mov	es:id,4dh		; set id: there is a next
	mov	ax,lmem lcurrdesc.addr
	mov	es,ax
	mov	es:id,cl		; and init to free
	mov	es:owner,0
	mov	es:paras,bx
;
;	We have found an MCB at the right address. If it's not free,
;	abort. Else check the size. If the size is ok, we're done
;	(more or less).
;
mcb_found:
	mov	es,ax
	cmp	es:owner,0
	je	mcb_check		; continue if free
;
gmcb_abort:
	stc
	ret
;
mcb_check:
	mov	ax,es:paras		; size
	cmp	ax,lmem lcurrdesc.msize	; needed size
	jae	mcb_ok			; ok if enough space
;
;	If there's not enough room in this MCB, check if the next
;	MCB is free, too. If so, coalesce both MCB's and check again.
;
	cmp	es:id,4dh
	jnz	gmcb_abort		; halt if no next
	push	es			; save current
	mov	bx,es
	add	ax,bx
	inc	ax			; next MCB
	mov	es,ax
	cmp	es:owner,0		; next free ?
	jne	gmcb_abort		; halt if not
	mov	ax,es:paras		; else load size
	inc	ax			; + 1 for header
	mov	cl,es:id		; and load ID
	pop	es			; back to last MCB
	add	es:paras,ax		; increase size
	mov	es:id,cl		; and store ID
	jmp	mcb_check		; now try again
;
;	The MCB is free and large enough. If it's larger than the
;	wanted size, create another MCB after the wanted.
;
mcb_ok:
	mov	bx,es:paras
	sub	bx,lmem lcurrdesc.msize
	jz	mcb_no_next		; ok, no next to create
	push	es
	dec	bx			; size of next block
	mov	ax,es
	add	ax,lmem lcurrdesc.msize
	inc	ax			; next MCB addr
	mov	cl,es:id		; id of this block
	mov	es,ax			; address next
	mov	es:id,cl		; store id
	mov	es:paras,bx		; store size
	mov	es:owner,0		; and mark as free
	pop	es			; back to old MCB
	mov	es:id,4dh		; mark next block present
	mov	ax,lmem lcurrdesc.msize	; and set size to wanted
	mov	es:paras,ax
;
mcb_no_next:
	mov	es:owner,cx		; set owner to current PSP
;
;	Set the 'first_mcb' pointer to the current one, so we don't
;	walk through all the previous blocks the next time.
;	Also, check if the block we just allocated is the environment
;	segment of the program. If so, restore the environment pointer
;	in the PSP.
;
	mov	ax,es
	mov	lmem lprep.first_mcb,ax
;	cmp	lmem lprep.env_mcb,ax
;	jne	getmcb_finis
;	inc	ax
;	mov	lmem psp_envptr,ax
;
getmcb_finis:
	clc
	ret				; all finished (whew!)
;
get_mcb	endp
;
;
ireti:
	iret
;
;
;	The actual EXEC call.
;	Registers on entry:
;		BX	= paragraphs to keep (0 if no swap)
;		CX 	= length of environment to copy (words) or zero
;		DS:SI	= environment source
;		ES:DI	= environment destination
;		(ES = our low core code segment)
;
;
;	copy environment buffer down if present
;
doexec:
;	jcxz	noenvcpy
;	rep movsw
;
noenvcpy:
	push	es			; DS = ES = low core = PSP
	pop	ds
	or	bx,bx
	jz	no_shrink
;
;	first, shrink the base memory block down.
;
;	int	3

IFDEF SHRINKPSP
	mov	ah,04ah
	int	21h                     ; resize memory block
ENDIF
;
;	Again walk all MCBs. This time, all blocks owned by the
;	current process are released.
;
	mov	si,lmem lprep.first_mcb
	or	si,si
	jz	no_shrink
	mov	dx,lmem lprep.psp_mcb
	mov	bx,dx
	inc	bx			; base PSP (MCB owner)
	mov	di,lmem lprep.noswap_mcb
	mov	cx,lmem lprep.noswap2_mcb
;
free_loop:
	cmp	si,dx
	je	free_next		; don't free base block
	cmp	si,di
	je	free_next
	cmp	si,cx
	je	free_next
	cmp	si,lmem lprep.noswap3_mcb
	je	free_next

	mov	es,si
	cmp	bx,es:owner		; our process?
	jne	free_next		; next if not
;	cmp	si,lmem lprep.env_mcb	; is this the environment block?
;	jne	free_noenv
;	mov	ds:psp_envptr,0		; else clear PSP pointer
;
free_noenv:
	inc	si
	mov	es,si
	dec	si
	mov	ah,049h			; free memory block
	int	21h
;
free_next:
	mov	es,si
	cmp	es:id,4dh		; normal block?
	jne	free_ready		; ready if end of chain
	add	si,es:paras		; start + length
	inc	si			; next MCB
	jmp	free_loop
;
free_ready:
no_shrink:
	mov	ah,2fh		; get DTA address
	int	21h
	mov	WORD PTR lmem dtasave,bx	; save original DTA
	mov	WORD PTR lmem dtasave+2,es
	mov	ax,ds
	mov	es,ax
	mov	WORD PTR lmem GoToWedge,0
	mov	WORD PTR lmem GoToWedge+2,fs
;

;	int	3

	mov	dx,filename		; params for exec
	mov	bx,expar

	call	dword ptr lmem [GoToWedge]
;	mov	ax,04b00h
;	int	21h			; exec
;
;	Return from EXEC system call. Don't count on any register except
;	CS to be restored (DOS 2.11 and previous versions killed all regs).
;
	mov	bx,cs
	mov	ds,bx
	mov	es,bx
	cli
	mov	ss,bx
	mov	sp,lmem spx
	sti
	cld
	mov	lmem eretcode,ax	; save return code
	pushf
	pop	bx
	mov	lmem retflags,bx	; and returned flags

; restore dta
	push	ds
	lds	dx,lmem dtasave
	mov	ah,1ah		; set DTA address
	int	21h
	pop	ds
;
;	Cancel Redirection
;
	IF	REDIRECT
	mov	si,lredirsav
	mov	di,psp_handletab+5
	mov	cx,3
	rep movsw
	mov	si,lstdinsav
	xor	cx,cx
;
lredirclose:
	lodsw
	cmp	ax,-1
	je	lredclosenext
	mov	bx,ax
	mov	ah,46h
	int	21h
;
lredclosenext:
	inc	cx
	cmp	cx,3
	jb	lredirclose
	ENDIF
;
IFDEF SHRINKPSP
	cmp	lmem lprep.swapmethod,0
	je	exec_memok
	jg	exec_expand
ELSE
	jmp	NEAR PTR exec_memok
ENDIF
;
;	Terminate.
;
	test	lmem retflags,1		; carry?
	jnz	exec_term		; use EXEc retcode if set
	mov	ah,4dh			; else get program return code
	int	21h
;
exec_term:
	mov	ah,4ch
	int	21h
;

;
exec_expand:
IFDEF SHRINKPSP
	mov	ah,4ah			; expand memory
	mov	bx,lmem lcurrdesc.msize
	int	21h
	jnc	exec_memok
	mov	ax,4cffh
	int	21h			; terminate on error
ENDIF
;
;	Swap memory back
;
	nop
	nop
	nop

;
exec_memok:
;
;	FALL THROUGH to the appropriate swap-in routine
;
;
getmcboff	=	offset get_mcb - offset lowcode_begin
iretoff		=	offset ireti - offset lowcode_begin
doexec_entry	=	offset doexec - offset lowcode_begin
base_length	=	offset $ - offset lowcode_begin
;
;-----------------------------------------------------------------------
;
;	The various swap in routines follow. Only one of the routines
;	is copied to low memory.
;	Note that the routines are never actually called, the EXEC return
;	code falls through. The final RET thus will return to the restored
;	memory image.
;
;	On entry, DS must point to low core.
;	On exit to the restored code, DS is unchanged.
;
;
;	swapin_ems:	swap in from EMS.
;
swapin_ems	proc	far
;
	xor	bx,bx
	mov	si,ems_parasize
	mov	dx,lmem lprep.handle	; EMS handle
;
swinems_main:
	push	ds
	mov	cx,lmem lcurrdesc.swsize	; block length in paras
	mov	di,lmem lcurrdesc.swoffset	; swap offset
	mov	es,lmem lcurrdesc.addr		; segment to swap
	mov	ds,lmem lprep.ems_pageframe	; page frame address
;
	mov	ax,ems_parasize		; max length
	sub	ax,si			; minus current offset
	jnz	swinems_ok		; go copy if nonzero
;
swinems_loop:
	mov	ax,4400h		; map in next page
	int	EMM_INT
	or	ah,ah
	jnz	swinems_error
	mov	si,0			; reset offset
	inc	bx			; bump up page number
	mov	ax,ems_parasize		; max length to copy
;
swinems_ok:
	cmp	ax,cx			; length to copy
	jbe	swinems_doit		; go do it if <= total length
	mov	ax,cx			; else use total length
;
swinems_doit:
	sub	cx,ax			; subtract copy length from total
	push	cx			; and save
	push	ax			; save the copy length in paras
	push	si
	push	di
	mov	cl,3
	shl	ax,cl			; convert to number of words (!)
	inc	cl
	shl	si,cl			; convert to byte address
	mov	cx,ax
	rep movsw
	pop	di
	pop	si
	pop	cx			; copy length in paras
	mov	ax,es
	add	ax,cx			; add copy length to dest segment
	add	si,cx			; and EMS page offset
	mov	es,ax
	pop	cx			; remaining length
	or	cx,cx			; did we copy everything?
	jnz	swinems_loop		; go loop if not
;
	pop	ds
	cmp	lmem lcurrdesc.num_follow,0	; another MCB?
	je	swinems_complete	; exit if not
;
;	Another MCB follows, read next mcb descriptor into currdesc
;
	cmp	si,ems_parasize
	jb	swinems_nonewpage	; no new block needed
	mov	ax,4400h		; map page, phys = 0
	int	EMM_INT
	or	ah,ah
	jnz	swinems_error1
	mov	si,0
	inc	bx
;
swinems_nonewpage:
	push	si
	push	ds
	mov	ax,ds
	mov	es,ax
	mov	ds,lmem lprep.ems_pageframe	; page frame address
	mov	cl,4
	shl	si,cl			; convert to byte address
	mov	cx,TYPE mcbdesc
	mov	di,lcurrdesc
	rep movsb
	pop	ds
	pop	si
	inc	si			; one paragraph
;
	push	bx
	call	lmem cgetmcb
	pop	bx
	jc	swinems_error1
	jmp	swinems_main
;
swinems_complete:
	mov	ah,45h			; release EMS pages
	int	EMM_INT
	ret
;
swinems_error:
	pop	ds
swinems_error1:
	mov	ah,45h			; release EMS pages on error
	int	EMM_INT
	mov	ax,4cffh
	int	21h			; terminate
;
swapin_ems	endp
;
swinems_length	= offset $ - offset swapin_ems
;
;
;	swapin_xms:	swap in from XMS.
;
swapin_xms	proc	far
;
	mov	ax,lmem lprep.handle	; XMS handle
	mov	lmem lxmsctl.srchnd,ax 	; source is XMS
	mov	lmem lxmsctl.desthnd,0 	; dest is normal memory
	mov	lmem lxmsctl.srclo,0
	mov	lmem lxmsctl.srchi,0
;
swinxms_main:
	mov	ax,lmem lcurrdesc.swsize ; size in paragraphs
	mov	cl,4
	rol	ax,cl			; size in bytes + high nibble
	mov	dx,ax
	and	ax,0fff0h		; low word
	and	dx,0000fh		; high word
	mov	lmem lxmsctl.lenlo,ax	; into control block
	mov	lmem lxmsctl.lenhi,dx
	mov	ax,lmem lcurrdesc.swoffset	; swap offset
	mov	lmem lxmsctl.destlo,ax 		; into control block
	mov	ax,lmem lcurrdesc.addr		; segment to swap
	mov	lmem lxmsctl.desthi,ax
	mov	si,lxmsctl
	mov	ah,0bh
	call	lmem lprep.xmm		; move it
	or	ax,ax
	jz	swinxms_error
	mov	ax,lmem lxmsctl.lenlo	; adjust source addr
	add	lmem lxmsctl.srclo,ax
	mov	ax,lmem lxmsctl.lenhi
	adc	lmem lxmsctl.srchi,ax
;
	cmp	lmem lcurrdesc.num_follow,0	; another MCB?
	je	swinxms_complete
;
	mov	lmem lxmsctl.lenlo,TYPE mcbdesc
	mov	lmem lxmsctl.lenhi,0
	mov	lmem lxmsctl.desthi,ds
	mov	lmem lxmsctl.destlo,lcurrdesc
	mov	si,lxmsctl
	mov	ah,0bh
	call	lmem lprep.xmm		; move it
	or	ax,ax
	jz	swinxms_error
	add	lmem lxmsctl.srclo,16	; one paragraph
	adc	lmem lxmsctl.srchi,0
;
	call	lmem cgetmcb
	jc	swinxms_error
	jmp	swinxms_main
;
swinxms_complete:
	mov	ah,0ah			; release XMS frame
	mov	dx,lmem lprep.handle   	; XMS handle
	call	lmem lprep.xmm
	ret
;
swinxms_error:
	mov	ah,0ah			; release XMS frame on error
	call	lmem lprep.xmm
	mov	ax,4c00h
	int	21h
;
swapin_xms	endp
;
swinxms_length	= offset $ - offset swapin_xms
;
;
;	swapin_file:	swap in from file.
;
swapin_file	proc	far
;
	mov	dx,lprep.sp_swapfilename
	mov	ax,3d00h			; open file
	int	21h
	jc	swinfile_error2
	mov	bx,ax				; file handle
;
; do this in case of shrinkage of non-owned MCB between PSP MCB and next owned MCB
	mov	ax,ds			; ax==PSP
	dec	ax				; ax==PSP MCB
	mov	lmem lprep.first_mcb,ax	; init first MCB to PSP

swinfile_main:
	push	ds
	mov	cx,lmem lcurrdesc.swsize	; size in paragraphs
	mov	dx,lmem lcurrdesc.swoffset	; swap offset
	mov	ds,lmem lcurrdesc.addr		; segment to swap
;
swinfile_loop:
	mov	ax,cx
	cmp	ah,8h			; above 32k?
	jbe	swinfile_ok		; go read if not
	mov	ax,800h			; else read 32k
;
swinfile_ok:
	sub	cx,ax			; remaining length
	push	cx			; save it
	push	ax			; and save paras to read
	mov	cl,4
	shl	ax,cl			; convert to bytes
	mov	cx,ax
	mov	ah,3fh			; read
	int	21h
	jc	swinfile_error
	cmp	ax,cx
	jne	swinfile_error
	pop	cx			; paras read
	mov	ax,ds
	add	ax,cx			; bump up dest segment
	mov	ds,ax
	pop	cx			; remaining length
	or	cx,cx			; anything left?
	jnz	swinfile_loop		; go loop if yes
;
	pop	ds
	cmp	lmem lcurrdesc.num_follow,0	; another MCB?
	je	swinfile_complete	; ready if not
	mov	cx,16			; read one paragraph
	mov	dx,lcurrdesc
	mov	ah,3fh
	int	21h
	jc	swinfile_error1
	cmp	ax,cx
	jne	swinfile_error1
;
	push	bx
	call	lmem cgetmcb
	pop	bx
	jc	swinfile_error1
	jmp	swinfile_main
;
;
swinfile_complete:
	mov	ah,3eh			; close file
	int	21h
	mov	dx,lprep.sp_swapfilename
	mov	ah,41h			; delete file
	int	21h
	ret
;
swinfile_error:
	pop	cx
	pop	cx
	pop	ds
swinfile_error1:
	mov	ah,3eh			; close file
	int	21h
swinfile_error2:
	mov	dx,lprep.sp_swapfilename
	mov	ah,41h			; delete file
	int	21h
	mov	ax,4cffh
	int	21h
;
swapin_file	endp
;
swinfile_length	= offset $ - offset swapin_file
;
;
;	swapin_none:	no swap, return immediately.
;
swapin_none	proc	far
;
	ret
;
swapin_none	endp
;
;
	IF	swinems_length GT swinxms_length
swcodelen	=	swinems_length
	ELSE
swcodelen	=	swinxms_length
	ENDIF
	IF	swinfile_length GT swcodelen
swcodelen	=	swinfile_length
	ENDIF
;
swap_codelen	=	((swcodelen + 3) / 4) * 4
;
codelen		=	base_length + swap_codelen
reslen		=	codebeg + codelen
keep_paras	=	(reslen + 15) shr 4	; paragraphs to keep
swapbeg		=	keep_paras shl 4	; start of swap space
savespace	=	swapbeg - 5ch	; length of overwritten area
;
;--------------------------------------------------------------------
;
;
;
;	Space for saving the part of the memory image below the
;	swap area that is overwritten by our code.
;
save_dat	db	savespace dup(?)
;
;	Variables used while swapping out.
;	The "swap_prep" structure is initialized by prep_swap.
;
swap_prep	prep_block	<>
nextmcb		mcbdesc		<>
currdesc	mcbdesc		<>
xmsctl		xms_control	<>
ems_curpage	dw		?	; current EMS page number
ems_curoff	dw		?	; current EMS offset (paragraph)
;
;--------------------------------------------------------------------
;
;
;	swapout_ems:	swap out an MCB block to EMS.
;
;	Entry:	"currdesc" 	contains description of block to swap
;		"nextmcb"	contains MCB-descriptor of next block
;				if currdesc.num_follow is nonzero
;
;	Exit:	0 if OK, != 0 if error, Zero-flag set accordingly.
;
;	Uses:	All regs excpt DS
;
swapout_ems	proc	near
;
	push	ds
	mov	cx,currdesc.swsize	; block length in paras
	mov	si,currdesc.swoffset	; swap offset
	mov	dx,swap_prep.handle	; EMS handle
	mov	bx,ems_curpage		; current EMS page
	mov	di,ems_curoff		; current EMS page offset (paras)
	mov	es,swap_prep.ems_pageframe	; page frame address
	mov	ds,currdesc.addr	; segment to swap
;
	mov	ax,ems_parasize		; max length
	sub	ax,di			; minus current offset
	jnz	swems_ok		; go copy if there's room
;
swems_loop:
	mov	ax,4400h		; map in next page
	int	EMM_INT
	or	ah,ah
	jnz	swems_error
	mov	di,0			; reset offset
	inc	bx			; bump up page number
	mov	ax,ems_parasize		; max length to copy
;
swems_ok:
	cmp	ax,cx			; length to copy
	jbe	swems_doit		; go do it if <= total length
	mov	ax,cx			; else use total length
;
swems_doit:
	sub	cx,ax			; subtract copy length from total
	push	cx			; and save
	push	ax			; save the copy length in paras
	push	si
	push	di
	mov	cl,3
	shl	ax,cl			; convert to number of words (!)
	inc	cl
	shl	di,cl			; convert to byte address
	mov	cx,ax
	rep movsw
	pop	di
	pop	si
	pop	cx			; copy length in paras
	mov	ax,ds
	add	ax,cx			; add copy length to source segment
	add	di,cx			; and EMS page offset
	mov	ds,ax
	pop	cx			; remaining length
	or	cx,cx			; did we copy everything?
	jnz	swems_loop		; go loop if not
;
	pop	ds
	cmp	currdesc.num_follow,0	; another MCB?
	je	swems_complete		; exit if not
;
;	Another MCB follows, append nextmcb to save block.
;
	cmp	di,ems_parasize
	jb	swems_nonewpage		; no new block needed
	mov	ax,4400h		; map page, phys = 0
	int	EMM_INT
	or	ah,ah
	jnz	swems_error1
	mov	di,0
	inc	bx
;
swems_nonewpage:
	push	di
	mov	cl,4
	shl	di,cl			; convert to byte address
	mov	cx,TYPE mcbdesc
	mov	si,offset nextmcb
	rep movsb
	pop	di
	inc	di			; one paragraph
;
swems_complete:
	mov	ems_curpage,bx
	mov	ems_curoff,di
	xor	ax,ax
	ret
;
swems_error:
	pop	ds
swems_error1:
	mov	ah,45h			; release EMS pages on error
	int	EMM_INT
	mov	ax,RC_SWAPERROR
	or	ax,ax
	ret
;
swapout_ems	endp
;
;
;	swapout_xms:	swap out an MCB block to XMS.
;
;	Entry:	"currdesc" 	contains description of block to swap
;		"nextmcb"	contains MCB-descriptor of next block
;				if currdesc.num_follow is nonzero
;
;	Exit:	0 if OK, -1 if error, Zero-flag set accordingly.
;
;	Uses:	All regs excpt DS
;
swapout_xms	proc	near
;
	push	ds
	pop	es
	mov	ax,currdesc.swsize	; size in paragraphs
	mov	cl,4
	rol	ax,cl			; size in bytes + high nibble
	mov	dx,ax
	and	ax,0fff0h		; low word
	and	dx,0000fh		; high word
	mov	xmsctl.lenlo,ax		; into control block
	mov	xmsctl.lenhi,dx
	mov	xmsctl.srchnd,0		; source is normal memory
	mov	ax,currdesc.swoffset	; swap offset
	mov	xmsctl.srclo,ax		; into control block
	mov	ax,currdesc.addr	; segment to swap
	mov	xmsctl.srchi,ax
	mov	ax,swap_prep.handle	; XMS handle
	mov	xmsctl.desthnd,ax
	mov	si,offset xmsctl
	mov	ah,0bh
	call	swap_prep.xmm		; move it
	or	ax,ax
	jz	swxms_error
	mov	ax,xmsctl.lenlo		; adjust destination addr
	add	xmsctl.destlo,ax
	mov	ax,xmsctl.lenhi
	adc	xmsctl.desthi,ax
;
	cmp	currdesc.num_follow,0	; another MCB?
	je	swxms_complete
;
	mov	xmsctl.lenlo,TYPE mcbdesc
	mov	xmsctl.lenhi,0
	mov	xmsctl.srchi,ds
	mov	xmsctl.srclo,offset nextmcb
	mov	si,offset xmsctl
	mov	ah,0bh
	call	swap_prep.xmm		; move it
	or	ax,ax
	jz	swxms_error
	add	xmsctl.destlo,16	; one paragraph
	adc	xmsctl.desthi,0
;
swxms_complete:
	xor	ax,ax
	ret
;
swxms_error:
	mov	ah,0ah			; release XMS frame on error
	mov	dx,swap_prep.handle	; XMS handle
	call	swap_prep.xmm
	mov	ax,RC_SWAPERROR
	or	ax,ax
	ret
;
swapout_xms	endp
;
;
;	swapout_file:	swap out an MCB block to file.
;
;	Entry:	"currdesc" 	contains description of block to swap
;		"nextmcb"	contains MCB-descriptor of next block
;				if currdesc.num_follow is nonzero
;
;	Exit:	0 if OK, -1 if error, Zero-flag set accordingly.
;
;	Uses:	All regs excpt DS
;
swapout_file	proc	near
;
	push	ds
	mov	cx,currdesc.swsize	; size in paragraphs
	mov	bx,swap_prep.handle	; file handle
	mov	dx,currdesc.swoffset	; swap offset
	mov	ds,currdesc.addr	; segment to swap
;
swfile_loop:
	mov	ax,cx
	cmp	ah,8h			; above 32k?
	jbe	swfile_ok		; go write if not
	mov	ax,800h			; else write 32k
;
swfile_ok:
	sub	cx,ax			; remaining length
	push	cx			; save it
	push	ax			; and save paras to write
	mov	cl,4
	shl	ax,cl			; convert to bytes
	mov	cx,ax
	mov	ah,40h			; write
	int	21h
	jc	swfile_error
	cmp	ax,cx
	jne	swfile_error
	pop	cx			; paras written
	mov	ax,ds
	add	ax,cx			; bump up source segment
	mov	ds,ax
	pop	cx			; remaining length
	or	cx,cx			; anything left?
	jnz	swfile_loop		; go loop if yes
;
	pop	ds
	cmp	currdesc.num_follow,0	; another MCB?
	je	swfile_complete		; ready if not
	mov	cx,16			; write one paragraph
	mov	dx,offset nextmcb
	mov	ah,40h
	int	21h
	jc	swfile_error1
	cmp	ax,cx
	jne	swfile_error1
;
swfile_complete:
	xor	ax,ax
	ret
;
swfile_error:
	pop	cx
	pop	cx
	pop	ds
swfile_error1:
	mov	ah,3eh			; close file
	int	21h
	mov	dx,offset swap_prep.sp_swapfilename
	mov	ah,41h			; delete file
	int	21h
	mov	ax,RC_SWAPERROR
	or	ax,ax
	ret
;
swapout_file	endp
;
;--------------------------------------------------------------------------
;
	IF	REDIRECT
;
;	@redirect: Redirect a file.
;
;	Entry:	DS:SI = Filename pointer
;		AX zero if filename is NULL
;		CX    = Handle to redirect
;		ES:DI = Handle save pointer
;
;	Exit:	Carry set on error, then AL has DOS error code
;		ES:DI updated
;
;	Uses:	AX,BX,DX,SI
;
@redirect	proc	near
		local	doserr:WORD
;
	or	ax,ax
	jz	no_redirect
	cmp	byte ptr [si],0
	jne	do_redirect
;
no_redirect:
	mov	ax,-1
	stosw
	ret
;
do_redirect:
	or	cx,cx
	jnz	redir_write
	mov	dx,si
	mov	ax,3d00h	; open file, read only
	int	21h
	mov	doserr,ax
	jc	redir_failed
;
redir_ok:
	mov	dx,ax
	mov	ah,45h		; duplicate handle
	mov	bx,cx
	int	21h
	mov	doserr,ax
	jc	redir_failed_dup
	push	ax
	mov	bx,dx
	mov	ah,46h		; force duplicate handle
	int	21h
	mov	doserr,ax
	pop	ax
	jc	redir_failed_force
	stosw
	mov	ah,3eh		; close file
	int	21h
	clc
	ret
;
redir_failed_force:
	mov	bx,ax
	mov	ah,3eh		; close file
	int	21h
;
redir_failed_dup:
	mov	bx,dx
	mov	ah,3eh		; close file
	int	21h
;
redir_failed:
	mov	ax,doserr
	stc
	ret
;
redir_write:
	cmp	byte ptr [si],'>'
	jne	no_append
	inc	si
	mov	dx,si
	mov	ax,3d02h		; open file, read/write
	int	21h
	jc	no_append
	mov	bx,ax
	push	cx
	mov	ax,4202h		; move file, offset from EOF
	xor	cx,cx
	mov	dx,cx
	int	21h
	mov	doserr,ax
	pop	cx
	mov	ax,bx
	jnc	redir_ok
	mov	dx,ax
	jmp	redir_failed_dup
;
no_append:
	mov	dx,si
	mov	ah,3ch
	push	cx
	xor	cx,cx
	int	21h
	mov	doserr,ax
	pop	cx
	jc	redir_failed
	jmp	redir_ok
;
@redirect	endp
;
	ENDIF
;
;--------------------------------------------------------------------------
;
;--------------------------------------------------------------------------
;--------------------------------------------------------------------------
;
;
;do_spawn	PROC	uses si di,swapping: word, execfname:ptr byte,params:ptr byte,envlen:word,envp:ptr byte,stdin:ptr byte, stdout:ptr byte, stderr:ptr byte

;	execfname	DB	"C:\DOS\COMMAND.COM",81 DUP (0)
;	params	DB	2,"/C",0dh,81 DUP (0)
	swapping	DW	?
	datseg	DW	?
	pspseg	DW	?
	currmcb	DW	?

	execfname	DB	"C:\COMMAND.COM",82 DUP (0)
;	params	DB	"/C C:\DOS\MEM.EXE",129 DUP (0)
	params	DB	129 DUP (0)
PSP		DW	?		; real mode PSP
PSPEnvironPtr	DW	?	; real mode PSP Environment Pointer
PMPSP	DW	?		; protected mode PSP
DPMIFlag	DB	?	; nonzero if DPMI
ReturnValue	DW	0	; return value from swap

do_spawn	PROC	FAR
;	local	datseg:WORD,pspseg:WORD,currmcb:WORD
;
;
	mov	datseg,ds		; save default DS
;

;
	mov	bx,PSP
	mov	pspseg,bx
;
;	Check if spawn is too low in memory
;
	mov	ax,cs
	mov	dx,offset lowcode_begin
	mov	cl,4
	shr	dx,cl
	add	ax,dx			; normalized start of this code
	mov	dx,keep_paras		; the end of the modified area
	add	dx,bx			; plus PSP = end paragraph
	cmp	ax,dx
	ja	doswap_ok	; ok if start of code > end of low mem
	mov	ax,RC_TOOLOW
	ret
;
doswap_ok:
	cmp	swapping,0
	jle	method_ok
;
;	check the swap method, to make sure prep_swap has been called
;
	mov	al,swap_prep.swapmethod
	cmp	al,USE_EMS
	je	method_ok
	cmp	al,USE_XMS
	je	method_ok
	cmp	al,USE_FILE
	je	method_ok
	mov	ax,RC_BADPREP
	ret
;
;	Save the memory below the swap space.
;	We must do this before swapping, so the saved memory is
;	in the swapped out image.
;	Anything else we'd want to save on the stack or anywhere
;	else in "normal" memory also has to be saved here, any
;	modifications done to memory after the swap will be lost.
;
;	Note that the memory save is done even when not swapping,
;	because we use some of the variables in low core for
;	simplicity.
;
method_ok:
	mov	es,datseg
	mov	ds,pspseg		; DS points to PSP
	mov	si,5ch
	mov	di,offset save_dat
	mov	cx,savespace / 2	; NOTE: savespace is always even
	rep movsw
;
	mov	ds,datseg
;
	mov	ax,swapping
	cmp	ax,0
	jg	begin_swap
;
;	not swapping, prep_swap wasn't called. Init those variables in
;  	the 'swap_prep' block we need in any case.
;
	mov	swap_prep.swapmethod,al
	je	no_reduce
;
	mov	ax,pspseg
	dec	ax
	mov	swap_prep.psp_mcb,ax
	mov	swap_prep.first_mcb,ax
	inc	ax
	mov	es,ax
	mov	bx,es:psp_envptr
	mov	swap_prep.env_mcb,bx
;	mov	swap_prep.noswap_mcb,0
;	cmp	envlen,0
;	jne	swp_can_swap_env
	mov	swap_prep.noswap_mcb,bx
;
swp_can_swap_env:
	xor	bx,bx
	mov	es,bx
	mov	ah,52h			; get list of lists
	int	21h
	mov	ax,es
	or	ax,bx
	jz	no_reduce
	mov	es,es:[bx-2]		; first MCB
	cmp	es:id,4dh		; normal ID?
	jne	no_reduce
	mov	swap_prep.first_mcb,es
;
no_reduce:
	jmp	no_swap1
;
;	set up first block descriptor
;
begin_swap:
	mov	ax,swap_prep.first_mcb
	mov	currmcb,ax
	mov	es,swap_prep.psp_mcb	; let ES point to base MCB
	mov	ax,es:paras
	mov	currdesc.msize,ax
	sub	ax,keep_paras
	mov	currdesc.swsize,ax
	mov	currdesc.addr,es
	mov	currdesc.swoffset,swapbeg + 16
;		NOTE: swapbeg is 1 para higher when seen from MCB
	mov	ax,swap_prep.total_mcbs
	mov	currdesc.num_follow,ax
;
;	init other vars
;
	mov	xmsctl.destlo,0
	mov	xmsctl.desthi,0
	mov	ems_curpage,0
	mov	ems_curoff,ems_parasize
;
;	Do the swapping. Each MCB block (except the last) has an
;	"mcbdesc" structure appended that gives location and size
;	of the next MCB.
;
swapout_main:
	cmp	currdesc.num_follow,0	; next block?
	je	swapout_no_next		; ok if not
;
;	There is another MCB block to be saved. So we don't have
;	to do two calls to the save routine with complicated
;	parameters, we set up the next MCB descriptor beforehand.
;	Walk the MCB chain starting at the current MCB to find
;	the next one belonging to this process.
;
	mov	ax,currmcb
	mov	bx,pspseg
	mov	cx,swap_prep.psp_mcb
	mov	dx,swap_prep.noswap_mcb
	mov	si,swap_prep.noswap2_mcb
;
swm_mcb_walk:
	mov	es,ax
	cmp	ax,cx
	jbe	swm_next_mcb
	cmp	ax,dx
	je	swm_next_mcb
	cmp	ax,si
	je	swm_next_mcb
	cmp	ax,swap_prep.noswap3_mcb
	je	swm_next_mcb
;
	cmp	bx,es:owner		; our process?
	je	swm_mcb_found		; found it if yes
;
swm_next_mcb:
	cmp	es:id,4dh		; normal block?
	jne	swm_mcb_error		; error if end of chain
	add	ax,es:paras		; start + length
	inc	ax			; next MCB
	jmp	swm_mcb_walk
;
;	MCB found, set up an mcbdesc in the "nextmcb" structure
;
swm_mcb_found:
	mov	nextmcb.addr,es
	mov	ax,es:paras		; get number of paragraphs
	mov	nextmcb.msize,ax	; and save
	inc	ax
	mov	nextmcb.swsize,ax
	mov	bx,es
	add	bx,ax
	mov	currmcb,bx
	mov	nextmcb.swoffset,0
	mov	ax,currdesc.num_follow
	dec	ax
	mov	nextmcb.num_follow,ax
;
swapout_no_next:
	cmp	swap_prep.swapmethod,USE_EMS
	je	swm_ems
	cmp	swap_prep.swapmethod,USE_XMS
	je	swm_xms
	call	swapout_file
	jmp	short swm_next
;
swm_ems:
	call	swapout_ems
	jmp	short swm_next
;
swm_xms:
	call	swapout_xms
;
swm_next:
	jnz	swapout_error
	cmp	currdesc.num_follow,0
	je	swapout_complete
;
;	next MCB exists, copy the "nextmcb" descriptor into
;	currdesc, and loop.
;
	mov	es,datseg
	mov	si,offset nextmcb
	mov	di,offset currdesc
	mov	cx,TYPE mcbdesc
	rep movsb
	jmp	swapout_main
;
;
swm_mcb_error:
	mov	ax,RC_MCBERROR
;
swapout_kill:
	cmp	swapping,0
	jl	swapout_error
	push	ax
	cmp	swap_prep.swapmethod,USE_FILE
	je	swm_mcberr_file
	cmp	swap_prep.swapmethod,USE_EMS
	je	swm_mcberr_ems
;
	mov	ah,0ah			; release XMS frame on error
	mov	dx,swap_prep.handle	; XMS handle
	call	swap_prep.xmm
	pop	ax
	jmp	short swapout_error
;
swm_mcberr_ems:
	mov	dx,swap_prep.handle	; EMS handle
	mov	ah,45h			; release EMS pages on error
	int	EMM_INT
	pop	ax
	jmp	short swapout_error
;
swm_mcberr_file:
	mov	bx,swap_prep.handle
	cmp	bx,-1
	je	swm_noclose
	mov	ah,3eh			; close file
	int	21h
swm_noclose:
	mov	dx,offset swap_prep.sp_swapfilename
	mov	ah,41h			; delete file
	int	21h
	pop	ax
;
swapout_error:
	ret
;
;
;	Swapout complete. Close the handle (EMS/file only),
;	then set up low memory.
;
swapout_complete:
	cmp	swap_prep.swapmethod,USE_FILE
	jne	swoc_nofile
;
;	File swap: Close the swap file to make the handle available
;
	mov	bx,swap_prep.handle
	mov	swap_prep.handle,-1
	mov	ah,3eh
	int	21h			; close file
	mov	si,offset swapin_file
	jnc	swoc_ready
	mov	ax,RC_SWAPERROR
	jmp	swapout_kill
;
swoc_nofile:
	cmp	swap_prep.swapmethod,USE_EMS
	jne	swoc_xms
;
;	EMS: Unmap page
;
	mov	ax,4400h
	mov	bx,-1
	mov	dx,swap_prep.handle
	int	EMM_INT
	mov	si,offset swapin_ems
	jmp	short swoc_ready
;
swoc_xms:
	mov	si,offset swapin_xms
	jmp	short swoc_ready
;
no_swap1:
	mov	si,offset swapin_none
;
;	Copy the appropriate swap-in routine to low memory.
;
swoc_ready:
	mov	es,pspseg
	mov	cx,swap_codelen / 4
	mov	di,codebeg + base_length
	push	ds
	mov	ax,cs
	mov	ds,ax
	rep movsd
;
;	And while we're at it, copy the MCB allocation routine (which
;	also includes the initial MCB release and exec call) down.
;
	mov	cx,base_length / 2
	mov	di,param_len
	mov	si,offset lowcode_begin
	rep movsw
;
	pop	ds
	mov	bx,es
	dec	bx
	mov	es,bx		; let ES point to base MCB
;
;	Again set up the base MCB descriptor, and copy it as well as
;	the variables set up by prep_swap to low memory.
;	This isn't too useful if we're not swapping, but it doesn't
;	hurt, either. The only variable used when not swapping is
;	lprep.swapmethod.
;
	mov	ax,es:paras
	mov	currdesc.msize,ax
	sub	ax,keep_paras
	mov	currdesc.swsize,ax
	mov	currdesc.addr,es
	mov	currdesc.swoffset,swapbeg + 16
	mov	ax,swap_prep.total_mcbs
	mov	currdesc.num_follow,ax
;
	mov	es,pspseg		; ES points to PSP again
;
	mov	cx,TYPE prep_block
	mov	si,offset swap_prep
	mov	di,lprep
	rep movsb
	mov	cx,TYPE mcbdesc
	mov	si,offset currdesc
	mov	di,lcurrdesc
	rep movsb
;
;	now set up other variables in low core
;
	mov	ds,pspseg
	mov	ds:cgetmcb,getmcboff + codebeg
	mov	ds:eretcode,0
	mov	ds:retflags,0
;
;
;	If 'NO_INHERIT' is nonzero, save the entries of the
;	handle table, and set the last 15 to 0ffh (unused).
;
	mov	si,psp_handletab
	mov	di,lhandlesave
	mov	cx,10
	rep movsw
	mov	si,psp_handlenum	; Length of handle table
	mov	ax,[si]
	stosw
	mov	word ptr [si],20	; set to default to be safe
	add	si,2
	lodsw				; Handle table pointer
	mov	bx,ax
	stosw
	lodsw
	stosw
	cmp	ax,pspseg
	jne	copy_handles
	cmp	bx,psp_handletab
	je	no_handlecopy
;
;	if the handle table pointer in the PSP does not point to
;	the default PSP location, copy the first five entries from
;	this table into the PSP - but only if we have DOS >= 3.3.
;
copy_handles:
	mov	ds,ax
	mov	si,bx
	mov	di,psp_handletab
	mov	es:psp_handleptro,di
	mov	es:psp_handleptrs,es
	movsw
	movsw
	movsb
;
no_handlecopy:
	mov	di,psp_handletab+5
	mov	ax,0ffffh
	stosb
	mov	cx,7
	rep stosw
;
;
;	Handle Redirection
;
	IF	REDIRECT
	mov	es,pspseg
	mov	di,lstdinsav
	mov	ax,-1
	stosw
	stosw
	stosw
	mov	di,lstdinsav
	xor	cx,cx
	IF	ptrsize
	lds	si,stdin
	mov	ax,ds
	or	ax,si
	ELSE
	mov	si,stdin
	mov	ds,datseg
	or	si,si
	ENDIF
	call	@redirect
	jc	failed_redir
	inc	cx
	IF	ptrsize
	lds	si,stdout
	mov	ax,ds
	or	ax,si
	ELSE
	mov	si,stdout
	or	si,si
	ENDIF
	call	@redirect
	jc	failed_redir
	inc	cx
	IF	ptrsize
	lds	si,stderr
	mov	ax,ds
	or	ax,si
	ELSE
	mov	si,stderr
	or	si,si
	ENDIF
	call	@redirect
	jnc	redir_complete
;
failed_redir:
	push	ax
;
;	restore handle table and pointer
;
	mov	ds,pspseg
	mov	si,lstdinsav
	xor	cx,cx
;
redirclose:
	lodsw
	cmp	ax,-1
	je	redclosenext
	mov	bx,ax
	mov	ah,46h
	int	21h
;
redclosenext:
	inc	cx
	cmp	cx,3
	jb	redirclose
;
	mov	ds,pspseg
	mov	es,pspseg
	mov	si,lhandlesave
	mov	di,psp_handletab
	mov	cx,10
	rep movsw
	mov	di,psp_handlenum
	movsw
	movsw
	movsw
;
;	Restore overwritten part of program
;
	mov	ds,datseg
	mov	es,pspseg
	mov	si,offset save_dat
	mov	di,5ch
	mov	cx,savespace
	rep movsb
;
	pop	ax
	mov	ah,RC_REDIRFAIL SHR 8
	jmp	swapout_kill
;
redir_complete:
	mov	ds,pspseg
	mov	es,pspseg
	mov	si,psp_handletab+5
	mov	di,lredirsav
	mov	cx,3
	rep movsw
	mov	di,psp_handletab+5
	mov	cx,3
	mov	ax,0ffffh
	rep stosw
	ENDIF
;
;	Prepare exec parameter block
;
	mov	ax,es
	mov	es:expar.fcb1seg,ax
	mov	es:expar.fcb2seg,ax
	mov	es:expar.pparseg,ax
	mov	es:expar.envseg,0
;
;	The 'zero' word is located at 80h in the PSP, the start of
;	the command line. So as not to confuse MCB walking programs,
;	a command line length of zero is inserted here.
;
	mov	es:zero,0d00h		; 00h,0dh = empty command line
;
;	Init default fcb's by parsing parameter string
;
;	IF	ptrsize
;	lds	si,params
;	ELSE
	mov	si,OFFSET params
	mov	ds,datseg
;	ENDIF
	push	si
	mov	di,xfcb1
	mov	es:expar.fcb1,di
	push	di
	mov	cx,16
	xor	ax,ax
	rep stosw			; init both fcb's to 0
	pop	di
	mov	ax,2901h
	int	21h
	mov	di,xfcb2
	mov	es:expar.fcb2,di
	mov	ax,2901h
	int	21h
	pop	si
;
;	move command tail string into low core
;
	mov	di,progpars
	mov	es:expar.ppar,di
	xor	cx,cx
	inc	di
cmdcpy:
	lodsb
	or	al,al
	jz	cmdcpy_end
	stosb
	inc	cx
	jmp	cmdcpy
;
cmdcpy_end:
	mov	al,0dh
	stosb
	mov	es:progpars,cl
;
;	move filename string into low core
;
;	IF	ptrsize
;	lds	si,execfname
;	ELSE
	mov	si,OFFSET execfname
;	ENDIF
	mov	di,filename
fncpy:
	lodsb
	stosb
	or	al,al
	jnz	fncpy
;
;	Setup environment copy
;
	mov	bx,keep_paras		; paras to keep

;	xor	bx,bx

COMMENT !
	mov	cx,envlen		; environment size
	jcxz	no_environ		; go jump if no environment

	cmp	swapping,0
	jne	do_envcopy
;
;	Not swapping, use the environment pointer directly.
;	Note that the environment copy must be paragraph aligned.
;
	IF	ptrsize
	mov	ax,word ptr (envp)+2
	mov	bx,word ptr (envp)
	ELSE
	mov	ax,ds
	mov	bx,envp
	ENDIF
	add	bx,15			; make sure it's paragraph aligned
	mov	cl,4
	shr	bx,cl			; and convert to segment addr
	add	ax,bx
	mov	es:expar.envseg,ax	; new environment segment
	xor	cx,cx			; mark no copy
	xor	bx,bx			; and no shrink
	jmp	short no_environ
;
;	Swapping or EXECing without return. Set up the pointers for
;	an environment copy (we can't do the copy yet, it might overwrite
;	this code).
;
do_envcopy:
	inc	cx
	shr	cx,1			; words to copy
	mov	ax,cx			; convert envsize to paras
	add	ax,7
	shr	ax,1
	shr	ax,1
	shr	ax,1
	add	bx,ax			; add envsize to paras to keep
	IF	ptrsize
	lds	si,envp
	ELSE
	mov	si,envp
	ENDIF
;
	mov	ax,es			; low core segment
	add	ax,keep_paras		; plus fixed paras
	mov	es:expar.envseg,ax	; = new environment segment
END COMMENT !
;
;	Save stack regs, switch to local stack
;
no_environ:
	mov	es:save_ss,ss
	mov	es:save_sp,sp
	mov	ax,es
	cli
	mov	ss,ax
	mov	sp,mystack
	sti
;
;	push	cx			; save env length
;	push	si			; save env pointer
	push	ds			; save env segment
;
;	save and patch INT0 (division by zero) vector
;
	xor	ax,ax
	mov	ds,ax
	mov	ax,word ptr ds:0
	mov	es:div0_off,ax
	mov	ax,word ptr ds:2
	mov	es:div0_seg,ax
	mov	word ptr ds:0,codebeg + iretoff
	mov	word ptr ds:2,es
;
	pop	ds			; pop environment segment
;	pop	si			; pop environment offset
;	pop	cx			; pop environment length
	mov	di,swapbeg		; environment destination
;
;	Push return address on local stack
;
	push	cs			; push return segment
	mov	ax,offset exec_cont
	push	ax			; push return offset
	mov	es:spx,sp		; save stack pointer
;
;	Goto low core code
;
	push	es			; push entry segment
    mov	ax,codebeg + doexec_entry
    push	ax			; push entry offset
	retf
;
;----------------------------------------------------------------
;
;	Low core code will return to this location, with DS set to
;	the PSP segment.
;
exec_cont:
	push	ds
	pop	es
	cli
	mov	ss,ds:save_ss		; reload stack
	mov	sp,ds:save_sp
	sti
;
;	restore handle table and pointer
;
	mov	si,lhandlesave
	mov	di,psp_handletab
	mov	cx,10
	rep movsw
	mov	di,psp_handlenum
	movsw
	movsw
	movsw
;
;	restore INT0 (division by zero) vector
;
	xor	cx,cx
	mov	ds,cx
	mov	cx,es:div0_off
	mov	word ptr ds:0,cx
	mov	cx,es:div0_seg
	mov	word ptr ds:2,cx
;
	mov	ds,datseg
;
;
	mov	ax,es:eretcode
	mov	bx,es:retflags
;
;	Restore overwritten part of program
;
	mov	si,offset save_dat
	mov	di,5ch
	mov	cx,savespace
	rep movsb
;

;	int	3

	test	bx,1			; carry set?
	jnz	exec_fault		; return EXEC error code if fault
	mov	ah,4dh			; else get program return code
	int	21h
	xor	ah,ah			; zap high value
	ret
;
exec_fault:
	mov	ax,SWAPPERERROR
;	mov	ah,3			; return error as 03xx
	ret
;
do_spawn	ENDP
;
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
;
emm_name	db	'EMMXXXX0'
;
;	prep_swap - prepare for swapping.
;
;	This routine checks all parameters necessary for swapping,
;	and attempts to set up the swap-out area in EMS/XMS, or on file.
;	In detail:
;
;	     1) Check whether the do_spawn routine is located
;		too low in memory, so it would get overwritten.
;		If this is true, return an error code (-2).
;
;	     2) Walk the memory control block chain, adding up the
;		paragraphs in all blocks assigned to this process.
;
;	     3) Check EMS (if the method parameter allows EMS):
;		- is an EMS driver installed?
;		- are sufficient EMS pages available?
;		if all goes well, the EMS pages are allocated, and the
;		routine returns success (1).
;
;	     4) Check XMS (if the method parameter allows XMS):
;		- is an XMS driver installed?
;		- is a sufficient XMS block available?
;		if all goes well, the XMS block is allocated, and the
;		routine returns success (2).
;
;	     5) Check file swap (if the method parameter allows it):
;		- try to create the file
;		- pre-allocate the file space needed by seeking to the end
;		  and writing a byte.
;		If the file can be written, the routine returns success (4).
;
;	     6) Return an error code (-1).
;
		pmethod	DW	?

;prep_swap	PROC	uses si di,pmethod:word,swapfname:ptr byte
prep_swap	PROC	FAR

	LOCAL	totparas: word
;
;
	mov	ax,fs		; ax -> wedge segment
	dec	ax
	cmp	DPMIFlag,0	; see if using DPMI
	jne	isdpmi
	dec	ax			; CauseWay uses 1 para for its own purposes, compute MCB

isdpmi:
	mov	swap_prep.noswap2_mcb,ax	; don't swap out wedge segment

	mov	ax,PSP
	mov	es,ax		; es -> PSP
	mov	ax,es:[36h]	; es -> file handle table segment
	dec	ax
	cmp	DPMIFlag,0	; see if using DPMI
	jne	isdpmi2
	dec	ax			; CauseWay uses 1 para for its own purposes, compute MCB

isdpmi2:
	mov	swap_prep.noswap3_mcb,ax	; don't swap out file handle table segment

	mov	ax,PSP
	dec	ax
	mov	swap_prep.psp_mcb,ax
	mov	swap_prep.first_mcb,ax	; init first MCB to PSP
;
;	Make a copy of the environment pointer in the PSP
;
	inc	ax
	mov	es,ax
	mov	bx,es:psp_envptr
	dec	bx
	mov	swap_prep.env_mcb,bx
;	mov	swap_prep.noswap_mcb,0
;	test	pmethod,DONT_SWAP_ENV
;	jz	can_swap_env
	mov	swap_prep.noswap_mcb,bx
;
can_swap_env:
;
;	Check if spawn is too low in memory
;
	mov	bx,cs
	mov	dx,offset lowcode_begin
	mov	cl,4
	shr	dx,cl
	add	bx,dx			; normalized start of this code
	mov	dx,keep_paras		; the end of the modified area
	add	dx,ax			; plus PSP = end paragraph
	cmp	bx,dx
	ja	prepswap_ok	; ok if start of code > end of low mem
	mov	ax,-2
	mov	swap_prep.swapmethod,al
	ret
;
;	Walk the chain of memory blocks, adding up the paragraphs
;	in all blocks belonging to this process.
;	We try to find the first MCB by getting DOS's "list of lists",
;	and fetching the word at offset -2 of the returned address.
;	If this fails, we use our PSP as the starting point.
;
prepswap_ok:
	xor	bx,bx
	mov	es,bx
	mov	ah,52h			; get list of lists
	int	21h
	mov	ax,es
	or	ax,bx
	jz	prep_no_first
	mov	es,es:[bx-2]		; first MCB
	cmp	es:id,4dh		; normal ID?
	jne	prep_no_first
	mov	swap_prep.first_mcb,es
;
prep_no_first:
	mov	es,swap_prep.psp_mcb	; ES points to base MCB
	mov	cx,es			; save this value
	mov	bx,es:owner		; the current process
	mov	dx,es:paras		; memory size in the base block
	sub	dx,keep_paras		; minus resident paragraphs
	mov	si,0			; number of MCBs except base
	mov	di,swap_prep.noswap_mcb
	mov	bp,swap_prep.noswap2_mcb
	mov	ax,swap_prep.first_mcb
	mov	swap_prep.first_mcb,0
;
prep_mcb_walk:
	mov	es,ax
	cmp	ax,cx			; base block?
	jbe	prep_walk_next		; then don't count again
	cmp	ax,di			; Non-swap MCB?
	je	prep_walk_next		; then don't count
	cmp	ax,bp			; Non-swap MCB?
	je	prep_walk_next		; then don't count
	cmp	ax,swap_prep.noswap3_mcb	; non-swap MCB
	je	prep_walk_next		; then don't count

;
	cmp	bx,es:owner		; our process?
	jne	prep_walk_next		; next if not
	inc	si
	mov	ax,es:paras		; else get number of paragraphs
	add	ax,2			; + 1 for descriptor + 1 for MCB
	add	dx,ax			; total number of paras
	cmp	swap_prep.first_mcb,0
	jne	prep_walk_next
	mov	swap_prep.first_mcb,es
;
prep_walk_next:
	cmp	es:id,4dh		; normal block?
	jne	prep_mcb_ready		; ready if end of chain
	mov	ax,es
	add	ax,es:paras		; start + length
	inc	ax			; next MCB
	jmp	prep_mcb_walk
;
prep_mcb_ready:
	mov	totparas,dx
	mov	swap_prep.total_mcbs,si
;
;
	test	pmethod,XMS_FIRST
	jnz	check_xms
;
;	Check for EMS swap
;
check_ems:
	test	pmethod,USE_EMS
	jz	prep_no_ems
;
	push	ds
	mov	al,EMM_INT
	mov	ah,35h
	int	21h			; get EMM int vector
	mov	ax,cs
	mov	ds,ax
	mov	si,offset emm_name
	mov	di,10
	mov	cx,8
	repz cmpsb			; EMM name present?
	pop	ds
	jnz	prep_no_ems
;
	mov	ah,40h			; get EMS status
	int	EMM_INT
	or	ah,ah			; EMS ok?
	jnz	prep_no_ems
;
	mov	ah,46h			; get EMS version
	int	EMM_INT
	or	ah,ah			; AH must be 0
	jnz	prep_no_ems
;
	cmp	al,30h			; >= version 3.0?
	jb	prep_no_ems
;
	mov	ah,41h			; Get page frame address
	int	EMM_INT
	or	ah,ah
	jnz	prep_no_ems
;
;	EMS present, try to allocate pages
;
	mov	swap_prep.ems_pageframe,bx
	mov	bx,totparas
	add	bx,ems_paramask
	mov	cl,ems_shift
	shr	bx,cl
	mov	ah,43h			; allocate handle and pages
	int	EMM_INT
	or	ah,ah			; success?
	jnz	prep_no_ems
;
;	EMS pages allocated, swap to EMS
;
	mov	swap_prep.handle,dx
	mov	ax,USE_EMS
	mov	swap_prep.swapmethod,al
	ret
;
;	No EMS allowed, or EMS not present/full. Try XMS.
;
prep_no_ems:
	test	pmethod,XMS_FIRST
	jnz	check_file		; don't try again
;
check_xms:
	test	pmethod,USE_XMS
	jz	prep_no_xms
;
	mov	ax,4300h		; check if XMM driver present
	int	2fh
	cmp	al,80h			; is XMM installed?
	jne	prep_no_xms
	mov	ax,4310h		; get XMM entrypoint
	int	2fh
	mov	word ptr swap_prep.xmm,bx	; save entry address
	mov	word ptr swap_prep.xmm+2,es
;
	mov	dx,totparas
	add	dx,xms_paramask		; round to nearest multiple of 1k
	mov	cl,xms_shift
	shr	dx,cl			; convert to k
	mov	ah,9			; allocate extended memory block
	call	swap_prep.xmm
	or	ax,ax
	jz	prep_no_xms
;
;	XMS block allocated, swap to XMS
;
	mov	swap_prep.handle,dx
	mov	ax,USE_XMS
	mov	swap_prep.swapmethod,al
	ret
;
;	No XMS allowed, or XMS not present/full. Try File swap.
;
prep_no_xms:
	test	pmethod,XMS_FIRST
	jz	check_file
	jmp	check_ems
;
check_file:
	test	pmethod,USE_FILE
	jnz	prep_do_file
	jmp	prep_no_file
;
prep_do_file:
	push	ds
;	IF	ptrsize
;	lds	dx,swapfname
;	ELSE
	mov	dx,OFFSET SwapFileName
;	ENDIF
	mov	cx,2			; hidden attribute
	test	pmethod,HIDE_FILE
	jnz	prep_hide
	xor	cx,cx			; normal attribute
;
prep_hide:
	mov	ah,3ch			; create file
	test	pmethod,CREAT_TEMP
	jz	prep_no_temp
	mov	ah,5ah
;
prep_no_temp:
	int	21h			; create/create temp
	jnc	prep_got_file
	cmp	ax,3		; see if path not found (bad TEMP setting)
	jne	prep_no_file
	mov	ah,19h		; get current disk
	int	21h
	add	al,'A'		; convert to ASCII drive
	mov	bx,dx
	mov	BYTE PTR ds:[bx],al	; default to current drive root
	mov	BYTE PTR ds:[bx+1],':'
	mov	BYTE PTR ds:[bx+2],'\'
	mov	BYTE PTR ds:[bx+3],0	; zero out path name and retry
;	mov	BYTE PTR ds:[bx],0	; zero out path name and retry
	jmp	SHORT prep_hide
;
prep_got_file:
	mov	bx,ax			; handle
;
;	save the file name
;
	pop	es
	push	es
	mov	di,offset swap_prep.sp_swapfilename
	mov	cx,81
	mov	si,dx
	rep movsb
;
	pop	ds
	mov	swap_prep.handle,bx
;
;	preallocate the file
;
	test	pmethod,NO_PREALLOC
	jnz	prep_noprealloc
	test	pmethod,CHECK_NET
	jz	prep_nonetcheck
;
;	check whether file is on a network drive, and don't preallocate
;	if so. preallocation can slow down swapping significantly when
;	running on certain networks (Novell)
;
	mov	ax,440ah	; check if handle is remote
	int	21h
	jc	prep_nonetcheck	; assume not remote if function fails
	test	dh,80h		; DX bit 15 set ?
	jnz	prep_noprealloc	; remote if yes
;
prep_nonetcheck:
	mov	dx,totparas
	mov	cl,4
	rol	dx,cl
	mov	cx,dx
	and	dx,0fff0h
	and	cx,0000fh
	sub	dx,1
	sbb	cx,0
	mov	si,dx			; save
	mov	ax,4200h		; move file pointer, absolute
	int	21h
	jc	prep_file_err
	cmp	dx,cx
	jne	prep_file_err
	cmp	ax,si
	jne	prep_file_err
	mov	cx,1			; write 1 byte
	mov	ah,40h
	int	21h
	jc	prep_file_err
	cmp	ax,cx
	jne	prep_file_err
;
	mov	ax,4200h		; move file pointer, absolute
	xor	dx,dx
	xor	cx,cx			; rewind to beginning
	int	21h
	jc	prep_file_err
;
prep_noprealloc:
	mov	ax,USE_FILE
	mov	swap_prep.swapmethod,al
	ret
;
prep_file_err:
	mov	ah,3eh			; close file
	int	21h
	mov	dx,offset swap_prep.sp_swapfilename
	mov	ah,41h			; delete file
	int	21h
;
prep_no_file:
	mov	ax,-1
	mov	swap_prep.swapmethod,al
	ret
;
prep_swap	endp
;

COMMENT !
O_RUNX:
	pusha
	push	ds
	push	es

	mov	bx,SWAP_TEXT
	mov	ax,0ff06h
	int	31h
	jc	SwapErr
	push	ax		; save -> swap routine data
	mov	es,ax
	mov	BYTE PTR es:[params],0	; kill leftovers

; get length of first passed parameter
	push	1
	call	__parclen
	add	sp,2
	or	ax,ax
	je	med3		; zero length

; get first parameter string in dx:ax
	push	1
	call	__parc
	add	sp,2
	pop	es
	push	es
	mov	di,OFFSET params	; es:di -> parameter storage
	push	ds		; save ds -> DGROUP
	mov	ds,dx
	mov	si,ax		; ds:si -> character string
	mov	ax,'/'+256*'C'	; put in /C for command.com, transient command
	stosw
	mov	al,' '
	stosb

xparamloop1:
	movsb
	cmp	BYTE PTR ds:[si-1],0	; see if hit null terminator
	jne	xparamloop1
	pop	ds			; restore ds -> DGROUP

	dec	di			; make di -> null terminator in case of second parameter

; get length of second passed parameter
	push	2
	call	__parclen
	add	sp,2
	or	ax,ax
	je	med3		; zero length

; get second parameter string in dx:ax
	push	2
	call	__parc
	add	sp,2
	mov	ds,dx
	mov	si,ax		; ds:si -> character string
	pop	es
	push	es		; es:di -> current position of stored string
	mov	al,' '
	stosb			; put in space

xparamloop2:
	movsb
	cmp	BYTE PTR ds:[si-1],0	; see if stored null terminator
	jne	xparamloop2
	jmp	SHORT med3
END COMMENT !

;SWPRUNCMD:
;OverLay:
CWSwap:


	pushad
	push	ds
	push	es
	cld


;**JW**
;works upto here...
;	pop	es
;	pop	ds
;	popa
;	db 66h
;	retf

; get command pointer on stack
	mov	eax,ss:[esp+44]	; 32 for pushad, 4 for push ds,es, 8 for return address

	push	eax		; save -> command
	mov	bx,SWAP_TEXT
	mov	ax,0ff06h
	int	31h
;	jc	SwapErr
	push	ax		; save -> swap routine data
	mov	es,ax
	mov	BYTE PTR es:[params],0	; kill leftovers

; get length of passed parameter
;	push	1
;	call	__parclen
;	add	sp,2
;	or	ax,ax
;	je	med3		; zero length

; get parameter string in dx:ax
;	push	1
;	call	__parc
;	add	sp,2
;	mov	ds,dx
;	mov	si,ax		; ds:si -> character string

	pop	es
	pop	esi			; ds:esi -> command
	push	es

	mov	di,OFFSET params	; es:di -> parameter storage
	mov	al,'/'
	mov	ax,'/'+256*'C'	; put in /C for command.com, transient command
	stosw
	mov	al,' '
	stosb

paramloop:
	mov	al,ds:[esi]
	mov	es:[di],al
	inc	esi
	inc	di
	cmp	BYTE PTR ds:[esi-1],0	; see if stored null terminator
	jne	paramloop

med3:
	pop	ds			; ds -> swap routine data
ASSUME	ds:SWAP_TEXT,es:SWAP_TEXT

	mov	AliasSelector,ds
	mov	IntVectorSelector,0
	mov	LowMemSelector,0
	mov	WedgeMemSelector,0

	xor	bh,bh
	mov	ah,3
	int	10h			; ready cursor size, position, and shape
	mov	CursorSize,cx
	mov	CursorRowCol,dx

; get selector for interrupt vector table
	mov	ax,0ff03h		; GetSel
	int	31h
	jc	SwapErr
	xor	cx,cx			; point selector at 0:0 for interrupt vectors
	mov	dx,cx
	mov	di,400h
	mov	si,cx
	mov	ax,0ff09h		; SetSelDet
	int	31h
	mov	INTVectorSelector,bx	; save selector

; get protected mode PSP of program
	mov	ah,62h		; get PSP
	int	21h

	mov	PMPSP,bx	; save protected mode PSP
	mov	ds,bx		; ds -> protected mode psp

	mov	ax,ds:[EPSP_RealENV]	; real mode environment block pointer
	mov	es:EnvironmentPointer,ax

	mov	edx,ds:[EPSP_INTMem]	; interrupt/exception vectors linear address
	mov	ax,0ff03h	; GetSel
	int	31h
	jc	SwapErr
	mov	ecx,(256*6)+(32*6)+400h	; selector limit
	mov	ax,0ff0ah	; SetSelDet32
	int	31h

	mov	ds,bx		; ds -> selector
	mov	si,(256*6)+(32*6)	; si -> past protected mode interrupt and exception vectors
	mov	di,OFFSET INTVectorTable	; es:di -> real mode vector table storage
	mov	cx,256
	rep	movsd		; save real mode vector table

; release interrupt/exception vectors selector
	mov	ax,0ff04h		; RelSel
	int	31h

	push	es
	pop	ds

; get/save memory allocation strategy
	mov	ax,5800h		; get allocation strategy
	int	21h
	mov	AllocStrat,ax	; save original strategy

; change memory allocation strategy to last fit
	mov	ax,5801h		; set allocation strategy
	mov	bx,2			; use highest available block that fits
	int	21h

; get wedge memory for EXEC'ing
	mov	bx,(WedgeSpace+15)/16
	mov	ax,0ff21h
	int	31h
	jnc	savewedge
	pushf				; setup stack so restore strategy still works properly
	push	ax
	jmp	NEAR PTR restalloc	; error in dos allocation

; save the wedge memory segment and selector
savewedge:
;	mov	RealRegs.Real_FS,ax	; save real mode segment of wedge memory
; alternate save real mode segment of wedge memory into bx since NT doesn't restore FS properly
	mov	WORD PTR RealRegs.Real_EBX,ax

	mov	WedgeMemSelector,dx

; copy wedge code to wedge segment
	mov	es,dx
	xor	di,di
	mov	si,OFFSET WedgeStart
	mov	cx,WedgeSpace
	push	cx
	shr	cx,2
	rep	movsd
	pop	cx
	and	cx,3
	rep	movsb

	push	ds
	pop	es

; track if DPMI or non-, to determine where MCB is in relation to allocation
	mov	ax,0ff00h
	int	31h
	and	di,8			; mask to DPMI bit
	mov	WORD PTR RealRegs.Real_EDX,di	; save real mode flag

; get DOS memory for moving code down for real mode execution
	mov	bx,(((OFFSET CodeEnd-OFFSET CodeStart)+15))/16	; amount of low memory needed
	mov	ax,0ff21h
	int	31h
	pushf				; save allocation error code
	push	ax			; save segment

; restore memory allocation strategy to original
restalloc:
	mov	ax,5801h		; set allocation strategy
	mov	bx,AllocStrat	; use highest available block that fits
	int	21h

	pop	ax				; restore segment
	popf				; restore allocation error code
	jnc	GotMemory

; error using swapper
SwapErr:
	mov	ax,SWAPPERERROR
	mov	WORD PTR es:RealRegs.Real_EAX,ax	; get return value
	jmp	NEAR PTR Done

; got low DOS memory for code/data transfer, ax == segment, dx == selector
; transfer code down to it
GotMemory:
	mov	WORD PTR RealRegs.Real_EAX,SWAPPERERROR	; init return value
	mov	RealRegs.Real_CS,ax	; save real mode segment
	mov	RealRegs.Real_IP,OFFSET NowInReal	; transfer address in low memory
	mov	es,dx		; es -> low memory selector
	mov	LowMemSelector,dx
	xor	di,di
	mov	si,OFFSET CodeStart
	mov	ax,(OFFSET CodeEnd-OFFSET CodeStart)
	mov	cx,ax
	shr	cx,2
	rep	movsd
	mov	cx,ax
	and	cx,3
	rep	movsb
	push	ds
	pop	es				; es -> swap data


;**JW**
;
;INC exec counter.
;

;	int	3

	mov	ah,62h		; get PSP
	int	21h

	push	ds
	mov	ds,bx		; ds -> protected mode psp
	lds	bx,ds:[EPSP_ExecCount]
	inc	byte ptr[bx]
	pop	ds

; transfer to low DOS memory real mode operation
	mov	edi,OFFSET RealRegs	; es:[e]di -> real mode register structure
;	lea	edi,RealRegs	; es:[e]di -> real mode register structure

;	mov	ax,0ff02h	; FarCallReal
;	int	31h


;**JW**
;
;Have to use DPMI far call function now because we've shut off CauseWay specific
;functions.
;
	mov	ax,301h
	xor	cx,cx
	xor	bx,bx
	mov	es:word ptr[di+Real_SP],0
	mov	es:word ptr[di+Real_SS],0

	int	31h

;**JW**
;
;DEC exec counter.
;
RestoreExecCount:
;	push	ds
;	mov	ds,cs:AliasSelector
;	movzx	eax,WORD PTR ds:ReturnValue
;	mov	ds:RealRegs.Real_EAX,eax	; save return value
;	pop	ds

	mov	ah,62h		; get PSP
	int	21h
	push	ds
	mov	ds,bx		; ds -> protected mode psp
	lds	bx,ds:[EPSP_ExecCount]
	dec	byte ptr[bx]
	pop	ds

; real mode code will return here
Done:
	xor	bh,bh
	mov	dx,cs:CursorRowCol
	mov	ah,2			; set cursor position
	int	10h
	mov	cx,cs:CursorSize
	mov	ah,1			; set cursor size and shape
	int	10h

;	mov	di,WORD PTR cs:RealRegs.Real_EAX	; get return value
	mov	bx,cs:AliasSelector	; release aliased selector
	or	bx,bx
	je	Adios
	mov	ax,0ff04h		; RelSel
	int	31h
	mov	bx,cs:INTVectorSelector	; release interrupt vector selector
	or	bx,bx
	je	Adios
	mov	ax,0ff04h		; RelSel
	int	31h
	mov	dx,cs:WedgeMemSelector	; release wedge memory
	or	dx,dx
	je	Adios
	mov	ax,0ff23h		; RelMemDOS
	int	31h
	mov	dx,cs:LowMemSelector	; release low memory
	or	dx,dx
	je	Adios
	mov	ax,0ff23h		; RelMemDOS
	int	31h

Adios:
	pop	es
	pop	ds
;	push	di
;	xor	ah,ah
;	or	di,di
;	sete	al
;	push	ax
;	call	__retni	; return value as integer to clipper
;	add	sp,2		; get return value off of stack
	popad
;	mov	eax,cs:RealRegs.Real_EAX	; get return value

;**JW**
;Need to do a 32-bit far return to match the call.
	db 66h

	retf

; this part is executing in low DOS memory real mode
; fs -> wedge memory segment
; dl is nonzero if DPMI environment
NowInReal	=	$
;	DB	66h
;	retf

;	int	3

	mov	fs,bx		; wedge memory segment to fs (NT bug workaround)

; swap original interrupt vector table with current
	cli
	mov	cx,256
	push	cs
	pop	ds
	xor	di,di
	mov	es,di			; es:di -> interrupt vector table
	mov	si,OFFSET IntVectorTable

vectloop:
	mov	eax,ds:[si]
	xchg	eax,es:[di]
	add	di,4
	mov	ds:[si],eax
	add	si,4
	loop	vectloop
	sti

	push	cs
	pop	es

; reset mouse driver
	xor	ax,ax
	int	33h

;	int	3

; save DPMI flag status
	mov	DPMIFlag,dl

; get real mode PSP of program
	mov	ah,62h		; get PSP
	int	21h
	mov	PSP,bx

; seek out EXE pathspec from environment block
	mov	ds,bx		; ds -> PSP
	mov	ax,ds:[2ch]	; get original PSP environment pointer
	mov	es:PSPEnvironPtr,ax	; save original PSP environment pointer
	mov	ax,es:EnvironmentPointer
	mov	ds:[2ch],ax	; update PSP environment pointer
	mov	ds,ax		; ds -> real mode PSP environment block
	xor	si,si		; ds:si -> start of environment

endloop:
	lodsb			; get environment char
	or	al,ds:[si]	; merge in next char
	jne	endloop		; not at end of environment block
	add	si,3		; si -> start of EXE file path
	mov	di,si		; save -> start
	xor	bx,bx		; init end of char pointer

pathloop:
	lodsb			; get char of file path
	or	al,al		; see if at end
	je	calcpath	; yes
	cmp	al,'\'		; see if directory indicator
	jne	pathloop	; no
	mov	bx,si		; save -> char past directory
	jmp	SHORT pathloop

calcpath:
	mov	si,di		; ds:si -> start of file path
	mov	di,OFFSET SwapFileName	; es:di -> swap file name slot
	or	bx,bx		; see if any path
	je	calcdone	; no

calcloop:
	movsb
	cmp	si,bx		; see if at end of path
	jne	calcloop	; no

calcdone:
	push	es
	pop	ds			; ds -> swap data

; search environment variable directories for TEMP setting
; override EXE Name SwapFileName if exists
	mov	bx,OFFSET TEMPText	; ds:bx -> string to match

; see if environment variable is present
	xor	si,si
	mov	es,EnvironmentPointer	; es:si -> environment block strings

t_sevscan:
	xor	di,di			; offset into target string to match

t_matchloop:
	lods	BYTE PTR es:[0]	; get byte from environment string
	cmp	al,ds:[bx+di]	; see if matches target
	je	t_bytematch		; yes
	or	al,es:[si]		; two zero values in a row mean end of environment
	jne	t_sevscan			; no target match, more chars left
	jmp	SHORT getcomspec	; end of environment, no match, search failed

; check that environment string match is not part of another environment string
t_bytematch:
	or	di,di			; di is zero if matching first char
	jne	t_sevnext			; not matching first char
	cmp	si,1			; si==1 if first string in environment block
	je	t_sevnext			; first string, not a part of another string by definition
	cmp	BYTE PTR es:[si-2],0	; char before environment string in block must be nonzero
	jne	t_sevscan			; nonzero, subset of another string, keep scanning for match

t_sevnext:
	inc	di				; match, move to next byte of target
	cmp	di,5			; check if all bytes matched
	jb	t_matchloop		; not yet

	mov	di,OFFSET SwapFileName	; di will -> TEMP SwapFilePrefix

t_transloop:
	lods	BYTE PTR es:[0]	; get path character
	cmp	al,';'			; see if terminator character
	je	t_namedone		; yes, name complete
	cmp	al,' '			; whitespace also terminates
	jbe	t_namedone
	mov	ds:[di],al		; save path character
	inc	di				; bump name storage slot
	jmp	SHORT t_transloop

t_namedone:
	cmp	di,OFFSET SwapFileName	; see if any path
	je	t_nullit		; no
	mov	al,'\'
	cmp	BYTE PTR ds:[di-1],al	; see if backslash terminated
	je	t_nullit		; yes
	mov	ds:[di],al		; must be backslash terminated

t_nullit:
	xor	al,al			; null terminate temporary file name
	mov	ds:[di],al

;	int	3

; get COMSPEC setting for execfname
getcomspec:
	xor	si,si
;	mov	es,PSP
;	mov	es,es:[2ch]		; es:si -> environment block strings
	mov	es,EnvironmentPointer	; es:si -> environment block strings

sevscan:
	mov	bx,OFFSET COMSPECText
	xor	di,di

matchloop:
	lods	BYTE PTR es:[0]	; get byte from environment string
	cmp	al,ds:[bx+di]		; see if matches target
	je	bytematch		; yes
	or	al,es:[si]		; two zero values in a row mean end of environment
	je	med2			; end of environment, no match, search failed
	jmp	SHORT sevscan	; no target match, more chars left

; check that environment string match is not part of another environment string
bytematch:
	or	di,di			; di is zero if matching first char
	jne	sevnext			; not matching first char
	cmp	si,1			; si==1 if first string in environment block
	je	sevnext			; first string, not a part of another string by definition
	cmp	BYTE PTR es:[si-2],0	; char before environment string in block must be nonzero
	jne	sevscan			; nonzero, subset of another string, keep scanning for match

sevnext:
	inc	di				; match, move to next byte of target
	cmp	di,8			; check if all bytes matched (all extension are four chars)
	jb	matchloop		; not yet

	mov	di,OFFSET execfname	; di will -> exec'ing name with path

transloop:
	lods	BYTE PTR es:[0]	; get path character
	cmp	al,';'			; see if terminator character
	je	namedone		; yes, name complete
	cmp	al,' '			; whitespace also terminates
	jbe	namedone
	mov	ds:[di],al		; save path character
	inc	di				; bump name storage slot
	jmp	SHORT transloop

namedone:
	xor	al,al			; null terminate COMSPEC
	mov	ds:[di],al

med2:

; copy low memory save (original code) to wedge segment storage
	push	fs
	pop	es
	mov	di,WedgeSave-WedgeStart	; es:di -> wedge storage
	mov	si,WEDGEOFFSET
	mov	ds,PSP		; ds:si -> low memory save
	mov	cx,savespace-(WEDGEOFFSET-5ch)+3	; bytes to copy
	push	cx
	shr	cx,2
	rep	movsd
	pop	cx
	and	cx,3
	rep	movsb

	mov	ax,cs
	mov	ds,ax
	mov	es,ax
	mov	pmethod,4184h
	mov	swapping,1
;	mov	swapping,0
	call	prep_swap
	cmp	swap_prep.swapmethod,0ffh	; see iferror opening temporary file
	jne	noterr

	mov	ax,SWAPPERERROR
	jmp	SHORT restevp	; error, don't continue

noterr:
	call	do_spawn

; restore original environment pointer in real mode PSP
restevp:
	push	cs
	pop	ds

	mov	ReturnValue,ax
	mov	ax,PSPEnvironPtr	; ax holds original environment pointer value
	mov	ds,PSP		; ds -> real mode PSP
	mov	ds:[2ch],ax	; restore PSP environment pointer

; restore interrupt vector table, ax holds return code
	mov	cx,256
	push	cs
	pop	ds
	xor	di,di
	mov	es,di			; es:di -> interrupt vector table
	mov	si,OFFSET IntVectorTable
	cli
	rep	movsd			; restore interrupt vector table
	sti

	retf				; return to protected mode code
ENDP

; wedge code
WedgeSpace	EQU	WedgeStackEnd-WedgeStart
WedgeStart:
	pop	WORD PTR cs:[CallerOffset-WedgeStart]	; save return address
	pop	WORD PTR cs:[CallerSegment-WedgeStart]
	mov	cs:[OriginalStackOff-WedgeStart],sp	; save stack and set to new
	mov	cs:[OriginalStackSeg-WedgeStart],ss
	mov	ax,cs
	mov	ss,ax
	mov	sp,WedgeStackEnd-WedgeStart

; swap wedge storage (original code) with low memory (new code)
	push	ds
	push	es
	mov	ds,cs:[CallerSegment-WedgeStart]
	mov	si,WEDGEOFFSET
	mov	di,WedgeSave-WedgeStart	; fs:di -> wedge storage
;	mov	cx,(savespace-(WEDGEOFFSET-5ch)+3)/4	; bytes to swap
	mov	cx,savespace-(WEDGEOFFSET-5ch)		; bytes to swap

wedgeloop:
;	mov	eax,ds:[si]
;	xchg	eax,fs:[di]
;	add	di,4
;	mov	ds:[si],eax
;	add	si,4
;	loop	wedgeloop

	mov	al,ds:[si]
	xchg	al,fs:[di]
	inc	di
	mov	ds:[si],al
	inc	si
	dec	cx
	jne	wedgeloop

	pop	es
	pop	ds

	mov	ax,4b00h			; do the exec
	int	21h

	mov	ax,cs
	mov	ss,ax
	mov	sp,WedgeStackEnd-WedgeStart

; copy wedge storage (new code) to low memory
	push	cs
	pop	ds
	mov	si,WedgeSave-WedgeStart	; ds:si -> wedge storage
	mov	es,cs:[CallerSegment-WedgeStart]
	mov	di,WEDGEOFFSET	; es:di -> low memory save
;	mov	cx,savespace-(WEDGEOFFSET-5ch)+3	; bytes to copy
	mov	cx,savespace-(WEDGEOFFSET-5ch)	; bytes to copy
	push	cx
	shr	cx,2
	rep	movsd
	pop	cx
	and	cx,3
	rep	movsb

	mov	ss,cs:[OriginalStackSeg-WedgeStart]	; restore stack to original
	mov	sp,cs:[OriginalStackOff-WedgeStart]

; transfer back to low memory
	jmp	DWORD PTR cs:[CallerOffset-WedgeStart]

	OriginalStackOff	DW	?
	OriginalStackSeg	DW	?
	CallerOffset	DW	?
	CallerSegment	DW	?
WedgeSave	DB	savespace-(WEDGEOFFSET-5ch)+3 DUP (?)
EVEN
	DB	256 DUP (?)		; wedge stack
WedgeStackEnd	=	$-1

; constant data
COMSPECText	DB	'COMSPEC='
TEMPText	DB	'TEMP='

; initialized data
LowMemSelector	DW	0	; selector for accessing low memory
WedgeMemSelector	DW	0	; selector for accessing wedge memory
AllocStrat	DW	0		; memory allocation strategy
SwapFileName	DB	81 DUP (0)

; uninitialized data
AliasSelector	DW	?	; alias data selector for code writes
EnvironmentPointer	DW	?	; real mode environment block pointer
INTVectorSelector	DW	?	; selector for accessing interrupt vector table
;PSP		DW	?		; real mode PSP
;SwapFileHandle	DW	?	; swap file handle
CursorSize	DW	?		; saved cursor size
CursorRowCol	DW	?	; saved cursor row and column

RealRegsStruc struc
Real_EDI	dd ?	;EDI
Real_ESI	dd ?	;ESI
Real_EBP	dd ?	;EBP
		dd ?	;Reserved.
Real_EBX	dd ?	;EBX
Real_EDX	dd ?	;EDX
Real_ECX	dd ?	;ECX
Real_EAX	dd ?	;EAX
Real_Flags	dw ?	;FLAGS
Real_ES	dw ?	;ES
Real_DS	dw ?	;DS
Real_FS	dw ?	;FS
Real_GS	dw ?	;GS
Real_IP	dw ?	;IP
Real_CS	dw ?	;CS
Real_SP	dw ?	;SP
Real_SS	dw ?	;SS
RealRegsStruc ends

RealRegs	RealRegsStruc	<>
;Now the extended PSP structure.
EPSP_Struc		struc
		db 256 dup (?)
 EPSP_Parent		dw ?		;Selector of parent/previous PSP, 0 for none.
 EPSP_Next		dw ?		;Next PSP.
 EPSP_Resource	dd ?		;Linear address of resource tracking table. 0
				;for none.
 EPSP_mcbHead	dd ?		;Linear address of MCB memory head. 0 for none.
 EPSP_mcbMaxAlloc	dd ?		;Size of MCB chunks.
 EPSP_DTA		df ?		;DTA address.
 EPSP_TransProt	dw ?		;Transfer buffer address.
 EPSP_TransReal	dw ?		;Transfer buffer real mode segment value.
 EPSP_TransSize	dd ?		;Transfer buffer size.
 EPSP_SSESP		df ?		;Return SS:ESP to use.
 EPSP_INTMem		dd ?		;linear address of interrupt/exception vector
				;save buffer. 0 for none.
 EPSP_DPMIMem	dw ?		;selector for DPMI state save buffer. 0 for
				;none.
 EPSP_MemBase	dd ?		;Program linear load address.
 EPSP_MemSize	dd ?		;Program memory size.
 EPSP_SegBase	dw ?		;Base program selector.
 EPSP_SegSize	dw ?		;Number of program selectors.

 EPSP_NearBase	dd ?		;NEAR function address translation base.

 EPSP_RealENV	dw ?		;Original real mode environment SEGMENT.

 EPSP_NextPSP	dd ?		;Pointer to next PSP
 EPSP_LastPSP	dd ?		;Pointer to last PSP

 EPSP_Exports	dd ?		;Pointer to export list.
 EPSP_Imports	dd ?		;Pointer to import list.

 EPSP_Links		dd ?		;Count of linked modules.

 EPSP_ExecCount	dd ?		;PMode pointer to exec counter.

EPSP_Struc		ends

INTVectorTable	DD	256 DUP (?)	; interrupt vector table storage

CodeEnd	=	$

ENDS

END
