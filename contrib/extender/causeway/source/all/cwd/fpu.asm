NOFPUWIDTH1	EQU	14
NOFPUWIDTH2	EQU	NOFPUWIDTH1-2
FPUADDWIDTH	EQU	34
CONDITIONCODE3	EQU	01000000B
CONDITIONCODE2	EQU	00000100B
CONDITIONCODE1	EQU	00000010B
CONDITIONCODE0	EQU	00000001B

.code

; get FPU values into printable text
GetFPUValues	PROC	NEAR
	cmp	IsFPUFlag,0		; see if FPU exists
	je	gfvret			; no
	fsaved	FPUState

	movzx	eax,FPUStatusWord
	mov	ecx,4
	mov	edi,OFFSET DebugSTATt
	call	Bin2Hex
	movzx	eax,FPUControlWord
	mov	ecx,4
	mov	edi,OFFSET DebugCONTt
	call	Bin2Hex
	movzx	eax,FPUTagWord
	mov	ecx,4
	mov	edi,OFFSET DebugTAGt
	call	Bin2Hex
	movzx	eax,FPUIPtrSelector
	mov	ecx,4
	mov	edi,OFFSET DebugIPTRSt
	call	Bin2Hex
	mov	eax,FPUIPtrOffset
	mov	ecx,8
	mov	edi,OFFSET DebugIPTROt
	call	Bin2Hex
	movzx	eax,FPUOPtrSelector
	mov	ecx,4
	mov	edi,OFFSET DebugOPTRSt
	call	Bin2Hex
	mov	eax,FPUOPtrOffset
	mov	ecx,8
	mov	edi,OFFSET DebugOPTROt
	call	Bin2Hex

	fld	FPUST0
	mov	edi,OFFSET DebugST0t
	call	FPUNumberType
	fld	FPUST1
	mov	edi,OFFSET DebugST1t
	call	FPUNumberType
	fld	FPUST2
	mov	edi,OFFSET DebugST2t
	call	FPUNumberType
	fld	FPUST3
	mov	edi,OFFSET DebugST3t
	call	FPUNumberType
	fld	FPUST4
	mov	edi,OFFSET DebugST4t
	call	FPUNumberType
	fld	FPUST5
	mov	edi,OFFSET DebugST5t
	call	FPUNumberType
	fld	FPUST6
	mov	edi,OFFSET DebugST6t
	call	FPUNumberType
	fld	FPUST7
	mov	edi,OFFSET DebugST7t
	call	FPUNumberType
	frstord	FPUState
gfvret:
	ret
GetFPUValues	ENDP

; upon entry ST(0)= register to display, edi -> ST(n) register ASCII storage
; convert FPU value to ASCII number or message
;  (denormal, NaN, infinity, unsupported, or Empty)
FPUNumberType	PROC	NEAR
	fxam
	fstsw	ax
	mov	ecx,OFFSET StatusUnsupported
	and	ah,(CONDITIONCODE3 OR CONDITIONCODE2 OR CONDITIONCODE1 OR CONDITIONCODE0)
	je	fntprint		; all clear not supported
	cmp	ah,CONDITIONCODE1
	je	fntprint
	mov	ecx,OFFSET StatusPlusDenormal
	cmp	ah,(CONDITIONCODE3 OR CONDITIONCODE2)
	je	fntprint
	mov	ecx,OFFSET StatusMinusDenormal
	cmp	ah,(CONDITIONCODE3 OR CONDITIONCODE2 OR CONDITIONCODE1)
	je	fntprint
	mov	ecx,OFFSET StatusPlusNaN
	cmp	ah,CONDITIONCODE0
	je	fntprint
	mov	ecx,OFFSET StatusMinusNaN
	cmp	ah,(CONDITIONCODE1 OR CONDITIONCODE0)
	je	fntprint
	mov	ecx,OFFSET StatusPlusInfinity
	cmp	ah,(CONDITIONCODE2 OR CONDITIONCODE0)
	je	fntprint
	mov	ecx,OFFSET StatusMinusInfinity
	cmp	ah,(CONDITIONCODE2 OR CONDITIONCODE1 OR CONDITIONCODE0)
	je	fntprint
	mov	ecx,OFFSET StatusEmpty
	mov	al,ah
	and	al,(CONDITIONCODE3 OR CONDITIONCODE0)
	cmp	al,(CONDITIONCODE3 OR CONDITIONCODE0)
	je	fntprint
	cmp	ah,CONDITIONCODE3
	je	fntpluszero
	cmp	ah,(CONDITIONCODE3 OR CONDITIONCODE1)
	je	fntminuszero

