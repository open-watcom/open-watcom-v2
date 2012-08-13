.386

_data16 segment use16 'DATA'
d16pw dw 0
d16pd dd 0
d16pf df 0
_data16 ends

_data32 segment use32 'DATA'
d32pw dw 0
d32pd dd 0
d32pf df 0
_data32 ends

_code16x segment private use16 'CODE'
	nop
c16xn label near
c16xf label far
	nop
	nop
	nop
_code16x ends

_code32x segment private use32 'CODE'
	nop
c32xn label near
c32xf label far
	nop
	nop
	nop
_code32x ends

_code16 segment use16 'CODE'

	nop
	nop
	nop
c16ln label near
c16lf label far
	nop
	nop

jmp word ptr [bx]
jmp word ptr [bx]+2

jmp dword ptr [bx]
jmp dword ptr [bx]+2

jmp fword ptr [bx]
jmp fword ptr [bx]+2

;jmpf dword ptr [bx]
;jmpf dword ptr [bx]+2

;jmpf fword ptr [bx]
;jmpf fword ptr [bx]+2

jmp near ptr word ptr [bx]
jmp near ptr word ptr [bx]+2

jmp near ptr dword ptr [bx]
jmp near ptr dword ptr [bx]+2

jmp far ptr dword ptr [bx]
jmp far ptr dword ptr [bx]+2

jmp far ptr fword ptr [bx]
jmp far ptr fword ptr [bx]+2

jmp word ptr [ebx]
jmp word ptr [ebx]+2

jmp dword ptr [ebx]
jmp dword ptr [ebx]+2

jmp fword ptr [ebx]
jmp fword ptr [ebx]+2

;jmpf dword ptr [ebx]
;jmpf dword ptr [ebx]+2

;jmpf fword ptr [ebx]
;jmpf fword ptr [ebx]+2

jmp near ptr word ptr [ebx]
jmp near ptr word ptr [ebx]+2

jmp near ptr dword ptr [ebx]
jmp near ptr dword ptr [ebx]+2

jmp far ptr dword ptr [ebx]
jmp far ptr dword ptr [ebx]+2

jmp far ptr fword ptr [ebx]
jmp far ptr fword ptr [ebx]+2

assume ds:_data16
jmp d16pw
jmp d16pw+2

jmp d16pd
jmp d16pd+2

jmp d16pf
jmp d16pf+2

jmp c16ln
jmp c16ln+2

jmp c16lf
jmp c16lf+2

;jmp c16xn
;jmp c16xn+2

jmp c16xf
jmp c16xf+2

jmp word ptr d16pw
jmp word ptr d16pw+2

jmp dword ptr d16pw
jmp dword ptr d16pw+2

jmp fword ptr d16pw
jmp fword ptr d16pw+2

assume ds:_data32
jmp d32pw
jmp d32pw+2

jmp d32pd
jmp d32pd+2

jmp d32pf
jmp d32pf+2

;jmp c32ln
;jmp c32ln+2

jmp c32lf
jmp c32lf+2

;jmp c32xn
;jmp c32xn+2

jmp c32xf
jmp c32xf+2

jmp word ptr d32pw
jmp word ptr d32pw+2

jmp dword ptr d32pw
jmp dword ptr d32pw+2

jmp fword ptr d32pw
jmp fword ptr d32pw+2

_code16 ends

_code32 segment use32 'CODE'

	nop
	nop
	nop
c32ln label near
c32lf label far
	nop
	nop

jmp word ptr [bx]
jmp word ptr [bx]+2

jmp dword ptr [bx]
jmp dword ptr [bx]+2

jmp fword ptr [bx]
jmp fword ptr [bx]+2

;jmpf dword ptr [bx]
;jmpf dword ptr [bx]+2

;jmpf fword ptr [bx]
;jmpf fword ptr [bx]+2

jmp near ptr word ptr [bx]
jmp near ptr word ptr [bx]+2

jmp near ptr dword ptr [bx]
jmp near ptr dword ptr [bx]+2

jmp far ptr dword ptr [bx]
jmp far ptr dword ptr [bx]+2

jmp far ptr fword ptr [bx]
jmp far ptr fword ptr [bx]+2

jmp word ptr [ebx]
jmp word ptr [ebx]+2

jmp dword ptr [ebx]
jmp dword ptr [ebx]+2

jmp fword ptr [ebx]
jmp fword ptr [ebx]+2

;jmpf dword ptr [ebx]
;jmpf dword ptr [ebx]+2

;jmpf fword ptr [ebx]
;jmpf fword ptr [ebx]+2

jmp near ptr word ptr [ebx]
jmp near ptr word ptr [ebx]+2

jmp near ptr dword ptr [ebx]
jmp near ptr dword ptr [ebx]+2

jmp far ptr dword ptr [ebx]
jmp far ptr dword ptr [ebx]+2

jmp far ptr fword ptr [ebx]
jmp far ptr fword ptr [ebx]+2

assume ds:_data16
jmp d16pw
jmp d16pw+2

jmp d16pd
jmp d16pd+2

jmp d16pf
jmp d16pf+2

;jmp c16ln
;jmp c16ln+2

jmp c16lf
jmp c16lf+2

;jmp c16xn
;jmp c16xn+2

jmp c16xf
jmp c16xf+2

jmp word ptr d16pw
jmp word ptr d16pw+2

jmp dword ptr d16pw
jmp dword ptr d16pw+2

jmp fword ptr d16pw
jmp fword ptr d16pw+2

assume ds:_data32
jmp d32pw
jmp d32pw+2

jmp d32pd
jmp d32pd+2

jmp d32pf
jmp d32pf+2

jmp c32ln
jmp c32ln+2

jmp c32lf
jmp c32lf+2

;jmp c32xn
;jmp c32xn+2

jmp c32xf
jmp c32xf+2

jmp word ptr d32pw
jmp word ptr d32pw+2

jmp dword ptr d32pw
jmp dword ptr d32pw+2

jmp fword ptr d32pw
jmp fword ptr d32pw+2

_code32 ends

;jmpf foo1
;jmpf foo1+2

;jmpf far ptr foo1
;jmpf far ptr foo1+2

;jmp foo
;jmp foo+2

;jmp word ptr foo
;jmp word ptr foo+2

;jmp dword ptr foo
;jmp dword ptr foo+2

;jmp fword ptr foo
;jmp fword ptr foo+2

;jmpf foo
;jmpf foo+2

;jmpf dword ptr foo
;jmpf dword ptr foo+2

;jmpf fword ptr foo
;jmpf fword ptr foo+2

end
