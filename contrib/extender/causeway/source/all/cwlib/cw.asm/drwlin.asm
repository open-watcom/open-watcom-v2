	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Draw a line in specified colour between specified points.
;
;On Entry:
;
;EAX	- Flags, bit significant if set.
;	0 - Fill value is RGB.
;	1 - Fill type, 0-normal, 1-store data, 2-replace data.
;	2 -/
;EBX	- Fill value or data pointer depending on flags.
;ECX	- start X co-ord.
;EDX	- start Y co-ord.
;ESI	- end X co-ord.
;EDI	- end Y co-ord.
;
;On Exit:
;
;nothing.
;
;All registers preserved.
;
;Notes:
;
;If type 1 is selected it is the callers responsibility to ensure that the
;buffer pointed to by EBX is big enough.
;
VideoDrawLine:
	public VideoDrawLine
_VideoDrawLine_ proc syscall
	local @@x1:dword, @@x2:dword, @@y1:dword, @@y2:dword, \
	@@deldy:dword, @@dels:dword, @@delp:dword, @@delsx:dword, \
	@@delsy:dword, @@delse:dword, @@delde:dword, @@colour:dword, \
	@@flags:dword, @@deldx:dword
	pushad
	mov	@@flags,eax
	mov	@@colour,ebx
	mov	@@x1,ecx
	mov	@@y1,edx
	mov	@@x2,esi
	mov	@@y2,edi
;
; set up x and y updates
;
	mov	esi,1		;start with positive 1 for x update
	mov	edi,1		;start with positive 1 for y update
;
; find |y2-y1|
;
	mov	edx,@@y2		;get y2
	sub	edx,@@y1		;subtract y2
	jge	@@storey		;skip if y2-y1 is non-negative
	neg	edi		;move in negative y direction
	neg	edx		;absolute value of y2-y1
@@storey:	mov	@@deldy,edi		;store y update for diagonal moves
;
; find |x1-x2|
;
	mov	ecx,@@x2		;get x2
	sub	ecx,@@x1		;subtract x2
	jge	@@storex		;skip if x2-x1 is non-negative
	neg	esi		;move in negative x direction
	neg	ecx		;absolute value of x2-x1
@@storex:	mov	@@deldx,esi		;store x update for diagonal moves
;
; sort |y2-y1| and |x2-x1|
;
	cmp	ecx,edx		; compare dels with delp
	jge	@@setdiag		; skip if straight moves in y direction
	xor	esi,esi		; if straight=vertical, kill & update
	xchg	ecx,edx		;   & exchange differences
	jmp	@@storedelsxy
;
@@setdiag:	xor	edi,edi		; if straight = horizontal, kill y update
;
; store dels, delp, delsx and delsy
;
@@storedelsxy:	mov	@@dels,ecx		; change in straight direction
	mov	@@delp,edx		; change in perpendicular to straight
	mov	@@delsx,esi		; x update in straight direction
	mov	@@delsy,edi		; y update in straight direction
;
; get initial values for x and y
;
	mov	esi,@@x1		; x coordinate
	mov	edi,@@y1		; y coordinate
;
; compute initial value and increments for error function
;
	mov	eax,@@delp
	sal	eax,1		; 2 * delp
	mov	@@delse,eax		; change if straight move
;
	sub	eax,ecx		; 2*delp - dels
	mov	ebx,eax		; initial value
;
	sub	eax,ecx		; 2*delp - 2*dels
	mov	@@delde,eax		; change if diagonal move
;
; adjust count
;
	inc	ecx
;
; get the color
;
	mov	edx,@@colour		; get the color
	mov	eax,@@flags
;
; main loop structure
;
	xchg	ecx,esi
	xchg	edx,edi
	xchg	edi,ebx
@@lineloop:	call	VideoSetPixel	; plot the point
	or	edi,edi		; determine if straight or diagonal move
	jge	diagonal
;
; case for straight move
;
straight:	add	ecx,@@delsx	; update x
	add	edx,@@delsy	; update y
	add	edi,@@delse	; update error term
	dec	esi
	jnz	@@lineloop	; next point
	jmp	@@lineexit
;
; case for diagonal move
;
diagonal:	add	ecx,@@deldx	; update x
	add	edx,@@deldy	; update y
	add	edi,@@delde	; update error term
	dec	esi
	jnz	@@lineloop	; next point
;
@@lineexit:	popad
	ret
_VideoDrawLine_ endp


	efile
	end