; normal value, must be calculated
	call	FPU_OUTPUT
	jmp	fntret

fntpluszero:
	mov	BYTE PTR [edi],'+'
	jmp	fntzero

fntminuszero:
	mov	BYTE PTR [edi],'-'

fntzero:
	inc	edi
	mov	ecx,OFFSET ZeroFill

fntprint:
	mov	al,[ecx]
	mov	[edi],al
	inc	ecx
	inc	edi
	test	al,al
	jne	fntprint

fntret:
	ret
FPUNumberType	ENDP

; check FPU existence
CheckFPU	PROC	NEAR
	finit
	push	eax
	fstcw	[esp]
	pop	eax
	cmp	ah,3
	sete	IsFPUFlag
	ret
CheckFPU	ENDP

; turn on and off FPU display
FPUDisplay	PROC	NEAR
	cmp	IsFPUFlag,0
	je	fdret			; no FPU -> no FPU display
	xor	RegsFPUDisplay,1
	mov	bp,RegsHandle
	call	LocateWindow
	call	PointWindow

	mov	ch,WindowY1[esi]
	mov	dh,WindowDepth1[esi]

	cmp	RegsFPUDisplay,0	; see if turning on or off FPU display
	je	fpuoff

; turning on FPU display
	cmp	[esi].WindowWidth1,NOFPUWIDTH1+FPUADDWIDTH
	ja	fpuon_2

	mov	al,NOFPUWIDTH1+FPUADDWIDTH
	sub	al,[esi].WindowWidth1
	cmp	[esi].WindowX1,al
	jb	fpuon_1
	sub	[esi].WindowX1,al
	sub	[esi].WindowX2,al

fpuon_1:
	add	[esi].WindowWidth1,al
	add	[esi].WindowWidth2,al

fpuon_2:
	mov	cl,WindowX1[esi]
	mov	dl,WindowWidth1[esi]

	jmp	fdisp

; turning off FPU display
fpuoff:
	mov	cl,WindowX1[esi]
	mov	dl,WindowWidth1[esi]

	cmp	[esi].WindowWidth1,NOFPUWIDTH1
	jb	fdisp
	mov	al,[esi].WindowWidth1
	sub	al,NOFPUWIDTH1
	mov	[esi].WindowWidth1,NOFPUWIDTH1
	mov	[esi].WindowWidth2,NOFPUWIDTH2
	add	[esi].WindowX1,al
	add	[esi].WindowX2,al

fdisp:
	call	WindowRegionUpdate
	call	RegisterDisplay	;Display current register values.

fdret:
	ret
FPUDisplay	ENDP

;*********************************|
;  OUTPUT ASCII STRING FROM FPU   |
;*********************************|
FPU_OUTPUT      PROC    NEAR
; Operation:
;          Converts the value in the top register of the FPU
;          stack into an ASCII decimal number in scientific
;          notation
;
; On entry:
;         EDI points to an unformatted caller's buffer area
;
; On exit:
;         a. Carry flag clear
;         The caller's buffer will contain the ASCII decimal
;         representation of the number at the FPU stack top
;         register, formatted in scientific notation, as follows:
;
;                 sm.mmmmmmmmmmmmmmmmm ESeeee
; where
; s = sign of number (blank = +)
; m = 18 significand digits
; . = decimal point following the first significand digit
; E = explicit letter E to signal start of exponent
; S = + or - sign of exponent
; e = up to four exponent digits
;
; Examples of output:
;                    1.78125234500000000 E-12
;                   -3.14163397000000000 E+0
;                    1.22334455667788998 E+1388
;
;      Signed zeros are reported as follows:
;                    0.00000000000000000 E+0 (positive zero)
;                   -0.00000000000000000 E+0 (negative zero)
;

;*******************|
;  extract exponent |
;*******************|
; The binary exponent must be converted into a decimal exponent
NORMAL_NUMBER:
        FSTCW   USERS_CW		; Save caller's control word
        FLDCW   ROUND_UP		; Force rounding up with reduced
								; precision to avoid spurious
								; rounding of results
