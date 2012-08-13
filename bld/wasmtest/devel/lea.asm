; Test different type of the LEA instruction
.8086

extern ar:fword
extern br:qword
extern ccr:tbyte
extern dr:dword
extern er:byte

.386	; default use to use32

extern a:fword
extern b:qword
extern cc:tbyte
extern d:dword
extern e:byte

.model small
.data
a1	df	?
b1	dq	?
cc1	dt	?
d1	dd	?
e1	db	?
.code
    lea bx, a
    lea bx, b
    lea bx, cc
    lea cx, d
    lea cx, e
    lea ebx, a
    lea ebx, b
    lea ebx, cc
    lea ecx, d
    lea ecx, e
    lea bx, ar
    lea bx, br
    lea bx, ccr
    lea cx, dr
    lea cx, er
    lea ebx, ar
    lea ebx, br
    lea ebx, ccr
    lea ecx, dr
    lea ecx, er
    lea bx, a1
    lea bx, b1
    lea bx, cc1
    lea cx, d1
    lea cx, e1
    lea ebx, a1
    lea ebx, b1
    lea ebx, cc1
    lea ecx, d1
    lea ecx, e1
    lea ecx, lc1
    lea dx,[bx]
    lea dx,[ebx]
    lea dx,[bx + 4]
    lea dx,[ebx + 4]
    lea dx,byte ptr[bx + 4]
    lea dx,byte ptr[ebx + 4]
    lea dx,word ptr[bx + 4]
    lea dx,word ptr[ebx + 4]
    lea dx,dword ptr[bx + 4]
    lea dx,dword ptr[ebx + 4]
    lea edx,byte ptr[bx + 4]
    lea edx,byte ptr[ebx + 4]
    lea edx,word ptr[bx + 4]
    lea edx,word ptr[ebx + 4]
    lea edx,dword ptr[bx + 4]
    lea edx,dword ptr[ebx + 4]
    ret
lc1:
end
