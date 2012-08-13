		IDEAL
		P486
		MODEL	USE32 SMALL
ENUM ONE_DIGIT	{
		ZERO
		ONE
		TWO
		THREE
		}
ENUM TWO_DIGITS	{
		THIRTY_ONE = 31
		THIRTY_TWO
		THIRTY_THREE
		}
		CODESEG
PROC		Main
		mov	eax,ONE
		add	eax,TWO
		sub	eax,THREE
		cmp	eax,ZERO
		mov	eax,THIRTY_ONE
		mov	ebx,THIRTY_TWO
		lea	eax,[eax + ebx + THIRTY_THREE]
ENDP
		END
