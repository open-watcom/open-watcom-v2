.386

_text16a segment use16 'CODE'
assume cs:_text16a
	nop
	nop
	nop
c161:
	jmp 200,200
	jmp 200,c161
	jmp 200,c162
	jmp 200,c321
	jmp 200,c322
	jmpf 200,200
	jmpf 200,c161
	jmpf 200,c162
	jmpf 200,c321
	jmpf 200,c322
	call 200,200
	call 200,c161
	call 200,c162
	call 200,c321
	call 200,c322
	callf 200,200
	callf 200,c161
	callf 200,c162
	callf 200,c321
	callf 200,c322
_text16a ends

_text16b segment use16 'CODE'
assume cs:_text16b
	nop
	nop
	nop
c162:
	jmp 200,200
	jmp 200,c161
	jmp 200,c162
	jmp 200,c321
	jmp 200,c322
	jmpf 200,200
	jmpf 200,c161
	jmpf 200,c162
	jmpf 200,c321
	jmpf 200,c322
	call 200,200
	call 200,c161
	call 200,c162
	call 200,c321
	call 200,c322
	callf 200,200
	callf 200,c161
	callf 200,c162
	callf 200,c321
	callf 200,c322
_text16b ends

_text32a segment use32 'CODE'
assume cs:_text32a
	nop
	nop
	nop
c321:
	jmp 200,200
	jmp 200,c161
	jmp 200,c162
	jmp 200,c321
	jmp 200,c322
	jmpf 200,200
	jmpf 200,c161
	jmpf 200,c162
	jmpf 200,c321
	jmpf 200,c322
	call 200,200
	call 200,c161
	call 200,c162
	call 200,c321
	call 200,c322
	callf 200,200
	callf 200,c161
	callf 200,c162
	callf 200,c321
	callf 200,c322
_text32a ends

_text32b segment use32 'CODE'
assume cs:_text32b
	nop
	nop
	nop
c322:
	jmp 200,200
	jmp 200,c161
	jmp 200,c162
	jmp 200,c321
	jmp 200,c322
	jmpf 200,200
	jmpf 200,c161
	jmpf 200,c162
	jmpf 200,c321
	jmpf 200,c322
	call 200,200
	call 200,c161
	call 200,c162
	call 200,c321
	call 200,c322
	callf 200,200
	callf 200,c161
	callf 200,c162
	callf 200,c321
	callf 200,c322
_text32b ends

end