;                               |  ST(0)  |  ST(1)  |  ST(2)  |
        FLD     ST(0)			;    #    |    #    |  EMPTY  |
        FABS					;   |#|   |    #    |  EMPTY  |
        FLD1					;    1    |   |#|   |   #     |
        FXCH    ST(1)			;   |#|   |    1    |   #     |
; Make |#| = x
        FYL2X					; Log2-x  |    #    |
        FLDL2T					; Log2-10 | Log2-x  |   #     |
        FXCH    ST(1)			; Log2-x  | Log2-10 |   #     |
        FDIV    ST,ST(1)		;  exp    | Log2-10 |   #     |
        FXCH    ST(1)			; Log2-10 |  exp    |   #     |
        FSTP    ST(0)			;  exp    |   #     |  EMPTY  |
; Change control word to round down
        FLDCW   ROUND_DOWN   ; Force rounding control
        FRNDINT					;  EXP    |   #     |  EMPTY  |
TEST_OVER:
        FLD     ST(0)			;  EXP    |  EXP    |   #     |
; Change again to normal rounding control
        FBSTP   PACKED_EXP   ;  EXP    |   #     |  EMPTY  |

;        FISTP   BINARY_EXP   ;   #     |  EMPTY  |
		fstp	st
;
        FLDCW   USERS_CW		; Restore original control word
;*********************|
; extract significand |
;*********************|
; The validity and required precision of the significand field
; is achieved by testing the 18th significand bit of the BCD
; result. If this bit is not a BCD digit, the significand is
; divided by 10. If this bit is 0, the significand is multiplied
; by 10. Conversion is repeated until the 18-th digit is valid
; and not zero
;                               |  ST(0)  |  ST(1) |  ST(2)  |
;                               |   #     |  EMPTY |  EMPTY  |
        FLD     ST(0)			;   #     |    #   |  EMPTY  |
; Round and store significand, just in case number it is integer
MANTISSA:
        FRNDINT
        FBSTP   PACKED_SIG   ;   #     |    #   |  EMPTY  |
; Test 18-th BCD for invalid or not zero
		FWAIT
		mov	esi,OFFSET PACKED_SIG
        ADD     ESI,8			; To most significand BCD byte
        MOV     AL,[ESI]
        CMP     AL,0FFH			; Integer is too large to test
        JE      TOO_LARGE
        AND     AL,11110000B	; Mask to clear bits 0 to 3
        JNZ     END_MANTISSA
; The required precision is not yet achieved
; Multiply by 10 and repeat
        FILD    TEN			; Load multiplier 
        FMULP   ST(1),ST		; Multiply by 10 
        FLD     ST(0)			; Adjust stack 
        JMP     MANTISSA		; Repeat conversion
; The significand contains invalid BCD digits
; Divide by 10 and repeat
TOO_LARGE:
        FILD    TEN			; Load divisor
        FDIVP   ST(1),ST		; Divide by 10 
        FLD     ST(0)			; Adjust stack
        JMP     MANTISSA		; Repeat conversion 
; The significand is valid and to sufficient precision
END_MANTISSA:
;*******************|
;   build decimal   |
;     number        |
;*******************|
; The decimal number is built from the data stored in the buffers
; PACKED_SIG and PACKED_EXP. The output number is formatted in
; exponential form and stored in the caller's buffer
		mov	esi,OFFSET PACKED_SIG	; Significand source
        ADD     ESI,9			; To sign byte of BCD
        MOV     AL,[ESI]		; Get sign byte from BCD
		mov	BYTE PTR [edi],'+'
        CMP     AL,0			; Zero is positive
        JE      POS_SIGNIF		; Significand is positive
        MOV     BYTE PTR [EDI],'-'	; Set negative sign
POS_SIGNIF:
        INC     EDI				; Bump decimal buffer pointer
        DEC     ESI				; and BCD significand pointer
; Move digit preceding the significand's decimal point
        MOV     AL,[ESI]		; Get packed BCD digits into AL
        CALL    NIBBS_2_DEC		; Decimal digits in AH and AL
        MOV     [EDI],AH			; Set first decimal digit
        INC     EDI
		mov	BYTE PTR [edi],'.'
        INC     EDI
        MOV     [EDI],AL			; Set second digit
        INC     EDI				; Bump pointer
; Move next eight BCD digit pairs as a block
        DEC     ESI				; Point to next BCD pair
        MOV     CX,8			; Digits to move
