
	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Draw a line in specified colour between specified points.
;
;On Entry:
;
;C style stack parameters as follows,
;
;flags	- flags, bit significant if set.
;colour	- fill colour.
;x1	- start X co-ord.
;y1	- start Y co-ord.
;x2	- end x co-ord.
;y2	- end y co-ord.
;bitmap	- bitmap to draw on.
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
BitmapDrawLine:
	public VideoDrawLine
_BitmapDrawLine proc syscall @@flags:dword, @@colour:dword, @@x1:dword, \
	@@y1:dword, @@x2:dword, @@y2:dword, @@bitmap
	local	@@deldy:dword, @@dels:dword, @@delp:dword, @@delsx:dword, \
	@@delsy:dword, @@delse:dword, @@delde:dword, @@deldx:dword
	pushad
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
@@lineloop:	push	ebp
	mov	ebp,@@bitmap
	call	BitmapSetPixel	; plot the point
	pop	ebp
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
_BitmapDrawLine endp


	efile
	end

