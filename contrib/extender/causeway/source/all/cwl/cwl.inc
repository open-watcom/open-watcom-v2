pushm	MACRO	R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16
	IRP	X,<R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16> 	;REPEAT FOR EACH PARM
	IFNB	<X>			;IF THIS PARM NOT BLANK
	PUSH	X			;SAVE THE REGISTER
	ENDIF				;END IFNB
	ENDM				;END IRP
	ENDM

popm	macro	R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16
	IRP	X,<R16,R15,R14,R13,R12,R11,R10,R9,R8,R7,R6,R5,R4,R3,R2,R1> ;REPEAT FOR EACH PARM
	IFNB	<X>				;IF THIS PARM NOT BLANK
	POP	X				;POP THE REGISTER
	ENDIF					;END IFNB
	ENDM					;END IRP
	ENDM

rep_movsb	macro
	push	ecx
	shr	ecx,2
	rep	movsd
	pop	ecx
	and	ecx,3
	rep	movsb
	endm

rep_stosb	macro
	push	eax
	mov	ah,al
	push	ax
	shl	eax,16
	pop	ax
	push	ecx
	shr	ecx,2
	rep	stosd
	pop	ecx
	and	ecx,3
	rep	stosb
	pop	eax
	endm


;------------------------------------------------------------------------------
;Some useful equates.
;------------------------------------------------------------------------------

b	equ	byte ptr
w	equ	word ptr
d	equ	dword ptr
f	equ	fword ptr






ListBumpL	equ	16
ListBumpG	equ	256

;------------------------------------------------------------------------------
;
;Expand a pointer list in ListBump sized chunks. The list only gets expanded
;every ListBump number of entries to save time.
;
;On Entry:
;
;p1	- Source of current list address.
;
;On Exit:
;
;Carry set on error else,
;
;p1	- destination for new list address.
;
;All other registers preserved.
;
;NOTE:
;
;p1 cannot be EAX, ECX or ESI.
;
ExpandList	macro p1,p2
	local __0
	pushm	eax,ecx,esi
	mov	esi,p1
	mov	ecx,[esi]			;Get current count.
	or	ecx,ecx
	jz	__0
	test	ecx,p2-1
	jnz	__0
	add	ecx,p2			;+1 for this new entry.
	shl	ecx,2				;dword entries.
	add	ecx,4				;include count dword.
	call	ReMalloc			;try and get new size.
	jc	__0
	mov	p1,esi			;store new address.
__0:	popm	eax,ecx,esi
	endm


;------------------------------------------------------------------------------
;
;Fetch an index value from a record.
;
;On Entry:
;
;EDX	- Length.
;ESI	- Current position.
;
;On Exit:
;
;EAX	- Value retrieved.
;EDX	- New length.
;ESI	- New position.
;
;All other registers preserved.
;
GetIndex	macro
	local __0
	xor	eax,eax
	mov	al,[esi]
	inc	esi
	dec	edx
	or	al,al
	jns	__0
	and	al,127
	mov	ah,al
	mov	al,[esi]
	inc	esi
	dec	edx
__0:	;
	endm


Obj	struc
Module	dd ?	;pointer to module.
MLength	dd ?	;module length.
MLNAMES	dd ?	;list of LNAME's.
MSegs	dd ?	;list of segments.
MGROUPS	dd ?	;list of group definitions.
MExtDefs	dd ?	;list of EXTDEF entries.
MName	dd ?	;pointer to module source name.
MFileName	dd ?	;pointer to file name.
Obj	ends


Lib	struc
LAddress	dd 0		;Address of library.
LLength	dd 0		;Length of library.
LName	db 256 dup (0) ;file name.
Lib	ends


LSeg	struc
SName	dd ?		;segment name index.
SClass	dd ?		;segment class index.
SOverlay	dd ?		;segment overlay name index.
SAlign	db ?		;Align type.
SCombine	db ?		;Combine type.
SBig	db ?		;Big bit.
SPage	db ?		;don't know.
SAddress	dd ?		;address of this segments memory.
SPosition	dd ?		;current position in this segment.
SLength	dd ?		;Length of segment before next segment alignment fix.
SILength	dd ?		;Length of segment with next segment alignment fix.
SLimit	dd ?		;Limit for this segment.
SBase	dd ?		;base offset from start of program.
SDebug	dd ?		;Debug segment ?
SFlags	dd ?		;control flags.
LSeg	ends