MOVE_8_DIGITS:
        MOV     AL,[ESI]		; Get packed BCD digits into AL
        CALL    NIBBS_2_DEC		; Decimal digits in AH and AL
        MOV     [EDI],AH			; Set first digit
        INC     EDI				; Bump decimal digits pointer
        MOV     [EDI],AL			; Place second digit
        INC     EDI				; Bump pointer
        DEC     ESI				; Source pointer to next BCD pair
        LOOP    MOVE_8_DIGITS
		mov	BYTE PTR [edi],'E'
		inc	edi			; move to exponent sign position
;*******************|
;  sign of exponent |
;*******************|
		mov	esi,OFFSET PACKED_EXP	; Exponent BCD storage
        ADD     ESI,9			; To sign byte of BCD
        MOV     BYTE PTR [EDI],'+'	; Assume positive exponent
        MOV     AL,[ESI]		; Get sign byte from BCD
        CMP     AL,0			; Zero is positive
        JE      POS_EXPONENT	; Significand is positive
        MOV     BYTE PTR [EDI],'-'	; Set negative sign
POS_EXPONENT:
        INC     EDI				; Bump decimal buffer pointer
;*******************|
; 4 exponent digits |
;*******************|
		mov	esi,OFFSET PACKED_EXP	; Reset pointer
        INC     ESI				; to first BCD digit pair of
								; exponent
; Move 4 exponent digits
        MOV     CX,2			; Counter for 2 packed BCDs
MOVE_2_DIGITS:
        MOV     AL,[ESI]		; Get packed BCD digits into AL
        CALL    NIBBS_2_DEC		; Decimal digits in AH and AL
        MOV     [EDI],AH			; Set first decimal digit
        INC     EDI				; Bump buffer pointer
        MOV     [EDI],AL			; Set second digit
        INC     EDI				; Bump buffer pointer
        DEC     ESI				; To next BCD digit pair
        LOOP    MOVE_2_DIGITS
        RET
FPU_OUTPUT      ENDP

NIBBS_2_DEC     PROC    NEAR
; Convert binary value in low and high nibble of AL into two
; ASCII decimal digits in AH and AL
; On entry:
;       AL holds two packed BCD digits in the range 0 to 9
;
; On exit:
;       AH = high-order ASCII digit (from AL bits xxxx ????)
;       AL = low-order ASCII digit (from AL bits ???? xxxx)
;
        MOV     AH,AL           ; Copy AL in AH
        SHR     AH,4            ; Isolate high nibble
        AND     AL,0FH          ; Isolate low nibble
		add	ax,3030h
        RET
NIBBS_2_DEC     ENDP

.data

; FPU state, 108 bytes total
FPUState	LABEL	BYTE
FPUControlWord	DW	0
FPUReserved1	DW	?
FPUStatusWord	DW	0
FPUReserved2	DW	?
FPUTagWord		DW	0
FPUReserved3	DW	?
FPUIPtrOffset	DD	?
FPUIPtrSelector	DW	?
FPUReserved4	DW	?
FPUOPtrOffset	DD	?
FPUOPtrSelector	DW	?
FPUReserved5	DW	?
FPUST0			DT	?
FPUST1			DT	?
FPUST2			DT	?
FPUST3			DT	?
FPUST4			DT	?
FPUST5			DT	?
FPUST6			DT	?
FPUST7			DT	?

RegsFPUDisplay	DB	0	; flag FPU display
IsFPUFlag	DB	?	; nonzero if FPU present, zero if not

ZeroFill		DB	'0.00000000000000000E+0000',0
StatusUnsupported	DB	'Unsupported               ',0
StatusPlusDenormal	DB	'+ Denormal                ',0
StatusMinusDenormal	DB	'- Denormal                ',0
StatusPlusNaN		DB	'+ NaN                     ',0
StatusMinusNaN		DB	'- NaN                     ',0
StatusPlusInfinity	DB	'+ Infinity                ',0
StatusMinusInfinity	DB	'- Infinity                ',0
StatusEmpty			DB	'Empty                     ',0

PACKED_EXP      DT      ?       ; 18-digit BCD exponent
PACKED_SIG      DT      ?       ; 18-digit BCD significand
USERS_CW		DW		?		; Storage for caller's control word
; Control words to force rounding to + or - infinity and reduce
; precision to 24 bits to avoid error during the extraction of
; the exponent
ROUND_DOWN      DW      14FFH
ROUND_UP        DW      18FFH
TEN             DW      10
