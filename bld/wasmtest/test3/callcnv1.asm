.MODEL small

.DATA

public C d1
public SYSCALL d2
public STDCALL d3
public FORTRAN d4

d1 db 0
d2 db 0
d3 db 0
d4 db 0

x dw 0

.CODE

p1 proc C public USES ES DI BX, gb: byte, gw1: word, gw2: word, gw3: word, gptr: dword
        mov     bl, gb
	les	di, gptr
        mov     ax, gw1
        mov     ax, x
	ret
p1 endp

p2 proc SYSCALL public USES ES DI BX, gb: byte, gw1: word, gw2: word, gw3: word, gptr: dword
        mov     bl, gb
	les	di, gptr
        mov     ax, gw1
        mov     ax, x
	ret
p2 endp

p3 proc STDCALL public USES ES DI BX, gb: byte, gw1: word, gw2: word, gw3: word, gptr: dword
        mov     bl, gb
	les	di, gptr
        mov     ax, gw1
        mov     ax, x
	ret
p3 endp

p4 proc FORTRAN public USES ES DI BX, gb: byte, gw1: word, gw2: word, gw3: word, gptr: dword
        mov     bl, gb
	les	di, gptr
        mov     ax, gw1
        mov     ax, x
	ret
p4 endp

public STDCALL p5
p5:
	ret

p6 proc STDCALL public
	ret
p6 endp

        END
