;
;Test of mixing 32-bit and 16-bit segment references in FLAT model
;

	.386
	.model small
	.stack 1024


	.code

start:
	mov	ax,DGROUP
	mov	ds,eax		; mov <segreg>,eax instead of mov <segreg>,ax shuts off
	mov	es,eax		;  MASM's superfluous 66h prefix byte (OPSIZE) generation.
					;  Other assemblers may insist on mov <segreg>,ax form.
					;  At least some versions of TASM don't care.

;
;A nice simple one, 32-bit to 32-bit
;
	mov	edx,offset DGROUP:Message1
	mov	ah,9
	int	21h
;
;32-bit to 16-bit as a FLAT reference (DGROUP should be the frame)
;
	mov	edx,offset DGROUP:Message2
	mov	ah,9
	int	21h
;
;32-bit to 16-bit with a 16-bit frame, should be a normal fixup.
;
	push	ds
	mov	ax,_DATA16
	mov	ds,eax
	assume ds:_DATA16
	mov	dx,offset _DATA16:Message3
	mov	ah,9
	int	21h
	assume ds:DGROUP
	pop	ds
;
;Give 16-bit code a shout.
;

	call	fword ptr to16

	mov	ax,4c00h
	int	21h

	.data

Message1	db "32-bit to 32-bit FLAT fixup",13,10,"$"
Message4	db "16-bit to 32-bit (32-bit frame) FLAT fixup",13,10,"$"

; a terribly ugly but effective way to get the assembler to generate
; the proper set of instructions here for calling 16-bit code
to16	DW	OFFSET start16
		DW	0
		DW	_TEXT16

_TEXT16	segment word public use16 'CODE'
	assume cs:_TEXT16

;
;Assume DS would normally address DGROUP like it does here.
;
	assume ds:DGROUP

start16:

;
;Reference 32-bit data with a 32-bit frame even though we're in a 16-bit code
;segment, should come out FLAT.
;
	mov	edx,offset DGROUP:Message4
	mov	ah,9
	int	21h
;
;Reference 16-bit data but use a 32-bit frame (DGROUP) and we should still get
;FLAT.
;
	mov	edx,offset DGROUP:Message5
	mov	ah,9
	int	21h
;
;Do normal reference.
;
	push	ds
	mov	ax,_DATA16
	mov	ds,ax
	assume ds:_DATA16
	mov	dx,offset _DATA16:Message6
	mov	ah,9
	int	21h
	assume ds:DGROUP
	pop	ds

	db 66h	;need an override to return to 32-bit
	retf

_TEXT16	ends


_DATA16	segment word public use16 'DATA'

Message2	db "32-bit to 16-bit (32-bit frame) FLAT fixup",13,10,"$"
Message3	db "32-bit to 16-bit (16-bit frame) normal fixup",13,10,"$"
Message5	db "16-bit to 16-bit (32-bit frame) FLAT fixup",13,10,"$"
Message6	db "16-bit to 16-bit (16-bit frame) normal fixup",13,10,"$"

_DATA16	ends

	end	start
