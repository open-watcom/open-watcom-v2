	include ..\cwlib.inc
	scode

;------------------------------------------------------------------------------
;
;Clip a set of coords to current overiding region if enabled.
;
;On Entry:
;
;ECX	- X coord.
;EDX	- Y coord.
;ESI	- width.
;EDI	- depth.
;
;On Exit:
;
;Carry set if region completely obscured else,
;
;ECX	- clipped X coord.
;EDX	- clipped Y coord.
;ESI	- clipped width.
;EDI	- clipped depth.
;
;ALL other registers preserved.
;
WindowRegionClip proc near
	push	eax
	cmp	WindowRegionFlag,0
	jz	@@8
	;
	;clip top first.
	;
	cmp	edx,WindowRegionY
	jge	@@NoTop		;no clipping required.
	sub	edx,WindowRegionY
	neg	edx		;get positive clip size.
	sub	edi,edx		;reduce the depth.
	js	@@9
	mov	edx,WindowRegionY	;reset co-ordinate.
	;
@@NoTop:	;clip the left side.
	;
	cmp	ecx,WindowRegionX
	jge	@@NoLeft		;no clipping required.
	sub	ecx,WindowRegionX
	neg	ecx		;get positive clip size.
	sub	esi,ecx		;reduce the width.
	js	@@9
	mov	ecx,WindowRegionX	;reset co-ordinate.
	;
@@NoLeft:	;clip the bottom.
	;
	mov	eax,edx		;get the Y co-ord.
	add	eax,edi		;add in the depth.
	cmp	eax,WindowRegionD
	jl	@@NoBot		;no clipping required.
	sub	eax,WindowRegionD	;get clip size.
	cmp	eax,edi		;bigger than depth?
	sub	edi,eax		;reduce depth.
	js	@@9
	;
@@NoBot:	;clip the right.
	;
	mov	eax,ecx		;get the X co-ord
	add	eax,esi		;add in the width.
	cmp	eax,WindowRegionW
	jl	@@NoRight		;no clipping required.
	sub	eax,WindowRegionW	;get clip size.
	cmp	eax,esi		;bigger than width?
	sub	esi,eax		;reduce the width.
	js	@@9
	;
@@NoRight:	;do final check for silly sizes.
	;
	or	esi,esi		;check the width
	js	@@9
	or	edi,edi		;check the depth
	js	@@9
	;
@@8:	pop	eax
	xor	eax,eax
	clc
	ret
	;
@@9:	pop	eax
	mov	eax,-1
	or	eax,eax
	stc
	ret
WindowRegionClip endp


	efile
	end

