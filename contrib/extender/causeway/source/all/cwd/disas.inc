SymbolStruc	struc
SymbolNext	dd ?
SymbolType	db ?
SymbolDword	dd ?
SymbolSeg	dw ?
SymbolTLen	db ?
SymbolText	db ?
SymbolStruc	ends


RetCode_INT		equ	1

RetCode_RET		equ	2
RetCode_RET32	equ	3
RetCode_RETnn	equ	4
RetCode_RETnn32	equ	5
RetCode_RETF		equ	6
RetCode_RETF32	equ	7
RetCode_RETFnn	equ	8
RetCode_RETFnn32	equ	9
RetCode_IRET		equ	10
RetCode_IRET32	equ	11

RetCode_SS		equ	12

RetCode_Jcc		equ	13
RetCode_JMP		equ	14
RetCode_CALL		equ	15

RetCode_JMPea	equ	16
RetCode_JMPea32	equ	17
RetCode_JMPFea	equ	18
RetCode_JMPFea32	equ	19
RetCode_CALLea	equ	20
RetCode_CALLea32	equ	21
RetCode_CALLFea	equ	22
RetCode_CALLFea32	equ	23

RetCode_MOVS		equ	24

RetCode_INOUT	equ	25

RetCode_INV		equ	26


Watch_Abs	equ	0
Watch_CS	equ	1
Watch_DS	equ	2
Watch_ES	equ	3
Watch_FS	equ	4
Watch_GS	equ	5
Watch_SS	equ	6
Watch_EAX	equ	7
Watch_EBX	equ	8
Watch_ECX	equ	9
Watch_EDX	equ	10
Watch_ESI	equ	11
Watch_EDI	equ	12
Watch_EBP	equ	13
Watch_ESP	equ	14
Watch_EIP	equ	15
Watch_AX	equ	16
Watch_BX	equ	17
Watch_CX	equ	18
Watch_DX	equ	19
Watch_SI	equ	20
Watch_DI	equ	21
Watch_BP	equ	22
Watch_SP	equ	23
Watch_IP	equ	24

Watch_Mixed	equ	0
Watch_Bytes	equ	1
Watch_Words	equ	2
Watch_Dwords	equ	3
Watch_Text	equ	4

WatchStruc	struc
WatchFlags	dw ?		;Flags.
WatchHandle	dw ?		;Window handle for this watch.
WatchDisType	db ?		;Display format.
WatchSeg	db ?		;segment type.
WatchSelNum	dw ?		;abs value if needed.
WatchOff	db ?		;offset type.
WatchOffset	dd ?		;abs value if needed.
WatchMoved	dd ?		;additional offset to apply for scrolling etc.
WatchTitle	dd ?		;address of title memory.
WatchXPos	db ?
WatchYPos	db ?
WatchWidth	dd ?
WatchLastX	db ?
WatchLastY	db ?
WatchStruc	ends

