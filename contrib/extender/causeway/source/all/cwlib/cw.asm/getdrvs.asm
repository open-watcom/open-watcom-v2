	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Build a list of all valid drives on the system.
;
;On Entry:
;
;EDX	- Address to build list of drives.
;
;On Exit:
;
;ECX	- number of drives.
;
;ALL other registers preserved.
;
;The drive list uses real drive codes (not ASCII). Each entry uses 1 byte
;and the list is terminated with -1.
;
GetDrives	proc	near
	pushm	eax,edx,edi
	mov	edi,edx
	mov	ah,19h	; use get drive function
	int	21h
	push	eax	; save current drive.
	mov	dl,-1
	xor	cl,cl
	;
l0:	inc	dl
	cmp	dl,26
	jz	l1
	push	edx
	mov	ah,0eh	; use set drive function
	int	21h
	mov	ah,19h	; now get it again.
	int	21h
	pop	edx
	cmp	al,dl	; right drive?
	jnz	l0
	inc	cl
	stosb
	xor	al,al
	stosb
	jmp	l0
	;
l1:	xor	al,al
	stosb
	pop	edx
	mov	ah,0eh	; use set drive function
	int	21h
	movzx	ecx,cl
	popm	eax,edx,edi
	ret
GetDrives	endp


	efile
	end

