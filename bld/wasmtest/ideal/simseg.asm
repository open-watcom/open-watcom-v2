		IDEAL
		P486
		MODEL	USE32 SMALL
		DATASEG
Value1		DD	12345678h
		UDATASEG
Value2		DD	?
		STACK	1024
		CODESEG
PROC		Main
		mov	eax,[Value1]
		mov	[Value2],eax
ENDP
		END	Main