OSeg	struc
OGSeg	dd ?		;global SEGDEF index number.
OLength	dd ?		;Local segment length.
OBase	dd ?		;Offset of this local segment within global segment.
	dd ?
OSeg	ends


PUB	struc
PObj	dd 0		;Object pointer that defined this public.
PGRP	dd 0		;Group index. (local)
PSEG	dd 0		;Segment index. (local)
PFRAME	dd 0		;Frame number.
PType	dd 0		;Type index.
PValue	dd 0		;Value/offset.
PName	dd 0		;Address of name text.
PScope	dd 0		;Scope, 0=global, !=0 = local.
PFlags	dd 0		;Flags it as referanced.
PUB	ends


EXT	struc
EPub	dd 0		;Pointer to public that matches.
EName	dd 0		;Address of this name.
EObj	dd 0		;Defining OBJ file.
EScope	dd 0		;Scope, 0=global, !=0 = local.
EFlags	dd 0		;Flags IMPORT etc.
EXT	ends

EXPORT	struc
EXP_Offset	dd 0
EXP_Seg	dw 0
EXP_TextLen	db 0		;length of name.
EXPORT	ends

IMPORT	struc
IMP_Module	dd 0		;module name index.
IMP_Name	dd 0		;symbol name index.
IMP_IName	dd 0		;Internal name to be known by.
IMP_Flags	dd 0		;flags.
IMPORT	ends


THEADR	equ	080h
LHEADR	equ	082h
COMENT	equ	088h
MODEND	equ	08Ah
MODENDL	equ	08Bh
EXTDEFL	equ	08Ch
PUBDEF	equ	090h
PUBDEFL	equ	091h
LINNUM	equ	094h
LINNUML	equ	095h
LNAMES	equ	096h
SEGDEF	equ	098h
SEGDEFL	equ	099h
GRPDEF	equ	09Ah
FIXUPP	equ	09Ch
FIXUPPL	equ	09Dh
LEDATA	equ	0A0h
LEDATAL	equ	0A1h
LIDATA	equ	0A2h
LIDATAL	equ	0A3h
LEXTDEF	equ	0B4h
LPUBDEF	equ	0B6h
LPUBDEFL	equ	0B7h

COM_COMPILER	equ	000h		;Compiler name.
COM_DOSVER	equ	09Ch		;DOS version.
COM_MODEL	equ	09Dh		;Memory model.
COM_DOSSEG	equ	09Eh		;DOSSEG segment ordering.
COM_LIBDEF	equ	09Fh		;LIB name.
COM_OMF1	equ	0A0h		;OMF extension type 1.
COM_OMF2	equ	0A1h		;OMF extension type 2.
COM_LIBMOD	equ	0A3h		;LIB comment record?
COM_EXESTR	equ	0A4h		;?
COM_NOPAD	equ	0A7h		;No segment padding.
COM_WKEXT	equ	0A8h		;Weak external?
COM_LZEXT	equ	0A9h		;Lazy external.
COM_PHARLAP	equ	0AAh		;Pharlap record format.
COM_COMP2	equ	0DBh		;Compiler string?
COM_DATE	equ	0DCh		;Date?
COM_TIME	equ	0DDh		;Date?
COM_USER	equ	0DFh		;User?
COM_DEPEND	equ	0E8h		;Dependancy file.
COM_DEPEND2	equ	0E9h		;   "

COM_OMF1_IMPDEF equ 001h		;Import definition.
COM_OMF1_EXPDEF equ 002h		;Export definition.
COM_OMF1_LNKDIR equ 005h		;C++ Linker directives.

SymbolStruc	struc
SymbolNext	dd ?
SymbolType	db ?
SymbolDword	dd ?
SymbolSeg	dw ?
SymbolTLen	db ?
SymbolText	db ?
SymbolStruc	ends


